typedef struct cell {
    char *data;
    struct cell *next;
} Cell;

typedef Cell *List;

typedef struct {
    char *name;
    char *hash;
    int mode;
} WorkFile;

typedef struct {
    WorkFile *tab;
    int size;
    int n;
} WorkTree;

typedef struct key_value_pair {
    char *key;
    char *value;
} kvp;

typedef struct hash_table {
    kvp **T;
    int n;
    int size;
} HashTable;

typedef HashTable Commit;

//exo1
int hashFile(char *source, char *dest);

char *sha256file(char *file);

//exo2
List *initList();

Cell *buildCell(char *ch);

void insertFirst(List *l, Cell *C);

char *ctos(Cell *c);

char *ltos(List *L);

Cell *listGet(List *L, int i);

Cell *searchList(List *L, char *str);

List *stol(char *s);

void ltof(List *L, char *path);

List *ftol(char *path);

//exo3
List *listdir(char *root_dir);

void freeList(List *L);

int file_exists(char *file);

void cp(char *to, char *from);

char *hashToPath(char *hash);

void blobFile(char *file);

//exo4
void litInit();

char *addLitPrefix(char *prefix, char *path);

char *addLitBlobPath(char *path);

char *addLitRefsPath(char *path);

void createFile(char *file);

WorkFile *createWorkFile(char *name);

char *wfts(WorkFile *wf);

WorkFile *stwf(char *ch);

WorkTree *initWorkTree();

int inWorkTree(WorkTree *wt, char *name);

int appendWorkTree(WorkTree *wt, char *name, char *hash, int mode);

char *wtts(WorkTree *wt);

WorkTree *stwt(char *str);

int wttf(WorkTree *wt, char *file);

WorkTree *ftwt(char *file);

//exo5
char *blobWorkTree(WorkTree *wt);

char *joinPath(char *path1, char *path2);

int getChmod(const char *path);

void setMode(int mode, char *path);

char *saveWorkTree(WorkTree *wt, char *path);

int isWorkTree(char *hash);

void freeWorkTree(WorkTree *wt);

void restoreWorkTree(WorkTree *wt, char *path);

//exo6
kvp *createKeyVal(char *key, char *val);

void freeKeyVal(kvp *kv);

char *kvts(kvp *k);

kvp *stkv(char *str);

Commit *initCommit();

Commit *createCommit(char *hash);

unsigned long hash(unsigned char *str);

void commitSet(Commit *c, char *key, char *value);

char *commitGet(Commit *c, char *key);

char *cts(Commit *c);

Commit *stc(char *ch);

void ctf(Commit *c, char *file);

Commit *ftc(char *file);

char *blobCommit(Commit *c);

void freeCommit(Commit *c);

//exo7
void litInit();

void createUpdateRef(char *ref_name, char *hash);

void deleteRef(char *ref_name);

char *getRef(char *ref_name);

void litListRefs();

void litAdd(char *file_or_folder);

void litPrintStagingArea();

void litClearStagingArea();

void litCommit(char *branchName, char *message);

//exo8
int branchExists(char *branch);

void createBranch(char *branch);

char *getCurrentBranch();

char *hashToPathCommit(char *hash);

void printBranch(char *branch);

List *branchList(char *branch);

List *getAllCommits();
