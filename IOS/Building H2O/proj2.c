/*
 *   IOS Projekt 2 - Building H2O
 *   Autor: Aurel Strigáč (xstrig00)
 *   čiastočne inšpirované knihou The Little Book of Semaphores (Allen B. Downey)
 *   Syntax pre spustenie: ./proj2 NO NH TI TB
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

#define CORRECT_ARG_CNT 5  // správny počet argumentov spúšťacieho príkazu
#define MIN_ARG_VALUE 0    // minimálna hodnotu argumentu
#define MAX_ARG_VALUE 1000 // maximálna hodnota argumentu času
#define N 3                // počet prvkov potrebných na bariére

// makrá pre namapovanie a uvoľnenie zdieľanej pamäte
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
    // premenné
    int id_mol;         // číslo molekuly
    int oxygen;         // počet kyslíkov momentálne v rade
    int hydrogen;       // počet vodíkov momentálne v rade
    int oxy_leftover;   // počet zvyšných atómov kyslíka ktoré ešte nie sú v rade
    int count;          // počet atómov čakajúcich na bariére
    int hydro_leftover; // počet zvyšných atómov vodíka ktoré ešte nie sú v rade
    int hydro_in_queue; // počet vodíkov pripravených v rade
    int oxy_in_queue;   // počet kyslíkov pripravených v rade
    int exiting;        // číslo určujúce či sa jedná o stav kedy majú vodíky vypisovať nedostatok H alebo O,
                        // inými slovami "flag" pre ukončenie procesov vodíkov
    size_t line_num;    // číslo riadku

    // semafóry
    sem_t sem_mutex;      // semafór kontrolujúci spúšťanie procesov vodíka a kyslíka
    sem_t sem_oxyQ;       // semafór ktorý púšťa atómy kyslíka z dary
    sem_t sem_hydroQ;     // semafór ktorý púšťa atómy vodíka z rady
    sem_t sem_mutex2;     // semafór kontrolujúci vstup na bariéru
    sem_t sem_turnstile;  // semafór je súčasťou bariéry
    sem_t sem_turnstile2; // semafór je súčasťou bariéry
    sem_t sem_out;        // semafór kontrolujúci písanie do projľ.out

    // súbor
    FILE *pFile; // súbor slúžiaci na načítanie súboru proj2.out

} shared_t;

// funkcia pre uvoľnenie zdieľanej pamäte
void destruct(shared_t *shared)
{
    sem_destroy(&(shared->sem_mutex));
    sem_destroy(&(shared->sem_oxyQ));
    sem_destroy(&(shared->sem_hydroQ));
    sem_destroy(&(shared->sem_mutex2));
    sem_destroy(&(shared->sem_turnstile));
    sem_destroy(&(shared->sem_turnstile2));
    sem_destroy(&(shared->sem_out));
    fclose(shared->pFile);
}

// funkcia pre inicializovanie zdieľanej pamäte
void init(shared_t *shared)
{
    if ((shared->pFile = fopen("proj2.out", "w")) == NULL) // kontrola že sa súbor správne otvoril
    {
        fprintf(stderr, "[ ERROR ] proj2.out couldn't be opened\n");
        UNMAP(shared);
        exit(1);
    }
    setbuf(shared->pFile, NULL);

    // inicializácia premenných
    shared->hydrogen = 0;
    shared->oxygen = 0;
    shared->id_mol = 0;
    shared->count = 0;
    shared->line_num = 0;
    shared->oxy_leftover = 0;
    shared->hydro_leftover = 0;
    shared->hydro_in_queue = 0;
    shared->oxy_in_queue = 0;
    shared->exiting = 0;

    // inicializácia semafórov, sem_init vracia -1 ak sa nepodarí
    if (
        (sem_init(&(shared->sem_mutex), 1, 1) == -1) ||
        (sem_init(&(shared->sem_hydroQ), 1, 0) == -1) ||
        (sem_init(&(shared->sem_oxyQ), 1, 0) == -1) ||
        (sem_init(&(shared->sem_mutex2), 1, 1) == -1) ||
        (sem_init(&(shared->sem_turnstile), 1, 0) == -1) ||
        (sem_init(&(shared->sem_turnstile2), 1, 1) == -1) ||
        (sem_init(&(shared->sem_out), 1, 1) == -1))
    {
        fprintf(stderr, "[ ERROR ] Unsuccesful semaphore initialization\n");
        fclose(shared->pFile);
        UNMAP(shared);
        exit(1);
    }
}

// funkcia pre kontolu správnosti argumentov
// i je poriadie argumentu, čiže prvé 2 sú počty prvkov a ostatné sú časy
void checkArgValue(char *argv, int i)
{
    int num = atoi(argv);

    if (i < 3) // argument je element
    {
        // kontorla hodnôt prvkov
        if (num <= MIN_ARG_VALUE) // ak je argument menší ako 0
        {
            fprintf(stderr, "[ ERROR ] Incorrect element count value\n");
            exit(1);
        }
    }
    else // argument je čas
    {
        // kontrola hodnôt časov
        if (num < MIN_ARG_VALUE || num > MAX_ARG_VALUE) // ak je argument menší ako 0 alebo väčší ako 1000
        {
            fprintf(stderr, "[ ERROR ] Incorrect time value\n");
            exit(1);
        }
    }
}

// funkcia pre vyvolanie čakania procesu
void my_sleep(unsigned max_time)
{
    usleep((rand() % (max_time + 1)) * 1000);
}

// funkcia pre proces kyslíku
// Parametre:
// id - číslo atómu
// TI - maximálny čas zaradenia atómu do rady
// TB - maximálny čas vytvorenia molekuly
// shared - zdieľaná pamäť
void oxygen(int id, int TI, int TB, shared_t *shared)
{
    sem_wait(&shared->sem_mutex); // čakáme dokým tento proces dostane mutex

    shared->id_mol++; // navýšim číslo molekuly ktorú budem vytvárať o 1

    // výpis že atóm kyslíku bol spustený
    sem_wait(&shared->sem_out);
    fprintf(shared->pFile, "%ld: O %d: started\n", ++shared->line_num, id);
    fflush(shared->pFile);
    sem_post(&shared->sem_out);

    my_sleep(TI); // čakanie pred tým ako atóm môže vstúpiť do rady

    // kód pre proces kyslíku - The Little Book Of Semaphores strana č. 147
    shared->oxygen++;
    if (shared->hydrogen >= 2)
    {
        sem_post(&shared->sem_hydroQ);
        sem_post(&shared->sem_hydroQ);
        shared->hydrogen -= 2;
        sem_post(&shared->sem_oxyQ);
        shared->oxygen--;
    }
    else
    {
        sem_post(&shared->sem_mutex);
    }

    // výpis že atóm kyslíku vstúpil do rady
    sem_wait(&shared->sem_out);
    fprintf(shared->pFile, "%ld: O %d: going to queue\n", ++shared->line_num, id);
    fflush(shared->pFile);
    sem_post(&shared->sem_out);

    shared->oxy_leftover--; // počet kyslíkov ktoré ešte nie sú v rade zmenším o 1
    shared->oxy_in_queue++; // počet kyslíkov už v rade navýšim o 1

    // ak nie je dosť vodíkov na vytvorenie molekuly
    if (shared->hydro_in_queue < 2 && shared->hydro_leftover == 0)
    {
        // výpis že kyslíku chýbajú vodíky
        sem_wait(&shared->sem_out);
        fprintf(shared->pFile, "%ld: O %d: not enough H\n", ++shared->line_num, id);
        fflush(shared->pFile);
        sem_post(&shared->sem_out);

        shared->oxy_in_queue--;                                      // počet kyslíkov v rade znížime o 1
        if (shared->oxy_in_queue == 0 && shared->hydro_in_queue > 0) // ak v rade vodíkov zostali ešte atómy
        {
            shared->exiting = 1;           // nastavíme "flag" ukončenia vodíkov na 1
            sem_post(&shared->sem_hydroQ); // pustíme 1 vodík z rady
        }
        exit(0);
    }

    sem_wait(&shared->sem_oxyQ); // čakáme na vypustenie kyslíku z rady

    // výpis že kyslík je v procese vytvárania molekuly
    sem_wait(&shared->sem_out);
    fprintf(shared->pFile, "%ld: O %d: creating molecule %d\n", ++shared->line_num, id, shared->id_mol);
    fflush(shared->pFile);
    sem_post(&shared->sem_out);

    // znovupoužiteľná bariéra - The Little Book of Semaphores strana č. 41
    sem_wait(&shared->sem_mutex2);
    shared->count++;
    if (shared->count == N)
    {
        sem_wait(&shared->sem_turnstile2);
        sem_post(&shared->sem_turnstile);
    }
    sem_post(&shared->sem_mutex2);

    sem_wait(&shared->sem_turnstile);
    sem_post(&shared->sem_turnstile);

    my_sleep(TB); // čakaním imitujeme vytvorenie molekuly

    // výpis že kyslíku sa podarilo vytvoriť molekulu
    sem_wait(&shared->sem_out);
    fprintf(shared->pFile, "%ld: O %d: molecule %d created\n", ++shared->line_num, id, shared->id_mol);
    fflush(shared->pFile);
    sem_post(&shared->sem_out);

    sem_wait(&shared->sem_mutex2);
    shared->count--;
    if (shared->count == 0)
    {
        sem_wait(&shared->sem_turnstile);
        sem_post(&shared->sem_turnstile2);
    }
    sem_post(&shared->sem_mutex2);

    sem_wait(&shared->sem_turnstile2);
    sem_post(&shared->sem_turnstile2);

    sem_post(&shared->sem_mutex); // predávame mutex inému procesu

    shared->oxy_in_queue--;                                                                   // dekrementujeme počet kyslíkov v rade o 1
    if (shared->oxy_in_queue == 0 && shared->hydro_in_queue > 0 && shared->oxy_leftover == 0) // ak v rade s vodíkmi ešte niečo ostalo
    {
        shared->exiting = 1;           // nastavíme "flag" ukončenia vodíkov na 1
        sem_post(&shared->sem_hydroQ); // pustíme 1 vodík z rady
    }

    exit(0);
}

// funkcia pre proces vodíku
// Parametre:
// id - číslo atómu
// TI - maximálny čas zaradenia atómu do rady
// TB - maximálny čas vytvorenia molekuly
// shared - zdieľaná pamäť
void hydrogen(int id, int TI, shared_t *shared)
{
    sem_wait(&shared->sem_mutex); // čakáme dokým tento proces dostane mutex

    // výpis že atóm vodíku bol spustený
    sem_wait(&shared->sem_out);
    fprintf(shared->pFile, "%ld: H %d: started\n", ++shared->line_num, id);
    fflush(shared->pFile);
    sem_post(&shared->sem_out);

    my_sleep(TI); // čakanie pred tým ako atóm môže vstúpiť do rady

    // kód pre proces vodíku - The Little Book Of Semaphores strana č. 147
    shared->hydrogen++;
    if (shared->hydrogen >= 2 && shared->oxygen >= 1)
    {
        sem_post(&shared->sem_hydroQ);
        sem_post(&shared->sem_hydroQ);
        shared->hydrogen -= 2;
        sem_post(&shared->sem_oxyQ);
        shared->oxygen--;
    }
    else
    {
        sem_post(&shared->sem_mutex);
    }

    // výpis že atóm vodíku vstúpil do rady
    sem_wait(&shared->sem_out);
    fprintf(shared->pFile, "%ld: H %d: going to queue\n", ++shared->line_num, id);
    fflush(shared->pFile);
    sem_post(&shared->sem_out);

    shared->hydro_leftover--; // počet vodíkov ktoré v rade ešte nie sú dekrementujeme o 1
    shared->hydro_in_queue++; // počet vodíkov v rade navýšime o 1

    sem_wait(&shared->sem_hydroQ); // čakáme na vypustenie vodíku z rady

    if (shared->exiting) // ak je "flag" pre ukončenie procesov vodíkov nastavený na 1
    {
        // výpis že vodík nemá dostatok kyslíkov alebo vodíkov
        sem_wait(&shared->sem_out);
        fprintf(shared->pFile, "%ld: H %d: not enough O or H\n", ++shared->line_num, id);
        fflush(shared->pFile);
        sem_post(&shared->sem_out);

        shared->hydro_in_queue--;       // počet vodíkov v rade dekrementujem o 1
        if (shared->hydro_in_queue > 0) // ak sú ešte nejaké vodíky v rade
        {
            sem_post(&shared->sem_hydroQ); // vypustím ďalší vodík z rady
        }
        exit(0);
    }

    // výpis že vodík je v procese vytvárania molekuly
    sem_wait(&shared->sem_out);
    fprintf(shared->pFile, "%ld: H %d: creating molecule %d\n", ++shared->line_num, id, shared->id_mol);
    fflush(shared->pFile);
    sem_post(&shared->sem_out);

    // znovupoužiteľná bariéra - The Little Book of Semaphores strana č. 41
    sem_wait(&shared->sem_mutex2);
    shared->count++;
    if (shared->count == N)
    {
        sem_wait(&shared->sem_turnstile2);
        sem_post(&shared->sem_turnstile);
    }
    sem_post(&shared->sem_mutex2);

    sem_wait(&shared->sem_turnstile);
    sem_post(&shared->sem_turnstile);

    // výpis že vodíku sa podarilo vytvoriť molekulu
    sem_wait(&shared->sem_out);
    fprintf(shared->pFile, "%ld: H %d: molecule %d created\n", ++shared->line_num, id, shared->id_mol);
    fflush(shared->pFile);
    sem_post(&shared->sem_out);

    sem_wait(&shared->sem_mutex2);
    shared->count--;
    if (shared->count == 0)
    {
        sem_wait(&shared->sem_turnstile);
        sem_post(&shared->sem_turnstile2);
    }
    sem_post(&shared->sem_mutex2);

    sem_wait(&shared->sem_turnstile2);
    sem_post(&shared->sem_turnstile2);

    shared->hydro_in_queue--; // počet vodíkov v rade dekrementujem o 1
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc != CORRECT_ARG_CNT) // v prípade že syntax spúšťania nie je správny
    {
        fprintf(stderr, "[ ERROR ] Incorrect starting command format\n Use /proj2 NO NH TI TB\n");
        exit(1);
    }

    // kontrola argumentov
    for (int i = 1; i < argc; i++)
    {
        // kontrola že sú argumenty čísla
        if (isdigit(*argv[i]))
        {
            // kontorla že má argument správnu hodnotu
            checkArgValue(argv[i], i);
        }
        else
        {
            fprintf(stderr, "[ ERROR ] Incorrect argument format\n");
            exit(1);
        }
    }

    // načítavanie argumentov
    int NO = atoi(argv[1]); // prvý argument je počet kyslíkov
    int NH = atoi(argv[2]); // druhý argument je počet vodíkov
    int TI = atoi(argv[3]); // tretí argument je maximálny čas na zaradenie do rady
    int TB = atoi(argv[4]); // štvrtý argument je maximálny čas na vytvorenie molekuly

    // inicializácia a deklarácia zdielanej pamäte
    shared_t *shared = NULL;
    MMAP(shared);

    if (shared == MAP_FAILED) // ak sa namapovanie zdieľanej pamäte nepodarilo
    {
        fprintf(stderr, "[ ERROR ] Could not initialize shared memory\n");
        exit(1);
    }

    init(shared); // inicializácia zdieľanej pamäte

    shared->oxy_leftover = NO;   // momentálne všetky kyslíky nie sú v rade
    shared->hydro_leftover = NH; // momentálne všetky vodíky nie sú v rade

    pid_t pid;                    // id procesu
    for (int i = 1; i <= NH; i++) // všetkým vodíkom vytvorím procesy
    {
        pid = fork();
        if (pid == 0) // ak sa vytvorenie dieťaťa podarilo
        {
            hydrogen(i, TI, shared); // v detskom procese spúšťam funkciu vodíku
            exit(0);                 // ukončujem detský proces
        }
        else if (pid < 0) // v prípade že sa fork() nepodaril
        {
            fprintf(stderr, "[ ERROR ] Creating child process went wrong\n");
            destruct(shared);
            UNMAP(shared); // odmapujem zdieľanú pamäť
            exit(1);
        }
    }

    for (int i = 1; i <= NO; i++) // všetkým kyslíkom vytvorím procesy
    {
        pid = fork();
        if (pid == 0) // ak sa vytvorenie dieťaťa podarilo
        {
            oxygen(i, TI, TB, shared); // v detskom procese spúšťam funkciu kyslíku
            exit(0);                   // ukončujem detský proces
        }
        else if (pid < 0) // v prípade že sa fork() nepodaril
        {
            fprintf(stderr, "[ ERROR ] Creating child process went wrong\n");
            destruct(shared);
            UNMAP(shared); // odmapujem zdieľanú pamäť
            exit(1);
        }
    }

    destruct(shared);
    UNMAP(shared); // odmapujem zdieľanú pamäť
    return 0;
}
