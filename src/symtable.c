//
// Created by xdanco00 on 12. 11. 2020.
//

#include "symtable.h"


void initTable(tableNodePtr* root) {

    *root = NULL;

}

errorCode insertNode (tableNodePtr* root, char* id, dataType* types, list parameters, bool defined) {

    tableNodePtr node = *root;
    if (node == NULL) {
        if ((node = malloc(sizeof(struct tTableNode))) != NULL) {
            node->left = NULL;
            node->right = NULL;
            initString(&node->data.id);
            makeString(id, &node->data.id);
            node->data.types = types;
            node->data.parameters = parameters;
            node->data.defined = defined;
            node->data.scope = true;
            *root = node;
            return OK;
        }
        return INTERNAL_ERROR;
    }
    if (strcmp(node->data.id.data, id) > 0) {
        if (node->left == NULL) {
            if ((node->left = malloc(sizeof(struct tTableNode))) != NULL) {
                tableNodePtr temp = node->left;
                temp->left = NULL;
                temp->right = NULL;
                initString(&temp->data.id);
                makeString(id, &temp->data.id);
                temp->data.types = types;
                temp->data.parameters = parameters;
                temp->data.defined = defined;
                temp->data.scope = true;
                return OK;
            }
        }
        else
            return insertNode(&node->left, id, types, parameters, defined);
    }

}

data* copyNode(tableNodePtr* root, char* id) {

}

void invalidateNode(tableNodePtr* root, char* id) {

}

void deleteTable(tableNodePtr* root) {

    tableNodePtr node = *root;
    if (node != NULL) {
        if (node->right != NULL)
            deleteTable(&node->right);
        if (node->left != NULL)
            deleteTable(&node->left);
        free(node);
        *root = NULL;
    }

}