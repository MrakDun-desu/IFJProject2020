//
// Created by xdanco00 on 12. 11. 2020.
//

#include "parser.h"

bool equalStrings(char* s1, char* s2) {
    return (strcmp(s1, s2) == 0);
}

bool checkDatatype(dataType type, token* tok, tableNodePtr varTable) {

    data* var;
    switch (type) {
        case TYPE_STRING:
            if (tok->tokenType == STRING_LIT) return true;
            if ((var = copyNode(&varTable, tok->tokenName.data)) != NULL)
                if (var->types[0] == TYPE_STRING) return true;
            break;
        case TYPE_INT:
            if (tok->tokenType == INT_LIT) return true;
            if ((var = copyNode(&varTable, tok->tokenName.data)) != NULL)
                if (var->types[0] == TYPE_INT) return true;
            break;
        case TYPE_FLOAT:
            if (tok->tokenType == FLOAT_LIT) return true;
            if ((var = copyNode(&varTable, tok->tokenName.data)) != NULL)
                if (var->types[0] == TYPE_FLOAT) return true;
            break;
        default:
            break;
    }
    return false;
}

errorCode semanticAnalyser(list* tokenList, tableNodePtr globalTable, tableNodePtr localTable, data* function) {

    if (tokenList != NULL) {
        for (token* tok = tokenList->first; tok != NULL; tok = tok->nextToken) { // checking if everything is defined
            if (equalStrings(tok->nextToken->tokenName.data, ":="))
                continue;
            if (tok->tokenType == IDENT) {
                if (copyNode(&globalTable, tok->tokenName.data) == NULL && copyNode(&localTable, tok->tokenName.data) == NULL)
                    return DEFINITION_ERROR;
            }
        }
        for (token* tok = tokenList->first; tok != NULL; tok = tok->nextToken) { // checking expressions
            if (tok->nextToken != NULL) { // first check if there is next token
                token* operator = tok->nextToken;
                if (operator->tokenType == ARIT_OPERATOR || operator->tokenType == COMP_OPERATOR) { // then check if it's operator
                    if (operator->nextToken != NULL) { // check if it has next token just to be sure
                        if (equalStrings(operator->tokenName.data, "/") && // check if operator is division
                        operator->nextToken->tokenType != IDENT) { // check if next token is literal
                            // if next literal is zero, return ZERO_DIVISION_ERROR
                            bool wasDot = false;
                            bool isZero = true;
                            for (size_t i = 0; i < strlen(operator->nextToken->tokenName.data); i++) {
                                if (operator->nextToken->tokenName.data[i] != '0' && operator->nextToken->tokenName.data[i] != '.') {
                                    isZero = false; // if current char isn't 0 or ., it means literal is not zero value
                                    break;
                                }
                                if (operator->nextToken->tokenName.data[i] == '.') { // there can be only one dot
                                    if (wasDot) {
                                        isZero = false;
                                        break;
                                    }
                                    else wasDot = true;
                                }
                            }
                            if (isZero) return ZERO_DIVISION_ERROR;
                        }
                        // checking type compatibility between variables/literals
                        data* var = copyNode(&localTable, tok->tokenName.data);
                        bool varString = false;
                        bool varInt = false;
                        bool varFloat = false;
                        if (var != NULL) {
                            varString = (var->types[0] == TYPE_STRING);
                            varInt = (var->types[0] == TYPE_INT);
                            varFloat = (var->types[0] == TYPE_FLOAT);
                        }
                        if (tok->tokenType == STRING_LIT || varString) { // if first token was string, operator cannot be - * /
                            if (equalStrings(operator->tokenName.data, "-")) return TYPE_COMPATIBILITY_ERROR;
                            if (equalStrings(operator->tokenName.data, "*")) return TYPE_COMPATIBILITY_ERROR;
                            if (equalStrings(operator->tokenName.data, "/")) return TYPE_COMPATIBILITY_ERROR;
                            if (!checkDatatype(TYPE_STRING, operator->nextToken, localTable)) return TYPE_COMPATIBILITY_ERROR;
                        }
                        
                        if (tok->tokenType == INT_LIT || varInt)
                            if (!checkDatatype(TYPE_INT, operator->nextToken, localTable)) return TYPE_COMPATIBILITY_ERROR;

                        if (tok->tokenType == FLOAT_LIT || varFloat)
                            if (!checkDatatype(TYPE_FLOAT, operator->nextToken, localTable)) return TYPE_COMPATIBILITY_ERROR;

                    }
                }
            }
        }


        if (tokenList->first->tokenType == RETURN) {
        }
    }
    return OK;

}