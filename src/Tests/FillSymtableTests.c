//
// Created by xdanco00 on 27. 11. 2020.
//

#include "../parser.h"
#include "../DynamicString.h"
#include "../List.h"
#include "../Scanner.h"

/// TODO Otestovat funkciu fillSymtable//prve
int main() {

    string testString;
    list testList;
    tableNodePtr symtable;
    initString(&testString);
    makeString("int i = 1 " , &testString);
    initList(&testList);
    initTable(&symtable);
    CodeAnalyzer(&testList,testString);
    fillSymtable(symtable,&testList);



}
