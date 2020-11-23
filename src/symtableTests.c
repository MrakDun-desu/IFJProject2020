//
// Created by praca on 15. 11. 2020.
//

#include "symtable.h"

void printNode(data* data) {
    printf("Symtable:\n\n");
    printf("Identifier: %s\n", data->id.data);
    printf("Datatypes: ");
    size_t typeCount = sizeof(data->types) / sizeof(dataType);
    for (size_t i = 0; i < typeCount; i++)
        printf("%d ", data->types[i]);
    printf("\n");
    printf("Parameters: ");
    for (token* tok = data->parameters->first; tok != NULL; tok = tok->nextToken)
        printf("%s ", tok->tokenName.data);
    printf("\n");
    printf("Defined: %d\n", data->defined);
    printf("Scope: %lu\n", data->scope);
}

int main() {

    tableNodePtr symtable;
    initTable(&symtable);

    list tokenList;
    initList(&tokenList);
    addToken(&tokenList, VAR_ID, "a");
    addToken(&tokenList, INT, "int");
    dataType types[] = {TYPE_INT, TYPE_STRING};
    insertNode(&symtable, "myFunc", types, &tokenList, 0);

    list tokenList2;
    initList(&tokenList2);
    addToken(&tokenList2, VAR_ID, "b");
    addToken(&tokenList2, FLOAT, "float64");
    dataType types2[] = {TYPE_INT, TYPE_STRING};
    insertNode(&symtable, "a", types2, &tokenList2, 0);

    printNode(copyNode(&symtable, "myFunc"));
    printNode(copyNode(&symtable, "a"));


    deleteTable(&symtable);

    return 0;
}
