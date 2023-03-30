typedef struct cell {
    char *data;
    struct cell *next;
} Cell;

typedef Cell* List;

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

char *magic_reallocating_fgets(char **bufp, size_t *sizep, FILE *fp);

List *ftol(char *path);

List* listdir ( char* root_dir );

int file_exists(char *file);

void cp(char* to, char* from);

char* hashToPath ( char * hash );

void blobFile ( char* file );
