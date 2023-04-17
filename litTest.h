#include "litCore.h"

void firstExercisesTest();

char *testWorkTree();

void testCommit(char *wtHash);

void testLit();

void testLitMerge();

void mergeConflictOption1(List *conflicts, char *remoteBranch, char *message);

void mergeConflictOption2(List *conflicts, char *remoteBranch, char *message);

void mergeConflictOption3(List *conflicts, char *remoteBranch, char *message);

void acceptChanges(int hasAcceptedLocalChanges, List *conflicts, char *remoteBranch);