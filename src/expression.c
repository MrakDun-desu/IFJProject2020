//
// Created by xdanco00 on 12. 11. 2020.
//

#include "expression.h"

char* precedence[] = {"*/", "+-", "<>=!"};

bool isLarger(char op1, char op2) {
    bool wasOp1 = false;
    bool wasOp2 = false;
    for (size_t i = 0; i<3 && !wasOp2; i++) {
        for (size_t j = 0; j < strlen(precedence[i]); j++) {
            if (precedence[i][j] == op1) wasOp1 = true;
            if (precedence[i][j] == op2) wasOp2 = true;
        }
        if (wasOp1) return true;
    }
    return false;
}

errorCode evalExpression(list* tokenList, list* outList, size_t* pos) {
    list helpList;
    initList(&helpList);
    bool justReturned = false;
    for (size_t i = *pos; i < tokenList->size; i++) {
        token* tok;
        tok = copyToken(tokenList, i);
        if (justReturned) {
            while (helpList.first != NULL && isLarger(helpList.first->tokenName.data[0], tok->tokenName.data[0])) {
                token* helpToken = popToken(&helpList);
                if (addToken(outList, helpToken->tokenType, helpToken->tokenName.data)) {
                    deleteList(&helpList);
                    return INTERNAL_ERROR;
                }
                destroyString(&helpToken->tokenName);
                free(helpToken);
            }
            justReturned = false;
        }
        if (tok->tokenType == IDENT ||
            tok->tokenType == INT_LIT ||
            tok->tokenType == STRING_LIT ||
            tok->tokenType == FLOAT_LIT) {
            addToken(outList, tok->tokenType, tok->tokenName.data);
            if (tok->nextToken != NULL) {
                while (helpList.first != NULL &&
                       isLarger(helpList.first->tokenName.data[0], tok->nextToken->tokenName.data[0])) {
                    token *helpToken = popToken(&helpList);
                    if (addToken(outList, helpToken->tokenType, helpToken->tokenName.data)) {
                        destroyString(&helpToken->tokenName);
                        free(helpToken);
                        deleteList(&helpList);
                        return INTERNAL_ERROR;
                    }
                    destroyString(&helpToken->tokenName);
                    free(helpToken);
                }
            } else {
                while (helpList.first != NULL) {
                    token *helpToken = popToken(&helpList);
                    if (addToken(outList, helpToken->tokenType, helpToken->tokenName.data)) {
                        destroyString(&helpToken->tokenName);
                        free(helpToken);
                        deleteList(&helpList);
                        return INTERNAL_ERROR;
                    }
                    destroyString(&helpToken->tokenName);
                    free(helpToken);
                }
            }
        } else if (tok->tokenType == COMP_OPERATOR ||
            tok->tokenType == ARIT_OPERATOR) {
            if (pushToken(&helpList, tok)) {
                deleteList(&helpList);
                return INTERNAL_ERROR;
            }
        } else if (equalStrings(tok->tokenName.data, "(")) {
            i++;
            if (evalExpression(tokenList, outList, &i)) {
                deleteList(&helpList);
                return INTERNAL_ERROR;
            }
            justReturned = true;
        } else if (equalStrings(tok->tokenName.data, ")")) {
            while(helpList.first != NULL) {
                token* helpToken = popToken(&helpList);
                if (addToken(outList, helpToken->tokenType, helpToken->tokenName.data)) {
                    destroyString(&helpToken->tokenName);
                    free(helpToken);
                    deleteList(&helpList);
                    return INTERNAL_ERROR;
                }
                destroyString(&helpToken->tokenName);
                free(helpToken);
            }
            return i;
        }
    }
    while(helpList.first != NULL) {
        token* helpToken = popToken(&helpList);
        if (addToken(outList, helpToken->tokenType, helpToken->tokenName.data)) {
            destroyString(&helpToken->tokenName);
            free(helpToken);
            deleteList(&helpList);
            return INTERNAL_ERROR;
        }
        destroyString(&helpToken->tokenName);
        free(helpToken);
    }
    return tokenList->size;
}

errorCode applyPrecedence(list* tokenList, tableNodePtr varTable) {

    list outList;
    initList(&outList);

    size_t pos = 0;
    if (evalExpression(tokenList, &outList, &pos)) return INTERNAL_ERROR;
    if (pos != tokenList->size) return INTERNAL_ERROR;

    if (generateExpression(&outList, varTable)) return INTERNAL_ERROR;

    deleteList(&outList);

    return OK;

}