#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lit.h"

void acceptChanges(int hasAcceptedLocalChanges, List *conflicts, char *remoteBranch) {
    char deletingConflictsCommitMessage[100];
    if (hasAcceptedLocalChanges == 1) {
        sprintf(deletingConflictsCommitMessage, "Remove conflicting files from remote branch '%s'.", remoteBranch);
        createDeletionCommit(remoteBranch, conflicts, deletingConflictsCommitMessage);
    } else {
        sprintf(deletingConflictsCommitMessage, "Remove conflicting files from local branch '%s'.", getCurrentBranch());
        createDeletionCommit(getCurrentBranch(), conflicts, deletingConflictsCommitMessage);
    }
}

int showMergeConflictsMessage(List *conflicts, char *remoteBranch, char *message) {
    printf("\n\nThe following files are causing merge conflicts:\n%s\n\nChoose one of these options:\n", ltosNewLine(conflicts));
    printf("1. Accept local changes on current branch '%s'\n", getCurrentBranch());
    printf("2. Accept changes from remote branch '%s'\n", remoteBranch);
    printf("3. Pick files' changes individually\n");

    int selectedOption;
    printf("\nOption number: ");
    scanf("%d", &selectedOption);

    if (selectedOption == 1) {
        acceptChanges(1, conflicts, remoteBranch);
        merge(remoteBranch, message);
    } else if (selectedOption == 2) {
        acceptChanges(0, conflicts, remoteBranch);
        merge(remoteBranch, message);
    } else if (selectedOption == 3) {
        printf("For each file, accept either local or remote changes.\n");

        List *localChanges = initList();
        List *remoteChanges = initList();
        for (List tmp = *conflicts; tmp != NULL; tmp = tmp->next) {
            printf("\nFile: %s\n", ctos(tmp));
            printf("1. Accept local changes\t\t2. Accept remote changes");

            int fileSelectedOption;
            printf("\nOption number: ");
            scanf("%d", &fileSelectedOption);

            if (fileSelectedOption == 1) {
                insertFirst(localChanges, buildCell(ctos(tmp)));
            } else if (fileSelectedOption == 2) {
                insertFirst(remoteChanges, buildCell(ctos(tmp)));
            }
        }

        acceptChanges(1, localChanges, remoteBranch);
        acceptChanges(0, remoteChanges, remoteBranch);

        merge(remoteBranch, message);

        free(remoteChanges);
        free(localChanges);
    } else {
        printf("Unrecognized option.\n");
        return showMergeConflictsMessage(conflicts, remoteBranch, message);
    }

    return 0;
}

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
    } else if (strcmp(argv[1], "checkout-branch") == 0) {
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
    } else if (strcmp(argv[1], "merge") == 0) {
        if (argc != 4) {
            printf("Usage: %s merge <branch> <message>\n", argv[0]);
            return 1;
        }
        List *conflicts = merge(argv[2], argv[3]);
        if (conflicts == NULL) {
            printf("Successfully merged '%s' into '%s'.\nCommit message: \"%s\".\n", argv[2], getCurrentBranch(), argv[3]);
        } else {
            return showMergeConflictsMessage(conflicts, argv[2], argv[3]);
        }
    } else {
        printf("Unrecognized command: %s\n", argv[1]);
        return 1;
    }

    return 0;
}
