//
// Created by xdanco00 on 27. 11. 2020.
//

#include "../expression.h"
#include "../Scanner.h"

/// TODO Otestovat funkciu applyPrecedence//zatvorky operatory literary ...
int main() {
    tableNodePtr tabulke;
    initTable(&tabulke);
    token testToken;
    string testString;
    initString(&testString);
    list testList;
    initList(&testList);
    makeString("(5+5)",&testString);
    CodeAnalyzer(&testList,testString);
    applyPrecedence(&testList,tabulke,&testToken);
}
