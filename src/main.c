//
// Created by xdanco00 on 12. 11. 2020.
//

#include "parser.h"
#include "Scanner.h"

/// TODO Call scanner, send tokens to parser, which will send them to generator. Check for errors and return the proper error code.
// Parser calls generator directly to reduce work. Main will only call function generatorWrite at the end of program translation.

int main() {

    string sourceCode;
    list tokenList;
    initList(&tokenList);
    initString(&sourceCode);
    errorCode out;

    char* buffer;
    size_t len;
    fseek(stdin, 0, SEEK_END);
    len = ftell(stdin) + 1;
    fseek(stdin, 0, SEEK_SET);
    buffer = malloc(len);
    if (buffer) {
        fread(buffer, 1, len-1, stdin);
        buffer[len-1] = '\0';
    } else {
        return INTERNAL_ERROR;
    }

    out = makeString(buffer, &sourceCode);
    if (out) {
        free(buffer);
        return out;
    }

    out = CodeAnalyzer(&tokenList, sourceCode);
    if (out) {

        free(buffer);
        return out;
    }

    out = parse(&tokenList);
    if (out) {

        free(buffer);
        return out;
    }

    generatorWrite(stdout);

    destroyString(&sourceCode);
    free(buffer);
    deleteList(&tokenList);
    generatorClear();

    return 0;
}