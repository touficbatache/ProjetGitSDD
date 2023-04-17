#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "litTest.h"

int main() {
    firstExercisesTest();

    char *wtHash = testWorkTree();
    testCommit(wtHash);

    testLit();

    testLitMerge();

    return 0;
}

void firstExercisesTest() {
    char *fileName = "tests/test.txt";
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

    char *fileNameSaveList = "tests/testingC.txt";
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
    if (appendWorkTree(wt, fileName, NULL, 0) == 0) {
        printf("Successfully added \"%s\" to WorkTree.\n", fileName);
    }
    if (appendWorkTree(wt, fileNameSaveList, NULL, 0) == 0) {
        printf("Successfully added \"%s\" to WorkTree.\n", fileNameSaveList);
    }

    char *wts = wtts(wt);
    printf("\nCurrent WorkTree :\n%s\n", wts);
    WorkTree *wt2 = stwt(wts);
    printf("Cloned WorkTree :\n%s\n\n", wtts(wt2));

    char *fileNameSaveWorkTree = "tests/work_tree_save.txt";
    printf("Saving it to file %s...\n", fileNameSaveList);
    wttf(wt2, fileNameSaveWorkTree);
    printf("Done!\n");

    printf("Reading it back to test...\n");
    WorkTree *wtReadFromFile = ftwt(fileNameSaveWorkTree);
    printf("\nRead WorkTree :\n%s\n", wtts(wtReadFromFile));

    saveWorkTree(wt, ".");

    printf("\nWorkTree after saving :\n%s\n", wtts(wt));
}

char *testWorkTree() {
    WorkTree *wt = initWorkTree();
    printf("\nInitialized empty WorkTree.\n");

    char *file1 = "tests/test.txt";
    if (appendWorkTree(wt, file1, NULL, 0) == 0) {
        printf("Successfully added \"%s\" to WorkTree.\n", file1);
    }

    char *file2 = "tests/test2.txt";
    if (appendWorkTree(wt, file2, NULL, 0) == 0) {
        printf("Successfully added \"%s\" to WorkTree.\n", file2);
    }

    char *dir1 = "tests/testdir";
    if (appendWorkTree(wt, dir1, NULL, 0) == 0) {
        printf("Successfully added \"%s\" to WorkTree.\n", dir1);
    }

    printf("\nSaving WorkTree... :\n%s\n", wtts(wt));
    char *wtHash = saveWorkTree(wt, ".");

    // To test restore function:
    printf("Saved WorkTree. You may now modify the files to test. Restoring in 10s...\n");

    sleep(10);

    restoreWorkTree(wt, ".");
    printf("\nRestoring WorkTree... :\n%s\n", wtts(wt));

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
    char *path = hashToBlobPath(cHash);
    strcat(path, ".c");
    Commit *blobbedC = ftc(path);
    printf("Blobbed commit. Now showing blobbed version :\n%s\n", cts(blobbedC));

    printf("Trying to get the author... Author : %s\n", commitGet(c, "author"));

    // To free the commit, uncomment line below:
    // freeCommit(c);
}

void testLit() {
    char *testBranch = "testbranch";

    if (file_exists(".lit")) system("rm .lit");
    printf("Initializing lit and adding test file and directory...\n");
    litInit();
    litAdd("tests/test.txt");
    litAdd("tests/testdir");

    printf("\nCurrent branch is %s\n", getCurrentBranch());
    litPrintStagingArea();

    printf("\nCommitting...\n");
    litCommit(getCurrentBranch(), "This is 1st a random commit test message");

    printf("\n'%s' contents :\n", getCurrentBranch());
    printBranch(getCurrentBranch());

    printf("\nAll commits :\n%s\n", ltos(getAllCommits()));

    printf("\nCreating '%s' and checking into it...\n", testBranch);
    createBranch(testBranch);
    litCheckoutBranch(testBranch);

    printf("\nAdding new test file...\n");
    litAdd("tests/test2.txt");

    printf("\nCurrent branch is %s\n", getCurrentBranch());
    litPrintStagingArea();

    printf("\nCommitting...\n");
    litCommit(getCurrentBranch(), "This is a 2nd random commit test message");

    printf("\n'%s' contents :\n", getCurrentBranch());
    printBranch(getCurrentBranch());

    printf("\nAll commits :\n%s\n", ltos(getAllCommits()));
}

void testLitMerge() {
    if (file_exists(".lit")) system("rm .lit");

    char *mergeIntoBranch = "master";
    char *mergeFromBranch = "testbranch";
    char *testFile = "tests/test.txt";

    printf("Initializing lit and adding test file and directory...\n");
    litInit();
    litAdd(testFile);
    litAdd("tests/testdir");
    litAdd("tests/test2.txt");

    printf("\nCommitting into '%s'...\n", getCurrentBranch());
    litCommit(getCurrentBranch(), "This is a 1st commit to merge later");

    printf("\n'%s' contents :\n", getCurrentBranch());
    printBranch(getCurrentBranch());

    printf("\nCreating '%s' and checking into it...\n", mergeFromBranch);
    createBranch(mergeFromBranch);
    litCheckoutBranch(mergeFromBranch);

    printf("\nYou now have 8 seconds to modify the '%s' file...\n", testFile);

    sleep(8);

    printf("\nAdding '%s' file...\n", testFile);
    litAdd(testFile);
    litAdd("tests/testdir");

    printf("\nCommitting into '%s'...\n", mergeFromBranch);
    litCommit(getCurrentBranch(), "This is a 2nd commit to merge later");

    printf("\n'%s' contents :\n", getCurrentBranch());
    printBranch(getCurrentBranch());

    printf("\nChecking back into '%s'...\n", mergeIntoBranch);
    litCheckoutBranch(mergeIntoBranch);

    printf("\nMerging '%s' into '%s'...\n", mergeFromBranch, mergeIntoBranch);
    char mergeCommitMessage[100];
    sprintf(mergeCommitMessage, "Merge '%s' into '%s'", mergeFromBranch, mergeIntoBranch);
    List *conflicts = merge(mergeFromBranch, mergeCommitMessage);
    if (conflicts != NULL) {
        printf("\nConflicts:\n%s\n", ltos(conflicts));

        // Uncomment to try option 1
        // mergeConflictOption1(conflicts, mergeFromBranch, mergeCommitMessage);

        // Uncomment to try option 2
        // mergeConflictOption2(conflicts, mergeFromBranch, mergeCommitMessage);

        // Uncomment to try option 3
        //mergeConflictOption3(conflicts, mergeFromBranch, mergeCommitMessage);
    }

    printf("\nAll commits :\n%s\n", ltos(getAllCommits()));
}

void mergeConflictOption1(List *conflicts, char *remoteBranch, char *message) {
    printf("\nRunning option 1...\n");

    acceptChanges(1, conflicts, remoteBranch);
    merge(remoteBranch, message);
}

void mergeConflictOption2(List *conflicts, char *remoteBranch, char *message) {
    printf("\nRunning option 2...\n");

    acceptChanges(0, conflicts, remoteBranch);
    merge(remoteBranch, message);
}

void mergeConflictOption3(List *conflicts, char *remoteBranch, char *message) {
    printf("\nRunning option 3...\n");

    printf("For each file, accept either local or remote changes.\n");

    List *localChanges = initList();
    List *remoteChanges = initList();

    // file 1
    printf("\nFile: %s\n", (*conflicts)->data);
    printf("1. Accept local changes\t\t2. Accept remote changes");

    printf("\nSelected option 2.\n");

    insertFirst(remoteChanges, buildCell((*conflicts)->data));

    // file 2
    printf("\nFile: %s\n", (*conflicts)->next->data);
    printf("1. Accept local changes\t\t2. Accept remote changes");

    printf("\nSelected option 1.\n");

    insertFirst(localChanges, buildCell((*conflicts)->next->data));

    acceptChanges(1, localChanges, remoteBranch);
    acceptChanges(0, remoteChanges, remoteBranch);

    merge(remoteBranch, message);
}

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
