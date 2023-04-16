#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "litCore.c"
#include "litTest.h"

int main() {
//    char *wtHash = testWorkTree();
//
//    testCommit(wtHash);

    litInit();
    litAdd("test.txt");
    litAdd("test2.txt");
    litAdd("testdir");

    litPrintStagingArea();

    litCommit("master", "This is a random commit test message");

    printf("Branch contents :\n");
    printBranch(getCurrentBranch());

    printf("Commits :\n%s\n", ltos(getAllCommits()));

//    createBranch("testbranch");
//    printf("Current branch is %s\n", getCurrentBranch());

    return 0;
}

char *testWorkTree() {
    WorkTree *wt = initWorkTree();
    printf("Initialized empty WorkTree.\n");

    char *file1 = "test.txt";
    if (appendWorkTree(wt, file1, 0, NULL) == 0) {
        printf("Successfully added \"%s\" to WorkTree.\n", file1);
    }

    char *file2 = "test2.txt";
    if (appendWorkTree(wt, file2, 0, NULL) == 0) {
        printf("Successfully added \"%s\" to WorkTree.\n", file2);
    }

    char *dir1 = "testdir";
    if (appendWorkTree(wt, dir1, 0, NULL) == 0) {
        printf("Successfully added \"%s\" to WorkTree.\n", dir1);
    }

    printf("\nSaving WorkTree... :\n%s\n", wtts(wt));
    char *wtHash = saveWorkTree(wt, ".");

    // To test restore function, uncomment lines below:
    // printf("Saved WorkTree. Restoring in 5s...");
    // sleep(5);
    //
    // restoreWorkTree(wt, ".");
    // printf("\nRestoring WorkTree... :\n%s\n", wtts(wt));

    printf("\n");

    return wtHash;
}

void testCommit(char *wtHash) {
    Commit *c = createCommit(wtHash);
    commitSet(c, "author", "Toufic Batache");
    commitSet(c, "message", "Ceci est un commit de test");
    printf("Created commit :\n%s\n", cts(c));

    printf("Blobbing commit...\n");
    char *cHash = blobCommit(c);
    char *path = hashToPath(cHash);
    strcat(path, ".c");
    Commit *blobbedC = ftc(path);
    printf("Blobbed commit. Now showing blobbed version :\n%s\n", cts(blobbedC));

    printf("Trying to get the author... Author : %s\n", commitGet(c, "author"));

    // To free the commit, uncomment line below:
    // freeCommit(c);
}

//int main() {
//    char *fileName = "test.txt";
//    char *hash = sha256file(fileName);
//    printf("Data read back from temporary file is [%s]\n", hash);
//
//    List *l = initList();
//    insertFirst(l, buildCell("world"));
//    insertFirst(l, buildCell("Hello"));
//    char *stringList = ltos(l);
//    printf("List data is [%s]\n", stringList);
//
//    printf("Element at index 0 is [%s]\n", ctos(listGet(l, 0)));
//    printf("Element at index 1 is [%s]\n", ctos(listGet(l, 1)));
//
//    List *l2 = stol(stringList);
//    printf("List2 data is [%s]\n", ltos(l2));
//
//    char *fileNameSaveList = "testingC.txt";
//    ltof(l, fileNameSaveList);
//    List *l3 = ftol(fileNameSaveList);
//    printf("List3 data is [%s]\n", ltos(l3));
//
////    List *l04 = listdir("/users/Etu5/28725545/LU2IN006");
////    printf("List04 data is [%s]\n", ltos(l04));
////
////    printf("Attendu : 0\tObtenu : %d\n", searchList("/users/Etu5/28725545/LU2IN006", "bday.py"));
////    printf("Attendu : 1\tObtenu : %d\n", searchList("/users/Etu5/28725545/Document", "bday.py"));
//
//    WorkFile *wf = createWorkFile("Test work file");
//    char *wfs = wfts(wf);
//    printf("\nCreated WorkFile : %s\n\n", wfs);
//    WorkFile *wf2 = stwf(wfs);
//    printf("Cloned WorkFile : %s\n\n", wfts(wf2));
//
//    WorkTree *wt = initWorkTree();
//    printf("Initialized empty WorkTree.\n");
//    if (appendWorkTree(wt, fileName, hash, 777) == 0) {
//        printf("Successfully added \"%s\" to WorkTree.\n", fileName);
//    }
//    if (appendWorkTree(wt, fileNameSaveList, sha256file(fileNameSaveList), 777) == 0) {
//        printf("Successfully added \"%s\" to WorkTree.\n", fileNameSaveList);
//    }
//
//    char *wts = wtts(wt);
//    printf("\nCurrent WorkTree :\n%s\n", wts);
//    WorkTree *wt2 = stwt(wts);
//    printf("Cloned WorkTree :\n%s\n\n", wtts(wt2));
//
//    char *fileNameSaveWorkTree = "work_tree_save.txt";
//    printf("Saving it to file %s...\n", fileNameSaveList);
//    wttf(wt2, fileNameSaveWorkTree);
//    printf("Done!\n");
//
//    printf("Reading it back to test...\n");
//    WorkTree *wtReadFromFile = ftwt(fileNameSaveWorkTree);
//    printf("\nRead WorkTree :\n%s\n", wtts(wtReadFromFile));
//
////    blobWorkTree(wt);
//
//    saveWorkTree(wt, ".");
//
//    printf("\nWorkTree after saving :\n%s\n", wtts(wt));
//
////    TODO: free allocated memory
////    free(hash);
//    return 0;
//}