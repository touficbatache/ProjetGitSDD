#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "main.h"

#define FILE_SIZE 1000
#define WORKTREE_SIZE 10

int hashFile(char *source, char *dest) {
    char cmd[100];
    sprintf(cmd, "sha256sum < %s > %s", source, dest);
    system(cmd);
    return 0;
}

char *sha256file(char *file) {
    static char template[] = "/tmp/hashed_file_XXXXXX";
    char fname[1000];
    strcpy(fname, template);
    int fd = mkstemp(fname);

    hashFile(file, fname);

    char *buffer = calloc(64, sizeof(char));
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
    List *l = malloc(sizeof(List));

    if (l == NULL) {
        printf("Malloc failure");
        return NULL;
    }

    return l;
}

Cell *buildCell(char *ch) {
    Cell *cell = malloc(sizeof(Cell));

    if (cell == NULL) {
        printf("Malloc failure");
        return NULL;
    }

    cell->data = strdup(ch);
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

    char *str = calloc(strLen, sizeof(char));
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
        index++;
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

List *ftol(char *path) {
    FILE *fp = fopen(path, "r");

    char *line = malloc(FILE_SIZE);

    fgets(line, FILE_SIZE, fp);

    fclose(fp);

    return stol(line);
}

WorkFile *createWorkFile(char *name) {
    WorkFile *workFile = malloc(sizeof(WorkFile));

    if (workFile == NULL) {
        printf("Malloc failure");
        return NULL;
    }

    workFile->name = strdup(name);
    workFile->hash = NULL;
    workFile->mode = 0;

    return workFile;
}

char *wfts(WorkFile *wf) {
    char *s = calloc(100, sizeof(char));
    sprintf(s, "%s\t%s\t%d", wf->name, wf->hash, wf->mode);
    return s;
}

WorkFile *stwf(char *ch) {
    char *name = calloc(100, sizeof(char));
    char *hash = calloc(100, sizeof(char));
    int mode;

    sscanf(ch, "%[^\t]\t%[^\t]\t%d", name, hash, &mode);

    WorkFile *wf = createWorkFile(name);
    free(name);

//    if (strcmp(hash, "(null)") != 0) {
//        wf->hash = strdup(hash);
//    }

    wf->hash = hash;
    wf->mode = mode;
    return wf;
}

WorkTree *initWorkTree() {
    WorkTree *workTree = malloc(sizeof(WorkTree));

    if (workTree == NULL) {
        printf("Malloc failure");
        return NULL;
    }

    workTree->size = WORKTREE_SIZE;
    workTree->tab = calloc(workTree->size, sizeof(WorkFile));
    workTree->n = 0;

    return workTree;
}

int inWorkTree(WorkTree *wt, char *name) {
    for (int i = 0; i < wt->n; i++) {
        if (strcmp(wt->tab[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int appendWorkTree(WorkTree *wt, char *name, char *hash, int mode) {
    if (wt->n == wt->size - 1) {
        printf("Erreur : le work tree est plein.\n");
        return 1;
    }

    if (inWorkTree(wt, name) != -1) {
        printf("Erreur : le fichier \"%s\" existe déjà dans le work tree.\n", name);
        return 1;
    }

    WorkFile *wf = createWorkFile(name);
    wf->hash = strdup(hash);
    wf->mode = mode;

    wt->tab[wt->n] = *wf;
    wt->n++;

    return 0;
}

char *wtts(WorkTree *wt) {
    char *s = calloc(FILE_SIZE * wt->n, sizeof(char));

    int i = 0;
    while (i < wt->n) {
        strcat(s, wfts(&((wt->tab)[i])));
        if (i < wt->n - 1) {
            strcat(s, "\n");
        }
        i++;
    }

    return s;
}

WorkTree *stwt(char *ch) {
    WorkTree *wt = initWorkTree();

    char *wfs = strtok(ch, "\n");
    while (wfs != NULL) {
        WorkFile *wf = stwf(wfs);
        appendWorkTree(wt, wf->name, wf->hash, wf->mode);
        wfs = strtok(NULL, "\n");
    }

    return wt;
}

int wttf(WorkTree *wt, char *file) {
    FILE *fp = fopen(file, "w+");

    if (fp == NULL) {
        return -1;
    }

    fputs(wtts(wt), fp);
    fclose(fp);
    return 0;
}

WorkTree *ftwt(char *file) {
    FILE *fp = fopen(file, "r");

    char *lines = calloc(FILE_SIZE * WORKTREE_SIZE, sizeof(char));

    char *line = malloc(FILE_SIZE);

    while (fgets(line, FILE_SIZE, fp) != NULL) {
        if (strlen(lines) > 0) {
            strcat(lines, "\n");
        }
        strcat(lines, line);
    }

    fclose(fp);

    return stwt(lines);
}

int main() {
    char *fileName = "test.txt";
    char *hash = sha256file(fileName);
    printf("Data read back from temporary file is [%s]\n", hash);

    List *l = initList();
    insertFirst(l, buildCell("world"));
    insertFirst(l, buildCell("Hello"));
    char *stringList = ltos(l);
    printf("List data is [%s]\n", stringList);

    printf("Element at index 0 is [%s]\n", ctos(listGet(l, 0)));
    printf("Element at index 1 is [%s]\n", ctos(listGet(l, 1)));

    List *l2 = stol(stringList);
    printf("List2 data is [%s]\n", ltos(l2));

    char *fileNameSaveList = "testingC.txt";
    ltof(l, fileNameSaveList);
    List *l3 = ftol(fileNameSaveList);
    printf("List3 data is [%s]\n", ltos(l3));

    WorkFile *wf = createWorkFile("Test work file");
    char *wfs = wfts(wf);
    printf("\nCreated WorkFile : %s\n\n", wfs);
    WorkFile *wf2 = stwf(wfs);
    printf("Cloned WorkFile : %s\n\n", wfts(wf2));

    WorkTree *wt = initWorkTree();
    printf("Initialized empty WorkTree.\n");
    if (appendWorkTree(wt, fileName, hash, 777) == 0) {
        printf("Successfully added \"%s\" to WorkTree.\n", fileName);
    }
    if (appendWorkTree(wt, fileNameSaveList, sha256file(fileNameSaveList), 777) == 0) {
        printf("Successfully added \"%s\" to WorkTree.\n", fileNameSaveList);
    }

    char *wts = wtts(wt);
    printf("\nCurrent WorkTree :\n%s\n", wts);
    WorkTree *wt2 = stwt(wts);
    printf("Cloned WorkTree :\n%s\n\n", wtts(wt2));

    char *fileNameSaveWorkTree = "work_tree_save.txt";
    printf("Saving it to file %s...\n", fileNameSaveList);
    wttf(wt2, fileNameSaveWorkTree);
    printf("Done!\n");

    printf("Reading it back to test...\n");
    WorkTree *wtReadFromFile = ftwt(fileNameSaveWorkTree);
    printf("\nRead WorkTree :\n%s\n", wtts(wtReadFromFile));

//    TODO: free allocated memory
//    free(hash);
    return 0;
}
