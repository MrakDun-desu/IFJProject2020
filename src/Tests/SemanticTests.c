//
// Created by xdanco00 on 27. 11. 2020.
//

#include "parser.h"
#include "Scanner.h"

/// TODO Otestovat funkciu semanticAnalysis//cely jeden riadok a bere 2 sem tabulky//drhue
int main() {
    string out;
    gen.program = &out;
    generatorInit();
    tableNodePtr symtable;
    initTable(&symtable);
    string testString;
    initString(&testString);
    list testList;
    initList(&testList);
    makeString("4+5*7-(42+5)/42<=\"Hello there\"+(8-2*(16-5)+781)",&testString);
    CodeAnalyzer(&testList,testString);
    errorCode code;

    data func;
    func.types = NULL;
    func.parameters = NULL;
    func.defined = true;
    func.scope = 0;
    initString(&func.id);

    code = semanticAnalyser(&testList, symtable, symtable, &func);

    generatorClear();

    destroyString(&testString);
    deleteList(&testList);
    deleteTable(&symtable);

    return code;
}
