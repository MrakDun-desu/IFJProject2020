//
// Created by xdanco00 on 12. 11. 2020.
//

#ifndef SRC_SYMTABLE_H
#define SRC_SYMTABLE_H
#include <stdbool.h>
#include "DynamicString.h"
#include "List.h"

typedef enum tDataType {
    TYPE_UNDEFINED,
    TYPE_STRING,
    TYPE_INT,
    TYPE_FLOAT
} dataType;

typedef struct tData {
    string id; /// identifier of function / variable
    dataType* types; /// can be more values since functions can return more than one value
    list* parameters; /// token list of function parameters
    bool defined; /// true if function has been defined
    size_t scope; /// true if variable exists in this scope. Should be set to false when exiting the scope.
} data;

typedef struct tTableNode {
    data data; /// data of node (data.id will be used as key)
    struct tTableNode* left; /// pointer to left subtree
    struct tTableNode* right; /// pointer to right subtree
} *tableNodePtr;

/** @brief Init, should be called only once after creating table.
 * @param root Node that is to be initialized.
 */
void initTable(tableNodePtr* root);

/** @brief Searches for node and returns pointer to it
 * @param root Pointer to the root of table.
 * @param id Identifier of a variable/function that is to be copied.
 * @return value of the node, if the node is found, otherwise NULL.
 */
data* copyNode(tableNodePtr* root, char* id);
// for editing values in table you can use this function (adding parameters or adding data types)

/** @brief Inserts node into the table. If node with id is already defined, change other values.
 * @param root Root node of the table.
 * @param id Identifier of a variable/function that is to be defined.
 * @param types Return types of the function or the datatype of a variable.
 * @param parameters Parameters of a function (irrelevant with a variable).
 * @param scope Scope of a variable (irrelevant with a function). The lower a number the broader the scope.
 * @return OK if everything went well, INTERNAL_ERROR otherwise.
 */
errorCode insertNode(tableNodePtr* root, char* id, dataType* types, list* parameters, size_t scope);

/** @brief Sets the scope to false. Implemented so we don't have to remove nodes from table. If node doesn't exist, does nothing.
 * @param root Root node of the table.
 * @param id Identifier of a variable/function that is to be invalidated.
 */
void invalidateNode(tableNodePtr* root, char* id);

/** @brief Frees all the nodes in table correctly and sets it to NULL. Also deletes all parameter lists and id strings.
 * @param root Root node of the table.
 */
void deleteTable(tableNodePtr* root);

#endif //SRC_SYMTABLE_H
