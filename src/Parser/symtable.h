//
// Created by xdanco00 on 12. 11. 2020.
//

#ifndef SRC_SYMTABLE_H
#define SRC_SYMTABLE_H
#include <stdbool.h>
#include "../String/DynamicString.h"

typedef enum tDataType {
    UNDEFINED,
    STRING,
    INT,
    FLOAT
} dataType;

typedef struct tData {
    dataType* type; /// can be more values since functions can return more than one value
    bool defined; /// true if function has been defined
    string params; /// string of parameters (everything between parentheses)
    string id; /// identifier of function / variable
    bool scope; /// true if variable exists in this scope. Should be set to false when exiting the scope.
} data;

typedef struct tTableNode {
    data data; /// data of node (data.id will be used as key)
    struct tTableNode* left; /// pointer to left subtree
    struct tTableNode* right; /// pointer to right subtree
} tableNode;

/// @brief Init, should be called only once after creating table.
void initTable(tableNode* root);

/// @brief Searches for node and returns pointer to it
/// @return value of the node, if the node is found, otherwise NULL
data* copyNode(char* id);
// for editing values in table you can use this function (adding parameters or adding data types)

/// @brief Inserts node into the table. If node with id is already defined, change other values
/// @return If allocation fails 0, otherwise 1.
int insertNode(dataType* type, bool defined, char* params, char* id, bool global);

/// @brief Sets the scope to false. Implemented so we don't have to remove nodes from table.
void invalidateNode(char* id);

/// @brief Frees all the nodes in table correctly and sets it to NULL.
void deleteTable(tableNode* root);

#endif //SRC_SYMTABLE_H
