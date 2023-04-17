#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include "litCore.h"

#define FILE_SIZE 1000
#define WORKTREE_SIZE 10
#define COMMIT_SIZE 10

struct stat st = {0};

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

    char *buffer = calloc(65, sizeof(char));
    int errno = 0;
    // read the data from temporary file
    if (read(fd, buffer, 65) == -1) {
        // Error occurred
        printf("\n read failed with error [%s]\n", strerror(errno));
    }
    buffer[64] = '\0';

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

    *l = NULL;

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
    char *str = calloc(500, sizeof(char));
    for (List tmp = *l; tmp != NULL; tmp = tmp->next) {
        if (strlen(str) > 0) {
            strcat(str, "|");
        }
        strcat(str, ctos(tmp));
    }
    return str;
}

char *ltosNewLine(List *l) {
    char *str = calloc(500, sizeof(char));
    for (List *tmp = l; *tmp != NULL; *tmp = (*tmp)->next) {
        if (strlen(str) > 0) {
            strcat(str, "\n");
        }
        strcat(str, ctos(*tmp));
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
    List tmp = *l;
    while (tmp) {
        if (strcmp(ctos(tmp), str) == 0) {
            return tmp;
        }
        tmp = (tmp)->next;
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
    FILE *fp = fopen(path, "w");

    if (fp == NULL) {
        printf("Le fichier n'a pas pu être lu.");
        return;
    }

    fputs(ltos(l), fp);
    fclose(fp);
}

List *ftol(char *path) {
    FILE *fp = fopen(path, "r");

    if (fp == NULL) {
        printf("Le fichier n'a pas pu être lu.");
        return NULL;
    }

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
    struct stat buffer;
    return (stat(file, &buffer) == 0);
}

void cp(char *to, char *from) {
    FILE *fp1, *fp2;
    char line[BUFSIZ];

    if (!file_exists(from)) {
        fprintf(stderr, "cp: %s: No such file or directory\n", from);
        return;
    }

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

char *hashToBlobPath(char *hash) {
    char *path = malloc(sizeof(char) * (strlen(hash) + 2));
    if (path == NULL) {
        perror("hashToBlobPath: Error allocating memory");
        return NULL;
    }

    strncpy(path, hash, 2);
    path[2] = '/';
    strcpy(path + 3, hash + 2);

    char *blobPath = addLitBlobPath(path);
    free(path);
    return blobPath;
}

char *hashToPathWorkTree(char *hash) {
    char *buff = malloc(sizeof(char) * 100);
    sprintf(buff, "%s.t", hashToBlobPath(hash));
    return buff;
}

char *hashToPathCommit(char *hash) {
    char *buff = malloc(sizeof(char) * 100);
    sprintf(buff, "%s.c", hashToBlobPath(hash));
    return buff;
}

void blobFile(char *file) {
    char *hash = sha256file(file);

    char *ch2 = strdup(hash);
    ch2[2] = '\0';
    char *location = addLitBlobPath(ch2);
    if (!file_exists(location)) {
        char buff[100];
        sprintf(buff, "mkdir -p %s", location);
        system(buff);
    }
    char *ch = hashToBlobPath(hash);
    cp(ch, file);

    free(ch);
    free(location);
    free(ch2);
    free(hash);
}

// Exo4
void litInit() {
    // Create .lit directory
    if (!file_exists(".lit")) {
        system("mkdir .lit");
    }

    // Create .lit/refs directory and its children
    if (!file_exists(".lit/refs")) {
        char *masterPath = addLitRefsPath("master");
        createFile(masterPath);
        free(masterPath);

        char *headPath = addLitRefsPath("HEAD");
        createFile(headPath);
        free(headPath);
    }

    // Init current branch to master by setting .lit/current_branch
    if (!file_exists(".lit/current_branch")) {
        FILE *f = fopen(".lit/current_branch", "w");
        fputs("master", f);
        fclose(f);
    }
}

char *addLitPrefix(char *prefix, char *path) {
    char *location = malloc(strlen(path) + sizeof(char) * 11);
    sprintf(location, ".lit/%s/", prefix);

    if (!file_exists(location)) {
        char buff[256];
        sprintf(buff, "mkdir %s", location);
        system(buff);
    }

    strcat(location, path);
    return location;
}

char *addLitBlobPath(char *path) {
    return addLitPrefix("blob", path);
}

char *addLitRefsPath(char *path) {
    return addLitPrefix("refs", path);
}

void createFile(char *file) {
    char buff[100];
    sprintf (buff, "touch %s ", file);
    system(buff);
}

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
    sprintf(s, "%s\t%s\t%o", wf->name, wf->hash, wf->mode);
    return s;
}

WorkFile *stwf(char *ch) {
    char *name = calloc(100, sizeof(char));
    char *hash = calloc(100, sizeof(char));
    int mode;

    sscanf(ch, "%[^\t]\t%[^\t]\t%o", name, hash, &mode);

    WorkFile *wf = createWorkFile(name);
    free(name);
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
    if (wt->n == wt->size) {
        printf("Erreur : le work tree est plein.\n");
        return 1;
    }

    if (inWorkTree(wt, name) != -1) {
        printf("Erreur : \"%s\" existe déjà dans le work tree.\n", name);
        return 1;
    }

    if (name != NULL) {
        wt->tab[wt->n].name = strdup(name);
    }
    if (hash != NULL) {
        wt->tab[wt->n].hash = strdup(hash);
    }
    wt->tab[wt->n].mode = mode;

    wt->n++;

    return 0;
}

char *wtts(WorkTree *wt) {
    char *s = calloc(FILE_SIZE * wt->n, sizeof(char));

    int i = 0;
    while (i < wt->n) {
        strcat(s, wfts(&(wt->tab[i])));
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
        freeWorkFile(wf);
        wfs = strtok(NULL, "\n");
    }

    return wt;
}

int wttf(WorkTree *wt, char *file) {
    FILE *fp = fopen(file, "w");

    if (fp == NULL) {
        return -1;
    }

    char *wts = wtts(wt);
    fputs(wts, fp);
    fclose(fp);

    free(wts);
    return 0;
}

WorkTree *ftwt(char *file) {
    FILE *fp = fopen(file, "r");

    if (fp == NULL) return NULL;

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
char *hashToFile(char *hash) {
    char *ch2 = strdup(hash);
    ch2[2] = '\0';
    char *location = addLitBlobPath(ch2);
    if (!file_exists(location)) {
        mkdir(location, 0700);
    }
    free(location);
    free(ch2);
    return hashToBlobPath(hash);
}

char *blobWorkTree(WorkTree *wt) {
    char fname[100] = ".lit/temp_XXXXXX";

    litInit();

    mkstemp(fname);
    wttf(wt, fname);

    char *hash = sha256file(fname);
    char *path = hashToFile(hash);

    strcat(path, ".t");
    cp(path, fname);

    free(path);
    unlink(fname);
    return hash;
}

char *joinPath(char *path1, char *path2) {
    char *result = malloc(100);
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
    sprintf(buff, "chmod %o %s", mode, path);
    system(buff);
}

int isFile(const char *path) {
    if (stat(path, &st) != 0) {
        return 0;
    }
    return S_ISREG(st.st_mode);
}

char *saveWorkTree(WorkTree *wt, char *path) {
    for (int i = 0; i < wt->n; i++) {
        char *workingDirectoryPath = joinPath(path, wt->tab[i].name);
        if (isFile(workingDirectoryPath) == 1) {
            // If WF is a file, create a record snapshot of this file
            blobFile(workingDirectoryPath);

            // Save the hash and mode of the file in WF
            wt->tab[i].hash = sha256file(workingDirectoryPath);
            wt->tab[i].mode = getChmod(workingDirectoryPath);
        } else {
            // If WF is a directory, create a new WorkTree representing its contents
            WorkTree *newWt = initWorkTree();
            List *l = listdir(workingDirectoryPath);

            for (Cell *ptr = *l; ptr != NULL; ptr = ptr->next) {
                if (ctos(ptr)[0] == '.') { continue; }

                appendWorkTree(newWt, ctos(ptr), NULL, 0);
            }

            // Save the hash and mode of the sub-WorkTree in WF
            wt->tab[i].hash = saveWorkTree(newWt, workingDirectoryPath);
            wt->tab[i].mode = getChmod(workingDirectoryPath);
        }
        free(workingDirectoryPath);
    }

    // Create a snapshot of the WorkTree and return its hash
    return blobWorkTree(wt);
}

int isWorkTree(char *hash) {
    if (file_exists(strcat(hashToBlobPath(hash), ".t"))) {
        return 1;
    }
    if (file_exists(hashToBlobPath(hash))) {
        return 0;
    }
    return -1;
}

void freeWorkFile(WorkFile *wf) {
    free(wf->name);
    free(wf->hash);
}

void freeWorkTree(WorkTree *wt) {
    for (int i = 0; i < wt->n; i++) {
        freeWorkFile(&(wt->tab[i]));
    }
    free(wt->tab);
    free(wt);
}

void restoreWorkTree(WorkTree *wt, char *path) {
    for (int i = 0; i < wt->n; i++) {
        char *workingDirectoryPath = joinPath(path, wt->tab[i].name);
        char *litPath = hashToBlobPath(wt->tab[i].hash);
        char *hash = wt->tab[i].hash;
        int chmod = wt->tab[i].mode;
        if (isWorkTree(hash) == 0) { //si c’est un fichier
            cp(workingDirectoryPath, litPath);
            setMode(chmod, workingDirectoryPath);
        } else if (isWorkTree(hash) == 1) { //si c’est un repertoire
            mkdir(workingDirectoryPath, chmod);
            strcat(litPath, ".t");
            WorkTree *newWt = ftwt(litPath);
            restoreWorkTree(newWt, workingDirectoryPath);
            freeWorkTree(newWt);
        }
        free(workingDirectoryPath);
        free(litPath);
    }
}

//exo6
kvp *createKeyVal(char *key, char *val) {
    kvp *k = malloc(sizeof(kvp));

    if (k == NULL) {
        perror("createKeyVal: Error allocating memory");
        return NULL;
    }

    if (key != NULL) {
        k->key = strdup(key);
    }
    if (val != NULL) {
        k->value = strdup(val);
    }
    return k;
}

void freeKeyVal(kvp *kv) {
    free(kv->key);
    free(kv->value);
    free(kv);
}

char *kvts(kvp *k) {
    char *buff = calloc(300, sizeof(char));
    sprintf(buff, "%s : %s", k->key, k->value);
    return buff;
}

kvp *stkv(char *str) {
    char key[100];
    char val[100];

    sscanf(str, "%s : %[^\0]", key, val);

    return createKeyVal(key, val);
}

Commit *initCommit() {
    Commit *c = malloc(sizeof(Commit));

    if (c == NULL) {
        perror("initCommit: Error allocating memory");
        return NULL;
    }

    c->T = calloc(COMMIT_SIZE, sizeof(kvp *));
    c->size = COMMIT_SIZE;
    for (int i = 0; i < c->size; i++) {
        c->T[i] = NULL;
    }
    c->n = 0;
    return c;
}

Commit *createCommit(char *hash) {
    Commit *c = initCommit();
    commitSet(c, "tree", hash);
    return c;
}

unsigned long hash(char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

void commitSet(Commit *c, char *key, char *value) {
    if (c->n == c->size) {
        printf("Erreur : le commit est plein.\n");
        return;
    }

    int index = hash(key) % c->size;
    if (c->T[index] == NULL) {
        while (c->T[index] != NULL) {
            index = (index + 1) % c->size; //probing linéaire
        }
        c->T[index] = createKeyVal(key, value);
        c->n++;
    }
}

char *commitGet(Commit *c, char *key) {
    int index = hash(key) % c->size;
    int attempt = 0;
    while (c->T[index] != NULL && attempt < c->size) {
        if (strcmp(c->T[index]->key, key) == 0) {
            return c->T[index]->value;
        }
        index = (index + 1) % c->size;
        attempt = attempt + 1;
    }
    return NULL;
}

char *cts(Commit *c) {
    char *str = calloc(200 * c->n, sizeof(char));

    for (int i = 0; i < c->size; i++) {
        if (c->T[i] != NULL) {
            strcat(str, kvts(c->T[i]));
            strcat(str, "\n");
        }
    }

    return str;
}

Commit *stc(char *ch) {
    Commit *c = initCommit();

    char *cs = strtok(ch, "\n");
    while (cs != NULL) {
        kvp *elem = stkv(cs);
        commitSet(c, elem->key, elem->value);
        freeKeyVal(elem);
        cs = strtok(NULL, "\n");
    }

    return c;
}

void ctf(Commit *c, char *file) {
    FILE *fp = fopen(file, "w");

    if (fp == NULL) {
        return;
    }

    fputs(cts(c), fp);
    fclose(fp);
}

Commit *ftc(char *file) {
    FILE *fp = fopen(file, "r");

    if (fp == NULL) {
        return NULL;
    }

    char *lines = calloc(FILE_SIZE, sizeof(char));

    char line[FILE_SIZE];

    while (fgets(line, FILE_SIZE, fp) != NULL) {
        if (strlen(lines) > 0) {
            strcat(lines, "\n");
        }
        strcat(lines, line);
    }

    fclose(fp);

    return stc(lines);
}

char *blobCommit(Commit *c) {
    char fname[100] = ".lit/temp_XXXXXX";

    litInit();

    mkstemp(fname);
    ctf(c, fname);

    char *hash = sha256file(fname);
    char *path = hashToFile(hash);

    strcat(path, ".c");
    cp(path, fname);

    free(path);
    unlink(fname);
    return hash;
}

void freeCommit(Commit *c) {
    for (int i = 0; i < c->n; i++) {
        kvp *pair = c->T[i];
        if (pair != NULL) {
            free(pair->key);
            free(pair->value);
            free(pair);
        }
    }
    free(c->T);
    free(c);
}

//exo7
void createUpdateRef(char *refName, char *hash) {
    char buff[100];
    char *path = addLitRefsPath(refName);
    if (hash == NULL) {
        hash = "";
    }
    sprintf(buff, "echo %s > %s", hash, path);
    system(buff);
    free(path);
}

void deleteRef(char *refName) {
    char *path = addLitRefsPath(refName);

    char buff[256];
    sprintf(buff, "%s", path);

    if (!file_exists(buff)) {
        printf("The reference %s does not exist", path);
        return;
    }

    sprintf(buff, "rm %s", path);
    system(buff);

    free(path);
}

char *getRef(char *ref_name) {
    char *path = addLitRefsPath(ref_name);

    char *result = calloc(256, sizeof(char));

    char buff[256];
    sprintf(buff, "%s", path);

    if (!file_exists(buff)) {
        printf("The reference %s does not exist", path);
        return NULL;
    }

    free(path);

    FILE *fp = fopen(buff, "r");
    if (fp == NULL) {
        printf("Error opening file.\n");
        return NULL;
    }
    fgets(result, 256, fp);
    fclose(fp);

    if (strlen(result) > 0 && strstr(result, "\n") != NULL) {
        result = strtok(result, "\n");
    }

    return result;
}

void litListRefs() {
    printf("REFS : \n");
    if (file_exists(".lit/refs")) {
        List *L = listdir(".lit/refs");
        for (Cell *ptr = *L; ptr != NULL; ptr = ptr->next) {
            if (ctos(ptr)[0] == '.')
                continue;
            char *content = getRef(ctos(ptr));
            printf("− %s \t %s \n", ctos(ptr), content);
        }
    }
}

void litAdd(char *file_or_folder) {
    WorkTree *wt;

    // Initialize staging area, either from blank or from "add" file
    if (!file_exists(".lit/add")) {
        createFile(".lit/add");
        wt = initWorkTree();
    } else {
        wt = ftwt(".lit/add");
    }

    // Add file or folder to staging area
    if (file_exists(file_or_folder)) {
        appendWorkTree(wt, file_or_folder, NULL, 0);
        wttf(wt, ".lit/add");
    } else {
        printf("File or folder %s does not exist\n", file_or_folder);
    }
}

void litPrintStagingArea() {
    printf("Staging area : \n");
    if (file_exists(".lit/add")) {
        WorkTree *wt = ftwt(".lit/add");
        printf("%s \n", wtts(wt));
    }
}

void litClearStagingArea() {
    if (file_exists(".lit/add")) system("rm .lit/add");
}

void litCommit(char *branchName, char *message) {
    litInit();

    char *branchPath = addLitRefsPath(branchName);

    char buff[256];
    sprintf (buff, "%s", branchPath);

    if (!file_exists(buff)) {
        printf("La branche n'existe pas\n");
    } else {
        char *last_hash = getRef(branchName);
        char *head_hash = getRef("HEAD");
        if (strcmp(last_hash, head_hash) != 0) {
            printf("HEAD doit pointer sur le commit de la branche\n");
        } else {
            WorkTree *wt = ftwt(".lit/add");
            if (wt == NULL) wt = initWorkTree();
            char *hashWt = saveWorkTree(wt, ".");

            Commit *c = createCommit(hashWt);
            if (strlen(last_hash) != 0) {
                commitSet(c, "predecessor", last_hash);
            }
            if (message != NULL) {
                commitSet(c, "message", message);
            }

            char *hashC = blobCommit(c);
            createUpdateRef(branchName, hashC);
            createUpdateRef("HEAD", hashC);
            litClearStagingArea();

            free(hashC);
            freeCommit(c);
            free(hashWt);
            freeWorkTree(wt);
        }

        free(head_hash);
        free(last_hash);
    }

    free(branchPath);
}

//exo 8
int branchExists(char *branch) {
    List *refs = listdir(".lit/refs");
    return searchList(refs, branch) != NULL;
}

void createBranch(char *branch) {
    if (branchExists(branch)) {
        printf("Branch %s already exists!\n", branch);
        return;
    }
    char *hash = getRef("HEAD");
    createUpdateRef(branch, hash);
    free(hash);
}

char *getCurrentBranch() {
    FILE *f = fopen(".lit/current_branch", "r");
    char *buff = calloc(100, sizeof(char));
    fscanf(f, "%s", buff);
    return buff;
}

void printBranch(char *branch) {
    if (!branchExists(branch)) {
        printf("Branch %s does not exist!\n", branch);
        return;
    }

    char *commit_hash = getRef(branch);
    char *commit_path = hashToPathCommit(commit_hash);
    Commit *c = ftc(commit_path);
    while (c != NULL) {
        if (commitGet(c, "message") != NULL)
            printf("%s −> %s \n", commit_hash, commitGet(c, "message"));
        else
            printf("%s \n", commit_hash);
        if (commitGet(c, "predecessor") != NULL) {
            commit_hash = commitGet(c, "predecessor");
            commit_path = hashToPathCommit(commit_hash);
            c = ftc(commit_path);
        } else {
            c = NULL;
        }
    }
    free(commit_path);
    free(commit_hash);
}

List *branchList(char *branch) {
    List *L = initList();
    char *commit_hash = getRef(branch);
    if (commit_hash == NULL) {
        printf("Branch %s is empty\n", branch);
        return NULL;
    }
    char *commit_path = hashToPathCommit(commit_hash);
    Commit *c = ftc(commit_path);
    while (c != NULL) {
        insertFirst(L, buildCell(commit_hash));
        if (commitGet(c, "predecessor") != NULL) {
            commit_hash = commitGet(c, "predecessor");
            commit_path = hashToPathCommit(commit_hash);
            c = ftc(commit_path);
        } else {
            c = NULL;
        }
    }
    free(commit_path);
    free(commit_hash);
    return L;
}

List *getAllCommits() {
    List *l = initList();
    List *content = listdir(".lit/refs");
    for (Cell *ptr = *content; ptr != NULL; ptr = ptr->next) {
        if (ctos(ptr)[0] == '.') { continue; }
        List *list = branchList(ctos(ptr));
        if (list == NULL) { continue; }
        Cell *cell = *list;
        while (cell != NULL) {
            if (searchList(l, ctos(cell)) == NULL) {
                insertFirst(l, buildCell(ctos(cell)));
            }
            cell = cell->next;
        }
    }
    return l;
}

//exo9
void restoreCommit(char *commitHash) {
    char *commitPath = hashToPathCommit(commitHash);
    Commit *c = ftc(commitPath);

    char *treeHash = commitGet(c, "tree");
    char *treePath = hashToBlobPath(treeHash);
    strcat(treePath, ".t");

    WorkTree *wt = ftwt(treePath);
    restoreWorkTree(wt, ".");

    free(treePath);
    free(treeHash);
    free(commitPath);
}

void litCheckoutBranch(char *branchName) {
    if (!branchExists(branchName)) {
        printf("Branch %s does not exist!\n", branchName);
        return;
    }

    FILE *f = fopen(".lit/current_branch", "w");
    fprintf(f, "%s", branchName);
    fclose(f);

    char *commitHash = getRef(branchName);
    createUpdateRef("HEAD", commitHash);
    restoreCommit(commitHash);

    free(commitHash);
}

List *filterList(List *l, char *pattern) {
    List *filtered = initList();
    for (Cell *ptr = *l; ptr != NULL; ptr = ptr->next) {
        char *c = strdup(ctos(ptr));
        c[strlen(pattern)] = '\0';
        if (strcmp(c, pattern) == 0) {
            insertFirst(filtered, buildCell(ctos(ptr)));
        }
        free(c);
    }
    return filtered;
}

void litCheckoutCommit(char *pattern) {
    List *commits = getAllCommits();
    List *filteredList = filterList(commits, pattern);
    if ((*filteredList)->next == NULL) {
        char *commit_hash = ctos(listGet(filteredList, 0));
        createUpdateRef("HEAD", commit_hash);
        restoreCommit(commit_hash);
    } else {
        if ((*filteredList) == NULL) {
            printf("No pattern matching.\n");
        } else {
            printf("Multiple matchings found : \n");
            for (Cell *ptr = *filteredList; ptr != NULL; ptr = ptr->next) {
                printf("−> %s \n", ctos(ptr));
            }
        }
    }
}

//exo11
WorkTree *mergeWorkTrees(WorkTree *wt1, WorkTree *wt2, List **conflicts) {
    WorkTree *mergedWorkTree = initWorkTree();

    for (int i = 0; i < wt1->n; i++) {
        int foundConflict = 0;
        for (int j = 0; j < wt2->n; j++) {
            // If the names are the same and hashes are different, add the file to the conflicts list
            if (strcmp(wt1->tab[i].name, wt2->tab[j].name) == 0 && strcmp(wt1->tab[i].hash, wt2->tab[j].hash) != 0) {
                foundConflict = 1;
                insertFirst(*conflicts, buildCell(wt1->tab[i].name));
                break;
            }
        }

        if (!foundConflict && inWorkTree(mergedWorkTree, wt1->tab[i].name) == -1) {
            appendWorkTree(mergedWorkTree, wt1->tab[i].name, wt1->tab[i].hash, wt1->tab[i].mode);
        }
    }
    for (int i = 0; i < wt2->n; i++) {
        if (inWorkTree(mergedWorkTree, wt2->tab[i].name) == -1 && searchList(*conflicts, wt2->tab[i].name) == NULL) {
            appendWorkTree(mergedWorkTree, wt2->tab[i].name, wt2->tab[i].hash, wt2->tab[i].mode);
        }
    }

    return mergedWorkTree;
}

void litCommitMerge(char *currentBranchName, char *remoteBranchName, WorkTree *wt, char *message) {
    litInit();

    char *branchPath = addLitRefsPath(currentBranchName);

    char buff[256];
    sprintf (buff, "%s", branchPath);

    if (!file_exists(buff)) {
        printf("La branche n'existe pas\n");
    } else {
        char *currentBranchLastHash = getRef(currentBranchName);
        char *headLastHash = getRef("HEAD");
        if (strcmp(currentBranchLastHash, headLastHash) != 0) {
            printf("HEAD doit pointer sur le commit de la branche\n");
        } else {
            char *remoteBranchLastHash = getRef(remoteBranchName);

            char *hashWt = blobWorkTree(wt);

            Commit *c = createCommit(hashWt);
            if (strlen(currentBranchLastHash) != 0) {
                commitSet(c, "predecessor", currentBranchLastHash);
            }
            if (strlen(remoteBranchLastHash) != 0) {
                commitSet(c, "merged_predecessor", remoteBranchLastHash);
            }
            if (message != NULL) {
                commitSet(c, "message", message);
            }

            char *hashC = blobCommit(c);
            createUpdateRef(currentBranchName, hashC);
            createUpdateRef("HEAD", hashC);

            deleteRef(remoteBranchName);

            restoreWorkTree(wt, ".");

            free(hashC);
            freeCommit(c);
            free(hashWt);
            free(remoteBranchLastHash);
        }

        free(headLastHash);
        free(currentBranchLastHash);
    }

    free(branchPath);
}

List *merge(char *remoteBranch, char *message) {
    char *currentBranch = getCurrentBranch();
    char *currentBranchRef = getRef(currentBranch);
    char *currentBranchBlobPath = hashToPathCommit(currentBranchRef);
    Commit *currentBranchLatestCommit = ftc(currentBranchBlobPath);
    char *currentBranchLatestCommitHash = commitGet(currentBranchLatestCommit, "tree");
    char *currentBranchWorkTreeBlobPath = hashToPathWorkTree(currentBranchLatestCommitHash);
    WorkTree *currentBranchWorkTree = ftwt(currentBranchWorkTreeBlobPath);

    char *remoteBranchRef = getRef(remoteBranch);
    char *remoteBranchBlobPath = hashToPathCommit(remoteBranchRef);
    Commit *remoteBranchLatestCommit = ftc(remoteBranchBlobPath);
    char *remoteBranchLatestCommitHash = commitGet(remoteBranchLatestCommit, "tree");
    char *remoteBranchWorkTreeBlobPath = hashToPathWorkTree(remoteBranchLatestCommitHash);
    WorkTree *remoteBranchWorkTree = ftwt(remoteBranchWorkTreeBlobPath);

    List *conflicts = initList();
    WorkTree *mergedWorkTree = mergeWorkTrees(
            currentBranchWorkTree,
            remoteBranchWorkTree,
            &conflicts
    );

    if (*conflicts == NULL) {
        litCommitMerge(getCurrentBranch(), remoteBranch, mergedWorkTree, message);
    }

    freeWorkTree(mergedWorkTree);

    freeWorkTree(remoteBranchWorkTree);
    free(remoteBranchWorkTreeBlobPath);
    free(remoteBranchLatestCommitHash);
    freeCommit(remoteBranchLatestCommit);
    free(remoteBranchBlobPath);
    free(remoteBranchRef);

    freeWorkTree(currentBranchWorkTree);
    free(currentBranchWorkTreeBlobPath);
    free(currentBranchLatestCommitHash);
    freeCommit(currentBranchLatestCommit);
    free(currentBranchBlobPath);
    free(currentBranchRef);
    free(currentBranch);

    if (*conflicts != NULL) {
        printf("Error: Merge conflicts must be resolved before merging.");
        return conflicts;
    }

    freeList(conflicts);

    return NULL;
}

void createDeletionCommit(char *branch, List *conflicts, char *message) {
    char *currentBranch = getCurrentBranch();

    litCheckoutBranch(branch);

    char *branchRef = getRef(branch);
    char *branchBlobPath = hashToPathCommit(branchRef);
    Commit *branchLatestCommit = ftc(branchBlobPath);
    char *branchLatestCommitHash = commitGet(branchLatestCommit, "tree");
    char *branchWorkTreeBlobPath = hashToPathWorkTree(branchLatestCommitHash);
    WorkTree *branchWorkTree = ftwt(branchWorkTreeBlobPath);

    litClearStagingArea();

    for (int i = 0; i < branchWorkTree->n; i++) {
        if (searchList(conflicts, branchWorkTree->tab[i].name) == NULL) {
            litAdd(branchWorkTree->tab[i].name);
        }
    }

    litCommit(branch, message);

    litCheckoutBranch(currentBranch);

    freeWorkTree(branchWorkTree);
    free(branchWorkTreeBlobPath);
    free(branchLatestCommitHash);
    freeCommit(branchLatestCommit);
    free(branchBlobPath);
    free(branchRef);
    free(currentBranch);
}
