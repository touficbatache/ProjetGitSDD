#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include "main.h"
#include <dirent.h>


//Exo 1
static char template[] = "/tmp/hashed_file_XXXXXX";

int hashFile(char *source, char *dest) {
    char cmd[100];
    sprintf(cmd, "sha256sum < %s > %s", source, dest);
    system(cmd);
    return 0;
}

char *sha256file(char *file) {
    char fname[1000];
    strcpy(fname, template);
    int fd = mkstemp(fname);

    hashFile(file, fname);

    char *buffer = malloc(sizeof(char) * 64);
    int errno = 0;
    // read the data from temporary file
    if (read(fd, buffer, 64) == -1) {
        // Error occurred
        printf("\n read failed with error [%s]\n", strerror(errno));
    }

    unlink(fname);
    return buffer;
}

//Exo 2
List *initList() {
    return malloc(sizeof(List));
}

Cell *buildCell(char *ch) {
    Cell *cell = malloc(sizeof(Cell));
    cell->data = malloc(sizeof(char)*1000);
    strcpy(cell->data, ch);
    cell->next = NULL;
    return cell;
}

void insertFirst(List *l, Cell *c) {
    c->next = *l;
    *l = c;
}

char *ctos(Cell *c) {
    return c->data;
}

char *ltos(List *l) {
    unsigned long strLen = 0;
    List tmp = *l;
    while (tmp) {
        strLen += strlen(ctos(tmp)) + 1;
        tmp = tmp->next;
    }
    strLen -= 1;

    char *str = malloc(sizeof(char)*strLen);
    tmp = *l;
    while (tmp) {
        if (strlen(str) > 0) {
            strcat(str, "|");
        }
        strcat(str, ctos(tmp));
        tmp = tmp->next;
    }
    return str;
}

Cell *listGet(List *l, int i) {
    int index = 0;
    List tmp = *l;
    while (tmp) {
        if (index == i) {
            return tmp;
        }
        index += 1;
        tmp = tmp->next;
    }
    return NULL;
}

Cell *searchList(List *l, char *str) {
    while (*l) {
        if (strcmp(ctos(*l), str) == 0) {
            return *l;
        }
        *l = (*l)->next;
    }
    return NULL;
}

List *stol(char *s) {
    List* l = initList();
    char* cellChar = strtok(s, "|");
    while (cellChar != NULL) {
        insertFirst(l, buildCell(cellChar));
        cellChar = strtok(NULL, "|");
    }
    return l;
}

void ltof(List *l, char *path) {
    FILE *fp = fopen(path, "w+");
    fputs(ltos(l), fp);
    fclose(fp);
}

char *magic_reallocating_fgets(char **bufp, size_t *sizep, FILE *fp) {
    size_t len;
    if (fgets(*bufp, *sizep, fp) == NULL) return NULL;
    len = strlen(*bufp);
    while (strchr(*bufp, '\n') == NULL) {
        *sizep += 100;
        *bufp = realloc(*bufp, *sizep);
        if (fgets(*bufp + len, *sizep - len, fp) == NULL) return *bufp;
        len += strlen(*bufp + len);
    }
    return *bufp;
}

List *ftol(char *path) {
    FILE *fp = fopen(path, "r");

    size_t linelen = 80;
    char *line = malloc(linelen);

    magic_reallocating_fgets(&line, &linelen, fp);

    fclose(fp);

    return stol(line);
}

//Exo 3
List* listdir ( char* root_dir ) {
    DIR * dp ;
    struct dirent * ep ;
    List * L = initList () ;
    *L = NULL ;
    Cell * temp_cell ;
    dp = opendir( root_dir ) ;
    if ( dp != NULL ){
        while (( ep = readdir ( dp ) ) != NULL ){
            temp_cell = buildCell ( ep -> d_name ) ;
            insertFirst (L , temp_cell ) ;
            List ptr = *L ;
            while ( ptr != NULL ) {
                ptr = ptr->next ;
            }
        }
    (void)closedir(dp) ;
    }
    else{
        perror( "Ouverture impossible" ) ;
        return NULL ;
    }
    return L ;
}

int file_exists(char *file) { // à corriger :(
    List* list = listdir(".");
    if(!list){
        printf("Erreur listdir\".\" \n");
        return 0 ;
    }

    Cell* current = *list;

    while (current != NULL) {
        if (strcmp(current->data, file) == 0) {
            return 1;
        }
        current = current->next;
    }

    return 0;
}

void cp(char* to, char* from) {
    FILE* source = fopen(from, "r");
    if (source == NULL) {
        printf("Erreur: Le fichier source n'existe pas\n");
        return;
    }
    FILE* dest = fopen(to, "w");
    if (dest == NULL) {
        printf("Erreur: Fichier destination impossible à ouvrir\n");
        return;
    }
    char line[1024];
    while (fgets(line, 1024, source) != NULL) {
        fputs(line, dest);
    }
    fclose(source);
    fclose(dest);
}

char* hashToPath ( char * hash ) {
    char* dir = malloc ((strlen(hash) + 1 )*sizeof (char)) ;
    dir[0] = hash [0];
    dir[1] = hash [1];
    dir[2] = '/' ;
    int i ;
    for ( i = 3; i<=strlen(hash) ; i++) {
        dir [ i ] = hash [i - 1];
    }
    dir[i] = '\0 ' ;
    return dir ;
}

void blobFile ( char* file ) {
    char* hash = sha256file(file) ;
    char* ch2 = strdup(hash) ;
    ch2 [2] = '\0' ;
    if (!file_exists(ch2)) {
        char buff[100];
        sprintf (buff , "mkdir %s" ,ch2 ) ;
        system (buff) ;
    }
    char* ch = hashToPath(hash) ;
    cp(ch, file) ;
    }


int main() {
    // Test exo1
    char *hash = sha256file("test.txt");
    printf("\n Data read back from temporary file is [%s]\n", hash);
    free(hash);

    // Test exo2
    List *l = initList();
    insertFirst(l, buildCell("world"));
    insertFirst(l, buildCell("Hello"));
    char *stringList = ltos(l);
    printf("List data is [%s]\n", stringList);

    printf("Element at index 0 is [%s]\n", ctos(listGet(l, 0)));
    printf("Element at index 1 is [%s]\n", ctos(listGet(l, 1)));

    List *l2 = stol(stringList);
    printf("List2 data is [%s]\n", ltos(l2));

    /*ltof(l, "testingC.txt");
    List *l3 = ftol("testingC.txt");
    printf("List3 data is [%s]\n", ltos(l3));*/

    // Test exo3
    //List* listfichiers = listdir("/home/zahra/LU2IN006");
    //printf("Liste noms fichiers is [%s]\n", ltos(listfichiers));
    //printf("Existence du fichier test.txt :%d\n", file_exists("projet"));

    return 0;
}
