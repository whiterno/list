#include <stdio.h>
#include <stdlib.h>

#include "../include/list.h"

//!
//! @brief List verifier, checks list on any errors that can occure
//!
//! @param [in] list pointer on List structure's pointer (in order to change the pointer)
//!
//! @return Error code
//!
static int listVerify(List** list);

static void listFreeCtor(List** list);

static int createDotFile(List* list);

static FILE* dump_html = fopen(DUMP_HTML_FILENAME, "w");
static int dump_img_counter = 0;

int listCtor(List** list INIT_ARGS){
    // Check if list is safe to work with
    if (list == NULL){
        return NULL_VALUE_INSERTED;
    }

    // Allocate list variable in heap
    *list = (List*)calloc(1, sizeof(List));

    // Allocate list's array in heap
    (*list)->nodes = (Node*)calloc(LIST_SIZE, sizeof(nodeData));
    for (int i = 0; i < LIST_SIZE; i++){
        (*list)->nodes[i].data = 0;
    }

    // Init logical beginning, end, size and free list
    (*list)->head = 0;
    (*list)->tail = 0;
    (*list)->size = 0;
    listFreeCtor(list);

    INIT_DEBUG_VARS;

    // Check if an error occured and return the error code
    LIST_VERIFY(list);
    return  NO_ERROR;
}

static void listFreeCtor(List** list){
    (*list)->free = 1;
    for (int i = 1; i < LIST_SIZE; i++){
        (*list)->nodes[i].next = (i + 1) % LIST_SIZE;
        (*list)->nodes[i].prev = -1;
    }
}

int listDtor(List** list){
    LIST_VERIFY(list)

    free((*list)->nodes);
    free(*list);
    *list = NULL;

    return NO_ERROR;
}

static int listVerify(List** list){
    if (list == NULL){
        return NULL_VALUE_INSERTED;
    }
    if (*list == NULL){
        return LIST_POINTER_ERROR;
    }
    if ((*list)->nodes == NULL){
        return LIST_NODES_POINTER_ERROR;
    }

    return NO_ERROR;
}

int listPushBack(List* list, nodeData data){
    LIST_VERIFY(&list);

    if (list->free == 0){
        return LIST_OVERFLOW;
    }

    if (list->head == 0){
        list->head = 1;
    }

    int free = list->free;
    list->free = list->nodes[list->free].next;
    list->nodes[list->tail].next = free;

    list->nodes[free].data = data;
    list->nodes[free].prev = list->tail;
    list->nodes[free].next = 0;

    list->tail = free;
    list->size++;

    LIST_VERIFY(&list);
    return NO_ERROR;
}

int listPushFront(List* list, nodeData data){
    LIST_VERIFY(&list);

    if (list->free == 0){
        return LIST_OVERFLOW;
    }

    if (list->tail == 0){
        list->tail = 1;
    }

    int free = list->free;
    list->free = list->nodes[list->free].next;
    list->nodes[list->head].prev = free;

    list->nodes[free].data = data;
    list->nodes[free].prev = 0;
    list->nodes[free].next = list->head;

    list->head = free;
    list->size++;

    LIST_VERIFY(&list);
    return NO_ERROR;
}

int listPopBack(List* list, nodeData* data){
    LIST_VERIFY(&list);

    if (list->tail == 0 && list->head == 0){
        return LIST_UNDERFLOW;
    }

    if (list->head == list->tail){
        list->head = 0;
    }

    int pop_inx = list->tail;
    *data = list->nodes[pop_inx].data;
    list->tail = list->nodes[list->tail].prev;
    list->nodes[list->tail].next = 0;

    list->nodes[pop_inx].prev = -1;
    list->nodes[pop_inx].next = list->free;

    list->free = pop_inx;
    list->size--;

    LIST_VERIFY(&list);
    return NO_ERROR;
}

int listPopFront(List* list, nodeData* data){
    LIST_VERIFY(&list);

    if (list->tail == 0 && list->head == 0){
        return LIST_UNDERFLOW;
    }

    if (list->head == list->tail){
        list->tail = 0;
    }

    int pop_inx = list->head;
    *data = list->nodes[pop_inx].data;
    list->head = list->nodes[list->head].next;
    list->nodes[list->head].prev = 0;

    list->nodes[pop_inx].prev = -1;
    list->nodes[pop_inx].next = list->free;

    list->free = pop_inx;
    list->size--;

    LIST_VERIFY(&list);
    return NO_ERROR;
}

int listInsert(List* list, int pos, nodeData data){
    LIST_VERIFY(&list);

    if (pos > list->size || pos < 0){
        return POS_OUT_OF_BOUNDS;
    }

    if (pos == 0){
        return listPushFront(list, data);
    }
    if (pos == list->size){
        return listPushBack(list, data);
    }

    int push_before_inx = list->head;
    for (int i = 0; i < pos; i++){
        push_before_inx = list->nodes[push_before_inx].next;
    }
    int push_after_inx = list->nodes[push_before_inx].prev;

    int free = list->free;
    list->free = list->nodes[free].next;

    list->nodes[free].prev = push_after_inx;
    list->nodes[push_before_inx].prev = free;

    list->nodes[free].next = push_before_inx;
    list->nodes[push_after_inx].next = free;

    list->nodes[free].data = data;

    LIST_VERIFY(&list);
    return NO_ERROR;
}

int listPop(List* list, int pos, nodeData* data){
    LIST_VERIFY(&list);

    if (pos >= list->size || pos < 0){
        return POS_OUT_OF_BOUNDS;
    }

    if (pos == 0){
        return listPopFront(list, data);
    }
    if (pos == list->size - 1){
        return listPopBack(list, data);
    }

    int pop_inx = list->head;
    for (int i = 0; i < pos; i++){
        pop_inx = list->nodes[pop_inx].next;
    }
    *data = list->nodes[pop_inx].data;

    int after_pop_inx = list->nodes[pop_inx].next;
    int before_pop_inx = list->nodes[pop_inx].prev;

    list->nodes[after_pop_inx].prev = before_pop_inx;
    list->nodes[before_pop_inx].next = after_pop_inx;

    list->nodes[pop_inx].prev = -1;
    list->nodes[pop_inx].next = list->free;
    list->free = pop_inx;

    LIST_VERIFY(&list);
    return NO_ERROR;
}

//! TODO: this fucking dump with graphs, generate files, system doesn't call command,


int listDump(List* list, const char* filename, const char* funcname, size_t line){
    int error = createDotFile(list);
    system("bash ./cmd.txt");
    return 0;
}

static int createDotFile(List* list){
    if (list == NULL){
        return LIST_POINTER_ERROR;
    }
    if (list->nodes == NULL){
        return LIST_NODES_POINTER_ERROR;
    }

    FILE* dump_dot = fopen(DUMP_DOT_FILENAME, "w");
    fprintf(dump_dot, "digraph D{\n");
    fprintf(dump_dot, "rankdir = LR;\n");
    for (int i = 0; i < LIST_SIZE; i++){
        int data = list->nodes[i].data;
        int next = list->nodes[i].next;
        int prev = list->nodes[i].prev;

        fprintf(dump_dot, "node_%d [shape = record, label = \" data: %d | next: %d | prev: %d \"]\n", i, data, next, prev);
    }
    fprintf(dump_dot, "}");

    return 0;
}
