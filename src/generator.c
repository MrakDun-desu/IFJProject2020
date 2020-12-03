//
// Created by xdanco00 on 12. 11. 2020.
//

#include "generator.h"

#define ADDCHAR(des_str,src_char) {if(addConstChar(des_str, src_char)) return INTERNAL_ERROR;}

void generatorInit() {

}

errorCode generatorStart() {
    return OK;
}

errorCode makeExpVar(size_t number, char* out) {
    if ((out = malloc(sizeof(char)*8))==NULL)
        return INTERNAL_ERROR;
    else
        sprintf(out, "expVar%zu", number);
    return OK;
}

errorCode decideOperation(token* var, token* first, token* second, token* tok, char* scopes, dataType expressionType) {

    if (equalStrings(tok->tokenName.data, "+")) {
        if (expressionType != TYPE_STRING)
            if (generateArithmetic(var, first, second, scopes, '+')) return INTERNAL_ERROR;
        else
            if (generateConcat(var, first, second, scopes)) return INTERNAL_ERROR;
    }
    else if (equalStrings(tok->tokenName.data, "-")) {
        if (generateArithmetic(var, first, second, scopes, '-')) return INTERNAL_ERROR;
    }
    else if (equalStrings(tok->tokenName.data, "*")) {
        if (generateArithmetic(var, first, second, scopes, '*')) return INTERNAL_ERROR;
    }
    else if (equalStrings(tok->tokenName.data, "/")) {
        if (expressionType == TYPE_INT)
            if (generateArithmetic(var, first, second, scopes, '/')) return INTERNAL_ERROR;
        else
            if (generateArithmetic(var, first, second, scopes, '%')) return INTERNAL_ERROR;
    }
    else if (equalStrings(tok->tokenName.data, ">")) {
        if (generateGT(var, first, second, scopes)) return INTERNAL_ERROR;
    }
    else if (equalStrings(tok->tokenName.data, "<")) {
        if (generateLT(var, first, second, scopes)) return INTERNAL_ERROR;
    }
    else if (equalStrings(tok->tokenName.data, ">=")) {
        if (generateLT(var, first, second, scopes)) return INTERNAL_ERROR;
        if (generateNOT(var, var, "gg")) return INTERNAL_ERROR;
    }
    else if (equalStrings(tok->tokenName.data, "<=")) {
        if (generateGT(var, first, second, scopes)) return INTERNAL_ERROR;
        if (generateNOT(var, var, "gg")) return INTERNAL_ERROR;
    }
    else if (equalStrings(tok->tokenName.data, "==")) {
        if (generateEQ(var, first, second, scopes)) return INTERNAL_ERROR;
    }
    else if (equalStrings(tok->tokenName.data, "!=")) {
        if (generateEQ(var, first, second, scopes)) return INTERNAL_ERROR;
        if (generateNOT(var, var, "gg")) return INTERNAL_ERROR;
    }
    return OK;
}

errorCode generateExpression(list* expression, tableNodePtr varTable, token* lastToken) {

    bool usedTokens[expression->size];
    for (size_t i = 0; i < expression->size; i++) usedTokens[i] = false;
    dataType expressionType;
    token* varVals[10];
    for (size_t i = 0; i < 10; i++) varVals[i] = NULL;
    size_t usedVars = 0;
    data* node = NULL;

    if (expression != NULL) {
        switch (expression->first->tokenType) {
            case INT_LIT:
                expressionType = TYPE_INT;
                break;
            case FLOAT_LIT:
                expressionType = TYPE_FLOAT;
                break;
            case STRING_LIT:
                expressionType = TYPE_STRING;
                break;
            case IDENT:
                node = copyNode(&varTable, expression->first->tokenName.data);
                switch (node->types[0]) {
                    case TYPE_INT:
                        expressionType = TYPE_INT;
                        break;
                    case TYPE_FLOAT:
                        expressionType = TYPE_FLOAT;
                        break;
                    case TYPE_STRING:
                        expressionType = TYPE_STRING;
                        break;
                    default:
                        return INTERNAL_ERROR;
                }
                break;
            default:
                return INTERNAL_ERROR;
        }

        bool wasOperator = false;
        for (size_t i = 0; i < expression->size; i++) {
            token* tok = NULL;
            getToken(expression, i, tok);
            token* first = NULL;
            token* second = NULL;
            bool wasSecond = false;
            if (tok->tokenType == ARIT_OPERATOR || tok->tokenType == COMP_OPERATOR) {
                usedTokens[i] = true;
                wasOperator = true;
                for (size_t j = i-1; j >= 0; j--) {
                    if (!wasSecond) {
                        if (!usedTokens[j]) {
                            getToken(expression, j, second);
                            if (second->tokenType != ARIT_OPERATOR && second->tokenType != COMP_OPERATOR) {
                                usedTokens[j] = true;
                                wasSecond = true;
                            }
                        }
                    } else {
                        if (!usedTokens[j]) {
                            getToken(expression, j, first);
                            if (first->tokenType != ARIT_OPERATOR && first->tokenType != COMP_OPERATOR) {
                                usedTokens[j] = true;

                                if ((varVals[usedVars] = malloc(sizeof(token))) == NULL) return INTERNAL_ERROR;
                                varVals[usedVars]->tokenType = IDENT;
                                char* newName = NULL;
                                if ((newName = malloc(sizeof(char)*8))==NULL) return INTERNAL_ERROR;
                                sprintf(newName, "expVar%zu", usedVars);
                                if (makeString(newName, &varVals[usedVars]->tokenName)) return INTERNAL_ERROR;
                                free(newName);

                                if (decideOperation(varVals[usedVars], first, second, tok, "gtt", expressionType))
                                    return INTERNAL_ERROR;
                                usedVars++;
                                break;
                            }
                        }
                    }
                }
                if (second == NULL) {
                    usedVars--;
                    if (decideOperation(varVals[usedVars]-1, varVals[usedVars-1], varVals[usedVars], tok, "ggg", expressionType))
                        return INTERNAL_ERROR;
                    free(varVals[usedVars]);
                } else {
                    getToken(expression, i-1, first);
                    if (second == first) {
                        if (decideOperation(varVals[usedVars-1], varVals[usedVars-1], second, tok, "ggt", expressionType))
                            return INTERNAL_ERROR;
                    }
                    else {
                        if (decideOperation(varVals[usedVars-1], second, varVals[usedVars-1], tok, "gtg", expressionType))
                            return INTERNAL_ERROR;
                    }
                }
            }
        }

        if ((lastToken = malloc(sizeof(token))) == NULL) return INTERNAL_ERROR;
        else {
            makeString("expVar0", &lastToken->tokenName);
            lastToken->tokenType = IDENT;
            lastToken->nextToken = NULL;
        }

        if (!wasOperator) {
            char str[100];
            string tmp;
            switch (expression->first->tokenType) {
                case INT_LIT:
                    sprintf(str,"MOVE GF@expVar0 int@%s\n", expression->first->tokenName.data);
                    break;
                case STRING_LIT:
                    initString(&tmp);
                    makeString(expression->first->tokenName.data,&tmp);
                    transformString(&tmp);
                    sprintf(str,"MOVE GF@expVar0 string@%s\n", tmp.data);
                    destroyString(&tmp);
                    break;
                case FLOAT_LIT:
                    sprintf(str,"MOVE GF@expVar0 float@%s\n", expression->first->tokenName.data);
                    break;
                case IDENT:
                    sprintf(str,"MOVE GF@expVar0 TF@%s\n", expression->first->tokenName.data);
                    break;
                default:
                    break;
            }

            ADDCHAR(gen.program,str)

            return OK;

        }

    }

    return OK;
}