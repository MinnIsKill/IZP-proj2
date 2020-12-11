/**
 * @file:   sheet.c | VUT FIT BRNO | project N.2 | IZP
 * @author: Vojtěch Kališ, (xkalis03@stud.fit.vutbr.cz)
 * @date:   6.12.2020
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_ROW_LENGTH 10242
#define MAX_SIZE 1024
#define MAX 100

//PAK TO CHCI PŘEDĚLAT ABYCH PRACOVAL S TOUHLE STRUKTUROU MÍSTO TÉ GLOBÁLNÍ 2D ARRAY DOLE (prozatím to nefunguje)
typedef struct{
    char* string;
}Block;

typedef struct{
  Block** data;
  int rows;
  int cols;
}Tab;


/* Syntax spouštění: ./sps [-d DELIM] CMD_SEQUENCE FILE */

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

char *tab [MAX][MAX_ROW_LENGTH]; //2d arr for lines to be stored into (already delim-separated into cells)
bool malfail_flag = false;

/** FREETAB
 * @brief frees all allocated space
 * @param rows  number of rows in array
 * @param cols  number of columns in array
**/
void freetab(int *rows,int *cols){
    for (int i = 0; i < *rows; i++){
        for (int j = 0; j < *cols; j++){
            //printf("freeing tab[%i][%i] == %s\n",i,j,tab[i][j]);
            free(tab[i][j]);
        }
    }
}

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

/** STRTOK_ALT
 * @brief behaves like strtok() but returns empty tokens as well
 *        strtok(): iterates through string until it finds a delimiter, in which case the function
 *        replaces the delimiter with a string terminator, returns what it loaded in the session 
 *        and remembers the position it ended at so that is can start from said position in the next call
 * @param string the string we're working on
 * @param delim  array of delimiters
 * @return string without duplicates
**/
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

/** STRREMOVE
 * @brief remove substrings from called string
 * @param string pointer to the original string
 * @param sub    the substring to be removed
**/
char *strremove(char *str, const char *sub) {
    char *p, *q, *r;
    if ((q = r = strstr(str, sub)) != NULL) {
        size_t len = strlen(sub);
        while ((r = strstr(p = r + len, sub)) != NULL) {
            memmove(q, p, r - p);
            q += r - p;
        }
        memmove(q, p, strlen(p) + 1);
    }
    return str;
}

/** IROW
 * @brief inserts a row in place of current row
 * @param working_row   the row in which to insert
 * @param rows          number of rows in array
 * @param cols          number of columns in array
**/
void irow(int working_row,int *rows,int *cols){
    ++*rows;
    for (int i = 0; i < *cols;i++){
        //printf("i is %d\n",i);
        tab[*rows][i] = NULL;
        tab[*rows][i] = malloc(MAX_SIZE);
        if (tab[*rows][i] == NULL){
            fprintf(stderr,"Malloc error: inside function 'irow'\nThe program will free allocated memory and exit\n");
            freetab(rows,cols);
            exit(1);
        }
        //printf("tab[%i-1][%i] malloc'd\n",*rows,i);
    }
    char* tmp;
    for (int i = *rows-1; i > working_row; i--){
        for (int j = *cols-1; j > -1; j--){
            if (i == working_row){
                memset(tab[i][j],'\0',MAX_SIZE);
            }
            tmp = tab[i][j];
            tab[i][j] = tab[i-1][j];
            tab[i-1][j]=tmp;
        }
    }
}

/** AROW
 * @brief inserts a row after of current row
 * @param working_row   the row after which to insert
 * @param rows          number of rows in array
 * @param cols          number of columns in array
**/
void arow(int working_row,int *rows,int *cols){
    ++*rows;
    for (int i = 0; i < *cols;i++){
        //printf("i is %d\n",i);
        tab[*rows][i] = NULL;
        tab[*rows][i] = malloc(MAX_SIZE);
        if (tab[*rows][i] == NULL){
            fprintf(stderr,"Malloc error: inside function 'arow'\nThe program will free allocated memory and exit\n");
            freetab(rows,cols);
            exit(1);
        }
        //printf("tab[%i-1][%i] malloc'd\n",*rows,i);
    }
    char* tmp;
    for (int i = *rows-1; i > working_row+1; i--){
        for (int j = *cols-1; j > -1; j--){
            if (i == working_row+1){
                memset(tab[i][j],'\0',MAX_SIZE);
            }
            tmp = tab[i][j]; //ulozim si [3][3] do tmp
            tab[i][j] = tab[i-1][j]; // do [3][3] nahraju [2][3]
            tab[i-1][j]=tmp; // [2][3] bude [3][3]
        }
    }
}

/** DROW
 * @brief deletes the current row
 * @param working_row   the row to delete
 * @param rows          number of rows in array
 * @param cols          number of columns in array
**/
void drow(int working_row,int *rows,int *cols){
    for (int i = working_row; i < *rows-1; i++){ //i = working_row = tedka [0], *rows-1 = posledni radek [3]
        for (int j = *cols-1; j > -1; j--){ //prochazim vsechny cols to by mělo byt cajk
            tab[i][j] = tab[i+1][j]; //vrazim [1][3] do [0][3] atd...
        }
    }
    for (int i = 0; i < *cols;i++){
        //printf("i is %d\n",i);
        tab[*rows][i] = NULL;
        free(tab[*rows][i]);
        //printf("tab[%i-1][%i] malloc'd\n",*rows,i);
    }
    --*rows;
}

/** ICOL
 * @brief inserts a column in place of current column
 * @param working_col   the column to which to insert
 * @param rows          number of rows in array
 * @param cols          number of columns in array
**/
void icol(int working_col,int *rows,int *cols){
    ++*cols;
    for (int i = 0; i < *rows;i++){
        //printf("i is %d\n",i);
        tab[i][*cols] = NULL;
        tab[i][*cols] = malloc(MAX_SIZE);
        if (tab[i][*cols] == NULL){
            fprintf(stderr,"Malloc error: inside function 'icol'\nThe program will free allocated memory and exit\n");
            freetab(rows,cols);
            exit(1);
        }
        //printf("tab[%i][%i-1] malloc'd\n",i,*cols);
    }
    char* tmp;
    for (int i = *rows-1; i > -1; i--){ // i = 3, i > -1 (PREJDU KAZDY RADEK)
        for (int j = *cols-1; j > working_col; j--){ // j = 4, j > 1 (PREJDU KAZDY SLOUPEC (i novy) az po working_col)
            if (j == working_col){ // pokud narazim na working_col, vynuluju ho a nebudu nic vic resit asi
                memset(tab[i][j],'\0',MAX_SIZE);
                break;
            }
            tmp = tab[i][j];
            tab[i][j] = tab[i][j-1];
            tab[i][j-1]=tmp;
        }
    }
}

/** ACOL
 * @brief inserts a column behind current column
 * @param working_col   the column behind which to insert
 * @param rows          number of rows in array
 * @param cols          number of columns in array
**/
void acol(int working_col,int *rows,int *cols){
    ++*cols;
    for (int i = 0; i < *rows;i++){
        //printf("i is %d\n",i);
        tab[i][*cols] = NULL;
        tab[i][*cols] = malloc(MAX_SIZE);
        if (tab[i][*cols] == NULL){
            fprintf(stderr,"Malloc error: inside function 'acol'\nThe program will free allocated memory and exit\n");
            freetab(rows,cols);
            exit(1);
        }
        //printf("tab[%i-1][%i] malloc'd\n",*rows,i);
    }
    char* tmp;
    for (int i = *rows-1; i > -1; i--){ // i = 3, i > -1 (PREJDU KAZDY RADEK)
        for (int j = *cols-1; j > working_col+1; j--){ // j = 4, j > 1 (PREJDU KAZDY SLOUPEC (i novy) az po working_col)
            if (j == working_col+1){ // pokud narazim na working_col, vynuluju ho a nebudu nic vic resit asi
                memset(tab[i][j],'\0',MAX_SIZE);
                break;
            }
            tmp = tab[i][j];
            tab[i][j] = tab[i][j-1];
            tab[i][j-1]=tmp;
        }
    }
}

/** DCOL
 * @brief deletes the given column
 * @param working_col   the column to delete
 * @param rows          number of rows in array
 * @param cols          number of columns in array
**/
void dcol(int working_col,int *rows,int *cols){
    for (int i = 0; i < *rows; i++){ //i = [0], i < 4 --> i[0],i[1],i[2],i[3] (vsechny radky)
        for (int j = working_col; j < *cols-1; j++){ //j = [0], j < 3 --> j[0],j[1],j[2]
            tab[i][j] = tab[i][j+1]; //vrazim [0][1] do [0][0] atd...
            //printf("tab[%d][%d] do tab[%d][%d]\n",i,j+1,i,j);
        }
    }
    for (int i = 0; i < *rows; i++){
        tab[i][*cols-1] = NULL;
        free(tab[i][*cols-1]);
    }
    --*cols;
}

/** SETSTR
 * @brief sets the currently selected cells to string
 * @param working_row   the row in which to insert
 * @param working_col   the column to which to insert
 * @param str           the string to insert
**/
void setstr(int working_row, int working_col, char str[]){
    if (tab[working_row][working_col] == NULL){
        tab[working_row][working_col] = malloc(MAX_SIZE);
        if (tab[working_row][working_col] == NULL){
            malfail_flag = true;
            return;
        }
    }
    memset(tab[working_row][working_col],'\0',MAX_SIZE);
    strcpy(tab[working_row][working_col], str);
    //printf("tab[%d][%d] == %s\n",working_row,working_col,tab[working_row][working_col]);
}

/** CLEAR
 * @brief the contents of given blocks will be erased
 * @param working_row   the row from which to clear
 * @param working_col   the column from which to clear
 * @param ending_row   the row to which to clear
 * @param ending_col   the column to which to clear
**/
void clear(int working_row, int working_col, int ending_row, int ending_col){
    printf("\n");
    for (int i = working_row; i <= ending_row; i++){
        for (int j = working_col; j <= ending_col; j++){
            if (tab[i][j] == NULL){
                tab[i][j] = malloc(MAX_SIZE);
                if (tab[working_row][working_col] == NULL){
                    malfail_flag = true;
                    return;
                }
            } else {
                memset(tab[i][j],'\0',MAX_SIZE);
            }
        }
    }
}

/** SWAP
 * @brief swaps the contents of two given blocks
 * @param working_row   the row of one of the swapped blocks
 * @param working_col   the column of one of the swapped blocks
 * @param other_row   the row of the other one of the swapped blocks
 * @param other_col   the column of the other one of the swapped blocks
**/
void swap(int working_row, int working_col, int other_row, int other_col){
    if (tab[working_row][working_col] == NULL){
        tab[working_row][working_col] = malloc(MAX_SIZE);
        if (tab[working_row][working_col] == NULL){
            malfail_flag = true;
            return;
        }
        memset(tab[working_row][working_col],'\0',MAX_SIZE);
    }
    if (tab[other_row][other_col] == NULL){
        tab[other_row][other_col] = malloc(MAX_SIZE);
        memset(tab[other_row][other_col],'\0',MAX_SIZE);
    }
    char tmp[MAX_SIZE];
    memset(tmp,'\0',MAX_SIZE);
    memmove(tmp,tab[working_row][working_col],MAX_SIZE);
    //printf("first block: [%d][%d] == %s\n",working_row,working_col,tab[working_row][working_col]);
    //printf("second block: [%d][%d] == %s\n",other_row,other_col,tab[other_row][other_col]);
    memmove(tab[working_row][working_col],tab[other_row][other_col],MAX_SIZE);
    memmove(tab[other_row][other_col],tmp,MAX_SIZE);
    //printf("first block: [%d][%d] == %s\n",working_row,working_col,tab[working_row][working_col]);
    //printf("second block: [%d][%d] == %s\n",other_row,other_col,tab[other_row][other_col]);
}

/** SUM
 * @brief calculates the sum of all numbers in given block selection and saves it to given block
 * @param working_row   the row from which to work
 * @param working_col   the column from which to work
 * @param other_row   the row to which to work
 * @param other_col   the column to which to work
 * @param saveto_row   the row to which to save
 * @param saveto_col   the column to which to save
**/
void sum(int working_row, int working_col, int ending_row, int ending_col, int saveto_row, int saveto_col){
    if (tab[saveto_row][saveto_col] == NULL){
        tab[saveto_row][saveto_col] = malloc(MAX_SIZE);
        if (tab[saveto_row][saveto_col] == NULL){
            malfail_flag = true;
            return;
        }
        memset(tab[saveto_row][saveto_col],'\0',MAX_SIZE);
    }

    char* p;
    char* tmp;
    float sum = 0.0;
    float num = 0.0;

    for (int i = working_row; i < ending_row; i++){
        for (int j = working_col; j < ending_col; j++){
            if (tab[i][j] == NULL){
                break;
            }
            tmp = tab[i][j];
            while (*tmp){
                if (isdigit(*tmp)){
                    //printf ("found a number\n");
                    num = strtod(tmp,&p);
                    tmp = p;
                    sum += num;
                } else {
                ++tmp;
                }
            }
        }
    }
    char result[50];
    sprintf(result, "%g", sum);
    strcpy(tab[saveto_row-1][saveto_col-1],result);
}

/** AVG
 * @brief calculates the avg of all numbers in given block selection and saves it to given block
 * @param working_row   the row from which to work
 * @param working_col   the column from which to work
 * @param other_row   the row to which to work
 * @param other_col   the column to which to work
 * @param saveto_row   the row to which to save
 * @param saveto_col   the column to which to save
**/
void avg(int working_row, int working_col, int ending_row, int ending_col, int saveto_row, int saveto_col){
    if (tab[saveto_row][saveto_col] == NULL){
        tab[saveto_row][saveto_col] = malloc(MAX_SIZE);
        memset(tab[saveto_row][saveto_col],'\0',MAX_SIZE);
        if (tab[saveto_row][saveto_col] == NULL){
            malfail_flag = true;
            return;
        }
    }

    char* p;
    char* tmp;
    float avg = 0.0;
    float num = 0.0;

    for (int i = working_row; i < ending_row; i++){
        for (int j = working_col; j < ending_col; j++){
            if (tab[i][j] == NULL){
                break;
            }
            tmp = tab[i][j];
            while (*tmp){
                if (isdigit(*tmp)){
                    //printf ("found a number\n");
                    num = strtod(tmp,&p);
                    tmp = p;
                    avg = (avg + num) / 2.0;
                } else {
                ++tmp;
                }
            }
        }
    }
    char result[50];
    sprintf(result, "%g", avg); 
    strcpy(tab[saveto_row-1][saveto_col-1],result);
}

/** COUNT
 * @brief calculates the number of nun-null block in given block selection and saves it to given block
 * @param working_row   the row from which to work
 * @param working_col   the column from which to work
 * @param other_row   the row to which to work
 * @param other_col   the column to which to work
 * @param saveto_row   the row to which to save
 * @param saveto_col   the column to which to save
**/
void count(int working_row, int working_col, int ending_row, int ending_col, int saveto_row, int saveto_col){
    int cnt = 1;
    if (tab[saveto_row][saveto_col] == NULL){
        tab[saveto_row][saveto_col] = malloc(MAX_SIZE);
        if (tab[saveto_row][saveto_col] == NULL){
            malfail_flag = true;
            return;
        }
        memset(tab[saveto_row][saveto_col],'\0',MAX_SIZE);
        cnt--;
    }

    for (int i = working_row; i < ending_row; i++){
        for (int j = working_col; j < ending_col; j++){
            if (tab[i][j] != NULL){
                cnt++;
            }
        }
    }
    char result[50];
    sprintf(result, "%d", cnt);
    memmove(tab[saveto_row][saveto_col],result,MAX_SIZE);
}

/** LEN
 * @brief
**/
void len(){
    ;
}

/*********************
 *       MAIN
*********************/
int main( int argc, char *argv[] )
{
    if (argc <= 2){
        fprintf(stderr,"Error: Insufficient number of input parameters.\n");
        return 1;
    }

    for (int i = 0; i < MAX; i++){ // tohle je jen aby valgrind nekřičel
        for (int j = 0; j < MAX_ROW_LENGTH; j++){
            tab[i][j] = NULL;
        }
    }
    int start = 1;
    int row = 0;

    int cnt = 0;
    int cols = 0;
    int rows = 0;

    int working_row = 0;
    bool workrow_flag = false;
    int ending_row = 0;
    int working_col = 0;
    bool workcol_flag = false;
    int ending_col = 0;
    //bool window_flag = false;

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
                fprintf(stderr,"Error: Delim contains forbidden characters.\nThe program was stopped.\n");
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
            fprintf(stderr,"Error: It would appear delimiter was called but no delimiters were specified, setting it to default (space) instead.\n");
            strcpy(delim," "); //set delim to default
        }
    }


    start++;


/**
 *  THIS LOADS FILE AND SAVES IT INTO 'tab' ARRAY
**/
    FILE *fd;

    int cells = 0;
    char buff[1024]; //1d arr for line reading

    char* token = NULL;
    char tmp[1024];

    //printf("start is %d\n",start);
    fd = fopen(argv[start], "r");
    if (fd == NULL) {
        fprintf(stderr,"Error: Failed to open the given file\nThe program will exit\n");
        exit(1);
    }
    // TODO: KONTROLA JESTLI SE SOUBOR OTEVREL
    // ???TODO: KONTROLA JESTLI SOUBOR NENI PRAZDNY???
    //printf("delim is %s\n",delim);

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
    ending_row = rows;
    ending_col = cols;
    
    row = 0;
    cnt = 0;
    rewind(fd);

    while (!feof(fd)){
        if (fgets(buff, sizeof(buff), fd) != NULL){
            strcpy(tmp, buff);
            token = strtok_alt(buff,&delim[0]);
            if (token == tmp){
                tab[row][cnt] = NULL;
                tab[row][cnt] = malloc(MAX_SIZE);
                if (tab[row][cnt] == NULL){
                    printf("Error: Malloc error during file reading\nThe program will free allocated memory and exit\n");
                    freetab(&rows,&cols);
                    exit(1);
                }
                //printf("tab[%i][%i] == %s\n",row,cnt,tab[row][cnt]);
            } else
            while( token != NULL ) {
                tab[row][cnt] = NULL;
                tab[row][cnt] = malloc(MAX_SIZE);
                if (tab[row][cnt] == NULL){
                    printf("Error: Malloc error during file reading\nThe program will free allocated memory and exit\n");
                    freetab(&rows,&cols);
                    exit(1);
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
            //printf("row is %d\n",row);
            //printf("buff is %s\n",buff);
            strcpy(tmp, buff);
            //printf("tmp is %s\n",tmp);
            token = strtok_alt(buff,&delim[0]);
            //printf("token is %s\n",token);
            if (strcmp(token, tmp) == 0){
                strncpy(tab[row][cnt], token, strlen(token)+1);
                //printf("tab[%i][%i] == %s\n",row,cnt,tab[row][cnt]);
            } else
            while( token != NULL ) {
                //printf( " %s\n", token); //delete this later, for now it serves as an auxiliary line to print what it loaded
                //remchar(token, '\n'); //??????????? - nefunguje
                strncpy(tab[row][cnt], token, strlen(token)+1);
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

    fclose(fd);


    //printf("\n=========================================\n");
    start--;


/**
 *  THIS LOADS AND WORKS WITH COMMANDS
**/
//NEZAPOMENOUT PŘI VÝBĚRU BUNĚK NA NASTAVENÍ 'COLS' (POČET SLOUPCŮ)

    char* token2 = NULL;

    bool flag_comma = false;
    int curr_char;
    int occurences = 0;

    int endrow = 0;
    int endcol = 0;

    int min = 0;
    int max = 0;
    bool firsttime_flag = false;

    int rownum = -1;
    int colnum = -1;
    int countr = 0;

    bool find_flag = false;
    if (find_flag == true){
        ;
    }
    bool temp_flag = false;
    char find_str[MAX_ROW_LENGTH];
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
            char *minmax = NULL;
            //printf("argument for selection reached: %s\n",token2);
            if (occurences < 2){
                while (*p){
                    /// [min] ///
                    char *tmp = strstr(token2,"min");
                    if (tmp != 0){
                        //printf("min found\n");
                        if (workrow_flag == false || workcol_flag == false){
                            fprintf(stderr,"For function 'min': no previous row and column selection found\nThe program will free allocated memory and exit\n");
                            freetab(&rows,&cols);
                            exit(1);
                        }
                        endrow = ending_row;
                        endcol = ending_col;
                        for (int i = working_row; i < endrow; i++){
                            for (int j = working_col; j < endcol; j++){
                                if (tab[i][j] == NULL){
                                    ;
                                } else {
                                    minmax = tab[i][j];
                                    while (*minmax){
                                        if (isdigit(*minmax)){
                                            int num = strtol(minmax, &minmax, 10);
                                            if(firsttime_flag == false){
                                                firsttime_flag = true;
                                                min = num;
                                            } else if (num < min){
                                                working_row = i;
                                                ending_row = i;
                                                working_col = j;
                                                ending_col = j;
                                            }
                                        }
                                        ++minmax;
                                    }
                                }
                            }
                        }
                        //printf("working_row is %d\n",working_row);
                        //printf("working_col is %d\n",working_col);
                        break;
                    }
                    /// [max] ///
                    tmp = strstr(token2,"max");
                    if (tmp != 0){
                        //printf("max found\n");
                        if (workrow_flag == false || workcol_flag == false){
                            fprintf(stderr,"For function 'max': no previous row and column selection found\nThe program will free allocated memory and exit\n");
                            freetab(&rows,&cols);
                            exit(1);
                        }
                        endrow = ending_row;
                        endcol = ending_col;
                        for (int i = working_row; i < endrow; i++){
                            for (int j = working_col; j < endcol; j++){
                                if (tab[i][j] == NULL){
                                    ;
                                } else {
                                    minmax = tab[i][j];
                                    while (*minmax){
                                        if (isdigit(*minmax)){
                                            int num = strtol(minmax, &minmax, 10);
                                            if(firsttime_flag == false){
                                                firsttime_flag = true;
                                                max = num;
                                            } else if (num > max){
                                                working_row = i;
                                                ending_row = i;
                                                working_col = j;
                                                ending_col = j;
                                            }
                                        }
                                        ++minmax;
                                    }
                                }
                            }
                        }
                        //printf("working_row is %d\n",working_row);
                        //printf("working_col is %d\n",working_col);
                        break;
                    }
                    /// [find STR] ///      !!!JENOM PRVNÍ BUŇKU KTERÁ OBSAHUJE STR!!!
                    tmp = strstr(token2,"find");
                    if (tmp != 0){
                        if (workrow_flag == false || workcol_flag == false){
                            fprintf(stderr,"Error: For function 'find': no previous row and column selection found\nThe program will free allocated memory and exit\n");
                            freetab(&rows,&cols);
                            exit(1);
                        }
                        memset(find_str, 0, sizeof(find_str));
                        strcpy(find_str,&token2[6]);
                        int str_size = strlen(find_str);
                        find_str[str_size-1] = '\0';
                        find_flag = true;
                        endrow = ending_row;
                        endcol = ending_col;
                        for (int i = working_row; i < endrow; i++){
                            for (int j = working_col; j < endcol; j++){
                                if (tab[i][j] == NULL){
                                    ;
                                } else {
                                    tmp = strstr(tab[i][j], find_str);
                                    if (tmp != 0){
                                        //printf("string found on: tab[%i][%i]\n",i,j);
                                        temp_flag = true;
                                        working_row = i;
                                        ending_row = i;
                                        working_col = j;
                                        ending_col = j;
                                        break;
                                    }
                                }
                            }
                            if (temp_flag == true){
                                break;
                            }
                        }
                        if (temp_flag == false){
                            fprintf(stderr,"Error: For 'find': string %s not found in selection\nThe program skipped the argument\n",find_str);
                        }
                        temp_flag = false;
                        //printf("working_row is %d\n",working_row);
                        //printf("working_col is %d\n",working_col);
                        break;
                    }
                    /// [_] ///
                    if (occurences == 0){ //pokud tam neni zadna carka
                        tmp = strstr(token2,"_");
                        if (tmp != 0){
                            //printf("[_] found\n");
                            break;
                        }
                    }
                    if (*p == ','){
                        flag_comma = true;
                    }
                    if (*p == ']'){
                        break;
                    }
                    if (flag_comma != true){
                        if (isdigit(*p)){ /// [R,_] || [R,C] ///
                            workrow_flag = true;
                            working_row = strtol(p, &p, 10);
                            --working_row;
                            ending_row = working_row; //not working with it
                            //printf ("working_row = %d\n",working_row);
                        } else if (*p == '_'){ /// [_,C] || [_,_] ///
                            workrow_flag = true;
                            working_row = 0;
                            ending_row = rows; //all rows
                            //printf ("working_row = %d (all rows)\n",working_row);
                            p++;
                        } else {
                            p++;
                        }
                    } else {
                        if (isdigit(*p)){ /// [_,C] || [R,C] ///
                            workcol_flag = true;
                            working_col = strtol(p, &p, 10);
                            --working_col;
                            ending_col = working_col; //not working with it
                            //printf ("working_col = %d\n",working_col);
                        } else if (*p == '_'){ /// [R,_] || [_,_] ///
                            workcol_flag = true;
                            working_col = 0;
                            ending_col = cols; //all cols
                            //printf ("working_col = %d (all cols)\n",working_col);
                            p++;
                        } else {
                            p++;
                        }
                    }
                    ++curr_char;
                }
            } else if (occurences == 3){ /// [R1,C1,R2,C2] ///
                int commas = 0;
                while (*p){
                    //window_flag = true;
                    if (*p == ','){
                        commas++;
                    }
                    if (*p == ']'){
                        break;
                    }
                    if (commas == 0 || commas == 2){
                        if (isdigit(*p)){ /// [R1,_,R2,_] ///
                            workrow_flag = true;
                            if (commas == 0){
                                working_row = strtol(p, &p, 10);
                                --working_row;
                                //printf ("working_row = %d\n",working_row);
                            } else {
                                ending_row = strtol(p, &p, 10);
                                --ending_row;
                                if (ending_row < working_row){
                                    fprintf(stderr,"In function [R1,C1,R2,C2] for row and column selection: R1 and C1 mustn't be > R2 and C2\nThe program will free allocated memory and exit\n");
                                    freetab(&rows,&cols);
                                    exit(1);
                                }
                                //printf ("ending_row = %d\n",ending_row);
                            }
                        } else if (*p == '-'){ /// [_,C] || [_,_] ///
                            workrow_flag = true;
                            if (commas == 0){
                                working_row = 0;
                                //printf ("working_row = %d (all rows before ending_row)\n",working_row);
                            } else {
                                ending_row = rows;
                                //printf ("ending_row = %d (all rows after working_row)\n",ending_row);
                            }
                            p++;
                        } else {
                            p++;
                        }
                    } else {
                        if (isdigit(*p)){ /// [_,C] || [R,C] ///
                            workcol_flag = true;
                            if (commas == 1){
                                working_col = strtol(p, &p, 10);
                                --working_col;
                                //printf ("working_col = %d\n",working_col);
                            } else {
                                ending_col = strtol(p, &p, 10);
                                --ending_col;
                                if (ending_col < working_col){
                                    fprintf(stderr,"In function [R1,C1,R2,C2] for row and column selection: R1 and C1 mustn't be > R2 and C2\nThe program will free allocated memory and exit\n");
                                    freetab(&rows,&cols);
                                    exit(1);
                                }
                                //printf ("ending_col = %d\n",ending_col);
                            }
                        } else if (*p == '-'){ /// [_,C] || [_,_] ///
                            workcol_flag = true;
                            if (commas == 0){
                                working_col = 0;
                                //printf ("working_col = %d (all cols before ending_col)\n",working_col);
                            } else {
                                ending_col = cols;
                                //printf ("ending_col = %d (all cols after working_col)\n",ending_col);
                            }
                            p++;
                        } else {
                            p++;
                        }
                    }
                    ++curr_char;
                    //printf("[ , , , ] found\n"); // bude se pracovat od working_row po ending_row nebo rows a working_col po ending_col nebo cols
                }
            }
/** OTHER ARGUMENTS **
irow - vloží jeden prázdný řádek nalevo od vybraných buněk.
arow - přidá jeden prázdný řádek napravo od vybraných buněk.
drow - odstraní vybrané řádky.
icol - vloží jeden prázdný sloupec nalevo od vybraných buněk.
acol - přidá jeden prázdný sloupec napravo od vybraných buněk.
dcol - odstraní vybrané sloupce. **/
        } else {
            //printf("other argument reached: %s\n",token2);
            if (strncmp(token2, "irow", 4) == 0){
                irow(working_row,&rows,&cols);
            } else
            if (strncmp(token2, "arow", 4) == 0){
                arow(working_row,&rows,&cols);
            } else
            if (strncmp(token2, "drow", 4) == 0){
                drow(working_row,&rows,&cols);
            } else
            if (strncmp(token2, "icol", 4) == 0){
                icol(working_col,&rows,&cols);
            } else
            if (strncmp(token2, "acol", 4) == 0){
                acol(working_col,&rows,&cols);
            } else
            if (strncmp(token2, "dcol", 4) == 0){
                dcol(working_col,&rows,&cols);
            } else
            ///////////////////
            if (strncmp(token2, "set", 3) == 0){
                char strng[MAX_ROW_LENGTH];
                memset(strng, 0, sizeof(strng)+1);
                strcpy(strng,&token2[4]);
                int strng_size = strlen(strng);
                strng[strng_size] = '\0';
                setstr(working_row,working_col,strng);
                if (malfail_flag == true){
                    fprintf(stderr,"Error: Malloc error: inside function 'setstr'\nThe program will free allocated memory and exit\n");
                    freetab(&rows,&cols);
                    exit(1);
                }
            } else
            if (strncmp(token2, "clear", 5) == 0){
                clear(working_row,working_col,ending_row,ending_col);
                if (malfail_flag == true){
                    fprintf(stderr,"Error: Malloc error: inside function 'clear'\nThe program will free allocated memory and exit\n");
                    freetab(&rows,&cols);
                    exit(1);
                }
            } else
            if (strncmp(token2, "swap", 4) == 0){
                char* tok;
                char tmp[MAX];
                strcpy(tmp,&token2[6]);
                tok = strtok_alt(tmp,",");
                while (tok != 0){
                    if (countr > 2){
                        fprintf(stderr,"Error: Swap selection reading error\nThe program will free allocated memory and exit\n");
                        freetab(&rows,&cols);
                        exit(1);
                    }
                    if (isdigit(*tok)){
                        if (countr == 0){
                            rownum = strtol(tok,&tok,10);
                            --rownum;
                            //printf("rownum is %d\n",rownum);
                        } else {
                            colnum = strtol(tok,&tok,10);
                            --colnum;
                            //printf("colnum is %d\n",colnum);
                        }
                    }
                    tok = strtok_alt(NULL,",");
                    ++countr;
                }
                if (rownum == -1 && colnum == -1){
                    fprintf(stderr,"Error: Swap selection reading error\nThe program will free allocated memory and exit\n");
                    freetab(&rows,&cols);
                    exit(1);
                }

                swap(working_row,working_col,rownum,colnum);
                if (malfail_flag == true){
                    fprintf(stderr,"Error: Malloc error: inside function 'swap'\nThe program will free allocated memory and exit\n");
                    freetab(&rows,&cols);
                    exit(1);
                }

                rownum = 0;
                colnum = 0;
                countr = 0;
            } else
            if (strncmp(token2, "sum", 3) == 0){
                char* tok;
                char tmp[MAX];
                strcpy(tmp,&token2[5]);
                tok = strtok_alt(tmp,",");
                while (tok != 0){
                    if (countr > 2){
                        fprintf(stderr,"Error: Sum selection reading error\nThe program will free allocated memory and exit\n");
                        freetab(&rows,&cols);
                        exit(1);
                    }
                    if (isdigit(*tok)){
                        if (countr == 0){
                            rownum = strtol(tok,&tok,10);
                            //printf("rownum is %d\n",rownum);
                        } else {
                            colnum = strtol(tok,&tok,10);
                            //printf("colnum is %d\n",colnum);
                        }
                    }
                    tok = strtok_alt(NULL,",");
                    ++countr;
                }
                if (rownum == -1 && colnum == -1){
                    fprintf(stderr,"Error: Swap selection reading error\nThe program will free allocated memory and exit\n");
                    freetab(&rows,&cols);
                    exit(1);
                }

                sum(working_row,working_col,ending_row,ending_col,rownum,colnum);
                if (malfail_flag == true){
                    fprintf(stderr,"Error: Malloc error: in function 'sum'\nThe program will free allocated memory and exit\n");
                    freetab(&rows,&cols);
                    exit(1);
                }

                rownum = -1;
                colnum = -1;
                countr = 0;
            } else
            if (strncmp(token2, "avg", 3) == 0){
                char* tok;
                char tmp[MAX];
                strcpy(tmp,&token2[5]);
                tok = strtok_alt(tmp,",");
                while (tok != 0){
                    if (countr > 2){
                        fprintf(stderr,"Error: Sum selection reading error\nThe program will free allocated memory and exit\n");
                        freetab(&rows,&cols);
                        exit(1);
                    }
                    if (isdigit(*tok)){
                        if (countr == 0){
                            rownum = strtol(tok,&tok,10);
                            //printf("rownum is %d\n",rownum);
                        } else {
                            colnum = strtol(tok,&tok,10);
                            //printf("colnum is %d\n",colnum);
                        }
                    }
                    tok = strtok_alt(NULL,",");
                    ++countr;
                }
                if (rownum == -1 && colnum == -1){
                    fprintf(stderr,"Error: Swap selection reading error\nThe program will free allocated memory and exit\n");
                    freetab(&rows,&cols);
                    exit(1);
                }

                avg(working_row,working_col,ending_row,ending_col,rownum,colnum);
                if (malfail_flag == true){
                    fprintf(stderr,"Error: Malloc error: inside function 'avg'\nThe program will free allocated memory and exit\n");
                    freetab(&rows,&cols);
                    exit(1);
                }

                rownum = -2;
                colnum = -2;
                countr = 0;
            } else
            if (strncmp(token2, "count", 5) == 0){
                char* tok;
                char tmp[MAX];
                strcpy(tmp,&token2[7]);
                tok = strtok_alt(tmp,",");
                while (tok != 0){
                    if (countr > 2){
                        fprintf(stderr,"Error: Sum selection reading error\nThe program will free allocated memory and exit\n");
                        freetab(&rows,&cols);
                        exit(1);
                    }
                    if (isdigit(*tok)){
                        if (countr == 0){
                            rownum = strtol(tok,&tok,10);
                            --rownum;
                            //printf("rownum is %d\n",rownum);
                        } else {
                            colnum = strtol(tok,&tok,10);
                            --colnum;
                            //printf("colnum is %d\n",colnum);
                        }
                    }
                    tok = strtok_alt(NULL,",");
                    ++countr;
                }
                if (rownum == -2 && colnum == -2){
                    fprintf(stderr,"Error: Swap selection reading error\nThe program will free allocated memory and exit\n");
                    freetab(&rows,&cols);
                    exit(1);
                }

                count(working_row,working_col,ending_row,ending_col,rownum,colnum);
                if (malfail_flag == true){
                    fprintf(stderr,"Error: Malloc error: inside function 'count'\nThe program will free allocated memory and exit\n");
                    freetab(&rows,&cols);
                    exit(1);
                }

                rownum = -1;
                colnum = -1;
                countr = 0;
            } else
            if (strncmp(token2, "len", 3) == 0){
                len();
            } else {
                fprintf(stderr,"Error: command %s not recognized\nThe program will free allocated memory and exit\n",token2);
                freetab(&rows,&cols);
                exit(1);
            }
        }
        occurences = 0;
        flag_comma = false;
        token2 = strtok_alt(NULL,";");
    }


    //printf("=========================================\n");


/**
 *  SECTION FOR AUXILIARY PRINTS (DO NOT LEAVE IN FINAL FILE, OR AT LEAST HAVE IT BE COMMENTED OUT)
**/

//printf("start is %d\n",start);
//PRINT TABLE
    //printf("\n=========================================\n");

    fd = fopen(argv[start+1], "w");
    if (fd == NULL) {
        fprintf(stderr,"Error: Failed to open the given file\nThe program will exit\n");
        exit(1);
    }

    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            if(tab[i][j] == NULL){
                tab[i][j] = malloc(MAX_SIZE);
                memset(tab[i][j],'\0',MAX_SIZE);
            }
        }
    }
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            int length = strlen(tab[i][j]);
            if (tab[i][j][length-1] == '\n'){
                tab[i][j][length-1] = '\0';
            }
        }
    }

    
    char delimfirst[2];
    strncpy(delimfirst,delim,1);
    //printf("delimfirst is %s\n",delimfirst);
      for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if(tab[i][j] == NULL)
            {
                break;
            }
            fprintf(fd,"%s",tab[i][j]);
            if(j < cols-1)
            {
                fprintf(fd,"%s",delimfirst);
            }

        }
        fprintf(fd,"\n");
    }

    fclose(fd);

    //printf("=========================================\n");

/** PRINT USEFUL INFO
    printf("\n=========================================\n");
    printf ("\nargc is %i\n",argc);
    printf ("argv[2] is %s\n",argv[2]);
    printf ("delim is %s\n",delim);
    printf ("first delimiter, at delim[0], is %c\n",delim[0]);
    printf ("delim[1] is %c\n",delim[1]);
    printf ("number of non-null cells = %d\n",cells);
    printf ("number of rows is %d\n",rows);
    printf ("most columns is %d\n",cols);
    printf ("ending_row is %d\n",ending_row);
    printf ("ending_col is %d\n",ending_col);
    printf("=========================================\n");
**/

/**
 *  SECTION FOR MEMORY FREES
**/
    //printf("\n=========================================\n");
    freetab(&rows,&cols);
    //printf("=========================================\n");

    return 0;
}
