#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include "main.h"

#define FILE_SIZE 1000
#define WORKTREE_SIZE 10

//Exo1

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

//Exo2

List *initList() {
    List *l = malloc(sizeof(List));

    if (l == NULL) {
        perror("initList: Error allocating memory");
        return NULL;
    }

    return l;
}

Cell *buildCell(char *ch) {
    Cell *cell = malloc(sizeof(Cell));

    if (cell == NULL) {
        perror("buildCell: Error allocating memory");
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

//Exo3

List *listdir(char *root_dir) {
    DIR *dp;
    struct dirent *ep;
    List *L = initList();
    *L = NULL;
    Cell *temp_cell;
    dp = opendir(root_dir);
    if (dp != NULL) {
        while ((ep = readdir(dp)) != NULL) {
            temp_cell = buildCell(ep->d_name);
            insertFirst(L, temp_cell);
            List ptr = *L;
            while (ptr != NULL) {
                ptr = ptr->next;
            }
        }
        (void) closedir(dp);
    } else {
        perror("Couldn't open the directory");
        return NULL;
    }

    return L;
}

void freeList(List *L) {
    List ptr = *L;
    while (ptr != NULL) {
        Cell *temp_cell = ptr;
        ptr = ptr->next;
        free(temp_cell->data);
        free(temp_cell);
    }
    *L = NULL;
}

int file_exists(char *file) {
    List *dir_list = listdir(".");
    if (dir_list == NULL) {
        return 0;
    }
    List ptr = *dir_list;
    while (ptr != NULL) {
        if (strcmp(ptr->data, file) == 0) {
            freeList(dir_list);
            return 1;
        }
        ptr = ptr->next;
    }
    freeList(dir_list);
    return 0;
}

void cp(char *to, char *from) {
    FILE *fp1, *fp2;
    char line[BUFSIZ];

    // on ne peut pas utiliser cette fonction car on n'est pas dans le repertoire courant.. wtf?
//    if (!file_exists(from)) {
//        fprintf(stderr, "cp: %s: No such file or directory\n", from);
//        return;
//    }

    fp1 = fopen(from, "r");
    if (fp1 == NULL) {
        perror("cp: Error opening source file");
        return;
    }

    fp2 = fopen(to, "w");
    if (fp2 == NULL) {
        perror("cp: Error opening destination file");
        fclose(fp1);
        return;
    }

    while (fgets(line, BUFSIZ, fp1) != NULL) {
        fputs(line, fp2);
    }

    fclose(fp1);
    fclose(fp2);
}

char *hashToPath(char *hash) {
    char *path = malloc(sizeof(char) * (strlen(hash) + 2));
    if (path == NULL) {
        perror("hashToPath: Error allocating memory");
        return NULL;
    }

    strncpy(path, hash, 2);
    path[2] = '/';
    strcpy(path + 3, hash + 2);

    return path;
}

void blobFile(char *file) {
    char *hash = sha256file(file);
    char *path = hashToPath(hash);

    char *command = malloc(sizeof(char) * (strlen(path) + 10));
    if (command == NULL) {
        perror("blobFile: Error allocating memory");
        return;
    }

    sprintf(command, "mkdir -p %s", path);
    system(command);
    free(command);
    free(path);
}

// Exo4

WorkFile *createWorkFile(char *name) {
    WorkFile *workFile = malloc(sizeof(WorkFile));

    if (workFile == NULL) {
        perror("createWorkFile: Error allocating memory");
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
        perror("initWorkTree: Error allocating memory");
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

//exo5

char *blobWorkTree(WorkTree *wt) {
    char fname[100] = "/tmp/myfileXXXXXX";
    mkstemp(fname);
    wttf(wt, fname);

    char *hash = sha256file(fname);
    char *path = hashToPath(hash);
    // pq on ne mettrait pas des pointeurs vers hash et path ? ils sont déjà calculés dans blobFile
    blobFile(fname);

    strcat (path, ".t");
    cp(path, fname);

    unlink(fname);
    return hash;
}

char *joinPath(char *path1, char *path2) {
    char *result = malloc(strlen(path1) + strlen(path2) + 1);
    if (result == NULL) {
        printf("Error: unable to allocate memory\n");
        return NULL;
    }
    strcpy (result, path1); // Copy path1 to result
    strcat (result, "/"); // Append a slash to result
    strcat (result, path2); // Append path2 to result
    return result;
}

int getChmod(const char *path) {
    struct stat ret;

    if (stat(path, &ret) == -1) { return -1; }

    return (ret.st_mode & S_IRUSR) | (ret.st_mode & S_IWUSR) | (ret.st_mode & S_IXUSR) | /*owner*/
           (ret.st_mode & S_IRGRP) | (ret.st_mode & S_IWGRP) | (ret.st_mode & S_IXGRP) | /*group*/
           (ret.st_mode & S_IROTH) | (ret.st_mode & S_IWOTH) | (ret.st_mode & S_IXOTH); /*other*/
}

void setMode(int mode, char *path) {
    char buff[100];
    sprintf(buff, "chmod %d %s", mode, path);
    system(buff);
}

char *saveWorkTree(WorkTree *wt, char *path) {
    for (int i = 0; i < wt->n; i++) {
        char *fullpath = joinPath(path, wt->tab[i].name);
        if (file_exists(fullpath) == 1) {
            // If WF is a file, create a record snapshot of this file
            blobFile(fullpath);

            // Save the hash and mode of the file in WF
            wt->tab[i].hash = sha256file(fullpath);
            wt->tab[i].mode = getChmod(fullpath);
        } else {
            // If WF is a directory, create a new WorkTree representing its contents
            WorkTree *wt2 = initWorkTree();
            List *l = listdir(fullpath);

            for (Cell *ptr = *l; ptr != NULL; ptr = ptr->next) {
                if (ptr->data[0] == '.') {
                    continue;
                }

                appendWorkTree(wt2, ptr->data, 0, NULL);
            }

            // Save the hash and mode of the sub-WorkTree in WF
            wt->tab[i].hash = saveWorkTree(wt2, fullpath);
            wt->tab[i].mode = getChmod(fullpath);
        }
        free(fullpath);
    }

    // Create a snapshot of the WorkTree and return its hash
    return blobWorkTree(wt);
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

//    List *l04 = listdir("/users/Etu5/28725545/LU2IN006");
//    printf("List04 data is [%s]\n", ltos(l04));
//
//    printf("Attendu : 0\tObtenu : %d\n", searchList("/users/Etu5/28725545/LU2IN006", "bday.py"));
//    printf("Attendu : 1\tObtenu : %d\n", searchList("/users/Etu5/28725545/Document", "bday.py"));

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

    blobWorkTree(wt);

//    TODO: free allocated memory
//    free(hash);
    return 0;
}
