#include <stdio.h>
#include <stdlib.h>
#include <simlib.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <ctype.h>
#include <vector>
#include <getopt.h>
#include <string.h>
#include <iomanip>

// Definicia liniek
Facility Miesic("Miesic cesta");
Facility Delic("Delic cesta");
Facility Tvarovac("Tvarovac cesta");
Facility Pec("Pec");
Facility KKvality("Kontrola kvality chleba");
Facility Mycka("Umyvacka prepraviek");
Facility KCistoty("Kontrola cistoty prepravky");

// Definicie casovych usekov
const double DAY = 1440;
const double WEEK = 10080;
const double MONTH = 43200;
const double YEAR = 525600;

// Definicia dlzky casu obsluhy jednotlivych liniek
double TMiesic = 10;
double TDelic = 1/3;
double TTvarovac = 1/3;
double TKysnutie = 50;
double TPec = 25;
double TChladnutie = 15;
double TKKvality = 2/3; 
double TMycka = 0.5;
double TKCistoty = 0.05;

// Ciastocne vyrobky
int chleby = 0, prepravky = 0;

double neededPercentage = 0.4; // percento prepraviek venovane nasej linke
double objemMiesica = 200000; // objem miesica v g

// Pocitadla
int transakcie = 0;
int transakcieMAX = INT32_MAX;
int umyvacieCykly = 0, vyradeneChleby = 0, celkoveChleby = 0, vyradeneCesto = 0;
int chlebyOcakavajuceChladnutie = 0, chlebyOcakavajucePec = 0;
int cesto = 0;


// Casy vystupov transakcii
std::vector <double> transactionExitTimes; 


//--------------------------------- GENERATORY SMIEN ---------------------------------

class GenSmienMiesic : public Process {
public:
    double dlzkaSmeny;

    GenSmienMiesic(double interval) {
        dlzkaSmeny = interval;
    }

    void Behavior() {
        Seize(Miesic, 1);
        Wait(DAY - dlzkaSmeny);
        Release(Miesic);

        (new GenSmienMiesic(dlzkaSmeny))->Activate(Time + dlzkaSmeny);
    }
};

class GenSmienDelic : public Process {
public:
    double dlzkaSmeny;

    GenSmienDelic(double interval) {
        dlzkaSmeny = interval;
    }

    void Behavior() {
        Seize(Delic, 1);
        Wait(DAY - dlzkaSmeny);
        Release(Delic);

        (new GenSmienDelic(dlzkaSmeny))->Activate(Time + dlzkaSmeny);
    }
};

class GenSmienTvarovac : public Process {
public:
    double dlzkaSmeny;

    GenSmienTvarovac(double interval) {
        dlzkaSmeny = interval;
    }

    void Behavior() {
        Seize(Tvarovac, 1);
        Wait(DAY - dlzkaSmeny);
        Release(Tvarovac);

        (new GenSmienTvarovac(dlzkaSmeny))->Activate(Time + dlzkaSmeny);
    }
};

class GenSmienPec : public Process {
public:
    double dlzkaSmeny;

    GenSmienPec(double interval) {
        dlzkaSmeny = interval;
    }

    void Behavior() {
        Seize(Pec, 1);
        Wait(DAY - dlzkaSmeny);
        Release(Pec);

        (new GenSmienPec(dlzkaSmeny))->Activate(Time + dlzkaSmeny);
    }
};

class GenSmienKKvality : public Process {
public:
    double dlzkaSmeny;

    GenSmienKKvality(double interval) {
        dlzkaSmeny = interval;
    }

    void Behavior() {
        Seize(KKvality, 1);
        Wait(DAY - dlzkaSmeny);
        Release(KKvality);

        (new GenSmienKKvality(dlzkaSmeny))->Activate(Time + dlzkaSmeny);
    }
};

class GenSmienMycka : public Process {
public:
    double dlzkaSmeny;

    GenSmienMycka(double interval) {
        dlzkaSmeny = interval;
    }

    void Behavior() {
        Seize(Mycka, 1);
        Wait(DAY - dlzkaSmeny);
        Release(Mycka);

        (new GenSmienMycka(dlzkaSmeny))->Activate(Time + dlzkaSmeny);
    }
};

class GenSmienKCistoty : public Process {
public:
    double dlzkaSmeny;

    GenSmienKCistoty(double interval) {
        dlzkaSmeny = interval;
    }

    void Behavior() {
        Seize(KCistoty, 1);
        Wait(DAY - dlzkaSmeny);
        Release(KCistoty);

        (new GenSmienKCistoty(dlzkaSmeny))->Activate(Time + dlzkaSmeny);
    }
};

//--------------------------------- VYROBNY PROCES ---------------------------------

// Proces zistujuci dostupnost potrebnych predmetov
class Nakladanie : public Process {
    void Behavior() {

        if (prepravky >= 1 && chleby >= 7) {
            prepravky--;chleby-=7;

            transakcie++;
            transactionExitTimes.push_back(Time);

            if (transakcie >= transakcieMAX) {
                Stop();
            }
        }
    }
};

class KontrolaCistotyPrepraviek : public Process {
    void Behavior() {

        Seize(KCistoty);
        Wait(TKCistoty);
        Release(KCistoty);

        double rnd = Random();

        if(rnd < 0.9) {
            if(rnd < neededPercentage) {
                prepravky++;
                (new Nakladanie)->Activate();
            }
        }
    }
};

class UmyvaniePrepraviek : public Process {
    void Behavior() {

        Seize(Mycka);
        umyvacieCykly++;
        Wait(TMycka);
        Release(Mycka);

        (new UmyvaniePrepraviek)->Activate();
        (new KontrolaCistotyPrepraviek)->Activate();
    }
};

class KontrolaKvality : public Process {
    void Behavior() {

        Seize(KKvality);
        Wait(TKKvality);

        double rnd = Random();

        if(rnd < 0.98) {
            chleby++;celkoveChleby++;
            (new Nakladanie)->Activate();
        } else {
            vyradeneChleby++;
        }

        Release(KKvality);
    }
};

class Chladnutie : public Process {
    void Behavior() {

        double rnd = Random();

        if(rnd < 0.88) {
            Wait(TChladnutie);
            (new KontrolaKvality)->Activate();
        } else {
            vyradeneChleby++;
        }

        if(chlebyOcakavajuceChladnutie > 0){
            (new Chladnutie)->Activate();
        }
    }
};

class Pecenie : public Process {
    void Behavior() {

        Seize(Pec);
        Wait(TPec);
        Release(Pec);

        chlebyOcakavajuceChladnutie += 800;
        while(chlebyOcakavajuceChladnutie > 0) {
            chlebyOcakavajuceChladnutie--;
            (new Chladnutie)->Activate();
        }
    }
};

class Kysnutie : public Process {
    void Behavior() {

        Wait(TKysnutie);

        chlebyOcakavajucePec++;

        if (chlebyOcakavajucePec >= 800) {
            chlebyOcakavajucePec -= 800;
            (new Pecenie)->Activate();
        }
    }
};

class Tvarovanie : public Process {
    void Behavior() {

        Seize(Tvarovac);
        Wait(TTvarovac);
        Release(Tvarovac);

        (new Kysnutie)->Activate();
    }
};

class Delenie : public Process {
    void Behavior() {

        Seize(Delic);
        cesto-=575;
        Wait(TDelic);
        Release(Delic);

        (new Tvarovanie)->Activate();

        if(cesto >= 575) {
            (new Delenie)->Activate();
        } else {
            vyradeneCesto+=cesto;
            cesto = 0;
        }
    }
};

class Miesenie : public Process {
    void Behavior() {

        Seize(Miesic);
        Wait(TMiesic);
        Release(Miesic);

        cesto+=objemMiesica;

        (new Delenie)->Activate();
        (new Miesenie)->Activate();
    }
};

//--------------------------------- FINALNY VYPIS ---------------------------------

// Tlacenie plnohodnotnych statistyk
void printStats(int fullOutput) {
        if(fullOutput) {
            printf("-------------------- Vyrobna linka chlebu -----------------------\n\n");
            Miesic.Output();
            Delic.Output();
            Tvarovac.Output();
            Pec.Output();
            KKvality.Output();
            printf("\n\n");

            printf("--------------- Oplachovacia linka prepraviek ------------------\n\n");
            Mycka.Output();
            KCistoty.Output();
        }

        printf("Konecny pocet vyrobenych chlebov: %d\n", celkoveChleby);
        printf("Konecny pocet vykonanych transakcii: %d\n", transakcie);

}


//--------------------------------- SPRACOVANIE ARGUMENTOV ---------------------------------

void printHelp() {
    std::cout << "POUZITIE:\n";
    std::cout << "-h/help                                     |        Vytlacenie tejto napovedy\n";
    std::cout << "(-t/-time) [dni]                            |        Dlzka behu simulacie, implicitne 1\n";
    std::cout << "-o                                          |        Plny vypis informacii o vsetkych linkach, implicitne 0\n";
    std::cout << "-[linka/pismeno] [dlzka smeny v minutach]   |        Nastavenie dlzky smeny pre jednotlive linky, menovite:\n";
    std::cout << "                                            |        a/miesic, b/delic, c/tvarovac, d/pec, e/kkvality, f/mycka, g/kcistoty\n";
    std::cout << "-p [percentage]                             |        Percento umytych prepraviek urcene pre nasu linku, implicitne 0.4 (musi patrit do <0,1>) \n";
    std::cout << "-v [kg]                                     |        Objem miesica v kg, implicitne 200\n";
}

void getArgs(int argc, char *argv[], double *smenaMiesic, double* smenaDelic, double* smenaTvarovac, double* smenaPec,\
            double* smenaKKvality, double* smenaMycka, double* smenaKCistoty,
            bool * fullOutput, int* timespan, double* percentage, double* objem)
{
    if (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "-help"))) {
        printHelp();
        exit(0);
    }

    int opt_index = 0, c;
    double val;

    static struct option long_options[] =
    {
        {"", no_argument, NULL, '0'},
        {"miesic", required_argument, NULL, 'a'},
        {"delic", required_argument, NULL, 'b'},
        {"tvarovac", required_argument, NULL, 'c'},
        {"pec", required_argument, NULL, 'd'},
        {"kkvality", required_argument, NULL, 'e'},
        {"mycka", required_argument, NULL, 'f'},
        {"kcistoty", required_argument, NULL, 'g'},
        {"time", required_argument, NULL, 't'},
        {0, 0, 0, 0}
    };

    while (1) {
        c = getopt_long_only(argc,argv, "a:b:c:d:e:f:g:t:op:v:", long_options, &opt_index);

        if(c == -1) //end of args
            break;

        // ziskanie a osetrenie argumentu
        if(optarg != NULL)
        {
            std::string strval(optarg); //argument value char * -> string

            try
            {
                val = std::stod(strval);//string -> double hodnota
            }
            catch(std::exception &e)
            {
                std::cerr << "Zly argument:" << optarg << "\n";
                exit(1);
            }

            if((c != 't' && (val <= 0 || val > 1440)) || (c == 'p' && (val > 1 || val < 0)))
            {
                std::cerr << "Nevhodna hodnota argumentu: " << optarg << "\n";
                exit(1);
            }
        }

        switch (c)
        {
        case '0':
            break;

        case 'a':
            std::cout << "Dlzka smeny miesica nastavena na: " << val << " minut\n";;
            *smenaMiesic = val;
            break;

        case 'b':
            std::cout << "Dlzka smeny delica nastavena na: " << val << " minut\n";;
            *smenaDelic = val;
            break;

        case 'c':
            std::cout << "Dlzka smeny tvarovaca nastavena na: " << val << " minut\n";;
            *smenaTvarovac = val;
            break;

        case 'd':
            std::cout << "Dlzka smeny pece nastavena na: " << val << " minut\n";;
            *smenaPec = val;
            break;

        case 'e':
            std::cout << "Dlzka smeny kontroly kvality nastavena na: " << val << " minut\n";;
            *smenaKKvality = val;
            break;

        case 'f':
            std::cout << "Dlzka smeny umyvacky nastavena na: " << val << " minut\n";;
            *smenaMycka = val;
            break;

        case 'g':
            std::cout << "Dlzka smeny kontroly cistoty nastavena na: " << val << " minut\n";
            *smenaKCistoty = val;
            break;

        case 'o':
            std::cout << "Nastaveny cely vypis\n";
            *fullOutput = true;
            break;
        
        case 't':
            std::cout << "Dlzka simulacie nastavena na: " << val << " dni\n";
            *timespan = val * 1440;
            break;

        case 'p':
            std::cout << "Percento prepraviek nastavene na: " << val * 100 << "%\n";
            *percentage = val;
            break;
        
        case 'v':
            val = val * 1000;
            std::cout << "Objem miesica nastaveny na: " << val << " g\n";
            *objem = val;
            break;

        default:
            exit(1);
        }

    }
    std::cout << "\n";
}


int main(int argc, char* argv[]) {

    // Casoveho rozhrania simulacie
    int timespan = DAY;

    bool fullOutput = 0;

    // Implicitne nastavenie smien na 12 hodin
    double smenaMiesic = 1200;
    double smenaTvarovac = 1200;
    double smenaDelic = 1200;
    double smenaPec = 1200;
    double smenaKKvality = 1200;
    double smenaMycka = 1200;
    double smenaKCistoty = 1200;

    // Spracovanie argumentov
    if(argc > 1) {
        getArgs(argc, argv, &smenaMiesic, &smenaDelic, &smenaTvarovac, &smenaPec, &smenaKKvality, &smenaMycka, &smenaKCistoty,
           &fullOutput, &timespan, &neededPercentage, &objemMiesica);
    }

    Init(0, timespan);

    // Spustenie vyroby
    (new Miesenie)->Activate();
    (new UmyvaniePrepraviek)->Activate();

    // Spustenie generatorov smien pre jednotlive linky
     if(smenaMiesic != 1440)
        (new GenSmienMiesic(smenaMiesic))->Activate();
    if(smenaDelic != 1440)
        (new GenSmienDelic(smenaDelic))->Activate();
    if(smenaTvarovac != 1440)
        (new GenSmienTvarovac(smenaTvarovac))->Activate();
    if(smenaPec != 1440)
        (new GenSmienPec(smenaPec))->Activate();
    if(smenaKKvality != 1440)
        (new GenSmienKKvality(smenaKKvality))->Activate();
    if(smenaMycka != 1440)
        (new GenSmienMycka(smenaMycka))->Activate();
    if(smenaKCistoty!= 1440)
        (new GenSmienKCistoty(smenaKCistoty))->Activate();

    // Spustenie simulacie
    Run();

    printStats(fullOutput);

    return EXIT_SUCCESS;
}
