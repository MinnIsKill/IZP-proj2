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


/** REMCHAR
 * @brief remove called characters in called string
 * @param string pointer to the original string
 * @param chr    the character to be removed
**/
void remchar(char *string, char chr){
    int j = 0;
    for (int i = 0; string[i] != '\0'; i++){ // move through all of original 'string'
        if (string[i] != chr){
            string[j++] = string[i]; // 'j' only moves after we write a non-'chr'
        }
    }
   string[j] = '\0'; // re-null-terminate
}

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

/* behaves like strtok() except that it returns empty tokens also
    */
char* strtok_alt(char *str, const char *delim){
  static char *start = NULL; /* stores string str for consecutive calls */
  char *token = NULL; /* found token */
  /* assign new start in case */
  if (str){
      start = str;
  }
  /* check whether text to parse left */
  if (!start) return NULL;
  /* remember current start as found token */
  token = start;
  /* find next occurrence of delim */
  start = strpbrk(start, delim);
  /* replace delim with terminator and move start to follower */
  if (start) *start++ = '\0';
  /* done */
  return token;
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
    int start = 2;

    int row = 0;

    char delim[MAX];    // array for delimiters
    strcpy(delim," ");
    int delim_len = 0;

    //int cols = 0; //number of columns
    //int curr_row = 0; //currently printed row

    if (strcmp(argv[1], "-d") == 0){
        start++;
        if (strcmp(argv[2], "irow") != 0 && strcmp(argv[2], "arow") != 0 && strcmp(argv[2], "drow") != 0 && strcmp(argv[2], "drows") != 0 && strcmp(argv[2], "icol") != 0 && strcmp(argv[2], "acol") != 0 && strcmp(argv[2], "dcol") != 0 && strcmp(argv[2], "dcols") != 0 &&
        strcmp(argv[2], "cset") != 0 && strcmp(argv[2], "tolower") != 0 && strcmp(argv[2], "toupper") != 0 && strcmp(argv[2], "round") != 0 && strcmp(argv[2], "int") != 0 && strcmp(argv[2], "copy") != 0 && strcmp(argv[2], "swap") != 0 && strcmp(argv[2], "move") != 0){
            start++;
            strcpy(delim, argv[2]);
            delim_len = sizeof(delim)/sizeof(delim[0]); // create a variable to save the size of the delimiters string into and calculate it
            remdup(delim, delim_len);                   // then remove any duplicate characters
            //strncpy(&delim_first[0], &delim[0], 1);
        } else {
            fprintf(stderr,"It would appear the argument in place of the delimiter looks an awful lot like a command, setting it to default (space) instead. \nDid you perhaps forget to enter a delimiter? \n");
            strcpy(delim," ");     // set it to default ' ' --> (I'm not sure if it isn't set to be a blank space as default already, because the output was the same without this line?? But I'm leaving it in just to be sure lol.)
        }
    }


    FILE *fd;
    int i = 0, j = 0;
    char buff[1024];
    char *options [MAX][MAX_ROW_LENGTH];

    int test = 0;
    //int col_num = 0;

    char* token;

    int cnt = 0;

    i++; //ODSTRAN JAK BUDES POUZIVAT
    j++;
    test++;

    printf("start is %d\n",start);
    fd = fopen(argv[start], "r");
    printf("delim is %s\n",delim);

    while (!feof(fd)) {
        if (fgets(buff, sizeof(buff), fd) != NULL) {
            printf("buff is %s\n",buff);
            token = strtok_alt(buff,&delim[0]);
            while( token != NULL ) { // POTŘEBUJU JEŠTĚ ZJISTIT JAK ODSTRANIT '\n', 
                                     //A JAK ZAJISTIT VYTVOŘENÍ BUŇKY KDYŽ JSOU DVA DELIMETERY VEDLE SEBE
                printf( " %s\n", token);
                //remchar(token, '\n'); //??????????? - nefunguje
                options[row][cnt] = malloc(sizeof(MAX_SIZE));
                strcpy(options[row][cnt], token);
                token = strtok_alt(NULL,&delim[0]);
                cnt++;
            }
            /**for (; buff[j] != '\n'; j++){
                for (int k = 0; delim[k] != '\0'; k++){
                    options[row][j] = malloc(sizeof(MAX_SIZE));
                    sscanf(buff, "%s %s", options[row][j], options[row][j+1]);
                    test = k;
                }
                options[row][j] = malloc(sizeof(MAX_SIZE));
                sscanf(buff, "%s %s", options[row][j], options[row][j+1]);
                
                printf("j is %d\n",j);
                printf("k is %d\n",test);
                printf("buff[j] is %c\n",buff[j]);
                printf("delim[k] is %c\n",delim[test]);
                printf("delim is %s\n",delim);
            }
            if (buff[j] == '\n'){
                printf("newline reached\n");
            }**/
            printf("row is %d\n",row);
            cnt = 0;
            ++row;
            //j = 0;
        }
    }

    printf("options[0][0] is %s, options[0][1] is %s, options[0][2] is %s\noptions[1][0] is %s, options[1][1] is %s, options[1][2] is %s\noptions[2][0] is %s, options[2][1] is %s, options[2][2] is %s\n",options[0][0],options[0][1],options[0][2],options[1][0],options[1][1],options[1][2],options[2][0],options[2][1],options[2][2]);

/**    int tmp = 0;
    int tmp2 = row;
    while (tmp2 >= 0) { //TOHLE asi ani nemusim menit protoze mi to tedka slouzi jenom ke kontrolnimu vypisu, ale w/e
        tmp2--;
        if (tmp2 < 0){
            break;
        }
        printf ("Cell[%i][%i] is %s, Cell[%i][%i] is %s\n", tmp, tmp2, options[tmp][tmp2], tmp, tmp2+1, options[tmp][tmp2+1]);
    }**/


    printf ("\nargc is %i \n",argc);
    printf ("argv[2] is %s \n",argv[2]);            //! REMEMBER TO
    printf ("delim is %s \n",delim);                //! DELETE THESE
    printf ("first delimiter, at delim[0], is %c \n",delim[0]);          //! LATER ON
    printf ("delim[1] is %c \n",delim[1]);          //! YOU MONKEY

    fclose(fd);
    return 0;
}