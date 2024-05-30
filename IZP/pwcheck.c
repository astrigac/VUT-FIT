//created by Aurel Strigac xstrig00

#include <stdio.h>
#include <stdlib.h>
#define ALL_AVAILABLE_CHARS 95
#define MAX_LENGTH 101

//funkcia pre prvy level
int first_condition(char buffer[], int length)
{

    int small = 0;
    int big = 0;

    for (int i = 0; i < length; i++)
    {
        //hladam velke pismeno
        if (buffer[i] >= 'A' && buffer[i] <= 'Z')
        {
            big = 1;
        }

        //hladam male pismeno
        if (buffer[i] >= 'a' && buffer[i] <= 'z')
        {
            small = 1;
        }
    }

    //splnene ak som nasiel aj male a aj velke
    if (big == 1 && small == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//funkcia pre druhy level
int second_condition(char buffer[], int length, int parameter)
{

    int big = 0;
    int small = 0;
    int num = 0;
    int special = 0;
    int special_count = length;

    //zabezpecenie aby param nebol vacsi ako 4
    if (parameter > 4)
    {
        parameter = 4;
    }

    //cyklus pre hladanie pozadovanych skupin
    for (int i = 0; i < length; i++)
    {
        if (buffer[i] >= 'A' && buffer[i] <= 'Z')
        {
            big = 1;
            special_count--;
        }

        if (buffer[i] >= 'a' && buffer[i] <= 'z')
        {
            small = 1;
            special_count--;
        }
        if (buffer[i] >= '0' && buffer[i] <= '9')
        {
            num = 1;
            special_count--;
        }
    }

    //special<count mi vyjde vacsi ako 0 ak sa v hesle nachadza znak ktory nepatri ani do jednej z prvych 3 skupin
    if (special_count > 0)
    {
        special = 1;
    }

    //overenie ci heslo splna podmienku
    if (special + num + big + small >= parameter)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//funkcia pre treti level
int third_condition(char buffer[], int length, int parameter)
{
    int outcome = 1;

    for (int i = 0; i < length; i++)
    {

        //opatrenie proti preteceniu
        if (i + parameter - 1 > length)
        {

            return outcome;
        }
        else
        {

            if (buffer[i] == buffer[i + 1])
            { //detekovana je prva zhoda, ideme zistit ci splna parameter

                char temp = buffer[i]; //momentalne pismeno nastavim ako hladane pismeno
                int current_streak = 0;

                for (int j = i; j < i + parameter; j++)
                { //zacinam od momentalneho pismena a preskumavam
                    if (buffer[j] == temp)
                    { //dalsie pismena do vzdialenosti parametru
                        current_streak++;
                    }
                }

                if (current_streak == parameter)
                {                //ak sa nazbieralo tolko streakov aby to splnilo parameter
                    outcome = 0; //mozeme funkciu rovno vypnut
                    return outcome;
                }
            }
        }
    }
    return outcome;
}

//funkcia pre stvrty level
int fourth_condition(char buffer[], int length, int parameter)
{

    for (int i = 0; i < length; i++)
    {

        char str[parameter]; //definujem prvy string

        //opatrenie proti preteceniu
        if (i + 2 * parameter > length)
        {

            return 1;
        }

        for (int j = 0; j < parameter; j++)
        { //zadavam prvy string
            str[j] = buffer[i + j];
        }

        for (int j = i + parameter; j <= length - parameter; j++)
        {

            char temp[parameter]; //definujem druhy string

            for (int k = 0; k < parameter; k++)
            { //zadavam hodnotu druhemu stringu
                temp[k] = buffer[j + k];
            }

            int same_c = 0;

            //kontrolujem ci sa stringy rovnaju
            for (int k = 0; k < parameter; k++)
            {
                if (str[k] == temp[k])
                {
                    same_c++;
                }
            }

            //ak sa rovnaju, ukoncujem funkciu a heslo nesplna level
            if (same_c == parameter)
            {
                return 0;
            }
        }
    }
    return 1;
}

//funkcia na najdenie noveho minima
int min(int min_length, int length)
{
    if (length < min_length)
    {
        min_length = length;
    }
    return min_length;
}

//vypocet dlzky momentalneho buffera
int lengthf(char buffer[])
{

    int temp = 0;

    while (buffer[temp] != '\n')
    { //vypocet na dlzku hesla
        temp++;
    }

    return temp;
}

//funkcia pre zistenie ci sa momentalne pismeno zo zadaneho hesla uz nachadza v doteraz pouzitych charoch
int letter_check(char znaky[], char buffer[], int i, int znaky_n)
{
    int found = 0;

    for (int j = 0; j <= znaky_n; j++)
    {
        if (znaky[j] == buffer[i])
        {
            found = 1;
        }
    }

    return found;
}

//vlastna funkcia pre zistenie dlzky pola s pouzitymi unikatnymi znakmi
int length_of_znaky(char znaky[])
{
    int temp = 0;

    while (znaky[temp] != '\0')
    { //vypocet na dlzku hesla
        temp++;
    }

    return temp;
}

//funkcia ci argv[3] je rovny stats, pripadne vypise chybu pri pravopisnej chybe
int includes_stats(const char *stats)
{
    int found = 0;
    int chars_shared = 0;
    char temp[7] = "--stats";

    for (int i = 0; i < 7; i++)
    {
        if (temp[i] == stats[i])
        {
            chars_shared++;
            if (chars_shared == 7)
            {
                found = 1;
            }
        }
    }
    if (chars_shared > 1 && chars_shared < 7)
    {
        fprintf(stderr, "Chyba pri zadavani pojmu stats\n");
    }

    return found;
}

int main(int argc, const char *argv[])
{

    int parameter = atoi(argv[2]); // deklaracia pevne zadaneho parametru
    int level = atoi(argv[1]);     // deklaracia pevne zadaneho levelu
    const char *stats;

    if (argc > 3)
    {
        stats = argv[3];
    }

    //opatrenie proti zle zadanemu parametru
    if (parameter < 1)
    {
        fprintf(stderr, "Zle zadany parameter\n");
        return 1;
    }

    //opatrenie proti zle zadanemu levelu
    if (level < 1 || level > 4)
    {
        fprintf(stderr, "Zle zadany level\n");
        return 1;
    }

    int min_length;

    int overall_length = 0;
    int word_count = 0;

    char buffer[MAX_LENGTH];

    char znaky[ALL_AVAILABLE_CHARS];
    int znaky_n = 0;

    //postupne nacitavanie hesiel po riadkoch
    while (fgets(buffer, 100, stdin) != NULL)
    {

        int length = lengthf(buffer);
        overall_length += length;
        word_count++;

        //specialny pripad ak zadane heslo je prve v poradi
        if (word_count == 1)
        {
            znaky[znaky_n] = buffer[0];

            for (int i = 0; i < length; i++)
            {
                if (letter_check(znaky, buffer, i, znaky_n) == 0)
                {
                    znaky_n++;
                    znaky[znaky_n] = buffer[i];
                }
            }

            min_length = length;
        }
        else
        {
            for (int i = 0; i < length; i++)
            {
                if (letter_check(znaky, buffer, i, znaky_n) == 0)
                {
                    znaky_n++;
                    znaky[znaky_n] = buffer[i];
                }
            }
        }

        min_length = min(min_length, length); //zistenie ci je dane heslo najkratsie doposial

        //zistovanie ci heslo splna zadany level
        if (level == 1)
        {
            if (first_condition(buffer, length) == 1)
            {

                printf("%s", buffer);
            }
        }

        if (level == 2)
        {
            if (first_condition(buffer, length) == 1 && second_condition(buffer, length, parameter) == 1)
            {

                printf("%s", buffer);
            }
        }

        if (level == 3)
        {
            if (first_condition(buffer, length) == 1 && second_condition(buffer, length, parameter) == 1 && third_condition(buffer, length, parameter) == 1)
            {

                printf("%s", buffer);
            }
        }

        if (level == 4)
        {
            if (first_condition(buffer, length) == 1 && second_condition(buffer, length, parameter) == 1 && third_condition(buffer, length, parameter) == 1 && fourth_condition(buffer, length, parameter) == 1)
            {

                printf("%s", buffer);
            }
        }
    }

    //vypisovanie stats
    if (argc > 3)
    {
        if (includes_stats(stats) == 1)
        {

            printf("\nStatistika:\n");
            printf("Ruznych znaku: %d\n", length_of_znaky(znaky));
            printf("Minimalna dlzka: %d\n", min_length);
            printf("Priemerna dlzka: %0.1f\n", (double)overall_length / (double)word_count);
        }
    }

    return 0;
}

//created by Aurel Strigac xstrig00
