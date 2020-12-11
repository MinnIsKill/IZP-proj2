/**
 * @file:   sheet.c | VUT FIT BRNO | project N.2 | IZP
 * @author: Vojtěch Kališ, (xkalis03@stud.fit.vutbr.cz)
 * @date:   5.12.2020
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_ROW_LENGTH 10242
#define MAX_SIZE 1024
#define MAX 100

typedef struct{
    char* string;
}S_String;

typedef struct{
  S_String** data;
  unsigned int rows;
  unsigned int cols;
}S_Tab;

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

//char *tab [MAX][MAX_ROW_LENGTH]; //2d arr for lines to be stored into (already delim-separated into cells)

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

int string_resize(char** string, unsigned int new_size)
{
    char* tmp_str = NULL;
    tmp_str = (char*)realloc(*string, new_size);
    if(tmp_str == NULL)
    {
        fprintf(stderr,"Error: Realloc for string failed.\n");
        return -1;
    }
    *string = tmp_str;
    *string[0] = '\0';
    return 0;
}

void S_Tab_rsize_cols(S_Tab* Tab, unsigned int new_cols)
{
    if(Tab == NULL || Tab->cols == new_cols)
    {
        return;
    }

  for(unsigned int i = 0;i < Tab->rows; ++i)
  {
    if(Tab->cols > new_cols)
    {
      for(unsigned int j = new_cols ; j <  Tab->cols; ++j)
      {
        free(Tab->data[i][j].string);
      }
    }
    if(new_cols!=0)
    {
      S_String* tmp_cols=NULL;
      tmp_cols = (S_String*)realloc(Tab->data[i],new_cols*sizeof(S_String));
      if(tmp_cols == NULL)
      {
    fprintf(stderr,"Realloc fail.\n");
    return;
      }
      Tab->data[i] = tmp_cols;
    }
    if(Tab->cols < new_cols)
    {
      for(unsigned int j = Tab->cols; j < new_cols; ++j)
      {
        string_resize(&Tab->data[Tab->rows][j].string, 10);
      }
    }   
  }
  Tab->cols = new_cols;
}

void S_Tab_rsize_rows(S_Tab* Tab, unsigned int new_rows)
{
  if(Tab == NULL || Tab->rows == new_rows)
  {
    return;
  }
  
  if(Tab->rows > new_rows)
  {
    for(unsigned int i = new_rows; i < Tab->rows; ++i)
    {
      for(unsigned int j = 0; j < Tab->cols; ++j)
      {
        free(Tab->data[i][j].string);
      }
      free(Tab->data[i]);
      Tab->data[i]=NULL;
    }
  }
  
  if(new_rows != 0)
  {
    S_String** tmp_data= NULL;
    tmp_data = (S_String**)realloc(Tab->data,new_rows*sizeof(S_String*));
    if(tmp_data == NULL)
    {
      fprintf(stderr,"Failed to reallocate memory for table(rows).\n");
      return;
    }
    Tab->data = tmp_data;
  }
  if(Tab->rows < new_rows)
  {
    for(;Tab->rows < new_rows; ++Tab->rows)
    {
      Tab->data[Tab->rows]=(S_String*)malloc(Tab->cols*sizeof(S_String));
      if(Tab->data[Tab->rows]==NULL)
      {
    fprintf(stderr,"Failed to reallocate memory for table(rows).\n");
    return;
      }
      for(unsigned int j = 0; j < Tab->cols; ++j)
      {
        string_resize(&Tab->data[Tab->rows][j].string, 10);
      }
    }
  }
  Tab->rows=new_rows;
  if(new_rows == 0)
  {
    Tab->cols = 0;
  }
}

void irow(S_Tab* tab,int working_row,int *rows,int *cols){
    S_Tab_rsize_rows(tab,tab->rows+1);
    char* tmp;
    for (int i = tab->rows-1; i > working_row; i--){
        for (int j = tab->cols-1; j > -1; j--){
            if (i == working_row){
                memset(tab->data[i][j].string,'\0',strlen(tab->data[i][j].string));
            }
            tmp = tab->data[i][j].string;
            tab->data[i][j].string = tab->data[i-1][j].string;
            tab->data[i-1][j].string = tmp;
        }
    }
    printf("working_row = %d\n",working_row);
    printf("rows = %d\n",*rows);
    printf("cols = %d\n",*cols);
}

/**void freetab(int rows,int cols,char** tab){
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            printf("freeing tab[%i][%i] == %s\n",i,j,tab[i][j]);
            free(&tab[i][j]);
        }
    }
}**/

/*********************
 *       MAIN
*********************/
int main( int argc, char *argv[] )
{
    if (argc <= 2){
        fprintf(stderr,"Error: Insufficient number of input parameters.\n");
        return 1;
    }

    S_Tab tab;

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
    bool window_flag = false;

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



    start++;


/**
 *  THIS LOADS FILE AND SAVES IT INTO 'tab' ARRAY
**/
    FILE *fd;

    int cells = 0;
    char buff[1024]; //1d arr for line reading

    char* token = NULL;
    char tmp[1024];

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
    printf("hello 1\n");
    ending_row = rows;
    ending_col = cols;

    tab.cols = cols;
    //printf("rows is %d\n",rows);
    //printf("cols is %d\n",cols);
    S_Tab_rsize_rows(&tab,rows);
    printf("hello 3\n");

    
    printf("hello 2\n");

    row = 0;
    cnt = 0;
    rewind(fd);

    while (!feof(fd)){
        if (fgets(buff, sizeof(buff), fd) != NULL){
            strcpy(tmp, buff);
            token = strtok_alt(buff,&delim[0]);
            if (strcmp(token, tmp) == 0){
                string_resize(&(tab.data[row][cnt].string), strlen(token)+1);
                strncpy(tab.data[row][cnt].string, token, strlen(token)+1);
            } else
            while( token != NULL ) {
                string_resize(&(tab.data[row][cnt].string), strlen(token)+1);
                strncpy(tab.data[row][cnt].string, token, strlen(token)+1);
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

    printf("\n=========================================\n");
    start--;


/**
 *  THIS LOADS AND WORKS WITH COMMANDS
**/
//NEZAPOMENOUT PŘI VÝBĚRU BUNĚK NA NASTAVENÍ 'COLS' (POČET SLOUPCŮ)

    char* token2 = NULL;

    token2 = NULL;
    bool flag_comma = false;
    int curr_char;
    int occurences = 0;

    bool find_flag = false;
    if (find_flag == true){
        ;
    }
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
            //printf("argument for selection reached: %s\n",token2);
            if (occurences < 2){
                while (*p){
                    /// [min] ///
                    char *tmp = strstr(token2,"min");
                    if (tmp != 0){
                        if (workrow_flag == false || workcol_flag == false){
                            fprintf(stderr,"For function min: no previous row and column selection found\nThe program will free allocated memory and shut down\n");
                        ////////////////////////////////////////////
                            for (int i = 0; i < rows; i++){
                                for (int j = 0; j < cols; j++){
                                    printf("freeing tab[%i][%i] == %s\n",i,j,tab.data[row][cnt].string);
                                    free(tab.data[row][cnt].string);
                                }
                                free(tab.data[row]);
                            }
                            free(tab.data);
                        ////////////////////////////////////////////
                            exit(1);
                        }

                        printf("min found\n");
                        break;
                    }
                    /// [max] ///
                    tmp = strstr(token2,"max");
                    if (tmp != 0){
                        if (workrow_flag == false || workcol_flag == false){
                            fprintf(stderr,"For function max: no previous row and column selection found\nThe program will free allocated memory and shut down\n");
                            for (int i = 0; i < rows; i++){
                                for (int j = 0; j < cols; j++){
                                    printf("freeing tab[%i][%i] == %s\n",i,j,tab.data[row][cnt].string);
                                    free(tab.data[row][cnt].string);
                                }
                                free(tab.data[row]);
                            }
                            free(tab.data);
                            exit(1);
                        }
                        if (window_flag == true){
                            ;
                        } else {

                        }
                        printf("max found\n");
                        break;
                    }
                    /// [find STR] ///      !!!JENOM PRVNÍ BUŇKU KTERÁ OBSAHUJE STR!!!
                    tmp = strstr(token2,"find");
                    if (tmp != 0){
                        if (workrow_flag == false || workcol_flag == false){
                            fprintf(stderr,"For function find: no previous row and column selection found\nThe program will free allocated memory and shut down\n");
                            for (int i = 0; i < rows; i++){
                                for (int j = 0; j < cols; j++){
                                    printf("freeing tab[%i][%i] == %s\n",i,j,tab.data[row][cnt].string);
                                    free(tab.data[row][cnt].string);
                                }
                                free(tab.data[row]);
                            }
                            free(tab.data);
                            exit(1);
                        }

                        printf("find found\n");
                        memset(find_str, 0, sizeof(find_str));
                        strcpy(find_str,&token2[6]);
                        int str_size = strlen(find_str);
                        find_str[str_size-1] = '\0';
                        find_flag = true;
                        printf("string is %s\n",find_str);
                        break;
                    }
                    /// [_] ///
                    if (occurences == 0){ //pokud tam neni zadna carka
                        tmp = strstr(token2,"_");
                        if (tmp != 0){
                            printf("[_] found\n");
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
                            printf ("working_row = %d\n",working_row);
                        } else if (*p == '_'){ /// [_,C] || [_,_] ///
                            workrow_flag = true;
                            working_row = 0;
                            printf ("working_row = %d (all rows)\n",working_row);
                            p++;
                        } else {
                            p++;
                        }
                    } else {
                        if (isdigit(*p)){ /// [_,C] || [R,C] ///
                            workcol_flag = true;
                            working_col = strtol(p, &p, 10);
                            printf ("working_col = %d\n",working_col);
                        } else if (*p == '_'){ /// [R,_] || [_,_] ///
                            workcol_flag = true;
                            working_col = 0;
                            printf ("working_col = %d (all cols)\n",working_col);
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
                    window_flag = true;
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
                                printf ("working_row = %d\n",working_row);
                            } else {
                                ending_row = strtol(p, &p, 10);
                                if (ending_row < working_row){
                                    fprintf(stderr,"In function [R1,C1,R2,C2] for row and column selection: R1 and C1 mustn't be > R2 and C2\nThe program will free allocated memory and shut down\n");
                                    for (int i = 0; i < rows; i++){
                                        for (int j = 0; j < cols; j++){
                                            printf("freeing tab[%i][%i] == %s\n",i,j,tab.data[row][cnt].string);
                                            free(tab.data[row][cnt].string);
                                        }
                                    }
                                    exit(1);
                                }
                                printf ("ending_row = %d\n",ending_row);
                            }
                        } else if (*p == '-'){ /// [_,C] || [_,_] ///
                            workrow_flag = true;
                            if (commas == 0){
                                working_row = 0;
                                printf ("working_row = %d (all rows before ending_row)\n",working_row);
                            } else {
                                ending_row = rows;
                                printf ("ending_row = %d (all rows after working_row)\n",ending_row);
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
                                printf ("working_col = %d\n",working_col);
                            } else {
                                ending_col = strtol(p, &p, 10);
                                if (ending_col < working_col){
                                    fprintf(stderr,"In function [R1,C1,R2,C2] for row and column selection: R1 and C1 mustn't be > R2 and C2\nThe program will free allocated memory and shut down\n");
                                    for (int i = 0; i < rows; i++){
                                        for (int j = 0; j < cols; j++){
                                            printf("freeing tab[%i][%i] == %s\n",i,j,tab.data[row][cnt].string);
                                            free(tab.data[row][cnt].string);
                                        }
                                    }
                                    exit(1);
                                }
                                printf ("ending_col = %d\n",ending_col);
                            }
                        } else if (*p == '-'){ /// [_,C] || [_,_] ///
                            workcol_flag = true;
                            if (commas == 0){
                                working_col = 0;
                                printf ("working_col = %d (all cols before ending_col)\n",working_col);
                            } else {
                                ending_col = cols;
                                printf ("ending_col = %d (all cols after working_col)\n",ending_col);
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
            printf("other argument reached: %s\n",token2);
            if (strncmp(token2, "irow", 4) == 0){
                irow(&tab,working_row,&rows,&cols);
                printf("rows = %d\n",rows);
                printf("cols = %d\n",cols);
            } else
            if (strncmp(token2, "arow", 4) == 0){
                ;
            } else
            if (strncmp(token2, "drow", 4) == 0){
                ;
            } else
            if (strncmp(token2, "icol", 4) == 0){
                ;
            } else
            if (strncmp(token2, "acol", 4) == 0){
                ;
            } else
            if (strncmp(token2, "dcol", 4) == 0){
                ;
            } else {
                printf("command not recognized\n");
            }
        }
        occurences = 0;
        flag_comma = false;
        token2 = strtok_alt(NULL,";");
    }


    printf("=========================================\n");


/**
 *  SECTION FOR AUXILIARY PRINTS (DO NOT LEAVE IN FINAL FILE, OR AT LEAST HAVE IT BE COMMENTED OUT)
**/

    printf("\n=========================================\n");
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            printf("tab[%i][%i] == %s\n",i,j,tab.data[row][cnt].string);
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
    printf ("ending_row is %d\n",ending_row);
    printf ("ending_col is %d\n",ending_col);
    printf("=========================================\n");

/**
 *  SECTION FOR MEMORY FREES (JUST DELETE THE PRINTING FROM FINAL FILE)
**/
// FREE WHOLE 'tab' ARRAY (TRY TO PUT THIS INTO A FUNCTION FOR BETTER READABILITY)
    printf("\n=========================================\n");
    //freetab(rows,cols,*tab);
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            printf("freeing tab[%i][%i] == %s\n",i,j,tab.data[row][cnt].string);
            free(tab.data[row][cnt].string);
        }
        free(tab.data[row]);
    }
    free(tab.data);
    printf("=========================================\n");

    //printf("got here\n");

    //fclose(fd);
    return 0;
}