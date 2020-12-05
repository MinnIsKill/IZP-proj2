/**
 * @file:   sheet.c | VUT FIT BRNO | project N.2 | IZP
 * @author: Vojtěch Kališ, (xkalis03@stud.fit.vutbr.cz)
 * @date:   29.11.2020
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_ROW_LENGTH 10242
#define MAX_SIZE 1024
#define MAX 100


/* Syntax spouštění: ./sps [-d DELIM] CMD_SEQUENCE FILE */

/* Volitelně (usnadní pokročilý vývoj projektu, není třeba pro získání bodového hodnocení): Příkazy mohou být uloženy v textovém souboru. 
Každý příkaz bude oddělen znakem konce řádku. Soubor s příkazy bude specifikován namísto sekvence příkazů jako argument: -cCOMMAND_FILE. */

/// valgrind:       $ valgrind --leak-check=full --track-origins=yes ./sps [commands] tab.txt

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
    if (!start){
        return NULL;
    }
    /* remember current start as found token */
    token = start;
    /* find next occurrence of delim */
    start = strpbrk(start, delim);
    /* replace delim with terminator and move start to follower */
    if (start){
        *start++ = '\0';
    }
    /* done */
    return token;
}



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
    int start = 1;
    int row = 0;

    int cnt = 0;
    int cols = 0;
    int rows = 0;

    int working_row = 0;
    int working_col = 0;

    char delim[MAX];    // array for delimiters
    strcpy(delim," ");
    int delim_len = 0;


/***********************************
 *  ARGUMENTS PROCESSING SECTION   
***********************************/

/**
 *  THIS LOADS AND WORKS WITH DELIM
**/
    if (strcmp(argv[1], "-d") == 0){
        start++;
        for (int i = 0; argv[2][i] != '\0';i++){
            if (argv[2][i] == '\\' || argv[2][i] == '"'){
                fprintf(stderr,"ERROR: Delim contains forbidden characters.\nThe program was stopped.\n");
                exit(1);
            }
        }
        if (argv[2][0] != '['){
            start++;
            strcpy(delim, argv[2]);
            delim_len = sizeof(delim)/sizeof(delim[0]); // create a variable to save the size of the delimiters string into and calculate it
            remdup(delim, delim_len);                   // then remove any duplicate characters
            //strncpy(&delim_first[0], &delim[0], 1);
        } else {
            fprintf(stderr,"ERROR: It would appear delimiter was called but no delimiters were specified, setting it to default (space) instead.\n");
            strcpy(delim," "); //set delim to default
        }
    }










/**
 *  THIS LOADS AND WORKS WITH COMMANDS
**/
//NEZAPOMENOUT PŘI VÝBĚRU BUNĚK NA NASTAVENÍ 'COLS' (POČET SLOUPCŮ)

/**
 * !!!!!!!!!!!!!!!!!!!!!!! TAKHLE TO NEPŮJDE, MUSÍM ZKUSIT ULOŽIT SI ARGUMENTY JINAK
 * 
**/

    /**char *args [MAX_SIZE][MAX_SIZE]; //2d arr for arguments
    for (int i = 0; i < MAX_SIZE; i++){ // tohle je jen aby valgrind nekřičel
        for (int j = 0; j < MAX_SIZE; j++){
            args[i][j] = NULL;
        }
    }
    int arg_row = 0;**/

    //char args[MAX_SIZE];

    char* token2 = NULL;

    /**printf("argv[start] is %s\n",argv[start]);
    token2 = strtok_alt(argv[start],";");
    printf("!!! %s\n", token2);
    token2 = strtok_alt(NULL,";");
    printf("!!! %s\n", token2);**/

    /**token2 = strtok_alt(argv[start],";");
    while (token2 != NULL){
        printf( " %s\n", token2); //delete this later, for now it serves as an auxiliary line to print what it loaded
        //remchar(token, '\n'); //??????????? - nefunguje
        
        args[arg_row][0] = NULL;
        args[arg_row][0] = malloc(strlen(token2)+2;
        if (args[arg_row][0] == NULL){
            printf("REEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE (malloc error, should probably write something to deal with it)\n");
        }


        token2 = strtok_alt(NULL,";");
        ++arg_row;
    }

    arg_row = 0;**/
    token2 = NULL;
    bool flag_comma = false;
    int curr_char;
    int occurences = 0;
    //bool first = false;

    token2 = strtok_alt(argv[start],";");
    while (token2 != NULL){
        if (strncmp(token2, "[",1) == 0){
            for (int i = 0; token2[i] != '\0';i++){ //find number of ',' occurences in string
                if (token2[i] == ','){
                    ++occurences;
                }
            }
            char *p = token2;
            printf("argument for selection reached: %s\n",token2);
            if (occurences < 2){
                while (*p){
                    char *tmp = strstr(token2,"min");
                    if (tmp != 0){
                        printf("min found\n");
                        break;
                    }
                    tmp = strstr(token2,"max");
                    if (tmp != 0){
                        printf("max found\n");
                        break;
                    }
                    if (*p == ','){
                        flag_comma = true;
                    }
                    if (*p == ']'){
                        break;
                    }
                    if (flag_comma != true){
                        if (isdigit(*p)){
                            working_row = strtol(p, &p, 10);
                            printf ("working_row = %d\n",working_row);
                        } else if (*p == '_'){
                            working_row = -1;
                            printf ("working_row = %d (all rows)\n",working_row);
                            p++;
                        } else {
                            p++;
                        }
                    } else {
                        if (isdigit(*p)){
                            working_col = strtol(p, &p, 10);
                            printf ("working_col = %d\n",working_col);
                        } else if (*p == '_'){
                            working_col = -1;
                            printf ("working_col = %d (all cols)\n",working_col);
                            p++;
                        } else {
                            p++;
                        }
                    }
                    ++curr_char;
                }
            }
        } else {
            printf("other argument reached: %s\n",token2);
        }
        occurences = 0;
        flag_comma = false;
        token2 = strtok_alt(NULL,";");
    }

    /**token2 = strtok_alt(argv[start],";");
    while (token2 != NULL){
        if (first == false){
            printf( " %s\n", token2); //delete this later, for now it serves as an auxiliary line to print what it loaded
            strcpy(args, token2);
            strcat(args, "\n");
            token2 = strtok_alt(NULL,";");
            first = true;
        } else {
            printf( " %s\n", token2); //delete this later, for now it serves as an auxiliary line to print what it loaded
            //remchar(token, '\n'); //??????????? - nefunguje
            strcat(args, token2);
            strcat(args, "\n");
            token2 = strtok_alt(NULL,";");
        }
    }**/

    

    //printf("args are:\n%s\n",args);


    start++;
















    FILE *fd;
    //int i = 0, j = 0;
    int cells = 0;
    char buff[1024]; //1d arr for line reading
    //char comms[MAX][MAX]; //2d arr for commands

    char *tab [MAX][MAX_ROW_LENGTH]; //2d arr for lines to be stored into (already delim-separated into cells)
    for (int i = 0; i < MAX; i++){ // tohle je jen aby valgrind nekřičel
        for (int j = 0; j < MAX_ROW_LENGTH; j++){
            tab[i][j] = NULL;
        }
    }

    //int test = 0;
    //int col_num = 0;

    char* token = NULL;
    char tmp[1024];

    //bool flag_endrow = false;

    printf("start is %d\n",start);
    fd = fopen(argv[start], "r");
    // TODO: KONTROLA JESTLI SE SOUBOR OTEVREL
    // ???TODO: KONTROLA JESTLI SOUBOR NENI PRAZDNY???
    printf("delim is %s\n",delim);

    while (!feof(fd)){
        if (fgets(buff, sizeof(buff), fd) != NULL){
            strcpy(tmp, buff);
            token = strtok_alt(buff,&delim[0]);
            if (token == tmp){
                ++cells;
            } else
            while( token != NULL ) {
                token = strtok_alt(NULL,&delim[0]);
                ++cells;
                ++cnt;
            }
            if (cnt > cols){
                cols = cnt;
            }
            cnt = 0;
            ++row;
            ++rows;
        }
    }

    /**printf("\n=====================================\n");    
    printf ("number of cells = %d\n",cells);
    printf ("number of rows is %d\n",rows);
    printf ("most columns is %d\n",cols);
    printf("=====================================\n\n");**/
    
    row = 0;
    cnt = 0;
    rewind(fd);

    while (!feof(fd)){
        if (fgets(buff, sizeof(buff), fd) != NULL){
            strcpy(tmp, buff);
            token = strtok_alt(buff,&delim[0]);
            if (token == tmp){
                tab[row][cnt] = NULL;
                tab[row][cnt] = malloc(strlen(token)+1);
                if (tab[row][cnt] == NULL){
                    printf("REEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE (malloc error, should probably write something to deal with it)\n");
                }
                /**if (tab[row][cnt] == NULL){
                    exit;
                }**/
                //printf("tab[%i][%i] == %s\n",row,cnt,tab[row][cnt]);
            } else
            while( token != NULL ) { // POTŘEBUJU JEŠTĚ ZJISTIT JAK ODSTRANIT '\n',     EDIT: mám to vůbec odstraňovat? ten '\n' by tam
            // možná i měl zůstat, ne?)     EDIT2: no jo, ale co když přidám na konec každého řádku sloupec? ---> ANO, '\n' bych asi měl 
            // odstranit a pak přidat do poslední buňky každého řádku
                printf( " %s\n", token); //delete this later, for now it serves as an auxiliary line to print what it loaded
                //remchar(token, '\n'); //??????????? - nefunguje
                tab[row][cnt] = NULL;
                tab[row][cnt] = malloc(strlen(token)+1);
                if (tab[row][cnt] == NULL){
                    printf("REEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE (malloc error, should probably write something to deal with it)\n");
                }
                //printf("tab[%i][%i] == %s\n",row,cnt,tab[row][cnt]);
                token = strtok_alt(NULL,&delim[0]);
                ++cnt;
            }
            if (cnt > cols){
                cols = cnt;
            }
            cnt = 0;
            ++row;
        }
    }
    
    row = 0;
    cnt = 0;
    rewind(fd);

    while (!feof(fd)){
        if (fgets(buff, sizeof(buff), fd) != NULL){
            printf("row is %d\n",row);
            printf("buff is %s\n",buff);
            strcpy(tmp, buff);
            printf("tmp is %s\n",tmp);
            token = strtok_alt(buff,&delim[0]);
            printf("token is %s\n",token);
            if (strcmp(token, tmp) == 0){
                strncpy(tab[row][cnt], token, strlen(token)+1);
                printf("tab[%i][%i] == %s\n",row,cnt,tab[row][cnt]);
            } else
            while( token != NULL ) { // POTŘEBUJU JEŠTĚ ZJISTIT JAK ODSTRANIT '\n',     EDIT: mám to vůbec odstraňovat? ten '\n' by tam
            // možná i měl zůstat, ne?)     EDIT2: no jo, ale co když přidám na konec každého řádku sloupec? ---> ANO, '\n' bych asi měl 
            // odstranit a pak přidat do poslední buňky každého řádku
                printf( " %s\n", token); //delete this later, for now it serves as an auxiliary line to print what it loaded
                //remchar(token, '\n'); //??????????? - nefunguje
                strncpy(tab[row][cnt], token, strlen(token)+1); 
                token = strtok_alt(NULL,&delim[0]);
                ++cnt;
            }
            if (cnt > cols){
                cols = cnt;
            }
            cnt = 0;
            ++row;
        }
    }



/**
 *  SECTION FOR AUXILIARY PRINTS (DO NOT LEAVE IN FINAL FILE, OR AT LEAST HAVE IT BE COMMENTED OUT)
**/

    printf("\n=========================================\n");
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            printf("tab[%i][%i] == %s\n",i,j,tab[i][j]);
        }
    }
    printf("=========================================\n");

    printf("\n=========================================\n");
    printf ("\nargc is %i \n",argc);
    printf ("argv[2] is %s \n",argv[2]);
    printf ("delim is %s \n",delim);
    printf ("first delimiter, at delim[0], is %c \n",delim[0]);
    printf ("delim[1] is %c \n",delim[1]);
    printf ("number of non-null cells = %d\n",cells);
    printf ("number of rows is %d\n",rows);
    printf ("most columns is %d\n",cols);
    printf("=========================================\n");

/**
 *  SECTION FOR MEMORY FREES (JUST DELETE THE PRINTING FROM FINAL FILE)
**/
// FREE WHOLE 'tab' ARRAY (TRY TO PUT THIS INTO A FUNCTION FOR BETTER READABILITY)
    printf("\n=========================================\n");
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            printf("freeing tab[%i][%i] == %s\n",i,j,tab[i][j]);
            free(tab[i][j]);
        }
    }
    printf("=========================================\n");

    //printf("got here\n");

    fclose(fd);
    return 0;
}