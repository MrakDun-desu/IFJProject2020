//
// Created by praca on 5. 12. 2020.
//

#include "parser.h"
#include "Scanner.h"

int main() {

    string s;
    gen.program = &s;
    generatorInit();

    token var;
    initString(&var.tokenName);
    makeString("Hello", &var.tokenName);
    var.tokenType = IDENT;
    generateMove(&var);

    generatorWrite(stdout);

}
