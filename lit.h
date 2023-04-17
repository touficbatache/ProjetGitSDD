#include "litCore.h"

void acceptChanges(int hasAcceptedLocalChanges, List *conflicts, char *remoteBranch);

int showMergeConflictsMessage(List *conflicts, char *remoteBranch, char *message);