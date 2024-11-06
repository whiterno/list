#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/list.h"

static int listInsertResize(List* list);
static int listPopResize(List* list);
static void listFreeCtor(List** list, int free);

static int listVerify(List** list);
static int createDotFile(List* list, int error);
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
    (*list)->capacity = 4;
    (*list)->nodes = (Node*)calloc((*list)->capacity, sizeof(Node));
    for (int i = 0; i < (*list)->capacity; i++){
        (*list)->nodes[i].data = 0;
    }

    // Init size and free list
    (*list)->size = 0;
    listFreeCtor(list, 1);

    // Init cycle
    (*list)->nodes[0].next = 0;
    (*list)->nodes[0].prev = 0;

    INIT_DEBUG_VARS;

    // Check if an error occured and return the error code
    LIST_VERIFY(list);
    return  NO_ERROR;
}

int listDtor(List** list){
    LIST_VERIFY(list)

    free((*list)->nodes);
    free(*list);
    *list = NULL;

    return NO_ERROR;
}

int listInsert(List* list, int pos, nodeData data){
    LIST_VERIFY(&list);

    if (list->size == list->capacity - 1){
        if (listInsertResize(list)){
            return REALLOC_NULL;
        }
    }
    if ((pos > list->size + 1) || pos < 0){
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

    if (list->size == list->capacity / 4){
        listLinear(list);
        listPopResize(list);
    }

    LIST_VERIFY(&list);
    return NO_ERROR;
}

int listLinear(List* list){
    LIST_VERIFY(&list);

    List* list_linear = NULL;
    listCtor(&list_linear INIT(list_linear));

    int inx = list->nodes[0].next;

    for (int i = 0; i < list->size; i++){
        listInsert(list_linear, i, list->nodes[inx].data);
        inx = list->nodes[inx].next;
    }

    list->free = list_linear->free;

    memcpy(list->nodes, list_linear->nodes, list->capacity * sizeof(Node));
    listFreeCtor(&list, list->free);
    listDtor(&list_linear);

    LIST_VERIFY(&list);
    return NO_ERROR;
}

static int listInsertResize(List* list){
    list->capacity *= 2;
    list->nodes = (Node*)realloc(list->nodes, list->capacity * sizeof(Node));
    if (list->nodes == NULL){
        list->capacity /= 2;

        return REALLOC_NULL;
    }

    listFreeCtor(&list, list->size + 1);

    return NO_ERROR;
}

static int listPopResize(List* list){
    int last_free = list->free;

    for (int free_cnt = 0; free_cnt < list->size - 2; free_cnt++){
        last_free = list->nodes[last_free].next;
    }

    list->nodes[last_free].next = 0;

    list->capacity /= 2;
    list->nodes = (Node*)realloc(list->nodes, list->capacity * sizeof(Node));

    return NO_ERROR;
}

static void listFreeCtor(List** list, int free){
    (*list)->free = free;
    for (int i = free; i < (*list)->capacity; i++){
        (*list)->nodes[i].next = (i + 1) % (*list)->capacity;
        (*list)->nodes[i].prev = -1;
    }
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

    int cnt = 0;
    int inx = 0;
    for (; cnt <= (*list)->size; cnt++){
        inx = (*list)->nodes[inx].next;
    }
    if (inx != 0){
        return LIST_HAS_CYCLE;
    }

    cnt = 0;
    inx = (*list)->free;
    for (; cnt < (*list)->capacity - (*list)->size - 1; cnt++){
        inx = (*list)->nodes[inx].next;
    }
    if (inx != 0){
        return FREE_LIST_NOT_VALID;
    }

    return NO_ERROR;
}

int listDump(List* list, const char* filename, const char* funcname, size_t line, int error){
    createDotFile(list, error);

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

        return error;
    }
    else{
        fprintf(dump_html, "\n\t\t nodes[%p]", list->nodes);
        fprintf(dump_html, "\n\t\t free = %d", list->free);
        fprintf(dump_html, "\n\t\t size = %d", list->size);
        fprintf(dump_html, "\n\t\t capacity = %d", list->capacity);
    }

    fprintf(dump_html, "\n\t\t <img src = %d.png width = 50%%>", dump_img_counter - 1);

    fprintf(dump_html, "\n\n</pre>\n");
    return error;
}

static int createDotFile(List* list, int error){
    if (error == LIST_POINTER_ERROR || error == LIST_NODES_POINTER_ERROR){
        return error;
    }

    FILE* dump_dot = fopen(DUMP_DOT_FILENAME, "w");
    fprintf(dump_dot, "digraph D{\n");
    fprintf(dump_dot, "\trankdir = LR\n");

    fprintf(dump_dot, "\t{\n");
    fprintf(dump_dot, "\tnode[shape = plaintext]\n");
    fprintf(dump_dot, "\tedge[color = \"white\"]\n");
    fprintf(dump_dot, "\t");
    for (int i = 0; i < list->capacity - 1; i++){
        fprintf(dump_dot, "\"%d\" -> ", i);
    }
    fprintf(dump_dot, "\"%d\"\n", list->capacity - 1);
    fprintf(dump_dot, "\t}\n\n");

    if (list->free != 0){
        fprintf(dump_dot, "\tfree [color = \"darkblue\"]\n\n");
    }
    fprintf(dump_dot, "\tfictitious [color = \"orange\"]\n\n");

    for (int i = 0; i < list->capacity; i++){
        if (i == list->free && list->free != 0){
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

    for (int i = 0; i < list->capacity; i++){
        int data = list->nodes[i].data;
        int next = list->nodes[i].next;
        int prev = list->nodes[i].prev;

        fprintf(dump_dot, "\tnode_%d [shape = record, label = \" data: %d | next: %d | prev: %d \"]\n", i, data, next, prev);
    }

    fprintf(dump_dot, "\n");

    for (int i = 0; i < list->capacity - 1; i++){
        fprintf(dump_dot, "\tnode_%d -> node_%d [weight = 100, color = \"white\"]\n", i, i + 1);
    }

    fprintf(dump_dot, "\n");

    int cnt = 0;
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
        cnt++;
        if (cnt == list->capacity + 2){
            fclose(dump_dot);
            return LIST_HAS_CYCLE;
        }
    }

    fprintf(dump_dot, "\n");

    cnt = 0;
    for (int i = list->free; i != 0; i = list->nodes[i].next){
        fprintf(dump_dot, "\tnode_%d [color  = \"darkblue\"]\n", i);
        if (list->nodes[i].next == 0){
            break;
        }
        fprintf(dump_dot, "\tnode_%d -> node_%d [color = \"darkblue\"]\n", i, list->nodes[i].next);

        cnt++;
        if (cnt == list->capacity + 2){
            fclose(dump_dot);
            return FREE_LIST_NOT_VALID;
        }
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
    _DESCR(REALLOC_NULL);
    _DESCR(LIST_HAS_CYCLE);
    _DESCR(FREE_LIST_NOT_VALID);
    }

    return "IDK_YOU_IDIOT";
}
#undef _DESCR
