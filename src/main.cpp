#include <stdio.h>

#include "../include/list.h"

int printList(List* list){
    for (int i = list->head; i != 0; i = list->nodes[i].next){
        printf("%d ", list->nodes[i].data);
    }
    printf("\n");

    return NO_ERROR;
}

int main(){
    List* list = NULL;
    listCtor(&list INIT(list));


    listPushBack(list, 5);
    listPushBack(list, 10);
    listPushBack(list, 15);

    // int var = 0;
    // listPop(list, 1, &var);
    // printf("var - %d\n", var);
    // listInsert(list, 1, 20);

    // printf("Head - %d\n", list->head);
    // printf("Tail - %d\n", list->tail);
    // printf("Free - %d\n", list->free);
    // printf("Size - %d\n", list->size);

    // printList(list);
    listDump(DUMP(list));
}
