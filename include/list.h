#ifndef LIST_H
#define LIST_H

#define LIST_VERIFY(list){                  \
    int error = 0;                          \
    if ((error = listVerify(list))){        \
        return error;                       \
    }                                       \
}
#define INIT(list) , #list, __FILE__, __func__, __LINE__
#define INIT_ARGS , const char* name, const char* filename, const char* funcname, size_t line
#define INIT_DEBUG_VARS (*list)->name = name; (*list)->filename = filename; (*list)->funcname = funcname; (*list)->line = line;
#define DUMP(list) list, __FILE__, __func__, __LINE__

//! TODO: make dynamic reallocation if max is exceeded

//! @brief Max amount of elements contained in list's array
const int LIST_SIZE = 8;

//! @brief Max amount of symbols in cmd string for system() call
const int MAX_CMD_SIZE = 64;

//! @brief Dump .dot filename for png creating
const char DUMP_DOT_FILENAME[] = "log/dump.dot";

//! @brief Dump .html filename for html dump with log and images
const char DUMP_HTML_FILENAME[] = "log/dump.html";

//! @brief Typedef is made to change node's data type
typedef int nodeData;

//! @brief Structure contains node's data and indexes of the next and the previous node's
struct Node{
    nodeData data;  ///< node's data (custom type, defined above)
    int next;       ///< index of the next node in List array
    int prev;       ///< index of the previous node in list array
};

//! @brief Structure contains an array of nodes, indexes of List's logical end and beginning
struct List{
    const char* name;
    const char* filename;
    const char* funcname;
    size_t line;

    Node* nodes;    ///< array of nodes
    int free;       ///< index of the first free node in list
    int size;       ///< amount of elements in list's array
};

//! @brief Errors enumerator
enum Errors{
    NO_ERROR,                       ///< no error occured
    NULL_VALUE_INSERTED,            ///< NULL value was given as an argument for functions
    LIST_POINTER_ERROR,             ///< list pointer is NULL
    LIST_NODES_POINTER_ERROR,       ///< list.nodes is NULL
    LIST_OVERFLOW,                  ///< max amount of elements in list array is reached
    LIST_UNDERFLOW,                 ///< no elements in list array to remove
    POS_OUT_OF_BOUNDS,              ///< position index is not viable
};

//!
//! @brief List constructer, allocates the memory and initializes variables
//!
//! @param [in] list pointer on List structure's pointer (in order to change the pointer)
//!
//! @return Error code
//!
int listCtor(List** list INIT_ARGS);

//!
//! @brief List destructor, free allocated memory
//!
//! @param [in] list pointer on List structure's pointer (in order to change the pointer)
//!
//! @return Error code
//!
int listDtor(List** list);

int listInsert(List* list, int pos, nodeData data);

int listPop(List* list, int pos, nodeData* data);

int listDump(List* list, const char* filename, const char* funcname, size_t line);

#endif
