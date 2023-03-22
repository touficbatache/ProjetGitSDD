#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "main.h"

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

List *initList() {
    return malloc(sizeof(List));
}

Cell *buildCell(char *ch) {
    Cell *cell = malloc(sizeof(Cell));
    cell->data = ch;
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

    char *str = malloc(sizeof(char) * strLen);
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
    List *l = initList();
    char *cellChar = strtok(s, "|");
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

int main() {
    char *hash = sha256file("test.txt");
    printf("\n Data read back from temporary file is [%s]\n", hash);
    free(hash);

    List *l = initList();
    insertFirst(l, buildCell("world"));
    insertFirst(l, buildCell("Hello"));
    char *stringList = ltos(l);
    printf("List data is [%s]\n", stringList);

    printf("Element at index 0 is [%s]\n", ctos(listGet(l, 0)));
    printf("Element at index 1 is [%s]\n", ctos(listGet(l, 1)));

    List *l2 = stol(stringList);
    printf("List2 data is [%s]\n", ltos(l2));

    ltof(l, "testingC.txt");
    List *l3 = ftol("testingC.txt");
    printf("List3 data is [%s]\n", ltos(l3));

    return 0;
}
