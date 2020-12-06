//
// Created by xdanco00 on 12. 11. 2020.
//

#include "expression.h"

char* precedence[] = {"*/", "+-", "<>=!"};

bool isLarger(char op1, char op2) {
    bool wasOp1 = false;
    bool wasOp2 = false;
    for (size_t i = 0; i<3; i++) {
        for (size_t j = 0; j < strlen(precedence[i]); j++) {
            if (precedence[i][j] == op1) wasOp1 = true;
            if (precedence[i][j] == op2) wasOp2 = true;
        }
        if (wasOp1 && !wasOp2) return true;
    }
    return false;
}

size_t evalExpression(list* tokenList, list* outList, size_t pos) {
    list helpList;
    initList(&helpList);
    for (size_t i = pos; pos < tokenList->size; i++) {
        token* tok;
        tok = copyToken(tokenList, i);
        pushToken(&helpList, tok);
        if (tok->tokenType == IDENT ||
            tok->tokenType == INT_LIT ||
            tok->tokenType == STRING_LIT ||
            tok->tokenType == FLOAT_LIT) {
            appendToken(outList, tok);
            while (isLarger(helpList.first->tokenName.data[0], tok->nextToken->tokenName.data[0])) {
                appendToken(outList, popToken(&helpList));
            }

        } else if (tok->tokenType == COMP_OPERATOR ||
            tok->tokenType == ARIT_OPERATOR) {
            pushToken(&helpList, tok);
        } else if (equalStrings(tok->tokenName.data, "(")) {
            i = evalExpression(tokenList, outList, i+1);
        } else {
            deleteList(&helpList);
            return i+1;
        }
    }
    deleteList(&helpList);
    return tokenList->size;
}

errorCode applyPrecedence(list* tokenList, tableNodePtr varTable) {

    list outList;
    initList(&outList);

    size_t pos = 0;
    if (evalExpression(tokenList, &outList, pos) != tokenList->size) return INTERNAL_ERROR;

    if (generateExpression(&outList, varTable)) return INTERNAL_ERROR;

    return OK;

}