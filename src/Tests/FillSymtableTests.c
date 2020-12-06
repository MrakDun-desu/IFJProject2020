//
// Created by xdanco00 on 27. 11. 2020.
//

#include "../parser.h"
#include "../DynamicString.h"
#include "../List.h"
#include "../Scanner.h"

/// TODO Otestovat funkciu fillSymtable//prve

void printSymtable(tableNodePtr symtable) {

}
void drawTree(tableNodePtr symtable){
    string types ;
    initString(&types);
    switch(*symtable->data.types){
        case TYPE_UNDEFINED : makeString("Undefined",&types);
            break;
        case TYPE_STRING : makeString("String",&types);
        break;
        case TYPE_INT : makeString("int",&types);
        break;
        case TYPE_FLOAT : makeString("float",&types);
            break;
    }

    if(symtable == NULL)
    {
        printf("symtable is empty");
        return;
    }
    if(symtable != NULL){
        printf("\nid: %s ; type:%s parameters: ",symtable->data.id.data,types.data);
        list parameters;
        initList(&parameters);
        parameters = *symtable->data.parameters;
        for(int i = 1; i<=symtable->data.parameters->size ; i++)
        {
            printf("%s, ",parameters.first->tokenName);
            parameters.first = parameters.first->nextToken;
        }
    }
    if(symtable->right !=NULL)
    {
        drawTree(symtable->right);
    }
    if(symtable->left !=NULL)
    {
        drawTree(symtable->left);
    }

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


