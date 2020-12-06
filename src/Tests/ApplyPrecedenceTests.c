//
// Created by xdanco00 on 27. 11. 2020.
//

#include "../expression.h"
#include "../Scanner.h"

/// TODO Otestovat funkciu applyPrecedence//zatvorky operatory literary ...
int main() {

    string out;
    gen.program = &out;
    generatorInit();
    tableNodePtr tabulke;
    initTable(&tabulke);
    string testString;
    initString(&testString);
    list testList;
    initList(&testList);
    makeString("4+5*7-(42+5)/42<=15+(8-2*(16-5)+781)",&testString);
    CodeAnalyzer(&testList,testString);
    applyPrecedence(&testList,tabulke);

    generatorWrite(stdout);
    generatorClear();

    destroyString(&testString);
    deleteList(&testList);
    deleteTable(&tabulke);
}
