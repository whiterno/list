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

//! @brief Max amount of symbols in cmd string for system() call
const int MAX_CMD_SIZE = 64;

//! @brief Dump .dot filename for png creating
const char DUMP_DOT_FILENAME[] = "log/dump.dot";

//! @brief Dump .html filename for html dump with log and images
const char DUMP_HTML_FILENAME[] = "log/dump.html";

//! @brief Typedef is made to change node's data type
typedef int nodeData;

//! @brief Structure contains node's data and indexes of the next and the previous nodes
struct Node{
    nodeData data;  ///< node's data (custom type)
    int next;       ///< index of the next node in list
    int prev;       ///< index of the previous node in list
};

//! @brief Structure contains an array of nodes, first free element index, size of list and some debug info
struct List{
    const char* name;       ///< list's name
    const char* filename;   ///< filename where list was initialized
    const char* funcname;   ///< funcname where list was initialized
    size_t line;            ///< serial number of line where list was initialized

    Node* nodes;            ///< array of nodes
    int free;               ///< index of the first free node in list
    int size;               ///< amount of elements in list
    int capacity;           ///< amount of cells in list
};

//! @brief Errors enumerator
enum Errors{
    NO_ERROR,                       ///< no error occured
    NULL_VALUE_INSERTED,            ///< NULL value was given as an argument for functions
    LIST_POINTER_ERROR,             ///< list pointer is NULL
    LIST_NODES_POINTER_ERROR,       ///< list.nodes is NULL
    LIST_OVERFLOW,                  ///< max amount of elements in list is reached
    LIST_UNDERFLOW,                 ///< no elements in list to remove
    POS_OUT_OF_BOUNDS,              ///< position index is not viable
    REALLOC_NULL,                   ///< failed to allocate memory with realloc
    LIST_HAS_CYCLE,                 ///< pointer didn't return to 0 element
    FREE_LIST_NOT_VALID,            ///< free list has cycle or ends not in free list
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
//! @brief List destructor, frees allocated memory
//!
//! @param [in] list pointer on List structure's pointer (in order to change the pointer)
//!
//! @return Error code
//!
int listDtor(List** list);

//!
//! @brief Inserts elements in list by its logical position
//!
//! @param [in] list    pointer on List
//! @param [in] pos     logical position of an element, after which you want to insert element (starts with 0 counting from fict element)
//! @param [in] data    data you want to insert
//!
//! @return Error code
//!
int listInsert(List* list, int pos, nodeData data);

//!
//! @brief Pops elements from list by its logical position
//!
//! @param [in] list    pointer on List
//! @param [in] pos     logical position of an element, which you want to pop(0 to pop the 1st element and etc.)
//! @param [out] data   data that was written in popped node
//!
//! @return Error code
//!
int listPop(List* list, int pos, nodeData* data);

//!
//! @brief Creates png image and section in dump.html file with all debug info
//!
//! @param [in] list            pointer on List
//! @param [in] filename        the name of the file from which listDump was called
//! @param [in] funcname        the name of the function from which listDump was called
//! @param [in] line            the serial number of line from which listDump was called
//! @param [in] error           the code of error with which listDump was called
//!
//! @return Error code
//!
int listDump(List* list, const char* filename, const char* funcname, size_t line, int error);


//!
//! @brief Linearization function, sorts list by its logical bonds
//!
//! @param [in] list pointer on Lists
//!
//! @return Error code
//!
int listLinear(List* list);

#endif
