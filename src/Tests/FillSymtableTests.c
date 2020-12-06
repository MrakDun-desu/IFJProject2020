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
    fillSymtable(&symtable,&testList);
    if(symtable == NULL)
    {
        printf("chyba funkcia symtable nenapnila tabulku\n");
    }
    else
    {
        printf("|id :%s\n",symtable->data.id.data);

    }
}
