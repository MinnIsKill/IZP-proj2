/**
 * @file:   sheet.c | VUT FIT BRNO | project N.1 | IZP
 * @author: Vojtěch Kališ, (xkalis03@stud.fit.vutbr.cz)
 * @date:   29.11.2020
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROW_LENGTH 10242
#define MAX_SIZE 1024
#define MAX 100

/**     LORD GIVE ME THE MENTAL CAPACITY TO REMEMBER THESE, but in the meantime, this cheatsheet should suffice.
GitHub push flow:   $ git add .
                    $ git commit -m "message"
                    $ git push --set-upstream origin main   (where 'main' is name of branch to commit to)

!!!(Forces local file overwrite)!!!
GitHub pull flow:   $ git fetch origin main
                    $ git reset --hard origin/main

VSCode Keybind-sheet:  CTRL+SHIFT+B -> BUILD
                        F5 -> DEBUG
**/




/** REMDUP
 * @brief remove duplicate characters in string
 * @param string the string we're working on
 * @param n      number of characters in string (length of string)
 * @return string without duplicates
**/
char *remdup(char string[], int n){
    int i,j,index = 0;
    for (i = 0; i < n; i++){ // traverse through all characters
        for (j = 0; j < i; j++){ // check if string[i] already exists (has been saved earlier)
            if (string[i] == string[j]){
                break;
            }
        }
        if (j == i){ // if it doesn't yet exist, add it to resulting string
            string[index++] = string[i];
        }
    }
    return string;
}

/* Syntax spouštění: ./sps [-d DELIM] CMD_SEQUENCE FILE */

/* Volitelně (usnadní pokročilý vývoj projektu, není třeba pro získání bodového hodnocení): Příkazy mohou být uloženy v textovém souboru. 
Každý příkaz bude oddělen znakem konce řádku. Soubor s příkazy bude specifikován namísto sekvence příkazů jako argument: -cCOMMAND_FILE. */

int main( int argc, char *argv[] )
{
    if (argc <= 2){
        fprintf(stderr,"Error: Insufficient number of input parameters.\n");
        return 1;
    }
/**    char row[MAX_ROW_LENGTH];
    char str[MAX_ROW_LENGTH][MAX];

    while (fgets(row, MAX_ROW_LENGTH, stdin) != NULL){}

    printf("argc is %d\n",argc);
    if(argc != 2) {
        printf("Usage: %s filename\n", argv[0]);
        return 1;
    }
    
    printf("argv[1] is %s\n",argv[1]);
    FILE *file = fopen(argv[1], "r");

    if(file == 0) {
        printf("Can't open '%s'\n", argv[1]);
        return 1;
    }

    while(fscanf(file, "%s", &str) == 1)
    {
        printf("%s\n", str);
    }
    return 0;
}**/
    int start = 3;

    char delim[MAX];    // array for delimiters
    int delim_len = 0;

    int cols = 0; //number of columns
    int curr_row = 0; //currently printed row

    if (strcmp(argv[1], "-d") == 0){
        if (strcmp(argv[2], "irow") != 0 && strcmp(argv[2], "arow") != 0 && strcmp(argv[2], "drow") != 0 && strcmp(argv[2], "drows") != 0 && strcmp(argv[2], "icol") != 0 && strcmp(argv[2], "acol") != 0 && strcmp(argv[2], "dcol") != 0 && strcmp(argv[2], "dcols") != 0 &&
        strcmp(argv[2], "cset") != 0 && strcmp(argv[2], "tolower") != 0 && strcmp(argv[2], "toupper") != 0 && strcmp(argv[2], "round") != 0 && strcmp(argv[2], "int") != 0 && strcmp(argv[2], "copy") != 0 && strcmp(argv[2], "swap") != 0 && strcmp(argv[2], "move") != 0){
            strcpy(delim, argv[2]);
            delim_len = sizeof(delim)/sizeof(delim[0]); // create a variable to save the size of the delimiters string into and calculate it
            remdup(delim, delim_len);                       // then remove any duplicate characters
            //strncpy(&delim_first[0], &delim[0], 1);
        } else {
            fprintf(stderr,"It would appear the argument in place of the delimiter looks an awful lot like a command, setting it to default (space) instead. \nDid you perhaps forget to enter a delimiter? \n");
            delim[0] = ' ';     // set it to default ' ' --> (I'm not sure if it isn't set to be a blank space as default already, because the output was the same without this line?? But I'm leaving it in just to be sure lol.)
            --start;
        }
    }

    FILE *fd;
    int i = 0, j = 0;
    char buff[1024];
    char *options [2][20];
    printf("argc is %d\n",argc);

    fd = fopen(argv[1], "r");

    while (!feof(fd)) {
        if (fgets(buff, sizeof(buff), fd) != NULL) {
            for (; buff[j] != delim[0] || buff[j] != '\n'; j++){
                options[i][j] = malloc(sizeof(MAX));
                sscanf(buff, "%s%c%s", options[i][j], delim[0], options[i][j]);
            }
            if (buff[j] == delim[0]){
                printf("delim reached\n");
            }
            if (buff[j] == '\n'){
                printf("newline reached\n");
            }
            i++;
            j = 0;
        }
    }

    while (i >= 0) { //TOHLE asi ani nemusim menit protoze mi to tedka slouzi jenom ke kontrolnimu vypisu, ale w/e
        i--;
        if (i < 0){
            break;
        }
        printf ("Option %i is %s its value is %s\n", i, options[i][0], options[i][1]);
    }

    fclose(fd);
    return 0;
}