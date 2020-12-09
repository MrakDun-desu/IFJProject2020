/******************************** symtable.c **********************************/
/*  Predmet: IFJ a IAL						                                  */
/*  Datova struktura tabulky symbolov                                         */
/*  Vytvoril: Marek Danco xdanco00                                            */
/* ************************************************************************** */

#include "symtable.h"


void initTable(tableNodePtr* root) {

    *root = NULL;

}

errorCode insertNode (tableNodePtr* root, char* id, dataType* types, list* parameters, size_t scope) {

    tableNodePtr node = *root;
    errorCode alloc;
    if (node == NULL) {
        if ((node = malloc(sizeof(struct tTableNode))) != NULL) {
            node->left = NULL;
            node->right = NULL;
            initString(&node->data.id);
            if ((alloc = makeString(id, &node->data.id)) != OK) return alloc;
            node->data.types = types;
            node->data.parameters = parameters;
            node->data.defined = true;
            node->data.scope = scope;
            *root = node;
            return OK;
        }
        return INTERNAL_ERROR;
    }
    if (strcmp(node->data.id.data, id) > 0) {
        return insertNode(&node->left, id, types, parameters, scope);
    } else if (strcmp(node->data.id.data, id) < 0) {
        return insertNode(&node->right, id, types, parameters, scope);
    } else {
        if ((alloc = makeString(id, &node->data.id)) != OK) return alloc;
        node->data.types = types;
        node->data.parameters = parameters;
        node->data.defined = true;
        return OK;
    }

}

data* copyNode(tableNodePtr* root, char* id) {

    tableNodePtr node = *root;
    if (node == NULL)
        return NULL;
    if (strcmp(node->data.id.data, id) > 0)
        return copyNode(&node->left, id);
    if (strcmp(node->data.id.data, id) < 0)
        return copyNode(&node->right, id);
    return &node->data;

}

void invalidateScope(tableNodePtr* root, size_t scope) {

    if (*root != NULL) {
        invalidateScope(&(*root)->left, scope);
        invalidateScope(&(*root)->right, scope);
        if ((*root)->data.scope == scope)
            (*root)->data.defined = false;
    }

}

void deleteTable(tableNodePtr* root) {

    tableNodePtr node = *root;
    if (node != NULL) {
        deleteTable(&node->right);
        deleteTable(&node->left);
        if (node->data.parameters != NULL)
            deleteList(node->data.parameters);
        destroyString(&node->data.id);
        if (node->data.types != NULL)
            free(node->data.types);
        free(node);
        *root = NULL;
    }

}