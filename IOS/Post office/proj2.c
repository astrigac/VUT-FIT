/*
    IOS Projekt 2 - Posta (Post office)
    Autor: Aurel Strigac <xstrig00>
    Ciastocne inspirovane knihou The Little Book of Semaphores (Allen B. Downey)
    Spustenie: ./proj2 NZ NU TZ TU F
        • NZ: pocet zakaznikov
        • NU: počet uradnikov
        • TZ: Maximalny cas v milisekundach, po ktory zakaznik po svojom vytvoreni čaká, kým vojde na
              postu. 0<=TZ<=10000
        • TU: Maximalna dlzka prestavky uradnika v milisekundach. 0<=TU<=100
        • F: Maximalny cas v milisekundach, po ktorom je uzavreta posta pre novo prichadzajucich.
             0<=F<=10000
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <fcntl.h>

#define OPENED 1
#define CLOSED 0

// makra pre namapovanie a uvolnenie zdielanej pamate
#define MMAP(pointer)                                                                                          \
    {                                                                                                          \
        (pointer) = mmap(NULL, sizeof(*(pointer)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); \
    }
#define UNMAP(pointer)                        \
    {                                         \
        munmap((pointer), sizeof((pointer))); \
    }

typedef struct shared
{
    // premenne
    int post_state;         // stav posty (zatvorena/otvorena)
    int customs_at_post;    // pocet zostavajucich zakaznikov v budove
    size_t line_num;        // cislo riadku
                             
    // semafory a mutexy
    sem_t state_mutex;         // mutex pre ziskanie hodnoty stavu posty
    sem_t customer_mutex;      // mutex pre kontrolu zmeny poctu zakaznikov na prevadzke
    sem_t line_mutex;          // mutex pre zamykanie rad ked ich zamestnanec prehliada
    sem_t out_mutex;           // mutex kontrolujuci pisanie do proj2.out
    sem_t sem_start;           // semaphore for starting children processes
    sem_t sem_l1_customer;     // semafor pre synchronizaciu zakaznika a uradnika v rade cislo 1
    sem_t sem_l1_officer;      // semafor pre synchronizaciu zakaznika a uradnika v rade cislo 1
    sem_t sem_l2_customer;     // semafor pre synchronizaciu zakaznika a uradnika v rade cislo 2
    sem_t sem_l2_officer;      // semafor pre synchronizaciu zakaznika a uradnika v rade cislo 2
    sem_t sem_l3_customer;     // semafor pre synchronizaciu zakaznika a uradnika v rade cislo 3
    sem_t sem_l3_officer;      // semafor pre synchronizaciu zakaznika a uradnika v rade cislo 3
    sem_t sem_cus_at_office;   // semafor pre synchronizaciu vypisov zakaznika a uradnika

    // subor pre vypis priebehu
    FILE *pFile;  // subor sluziaci na nacitanie suboru proj2.out

} shared_t;

// funkcia pre uvoľnenie a odmapovanie zdieľanej pamäte
void sharedDtor(shared_t *shared)
{
    sem_destroy(&(shared->state_mutex));
    sem_destroy(&(shared->customer_mutex));
    sem_destroy(&(shared->line_mutex));
    sem_destroy(&(shared->sem_start));
    sem_destroy(&(shared->sem_l1_customer));
    sem_destroy(&(shared->sem_l1_officer));
    sem_destroy(&(shared->sem_l2_customer));
    sem_destroy(&(shared->sem_l2_officer));
    sem_destroy(&(shared->sem_l3_customer));
    sem_destroy(&(shared->sem_l3_officer));
    sem_destroy(&(shared->sem_cus_at_office));

    sem_destroy(&(shared->out_mutex));
    fclose(shared->pFile);

    UNMAP(shared);
}

// funkcia pre inicializovanie zdieľanej pamäte
void sharedCtor(shared_t *shared)
{
    if ((shared->pFile = fopen("proj2.out", "w")) == NULL) // kontrola ze sa subor spravne otvoril
    {
        fprintf(stderr, "Error: Failed to open proj2.out\n");
        UNMAP(shared);
        exit(1);
    }
    setbuf(shared->pFile, NULL);

    // inicializacia premennych
    shared->post_state = OPENED;
    shared->customs_at_post = 0;
    shared->line_num = 0;

    // inicializacia semaforov
    if ((sem_init(&(shared->state_mutex), 1, 1) == -1) ||
        (sem_init(&(shared->customer_mutex), 1, 1) == -1) ||
        (sem_init(&(shared->line_mutex), 1, 1) == -1) ||
        (sem_init(&(shared->sem_start), 1, 0) == -1) ||
        (sem_init(&(shared->sem_l1_customer), 1, 0) == -1) ||
        (sem_init(&(shared->sem_l1_officer), 1, 0) == -1) ||
        (sem_init(&(shared->sem_l2_customer), 1, 0) == -1) ||
        (sem_init(&(shared->sem_l2_officer), 1, 0) == -1) ||
        (sem_init(&(shared->sem_l3_customer), 1, 0) == -1) ||
        (sem_init(&(shared->sem_l3_officer), 1, 0) == -1) ||
        (sem_init(&(shared->sem_cus_at_office), 1, 0) == -1) ||
        (sem_init(&(shared->out_mutex), 1, 1) == -1))
    {
        fprintf(stderr, "Error: Failed to initialize semaphores\n");
        fclose(shared->pFile);
        UNMAP(shared);
        exit(1);
    }
}

// funkcia pre vyvolanie cakania
void mySleep(unsigned max_time)
{
    int sleep_time = (rand() % (max_time + 1)) * 1000;
    usleep(sleep_time);
}

// funkcia pre vyvolanie cakania hlavneho procesu pred zatvorenim posty
void closingSleep(unsigned F)
{
    int f_half = (int) F/2;
    int sleep_time = ((rand() % (f_half + 1)) + f_half) * 1000;
    usleep(sleep_time);
}

// funkcia pre kontrolu ci string sa sklada iba z cislic
void allDigitCheck(char argv[])
{
    for(size_t i = 0; i < strlen(argv); i++){
        if(!isdigit(argv[i])){
            fprintf(stderr, "Error: Argument(s) not digit\n");
            exit(1);
        }
    }
}

// funkcia pre nacitanie a overenie spravnosti argumentov
void parseArguments(int argc, char *argv[], int* NZ, int* NU, int* TZ, int* TU, int* F)
{
    int correct_arg_cnt = 6;
    if (argc != correct_arg_cnt) {
        fprintf(stderr, "Error: Incorrect argument count\nUsage: ./proj2 NZ NU TZ TU F\n");
        exit(1);
    }

    for(int i = 1; i < argc; i++) {
        allDigitCheck(argv[i]);
    }

    *NZ = atoi(argv[1]);
    *NU = atoi(argv[2]);
    *TZ = atoi(argv[3]);
    *TU = atoi(argv[4]);
    *F = atoi(argv[5]);

    if (*NZ < 0 || *NU <= 0 || *TZ < 0 || *TU < 0 || *F < 0 ||
        *TZ > 10000 || *TU > 100 || *F > 10000) {
        fprintf(stderr,"Error: Invalid argument(s)\nPlease make sure all values are within the specified range\n");
        exit(1);
    }
}

// funkcia pre zatvorenie posty
void closePostOffice(int F, shared_t *shared)
{
    closingSleep(F);

    // zatvorim postu a vypisem to
    sem_wait(&shared->state_mutex);
    shared->post_state = CLOSED;

    sem_wait(&shared->out_mutex);
    fprintf(shared->pFile, "%ld: closing\n", ++shared->line_num);
    fflush(shared->pFile);
    sem_post(&shared->out_mutex);

    sem_post(&shared->state_mutex);
}

// funkcia na zosynchronizovanie spustenia vsetkych detskych procesov
void waitForStart(shared_t *shared)
{
    sem_wait(&shared->sem_start);
    sem_post(&shared->sem_start);
}

void customerProcess(int id, int TZ, shared_t *shared)
{
    waitForStart(shared);

    sem_wait(&shared->out_mutex);
    fprintf(shared->pFile, "%ld: Z %d: started\n", ++shared->line_num, id);
    fflush(shared->pFile);
    sem_post(&shared->out_mutex);

    mySleep(TZ);

    sem_wait(&shared->state_mutex);
    if(shared->post_state == OPENED)
    {
        // inkrementacia poctu neobsluzenych zakaznikov na prevadzke
        sem_wait(&shared->customer_mutex);
        ++shared->customs_at_post;
        sem_post(&shared->customer_mutex);

        int action = id% 3 + 1;

        // pockanie na odomknutie radov
        sem_wait(&shared->line_mutex);
        sem_post(&shared->line_mutex);

        sem_wait(&shared->out_mutex);
        fprintf(shared->pFile, "%ld: Z %d: entering office for a service %d\n", ++shared->line_num, id, action);
        fflush(shared->pFile);
        sem_post(&shared->out_mutex);

        sem_post(&shared->state_mutex);

        switch(action){
            case 1:
                sem_post(&shared->sem_l1_customer); // zakaznik vstupil do rady
                sem_wait(&shared->sem_l1_officer);  // zakaznik caka na uradnika
                break;

            case 2:
                sem_post(&shared->sem_l2_customer); // zakaznik vstupil do rady
                sem_wait(&shared->sem_l2_officer);  // zakaznik caka na uradnika
                break;

            case 3:
                sem_post(&shared->sem_l3_customer); // zakaznik vstupil do rady
                sem_wait(&shared->sem_l3_officer);  // zakaznik caka na uradnika
                break;
        }

        sem_wait(&shared->out_mutex);
        fprintf(shared->pFile, "%ld: Z %d: called by office worker\n", ++shared->line_num, id);
        fflush(shared->pFile);
        sem_post(&shared->out_mutex);

        sem_post(&shared->sem_cus_at_office); // zakaznik vstupuje do kancelarie

        // dekrementacia poctu neobsluzenych zakaznikov na prevadzke
        sem_wait(&shared->customer_mutex);
        --shared->customs_at_post;
        sem_post(&shared->customer_mutex);

        mySleep(10);

        sem_wait(&shared->out_mutex);
        fprintf(shared->pFile, "%ld: Z %d: going home\n", ++shared->line_num, id);
        fflush(shared->pFile);
        sem_post(&shared->out_mutex);

        exit(0);
    }
    else  // posta je zatvorena
    {
        sem_post(&shared->state_mutex);

        sem_wait(&shared->out_mutex);
        fprintf(shared->pFile, "%ld: Z %d: going home\n", ++shared->line_num, id);
        fflush(shared->pFile);
        sem_post(&shared->out_mutex);
        exit(0);
    }

    exit(0);
}

void officerProcess(int id, int TU, shared_t *shared)
{
    waitForStart(shared); // cakanie (implementovane pomocou semaforov) na spustenie vsetkych procesov

    sem_wait(&shared->out_mutex);
    fprintf(shared->pFile, "%ld: U %d: started\n", ++shared->line_num, id);
    fflush(shared->pFile);
    sem_post(&shared->out_mutex);

    while(shared->post_state == OPENED || shared->customs_at_post > 0)
    {
        sem_wait(&shared->line_mutex);
        if(sem_trywait(&shared->sem_l1_customer) == 0){ // niekto je v 1. rade

            sem_post(&shared->line_mutex); // povolenie vstupu do rad

            sem_post(&shared->sem_l1_officer); // zavolanie cloveka z rady 1
            sem_wait(&shared->sem_cus_at_office); // pockanie na vstup zakaznika do kanclu

            sem_wait(&shared->out_mutex);
            fprintf(shared->pFile, "%ld: U %d: serving a service of type 1\n", ++shared->line_num, id);
            fflush(shared->pFile);
            sem_post(&shared->out_mutex);

            mySleep(10);

            sem_wait(&shared->out_mutex);
            fprintf(shared->pFile, "%ld: U %d: service finished\n", ++shared->line_num, id);
            fflush(shared->pFile);
            sem_post(&shared->out_mutex);

        } else if(sem_trywait(&shared->sem_l2_customer) == 0){ // niekto je v 2. rade

            sem_post(&shared->line_mutex); // povolenie vstupu do rad

            sem_post(&shared->sem_l2_officer); // zavolanie cloveka z rady 2
            sem_wait(&shared->sem_cus_at_office); // pockanie na vstup zakaznika do kanclu

            sem_wait(&shared->out_mutex);
            fprintf(shared->pFile, "%ld: U %d: serving a service of type 2\n", ++shared->line_num, id);
            fflush(shared->pFile);
            sem_post(&shared->out_mutex);

            mySleep(10);

            sem_wait(&shared->out_mutex);
            fprintf(shared->pFile, "%ld: U %d: service finished\n", ++shared->line_num, id);
            fflush(shared->pFile);
            sem_post(&shared->out_mutex);

        } else if(sem_trywait(&shared->sem_l3_customer) == 0){ // niekto je v 3. rade
            
            sem_post(&shared->line_mutex); // povolenie vstupu do rad

            sem_post(&shared->sem_l3_officer); // zavolanie cloveka z rady 3
            sem_wait(&shared->sem_cus_at_office); // pockanie na vstup zakaznika do kanclu

            sem_wait(&shared->out_mutex);
            fprintf(shared->pFile, "%ld: U %d: serving a service of type 3\n", ++shared->line_num, id);
            fflush(shared->pFile);
            sem_post(&shared->out_mutex);

            mySleep(10);

            sem_wait(&shared->out_mutex);
            fprintf(shared->pFile, "%ld: U %d: service finished\n", ++shared->line_num, id);
            fflush(shared->pFile);
            sem_post(&shared->out_mutex);

        } else {
            sem_wait(&shared->out_mutex);
            fprintf(shared->pFile, "%ld: U %d: taking break\n", ++shared->line_num, id);
            fflush(shared->pFile);
            sem_post(&shared->out_mutex);

            sem_post(&shared->line_mutex); // povolenie vstupu do rad

            mySleep(TU);

            sem_wait(&shared->out_mutex);
            fprintf(shared->pFile, "%ld: U %d: break finished\n", ++shared->line_num, id);
            fflush(shared->pFile);
            sem_post(&shared->out_mutex);
        }
    }

    sem_wait(&shared->out_mutex);
    fprintf(shared->pFile, "%ld: U %d: going home\n", ++shared->line_num, id);
    fflush(shared->pFile);
    sem_post(&shared->out_mutex);

    exit(0);
}

int main(int argc, char *argv[])
{
    // nacitanie argumentov
    int NZ, NU, TZ, TU, F;
    parseArguments(argc, argv, &NZ, &NU, &TZ, &TU, &F);

    // deklaracia zdielanej pamate
    shared_t *shared = NULL;
    MMAP(shared);

    // namapovanie zdielanej pamate prebehlo neuspesne
    if (shared == MAP_FAILED)
    {
        fprintf(stderr, "Error: Shared memory initialization failed\n");
        exit(1);
    }

    // inicializacia zdielanej pamate
    sharedCtor(shared);

    pid_t pid;  // id procesu

    // vytvorenie procesov zakaznikov
    for (int i = 1; i <= NZ; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            customerProcess(i, TZ, shared);
        }
        else if (pid < 0) // fork bol neuspesny
        {
            fprintf(stderr, "Error: Customer child process creation failed\n");
            sharedDtor(shared);
            exit(1);
        }
    }

    // vytvoranie procesov uradnikov
    for (int i = 1; i <= NU; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            officerProcess(i, TU, shared);
        }
        else if (pid < 0) // fork bol neuspesny
        {
            fprintf(stderr, "Error: Officer child process creation failed\n");
            sharedDtor(shared);
            exit(1);
        }
    }

    sem_post(&shared->sem_start);   //spustenie detskych procesov

    closePostOffice(F, shared);

    while(wait(NULL) > 0);      // cakanie na ukoncenie detskych procesov

    sharedDtor(shared);
    return 0;
}
