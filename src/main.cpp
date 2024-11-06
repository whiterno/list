#include <stdio.h>

#include "../include/list.h"

int printList(List* list){
    for (int i = list->nodes[0].next; i != 0; i = list->nodes[i].next){
        printf("%d ", list->nodes[i].data);
    }
    printf("\n");

    return NO_ERROR;
}

int main(){
    List* list = NULL;
    listCtor(&list INIT(list));

    int var = 0;

    int error = 0;

    listDump(DUMP(list), error);
    printf("error code - %d\n", error = listInsert(list, 0, 5));
    listDump(DUMP(list), error);
    printf("error code - %d\n", error = listInsert(list, 1, 10));
    listDump(DUMP(list), error);
    printf("error code - %d\n", error = listInsert(list, 1, 15));
    listDump(DUMP(list), error);
    printf("error code - %d\n", error = listInsert(list, 1, 15));
    listDump(DUMP(list), error);
    printf("error code - %d\n", error = listInsert(list, 1, 15));
    listDump(DUMP(list), error);
    printf("error code - %d\n", error = listInsert(list, 1, 15));
    listDump(DUMP(list), error);
    printf("error code - %d\n", error = listInsert(list, 1, 15));
    listDump(DUMP(list), error);
    printf("error code - %d\n", error = listInsert(list, 1, 15));
    listDump(DUMP(list), error);
    printf("error code - %d\n", error = listInsert(list, 1, 15));
    listDump(DUMP(list), error);
    printf("error code - %d\n", error = listPop(list, 0, &var));
    listDump(DUMP(list), error);
    printf("error code - %d\n", error = listPop(list, 2, &var));
    listDump(DUMP(list), error);
    printf("error code - %d\n", error = listPop(list, 3, &var));
    listDump(DUMP(list), error);
    printf("error code - %d\n", error = listPop(list, 2, &var));
    listDump(DUMP(list), error);
    printf("error code - %d\n", error = listPop(list, 0, &var));
    listDump(DUMP(list), error);
    printf("error code - %d\n", error = listInsert(list, 1, 20));
    listDump(DUMP(list), error);
    printf("error code - %d\n", error = listInsert(list, 1, 20));
    listDump(DUMP(list), error);
    printf("error code - %d\n", error = listInsert(list, 1, 20));
    listDump(DUMP(list), error);
    printf("error code - %d\n", error = listPop(list, 0, &var));
    listDump(DUMP(list), error);
    printf("error code - %d\n", error = listPop(list, 0, &var));
    listDump(DUMP(list), error);
    printf("error code - %d\n", error = listPop(list, 0, &var));
    listDump(DUMP(list), error);

    printList(list);
    listDump(DUMP(list), error);
    listLinear(list);
    listDump(DUMP(list), error);
}
