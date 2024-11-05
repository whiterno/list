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

    printf("error code - %d\n", listInsert(list, 0, 5));
    printf("error code - %d\n", listInsert(list, 1, 10));
    listDump(DUMP(list));
    printf("error code - %d\n", listInsert(list, 1, 15));
    printf("error code - %d\n", listPop(list, 0, &var));
    listDump(DUMP(list));
    printf("error code - %d\n", listInsert(list, 1, 20));
    // int var = 0;
    // listPop(list, 1, &var);
    // printf("var - %d\n", var);
    // listInsert(list, 1, 20);

    printf("Free - %d\n", list->free);
    printf("Size - %d\n", list->size);

    printList(list);
    listDump(DUMP(list));
}
