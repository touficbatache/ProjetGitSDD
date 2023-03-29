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

//exo1
int hashFile(char *source, char *dest);

char *sha256file(char *file);

//exo2
List *initList();

Cell *buildCell(char *ch);

void insertFirst(List *L, Cell *C);

char *ctos(Cell *c);

char *ltos(List *L);

Cell *listGet(List *L, int i);

Cell *searchList(List *L, char *str);

List *stol(char *s);

void ltof(List *L, char *path);

List *ftol(char *path);

//exo4
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