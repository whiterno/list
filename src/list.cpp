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

static const char* errorToString(int error);

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
    (*list)->size = 0;
    listFreeCtor(list);

    (*list)->nodes[0].next = 0;
    (*list)->nodes[0].prev = 0;

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

int listInsert(List* list, int pos, nodeData data){
    LIST_VERIFY(&list);

    if (pos > list->size || pos < 0){
        return POS_OUT_OF_BOUNDS;
    }

    int push_before_inx = list->nodes[0].next;
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
    list->size++;

    LIST_VERIFY(&list);
    return NO_ERROR;
}

int listPop(List* list, int pos, nodeData* data){
    LIST_VERIFY(&list);

    if ((pos >= list->size) || (pos < 0)){
        return POS_OUT_OF_BOUNDS;
    }

    int pop_inx = list->nodes[0].next;
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
    list->nodes[pop_inx].data = 0;
    list->size--;

    LIST_VERIFY(&list);
    return NO_ERROR;
}

int listDump(List* list, const char* filename, const char* funcname, size_t line){
    int error = createDotFile(list);

    fprintf(dump_html, "\n<pre>\n");

    fprintf(dump_html, "\tERROR: %s", errorToString(error));

    if (error == LIST_POINTER_ERROR){
        fprintf(dump_html, "\tError: list not found");

        return error;
    }

    fprintf(dump_html, "\n\tDump called from %s:%s:%lu", filename, funcname, line);
    fprintf(dump_html, "\n\tList %s born in %s:%s:%lu", list->name, list->filename, list->funcname, list->line);

    fprintf(dump_html, "\n\t%s[%p]", list->name, list);

    if (error == LIST_NODES_POINTER_ERROR){
        fprintf(dump_html, "\n\t\t nodes[NULL]");
    }
    else{
        fprintf(dump_html, "\n\t\t nodes[%p]", list->nodes);
        fprintf(dump_html, "\n\t\t free = %d", list->free);
        fprintf(dump_html, "\n\t\t size = %d", list->size);
    }

    fprintf(dump_html, "\n\t\t <img src = %d.png width = 55%%>", dump_img_counter - 1);

    fprintf(dump_html, "\n\n</pre>\n");
    return 0;
}

static int createDotFile(List* list){
    int error = listVerify(&list);

    if (error){
        return error;
    }

    FILE* dump_dot = fopen(DUMP_DOT_FILENAME, "w");
    fprintf(dump_dot, "digraph D{\n");
    fprintf(dump_dot, "\trankdir = LR\n");

    fprintf(dump_dot, "\t{\n");
    fprintf(dump_dot, "\tnode[shape = plaintext]\n");
    fprintf(dump_dot, "\tedge[color = \"white\"]\n");
    fprintf(dump_dot, "\t");
    for (int i = 0; i < LIST_SIZE - 1; i++){
        fprintf(dump_dot, "\"%d\" -> ", i);
    }
    fprintf(dump_dot, "\"%d\"\n", LIST_SIZE - 1);
    fprintf(dump_dot, "\t}\n\n");

    fprintf(dump_dot, "\tfree [color = \"darkblue\"]\n\n");
    fprintf(dump_dot, "\tfictitious [color = \"orange\"]\n\n");

    for (int i = 0; i < LIST_SIZE; i++){
        if (i == list->free){
            fprintf(dump_dot, "\t{rank = same;  \"%d\"; node_%d; free;}\n", i, i);
            continue;
        }
        if (i == 0){
            fprintf(dump_dot, "\t{rank = same;  \"%d\"; node_%d; fictitious;}\n", i, i);
            continue;
        }
        fprintf(dump_dot, "\t{rank = same;  \"%d\"; node_%d;}\n", i, i);
    }

    fprintf(dump_dot, "\n");

    for (int i = 0; i < LIST_SIZE; i++){
        int data = list->nodes[i].data;
        int next = list->nodes[i].next;
        int prev = list->nodes[i].prev;

        fprintf(dump_dot, "\tnode_%d [shape = record, label = \" data: %d | next: %d | prev: %d \"]\n", i, data, next, prev);
    }

    fprintf(dump_dot, "\n");

    for (int i = 0; i < LIST_SIZE - 1; i++){
        fprintf(dump_dot, "\tnode_%d -> node_%d [weight = 100, color = \"white\"]\n", i, i + 1);
    }

    fprintf(dump_dot, "\n");

    for (int i = 0; ; i = list->nodes[i].next){
        fprintf(dump_dot, "\tnode_%d -> node_%d [color = \"darkgreen\"]\n", i, list->nodes[i].next);
        if (i != 0){
            fprintf(dump_dot, "\tnode_%d [color  = \"darkgreen\"]\n", i);
        }
        else{
            fprintf(dump_dot, "\tnode_%d [color  = \"orange\"]\n", i);
        }
        if (list->nodes[i].next == 0){
            break;
        }
    }

    fprintf(dump_dot, "\n");

    for (int i = list->free;  ; i = list->nodes[i].next){
        fprintf(dump_dot, "\tnode_%d [color  = \"darkblue\"]\n", i);
        if (list->nodes[i].next == 0){
            break;
        }
        fprintf(dump_dot, "\tnode_%d -> node_%d [color = \"darkblue\"]\n", i, list->nodes[i].next);
    }

    fprintf(dump_dot, "}\n");

    fclose(dump_dot);

    char cmd[MAX_CMD_SIZE] = {};

    snprintf(cmd, MAX_CMD_SIZE, "dot ./log/dump.dot -Tpng -o ./log/%d.png", dump_img_counter++);
    system(cmd);

    return error;
}

#define _DESCR(error) case(error): return #error
static const char* errorToString(int error){
    switch (error)
    {
    _DESCR(NO_ERROR);
    _DESCR(NULL_VALUE_INSERTED);
    _DESCR(LIST_POINTER_ERROR);
    _DESCR(LIST_NODES_POINTER_ERROR);
    _DESCR(LIST_OVERFLOW);
    _DESCR(LIST_UNDERFLOW);
    _DESCR(POS_OUT_OF_BOUNDS);
    }

    return "IDK_YOU_IDIOT";
}
#undef _DESCR
