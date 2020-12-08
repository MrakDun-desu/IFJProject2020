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
    FILE *in_file;
    FILE *output;
    in_file = fopen("example2.go", "r");
    output = fopen("output.code", "w");
    initList(&tokenList);
    initString(&sourceCode);
    errorCode out;

    char* buffer;
    size_t len;
    if (in_file) {
        fseek(in_file, 0, SEEK_END);
        len = ftell(in_file);
        fseek(in_file, 0, SEEK_SET);
        buffer = malloc(len);
        if (buffer) {
            fread(buffer, 1, len, in_file);
        } else
            return INTERNAL_ERROR;
        fclose(in_file);
    } else {
        return INTERNAL_ERROR;
    }

    out = makeString(buffer, &sourceCode);
    if (out) return out;

    out = CodeAnalyzer(&tokenList, sourceCode);
    if (out) return out;

    out = parse(&tokenList);
    if (out) {
        generatorClear();
        return out;
    }

    generatorWrite(output);

    destroyString(&sourceCode);
    free(buffer);
    deleteList(&tokenList);
    generatorClear();

    return 0;
}