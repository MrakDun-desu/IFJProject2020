//
// Created by xdanco00 on 27. 11. 2020.
//

#include "../parser.h"
#include "../DynamicString.h"
#include "../List.h"
#include "../Scanner.h"

/// TODO Otestovat funkciu fillSymtable//prve

void drawTree(tableNodePtr symtable){

    if (symtable->left != NULL)
        drawTree(symtable->left);

    if(symtable == NULL) {
        printf("Symtable is empty\n");
        return;
    }

    printf("Identifier: %s\n", symtable->data.id.data);
    string types;
    initString(&types);
    int counter = 0;
    for (dataType type = symtable->data.types[counter]; type != TYPE_UNDEFINED; type = symtable->data.types[++counter]){
        printf("Datatype %d: %d\n", counter, type);
    }

    list* parameters;
    parameters = symtable->data.parameters;
    if (parameters != NULL) {
        printf("This symbol is a function. Parameters:\n");
        for (int i = 1; i <= symtable->data.parameters->size; i++) {
            printf("%s, ", parameters->first->tokenName.data);
            parameters->first = parameters->first->nextToken;
        }
    } else {
        printf("This symbol is a variable.");
    }

    printf("\nScope: %zu\n", symtable->data.scope);

    if (symtable->right != NULL)
        drawTree(symtable->right);

}

int main() {

    string testString;
    list testList;
    tableNodePtr symtable;
    initString(&testString);
    printf("Test 1:\n");
    makeString("func factorial(n int, f float64) (int) {\n}" , &testString);
    printf("beriem: %s\n",testString.data);
    initList(&testList);
    initTable(&symtable);
    CodeAnalyzer(&testList,testString);
    makeString("func aujo()  {\n}" , &testString);
    CodeAnalyzer(&testList,testString);
    makeString("func xavier(n int, f float64, jozgo string) (string) {\n}" , &testString);
    CodeAnalyzer(&testList,testString);
    fillSymtable(&symtable,&testList);
    if(symtable == NULL)
    {
        printf("chyba funkcia symtable nenapnila tabulku\n");
    }
    else
    {
        drawTree(symtable);

    }

}


