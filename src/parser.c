//
// Created by xdanco00 on 12. 11. 2020.
//

#include "parser.h"

errorCode semanticAnalyser(list* tokenList, tableNodePtr globalTable, tableNodePtr localTable, data* function) {

    if (tokenList != NULL) {
        for (token* tok = tokenList->first; tok != NULL; tok = tok->nextToken) {
            if (strcmp(tok->nextToken->tokenName.data, ":=") == 0)
                continue;
            if (tok->tokenType == IDENT) {
                if (copyNode(&globalTable, tok->tokenName.data) == NULL && copyNode(&localTable, tok->tokenName.data) == NULL)
                    return DEFINITION_ERROR;
            }
        }
        if (tokenList->first->tokenType == RETURN) {
        }
    }
    return OK;

}