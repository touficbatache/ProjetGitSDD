#include <stdio.h>
#include <string.h>
#include "litCore.c"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <command> [<args>]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "init") == 0) {
        litInit();
        return 0;
    }

    if (!file_exists(".lit")) {
        printf("lit is not initialized. Initialize it with:\n%s init\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "list-refs") == 0) {
        litListRefs();
    } else if (strcmp(argv[1], "create-ref") == 0) {
        if (argc != 4) {
            printf("Usage: %s create-ref <name> <hash>\n", argv[0]);
            return 1;
        }
        createUpdateRef(argv[2], argv[3]);
    } else if (strcmp(argv[1], "delete-ref") == 0) {
        if (argc != 3) {
            printf("Usage: %s delete-ref <name>\n", argv[0]);
            return 1;
        }
        deleteRef(argv[2]);
    } else if (strcmp(argv[1], "add") == 0) {
        if (argc < 3) {
            printf("Usage: %s add <elem> [<elem2> <elem3> ...]\n", argv[0]);
            return 1;
        }
        for (int i = 2; i < argc; i++) {
            litAdd(argv[i]);
        }
    } else if (strcmp(argv[1], "list-add") == 0) {
        litPrintStagingArea();
    } else if (strcmp(argv[1], "clear-add") == 0) {
        litClearStagingArea();
    } else if (strcmp(argv[1], "commit") == 0) {
        if (argc < 3) {
            printf("Usage: %s commit <branch name> [-m <message>]\n", argv[0]);
            return 1;
        }
        char *message = NULL;
        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
                message = argv[i + 1];
                break;
            }
        }
        litCommit(argv[2], message);
    } else if (strcmp(argv[1], "branch") == 0) {
        if (argc != 3) {
            printf("Usage: %s branch <name>\n", argv[0]);
            return 1;
        }
        createBranch(argv[2]);
    } else if (strcmp(argv[1], "branch-print") == 0) {
        if (argc != 3) {
            printf("Usage: %s branch-print <name>\n", argv[0]);
            return 1;
        }
        printBranch(argv[2]);
    } else if (strcmp(argv[1], "checkout−branch") == 0) {
        if (argc != 3) {
            printf("Usage: %s checkout-branch <name>\n", argv[0]);
            return 1;
        }
        litCheckoutBranch(argv[2]);
    } else if (strcmp(argv[1], "checkout-commit") == 0) {
        if (argc != 3) {
            printf("Usage: %s checkout-commit <name>\n", argv[0]);
            return 1;
        }
        litCheckoutCommit(argv[2]);
    } else {
        printf("Unrecognized command: %s\n", argv[1]);
        return 1;
    }

    return 0;
}
