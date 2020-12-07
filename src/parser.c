//
// Created by xdanco00 on 12. 11. 2020.
//

#include "parser.h"
#include <stdlib.h>

bool checkDatatype(dataType type, token *tok, tableNodePtr varTable) {

    data *var;
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

errorCode checkFunctionTypes(list *tokenList, data *func, size_t i, tableNodePtr localTable, dataType **back) {
    dataType *paramTypes = NULL;
    if (func->parameters != NULL) {
        paramTypes = malloc(sizeof(dataType) * func->parameters->size / 2);
    }
    size_t typeCount = 0;

    // first, we need to check which parameter types function expects
    for (token *iter = func->parameters->first; iter != NULL; iter = iter->nextToken) {
        switch (iter->tokenType) {
            case INT:
                paramTypes[typeCount++] = TYPE_INT;
                break;
            case STRING:
                paramTypes[typeCount++] = TYPE_STRING;
                break;
            case FLOAT:
                paramTypes[typeCount++] = TYPE_FLOAT;
                break;
            default:
                break;
        }
    }
    size_t nextCommaCount = 0;
    bool inParams = false;
    token *nextTok = NULL;
    nextTok = copyToken(tokenList, i + 1);
    for (size_t j = i + 1; nextTok != NULL; nextTok = copyToken(tokenList, ++j)) {
        if (!inParams && equalStrings(nextTok->tokenName.data, "("))
            inParams = true; // if we encounter (, we need to check parameters
        else if (inParams && equalStrings(nextTok->tokenName.data, ")"))
            break; // if we encounter ), we break the cycle
        else if (inParams && nextTok->tokenType == COMMA) { // when we reach a comma
            nextCommaCount++;
            if (nextCommaCount == typeCount) {
                // if there is same count of commas as of function params,
                // it means that there are more terms than needed. Return error.
                free(paramTypes);
                return PARAMETER_ERROR;
            }
        } else {
            if (inParams && !checkDatatype(paramTypes[nextCommaCount], nextTok, localTable)) {
                // if we have a term, we need to check for the right type. If it doesn't
                // correspond, return error
                free(paramTypes);
                return PARAMETER_ERROR;
            }
        }

    }
    if (typeCount != 0 && nextCommaCount != typeCount - 1) {
        free(paramTypes);
        return PARAMETER_ERROR;
    }

    *back = func->types;
    return OK;
}

errorCode semanticAnalyser(list *tokenList, tableNodePtr globalTable, tableNodePtr localTable, data *function) {

    // ------------------------------ Checking datatype compatibility and zero division ------------------------------ //

    if (tokenList != NULL) {
        size_t tokCounter = 0;
        for (token *tok = tokenList->first; tok != NULL; tok = tok->nextToken) {
            if (tok->nextToken != NULL) { // first check if there is next token
                token *operator = tok->nextToken;
                if (operator->tokenType == ARIT_OPERATOR ||
                    operator->tokenType == COMP_OPERATOR) { // then check if it's operator
                    if (operator->nextToken != NULL) { // check if it has next token just to be sure (should be handled by syntax analysis)
                        if (equalStrings(operator->tokenName.data, "/") && // check if operator is division
                            (operator->nextToken->tokenType == INT_LIT ||
                             operator->nextToken->tokenType == FLOAT_LIT)) { // and check if next token is literal
                            // if next literal is zero, return ZERO_DIVISION_ERROR
                            bool wasDot = false;
                            bool isZero = true;
                            for (size_t i = 0; i < strlen(operator->nextToken->tokenName.data); i++) {
                                if (operator->nextToken->tokenName.data[i] != '0' &&
                                    operator->nextToken->tokenName.data[i] != '.') {
                                    isZero = false; // if current char isn't 0 or ., it means literal is not zero value
                                    break;
                                }
                                if (operator->nextToken->tokenName.data[i] == '.') { // there can be only one dot
                                    if (wasDot) {
                                        isZero = false;
                                        break;
                                    } else wasDot = true;
                                }
                            }
                            if (isZero) return ZERO_DIVISION_ERROR;
                        }
                        // checking type compatibility between variables/literals
                        size_t count = 0;
                        token *prevTok;
                        for (prevTok = tok; prevTok != NULL; prevTok = copyToken(tokenList, tokCounter - count)) {
                            // run through tokens until we reach something which isn't ")"
                            if (!equalStrings(prevTok->tokenName.data,")")) // if token isn't ), that means we got variable or literal
                                break;
                            count++;
                        }

                        data *var = copyNode(&localTable, prevTok->tokenName.data);
                        bool varString = false;
                        bool varInt = false;
                        bool varFloat = false;
                        if (var != NULL) {
                            varString = (var->types[0] == TYPE_STRING);
                            varInt = (var->types[0] == TYPE_INT);
                            varFloat = (var->types[0] == TYPE_FLOAT);
                        }
                        if (prevTok->tokenType == STRING_LIT ||
                            varString) { // if first token was string, operator cannot be - * /
                            if (equalStrings(operator->tokenName.data, "-")) return TYPE_COMPATIBILITY_ERROR;
                            if (equalStrings(operator->tokenName.data, "*")) return TYPE_COMPATIBILITY_ERROR;
                            if (equalStrings(operator->tokenName.data, "/")) return TYPE_COMPATIBILITY_ERROR;
                            for (token *nextTok = operator->nextToken; nextTok != NULL; nextTok = nextTok->nextToken) {
                                if (!equalStrings(nextTok->tokenName.data, "(")) {
                                    // run through next tokens until variable or literal is reached
                                    if (!checkDatatype(TYPE_STRING, nextTok, localTable))
                                        return TYPE_COMPATIBILITY_ERROR;
                                    else break;
                                }
                            }
                        } else if (prevTok->tokenType == INT_LIT || varInt) {
                            for (token *nextTok = operator->nextToken; nextTok != NULL; nextTok = nextTok->nextToken) {
                                if (!equalStrings(nextTok->tokenName.data, "(")) {
                                    // run through next tokens until variable or literal is reached
                                    if (!checkDatatype(TYPE_INT, nextTok, localTable))
                                        return TYPE_COMPATIBILITY_ERROR;
                                    else break;
                                }
                            }
                        } else if (prevTok->tokenType == FLOAT_LIT || varFloat) {
                            for (token *nextTok = operator->nextToken; nextTok != NULL; nextTok = nextTok->nextToken) {
                                if (!equalStrings(nextTok->tokenName.data, "(")) {
                                    // run through next tokens until variable or literal is reached
                                    if (!checkDatatype(TYPE_FLOAT, nextTok, localTable))
                                        return TYPE_COMPATIBILITY_ERROR;
                                    else break;
                                }
                            }
                        }
                    }
                }
            }
            tokCounter++;
        }

        if (tokenList->first != NULL) { // all the next checks require first token, so checking if it's not null

            // ------------------------------ Checking if scope has ended ------------------------------ //

            if (tokenList->first->tokenType == BRACKET_CURLY && equalStrings(tokenList->first->tokenName.data, "}")) {
                if (scope != 0) { // if we aren't in the end of function
                    invalidateScope(&localTable, scope); // invalidate two scopes and decrease the scope two times
                    scope--;
                    invalidateScope(&localTable, scope);
                    scope--;
                } else // else we are at the end of function, so delete the whole local variable symtable
                    deleteTable(&localTable);
            }

            // ------------------------------ Checking if semantics ------------------------------ //

            if (tokenList->first->tokenType == IF) {
                bool wasComparator = false;
                size_t wasBracket = 0;
                for (token *tok = tokenList->first->nextToken;
                     tok != NULL; tok = tok->nextToken) {
                    if (equalStrings(tok->tokenName.data, "("))
                        wasBracket++; // if we encounter opening bracket, increase bracket count
                    else if (equalStrings(tok->tokenName.data, ")"))
                        wasBracket--; // decrease with closing bracket
                    else if (tok->tokenType == COMP_OPERATOR) {
                        if (wasComparator || wasBracket) return SEMANTIC_ERROR;
                        // if there was more than one comparator or comparator was in brackets, return general semantic error
                        wasComparator = true;
                    }
                }
                if (!wasComparator) return TYPE_COMPATIBILITY_ERROR; // if there was no comparator, return compatibility error
                scope += 2; // increasing the scope twice since we are entering if
            }

            // checking else
            if (tokenList->first->nextToken != NULL) {
                if (tokenList->first->nextToken->tokenType ==
                    ELSE) { // else should always be second token, if it is there
                    scope++; // just increase the scope, because there is no comparing with else
                }
            }

            // ------------------------------ Checking for semantics ------------------------------ //

            if (tokenList->first->tokenType == FOR) {
                scope++; // increasing the scope for the first line
                int semicolonCount = 0;
                bool wasComparator = false;
                size_t wasBracket = 0;
                bool wasDefinition = false;
                bool decidedType = false;
                token *newSymbol;
                for (token *tok = tokenList->first->nextToken; // we need to begin from second one, since first token is just for keyword
                     tok != NULL; tok = tok->nextToken) {
                    if (tok->tokenType == SEMICOL) {
                        semicolonCount++;
                        if (semicolonCount == 2 && wasComparator == false) // if we are after second semicolon and there
                            // was no comparator, we aren't getting bool value, so return error.
                            return TYPE_COMPATIBILITY_ERROR;
                    } else if (equalStrings(tok->tokenName.data, ":="))
                        wasDefinition = true;
                    else if (semicolonCount == 0) { // if there wasn't a semicolon, we are in definition part
                        if (tok->tokenType == IDENT &&
                            !wasDefinition) // if this is the identifier before definition token
                            newSymbol = tok; // store the token for future use
                        else if (wasDefinition && !decidedType) {

                            token *nextTok; // create next token, which should define the type of newSymbol
                            for (nextTok = tok; nextTok != NULL; nextTok = nextTok->nextToken) {
                                // run through next tokens until variable or literal is reached
                                if (!equalStrings(nextTok->tokenName.data, "("))
                                    break;
                            }

                            decidedType = true; // we are going to add this to symtable, so setting this so we don't do it again
                            dataType *type = malloc(sizeof(dataType));
                            bool isVar = false;
                            switch (nextTok->tokenType) { // token type just before the semicolon should only be a literal or identifier
                                case INT_LIT:
                                    type[0] = TYPE_INT;
                                    break;
                                case STRING_LIT:
                                    type[0] = TYPE_STRING;
                                    break;
                                case FLOAT_LIT:
                                    type[0] = TYPE_FLOAT;
                                case IDENT:
                                    isVar = true;
                                    break;
                                default: // shouldn't be any other type, so just breaking. Syntax error should occur if this is the case
                                    break;
                            }
                            if (isVar) { // if it was identifier, further checking is needed
                                data *lastVar;
                                if ((lastVar = copyNode(&localTable, nextTok->tokenName.data)) == NULL) {
                                    // if searching in local table yields no results, this variable was undefined
                                    free(type);
                                    return DEFINITION_ERROR;
                                } else {
                                    switch (lastVar->types[0]) {
                                        case TYPE_INT:
                                            type[0] = TYPE_INT;
                                            break;
                                        case TYPE_FLOAT:
                                            type[0] = TYPE_FLOAT;
                                            break;
                                        case TYPE_STRING:
                                            type[0] = TYPE_STRING;
                                            break;
                                        default:
                                            free(type);
                                            return DEFINITION_ERROR;
                                    }
                                }
                            }
                            insertNode(&localTable, newSymbol->tokenName.data, type, NULL, scope);

                        }
                    } else if (semicolonCount == 1) { // if there was one semicolon, we are in the comparing part
                        if (tok->tokenType == COMP_OPERATOR) {
                            if (wasComparator || wasBracket) return SEMANTIC_ERROR;
                                // if there was more than one comparator or comparator is in brackets, return general semantic error
                            else wasComparator = true;
                        } else if (equalStrings(tok->tokenName.data, "("))
                            wasBracket++;
                        else if (equalStrings(tok->tokenName.data, ")"))
                            wasBracket--;
                    }
                }
                scope++; // increasing the scope again after first line
            }

            // ------------------------------ Checking definition semantics ------------------------------ //

            bool wasAssign = false;
            for (token *tok = tokenList->first; tok != NULL; tok = tok->nextToken) {
                if (tok->tokenType == ASIGN_OPERATOR) wasAssign = true;
                if (tok->nextToken != NULL && equalStrings(tok->nextToken->tokenName.data,":=")) {
                    // if this is the definition command, create new entry in symtable
                    data *thisSymbol;
                    if (equalStrings(tok->tokenName.data, "_")) return DEFINITION_ERROR;

                    thisSymbol = copyNode(&localTable, tok->tokenName.data);
                    bool isNull = thisSymbol == NULL;
                    if (isNull || scope > thisSymbol->scope || !thisSymbol->defined) {
                        dataType *newType = malloc(sizeof(dataType));
                        for (token *nextTok = tok->nextToken; nextTok != NULL; nextTok = nextTok->nextToken) {
                            // with this cycle, we decide which datatype the variable should be
                            if (nextTok->tokenType == IDENT) {
                                data *var = copyNode(&localTable, nextTok->tokenName.data);
                                if (var == NULL) {
                                    free(newType);
                                    return DEFINITION_ERROR;
                                }
                                switch (var->types[0]) {
                                    case TYPE_INT:
                                        newType[0] = TYPE_INT;
                                        break;
                                    case TYPE_FLOAT:
                                        newType[0] = TYPE_FLOAT;
                                        break;
                                    case TYPE_STRING:
                                        newType[0] = TYPE_STRING;
                                        break;
                                    default:
                                        break;
                                }
                                break;
                            } else if (nextTok->tokenType == INT_LIT) {
                                newType[0] = TYPE_INT;
                                break;
                            } else if (nextTok->tokenType == STRING_LIT) {
                                newType[0] = TYPE_STRING;
                                break;
                            } else if (nextTok->tokenType == FLOAT_LIT) {
                                newType[0] = TYPE_FLOAT;
                                break;
                            }
                        }

                        if (isNull || thisSymbol->defined)
                            insertNode(&localTable, tok->tokenName.data, newType, NULL, scope);
                            // scope won't be overwritten, since variable already is in there
                        else {
                            thisSymbol->defined = true;
                            thisSymbol->types = newType;
                            thisSymbol->scope = scope; // scope will be overwritten, since we are defining it again
                        }
                    } else return DEFINITION_ERROR;
                } else if (tok->tokenType == IDENT && (!equalStrings(tok->tokenName.data, "_") || wasAssign)) {
                    if (copyNode(&globalTable, tok->tokenName.data) == NULL &&
                        copyNode(&localTable, tok->tokenName.data) == NULL)
                        return DEFINITION_ERROR;
                }
            }

            // ------------------------------ Checking return semantics ------------------------------ //

            if (tokenList->first->tokenType == RETURN) { // if we are returning from the function
                size_t commaCounter = 0; // for knowing which return value are we checking
                size_t typeCount = 0; // for knowing how much types we have to control
                dataType *types = function->types;
                for (dataType type = types[0]; type != TYPE_UNDEFINED; type = types[++typeCount]);
                for (token *tok = tokenList->first; tok != NULL; tok = tok->nextToken) {
                    if (tok->tokenType == COMMA) { // if we are at a coma, just increase the counter
                        commaCounter++;
                        if (commaCounter >= typeCount) return PARAMETER_ERROR;
                        // if commaCounter is bigger than it should, jump out of function right away
                    } else {

                        // return expressions must have the same data types as all variables/literals in expressions

                        if (types[commaCounter] == TYPE_INT) {
                            if (tok->tokenType != INT_LIT) { // if it's correct literal, we don't have to check
                                if (!checkDatatype(TYPE_INT, tok, localTable))
                                    return PARAMETER_ERROR;
                            }
                        } else if (types[commaCounter] == TYPE_STRING) {
                            if (tok->tokenType != STRING_LIT) {
                                if (!checkDatatype(TYPE_STRING, tok, localTable))
                                    return PARAMETER_ERROR;
                            }
                        } else if (types[commaCounter] == TYPE_FLOAT) {
                            if (tok->tokenType != FLOAT_LIT) {
                                if (!checkDatatype(TYPE_FLOAT, tok, localTable))
                                    return PARAMETER_ERROR;
                            }
                        }
                    }
                }

                if (commaCounter != typeCount - 1) return PARAMETER_ERROR;
                // if there wasn't same count of different expressions as return types, return error

            }

            // ------------------------------ Checking assign semantics ------------------------------ //

            for (size_t i = 0; i < tokenList->size; i++) { // we run through the list with a number so we can come back
                token *tok = NULL;
                tok = copyToken(tokenList, i);
                if (equalStrings(tok->tokenName.data, "=")) { // if we encounter =, it means this is assign
                    size_t prevCommaCount = 0; // comma counts to check the same number of expressions
                    size_t nextCommaCount = 0;
                    token *prevTok = NULL;
                    prevTok = copyToken(tokenList, i - 1); // previous token to check before the = operator
                    token *nextTok = NULL;
                    nextTok = copyToken(tokenList, i + 1); // next token to check after the = operator

                    dataType *prevTypes = NULL; // types that are before operator
                    dataType *nextTypes = NULL; // types that are after the operator

                    for (size_t j = i - 1; j != 0 && prevTok != NULL; prevTok = copyToken(tokenList, --j)) {
                        // before the = operator, there should only be commas and variable identifiers.

                        if (prevTok->tokenType == COMMA)
                            prevCommaCount++;
                        else if (prevTok->tokenType != IDENT)
                            break;
                    }

                    data *func;
                    if ((func = copyNode(&globalTable, nextTok->tokenName.data)) == NULL) {
                        // after the = operator, there can be function or a list of
                        // expressions separated by commas.
                        for (size_t j = i + 1; nextTok != NULL; nextTok = copyToken(tokenList, ++j)) {
                            // if there isn't a function, we need to count the commas
                            if (prevTok->tokenType == COMMA)
                                nextCommaCount++;
                            else if (prevTok->tokenType == COMP_OPERATOR)
                                return DATATYPE_ERROR;
                                // if there was comparator, that is not the right type to assign
                            else if (prevTok->tokenType != IDENT &&
                                     prevTok->tokenType != BRACKET_ROUND &&
                                     prevTok->tokenType != ARIT_OPERATOR &&
                                     prevTok->tokenType != INT_LIT &&
                                     prevTok->tokenType != STRING_LIT &&
                                     prevTok->tokenType != FLOAT_LIT)
                                break;
                            // if it isn't any of these types, we are out of the expression, so break the cycle
                        }
                    } else { // if there is a function, we need to check type compatibility with its parameters

                        errorCode code;
                        if ((code = checkFunctionTypes(tokenList, func, i, localTable, &nextTypes)) != OK)
                            return code;

                    }

                    if (func == NULL) {
                        if (prevCommaCount != nextCommaCount) // if there was no function, comma counts should be equal
                            return SEMANTIC_ERROR;

                        nextTypes = malloc(sizeof(dataType) * (nextCommaCount + 2));
                        for (size_t j = 0; j < nextCommaCount + 2; j++)
                            nextTypes[j] = TYPE_UNDEFINED;
                        // if there was no function, we need to check next data types term after term
                        nextTok = copyToken(tokenList, i + 1); // begin the iteration from start again
                        nextCommaCount = 0;
                        for (size_t j = i + 1; nextTok != NULL; nextTok = copyToken(tokenList, ++j)) {
                            if (nextTok->tokenType == COMMA)
                                nextCommaCount++;
                            else {
                                if (checkDatatype(TYPE_INT, nextTok, localTable))
                                    nextTypes[nextCommaCount] = TYPE_INT;
                                else if (checkDatatype(TYPE_STRING, nextTok, localTable))
                                    nextTypes[nextCommaCount] = TYPE_STRING;
                                else if (checkDatatype(TYPE_FLOAT, nextTok, localTable))
                                    nextTypes[nextCommaCount] = TYPE_FLOAT;
                            }
                        }
                    }
                    prevTypes = malloc(sizeof(dataType) * (prevCommaCount + 2));
                    for (size_t j = 0; j < prevCommaCount + 2; j++)
                        prevTypes[j] = TYPE_UNDEFINED;
                    prevTok = copyToken(tokenList, i - 1); // begin the iteration from start again
                    prevCommaCount = 0;
                    for (size_t j = i - 1; prevTok != NULL; prevTok = copyToken(tokenList, --j)) {
                        if (prevTok->tokenType == COMMA)
                            prevCommaCount++;
                        else {
                            if (checkDatatype(TYPE_INT, prevTok, localTable))
                                prevTypes[prevCommaCount] = TYPE_INT;
                            else if (checkDatatype(TYPE_STRING, prevTok, localTable))
                                prevTypes[prevCommaCount] = TYPE_STRING;
                            else if (checkDatatype(TYPE_FLOAT, prevTok, localTable))
                                prevTypes[prevCommaCount] = TYPE_FLOAT;
                        }
                    }

                    for (size_t j = 0; j < prevCommaCount +
                                           1; j++) { // at the end, iterate one last time to check if data types match
                        if (nextTypes[j] != prevTypes[j] || prevTypes[j] != TYPE_UNDEFINED)
                            // we need to add TYPE_UNDEFINED here in case one of prev identifiers was _ and undefined stayed with it
                            return TYPE_COMPATIBILITY_ERROR;
                    }

                    break;
                }
            }

            // ------------------------------ Checking function call semantics ------------------------------ //

            data *func;
            if ((func = copyNode(&globalTable, tokenList->first->tokenName.data)) != NULL) { // if we use function without assign
                dataType *types = NULL;
                errorCode code;
                if ((code = checkFunctionTypes(tokenList, func, 0, localTable, &types))) // check if types had been correctly assigned
                    return code;
                else if (types[0] != TYPE_UNDEFINED) // if yes, check if function has no returns
                    return SEMANTIC_ERROR;
            }

        }
    }
    return OK;

}

errorCode fillSymtable(tableNodePtr* globalTable, list *tokenList) {

    ///FUNC INPUTS
    string funcId;
    initString(&funcId);
    makeString("inputs", &funcId);

    dataType *funcInputsRetTypes = malloc(3 * sizeof(dataType));
    funcInputsRetTypes[0] = TYPE_STRING;
    funcInputsRetTypes[1] = TYPE_INT;
    funcInputsRetTypes[2] = TYPE_UNDEFINED;

    list *funcInputsParams = malloc(sizeof(list));
    initList(funcInputsParams);

    insertNode(globalTable, funcId.data, funcInputsRetTypes, funcInputsParams, 0);

    ///FUNC INPUTI
    makeString("inputi", &funcId);

    dataType *funcInputiRetTypes = malloc(3 * sizeof(dataType));
    funcInputiRetTypes[0] = TYPE_INT;
    funcInputiRetTypes[1] = TYPE_INT;
    funcInputiRetTypes[2] = TYPE_UNDEFINED;

    list *funcInputiParams = malloc(sizeof(list));
    initList(funcInputiParams);

    insertNode(globalTable, funcId.data, funcInputsRetTypes, funcInputiParams, 0);

    ///FUNC INPUTF
    makeString("inputf", &funcId);

    dataType *funcInputfRetTypes = malloc(3 * sizeof(dataType));
    funcInputfRetTypes[0] = TYPE_INT;
    funcInputfRetTypes[1] = TYPE_INT;
    funcInputfRetTypes[2] = TYPE_UNDEFINED;

    list *funcInputfParams = malloc(sizeof(list));
    initList(funcInputfParams);

    insertNode(globalTable, funcId.data, funcInputfRetTypes, funcInputfParams, 0);

    ///FUNC INT2FLOAT
    makeString("int2float", &funcId);

    dataType *funcInt2FloatRetTypes = malloc(2 * sizeof(dataType));
    funcInt2FloatRetTypes[0] = TYPE_FLOAT;
    funcInt2FloatRetTypes[1] = TYPE_UNDEFINED;

    list *funcInt2FloatParams = malloc(sizeof(list));
    initList(funcInt2FloatParams);

    string funcParamId;
    initString(&funcParamId);
    makeString("i", &funcParamId);

    token funcParam;
    funcParam.tokenType = IDENT;
    funcParam.tokenName = funcParamId;

    addToken(funcInt2FloatParams, funcParam.tokenType, funcParam.tokenName.data);

    makeString("int", &funcParamId);

    funcParam.tokenType = INT;
    funcParam.tokenName = funcParamId;
    addToken(funcInt2FloatParams, funcParam.tokenType, funcParam.tokenName.data);

    insertNode(globalTable, funcId.data, funcInt2FloatRetTypes, funcInt2FloatParams, 0);

    ///FUNC FLOAT2INT
    makeString("float2int", &funcId);

    dataType *funcFloat2IntRetTypes = malloc(2 * sizeof(dataType));
    funcFloat2IntRetTypes[0] = TYPE_INT;
    funcFloat2IntRetTypes[1] = TYPE_UNDEFINED;

    list *funcFloat2IntParams = malloc(sizeof(list));
    initList(funcFloat2IntParams);

    makeString("f", &funcParamId);

    funcParam.tokenType = IDENT;
    funcParam.tokenName = funcParamId;

    addToken(funcFloat2IntParams, funcParam.tokenType, funcParam.tokenName.data);

    makeString("float64", &funcParamId);

    funcParam.tokenType = FLOAT;
    funcParam.tokenName = funcParamId;

    addToken(funcFloat2IntParams, funcParam.tokenType, funcParam.tokenName.data);
    insertNode(globalTable, funcId.data, funcFloat2IntRetTypes, funcFloat2IntParams, 0);

    ///FUNC LEN
    makeString("len", &funcId);

    dataType *funcLenRetTypes = malloc(2 * sizeof(dataType));
    funcLenRetTypes[0] = TYPE_INT;
    funcLenRetTypes[1] = TYPE_UNDEFINED;

    list *funcLenParams = malloc(sizeof(list));
    initList(funcLenParams);

    makeString("s", &funcParamId);

    funcParam.tokenType = IDENT;
    funcParam.tokenName = funcParamId;

    addToken(funcLenParams, funcParam.tokenType, funcParam.tokenName.data);

    makeString("string", &funcParamId);

    funcParam.tokenType = STRING;
    funcParam.tokenName = funcParamId;

    addToken(funcLenParams, funcParam.tokenType, funcParam.tokenName.data);
    insertNode(globalTable, funcId.data, funcLenRetTypes, funcLenParams, 0);

    ///FUNC SUBSTR
    makeString("substr", &funcId);

    dataType *funcSubstrRetTypes = malloc(3 * sizeof(dataType));
    funcSubstrRetTypes[0] = TYPE_STRING;
    funcSubstrRetTypes[1] = TYPE_INT;
    funcSubstrRetTypes[2] = TYPE_UNDEFINED;

    list *funcSubstrParams = malloc(sizeof(list));
    initList(funcSubstrParams);

    makeString("s", &funcParamId);

    funcParam.tokenType = IDENT;
    funcParam.tokenName = funcParamId;
    addToken(funcSubstrParams, funcParam.tokenType, funcParam.tokenName.data);

    makeString("string", &funcParamId);

    funcParam.tokenType = STRING;
    funcParam.tokenName = funcParamId;
    addToken(funcSubstrParams, funcParam.tokenType, funcParam.tokenName.data);

    makeString(",", &funcParamId);

    funcParam.tokenType = COMMA;
    funcParam.tokenName = funcParamId;
    addToken(funcSubstrParams, funcParam.tokenType, funcParam.tokenName.data);

    makeString("i", &funcParamId);

    funcParam.tokenType = IDENT;
    funcParam.tokenName = funcParamId;
    addToken(funcSubstrParams, funcParam.tokenType, funcParam.tokenName.data);

    makeString("int", &funcParamId);

    funcParam.tokenType = INT;
    funcParam.tokenName = funcParamId;
    addToken(funcSubstrParams, funcParam.tokenType, funcParam.tokenName.data);

    makeString(",", &funcParamId);

    funcParam.tokenType = COMMA;
    funcParam.tokenName = funcParamId;
    addToken(funcSubstrParams, funcParam.tokenType, funcParam.tokenName.data);

    makeString("n", &funcParamId);

    funcParam.tokenType = IDENT;
    funcParam.tokenName = funcParamId;
    addToken(funcSubstrParams, funcParam.tokenType, funcParam.tokenName.data);

    makeString("int", &funcParamId);

    funcParam.tokenType = INT;
    funcParam.tokenName = funcParamId;
    addToken(funcSubstrParams, funcParam.tokenType, funcParam.tokenName.data);

    insertNode(globalTable, funcId.data, funcSubstrRetTypes, funcSubstrParams, 0);

    ///FUNC ORD
    makeString("ord", &funcId);

    dataType *funcOrdRetTypes = malloc(3 * sizeof(dataType));
    funcOrdRetTypes[0] = TYPE_INT;
    funcOrdRetTypes[1] = TYPE_INT;
    funcOrdRetTypes[2] = TYPE_UNDEFINED;

    list *funcOrdParams = malloc(sizeof(list));
    initList(funcOrdParams);

    makeString("s", &funcParamId);

    funcParam.tokenType = IDENT;
    funcParam.tokenName = funcParamId;
    addToken(funcOrdParams, funcParam.tokenType, funcParam.tokenName.data);

    makeString("string", &funcParamId);

    funcParam.tokenType = STRING;
    funcParam.tokenName = funcParamId;
    addToken(funcOrdParams, funcParam.tokenType, funcParam.tokenName.data);

    makeString(",", &funcParamId);

    funcParam.tokenType = COMMA;
    funcParam.tokenName = funcParamId;
    addToken(funcOrdParams, funcParam.tokenType, funcParam.tokenName.data);

    makeString("i", &funcParamId);

    funcParam.tokenType = IDENT;
    funcParam.tokenName = funcParamId;
    addToken(funcOrdParams, funcParam.tokenType, funcParam.tokenName.data);

    makeString("int", &funcParamId);

    funcParam.tokenType = INT;
    funcParam.tokenName = funcParamId;
    addToken(funcOrdParams, funcParam.tokenType, funcParam.tokenName.data);

    insertNode(globalTable, funcId.data, funcOrdRetTypes, funcOrdParams, 0);

    ///FUNC CHR
    makeString("chr", &funcId);

    dataType *funcChrRetTypes = malloc(3 * sizeof(dataType));
    funcChrRetTypes[0] = TYPE_STRING;
    funcChrRetTypes[1] = TYPE_INT;
    funcChrRetTypes[2] = TYPE_UNDEFINED;

    list *funcChrParams = malloc(sizeof(list));
    initList(funcChrParams);

    makeString("i", &funcParamId);

    funcParam.tokenType = IDENT;
    funcParam.tokenName = funcParamId;
    addToken(funcChrParams, funcParam.tokenType, funcParam.tokenName.data);

    makeString("int", &funcParamId);

    funcParam.tokenType = INT;
    funcParam.tokenName = funcParamId;
    addToken(funcChrParams, funcParam.tokenType, funcParam.tokenName.data);

    insertNode(globalTable, funcId.data, funcChrRetTypes, funcChrParams, 0);

    destroyString(&funcId);

    size_t length = tokenList->size;

    for (size_t i = 0; i < length; i++) { // run through the whole token list

        string funcIdent;
        initString(&funcIdent);
        token func;
        getToken(tokenList, i, &func);

        if (func.tokenType == FUNC) {  // *FUNC* ...

            if (func.nextToken != NULL && func.nextToken->tokenType == IDENT) { // if next token is ident
                i++; // increase iterator
                token ident;
                initString(&ident.tokenName);
                getToken(tokenList, i, &ident); // get the identifier token
                makeString(ident.tokenName.data, &funcIdent); //uloží identifikator do stringu

                if (equalStrings(funcIdent.data, "main")) { // if identifier is main, ignore it
                    destroyString(&funcIdent);
                    continue;
                }

                if (ident.nextToken != NULL && equalStrings("(", ident.nextToken->tokenName.data)) { // if next token is opening bracket
                    i += 2; // increase past the bracket
                    token param;

                    list *parameters = malloc(sizeof(list)); // create the list for storing parameters
                    initList(parameters); //list parametrov

                    getToken(tokenList, i, &param); // FUNC IDENT ( *IDENT* ...

                    while (!equalStrings(")", param.tokenName.data)) { // while we aren't on closing bracket
                        if (param.nextToken != NULL) {
                            switch (param.tokenType) {
                                case IDENT: // if this token is identifier
                                    if (param.nextToken->tokenType == INT || param.nextToken->tokenType == STRING ||
                                        param.nextToken->tokenType == FLOAT) { // next token can be only a datatype
                                        addToken(parameters, param.tokenType, param.tokenName.data);
                                        i++;
                                        getToken(tokenList, i, &param);
                                    } else {
                                        deleteList(parameters);
                                        destroyString(&funcIdent);
                                        return SYNTAX_ERROR;
                                    }
                                    break;

                                case INT:
                                case FLOAT:
                                case STRING: // if this token is datatype
                                    if (param.nextToken->tokenType == COMMA || // next token can be either a comma or )
                                         equalStrings(")", param.nextToken->tokenName.data)) {
                                        addToken(parameters, param.tokenType, param.tokenName.data); // ad the token to parameters list
                                        i++;
                                        getToken(tokenList, i, &param); // continue to next token
                                    } else {
                                        deleteList(parameters);
                                        destroyString(&funcIdent);
                                        return SYNTAX_ERROR;
                                    }
                                    break;

                                case COMMA: // if this token is a comma
                                    if (param.nextToken->tokenType == IDENT || param.nextToken->tokenType == EOL) {
                                        addToken(parameters, param.tokenType, param.tokenName.data); // next token can be identifier or eol
                                        i++;
                                        getToken(tokenList, i, &param);
                                    } else {
                                        deleteList(parameters);
                                        destroyString(&funcIdent);
                                        return SYNTAX_ERROR;
                                    }
                                    break;
                                case EOL: // if this token is eol
                                    if (param.nextToken->tokenType == EOL || param.nextToken->tokenType == IDENT) {
                                        // next token can be identifier or another eol
                                        i++;
                                        getToken(tokenList, i, &param);
                                    } else {
                                        deleteList(parameters);
                                        destroyString(&funcIdent);
                                        return SYNTAX_ERROR;
                                    }
                                    break;
                                default: // if there is anything except eols, idents, commas and datatypes, it's syntax error
                                    deleteList(parameters);
                                    destroyString(&funcIdent);
                                    return SYNTAX_ERROR;
                            }
                        } else {
                            deleteList(parameters);
                            destroyString(&funcIdent);
                            return SYNTAX_ERROR;
                        }

                    } // po while  ->  FUNC IDENT ( TYPE IDENT , ... *)*

                    dataType *retTypesArray = malloc(100 * sizeof(dataType));
                    for (int iter = 0; iter < 100; iter++) retTypesArray[iter] = TYPE_UNDEFINED;

                    if (param.nextToken != NULL && equalStrings("(", param.nextToken->tokenName.data)) {

                        i += 2; // jump right to datatype token
                        token returnTypes;
                        getToken(tokenList, i, &returnTypes); // FUNC IDENT ( TYPE IDENT , ... ) *(* ...
                        if (returnTypes.nextToken != NULL) {

                            int count = 0;
                            while (!equalStrings(")", returnTypes.tokenName.data)) {
                                if (returnTypes.nextToken != NULL) {
                                    switch (returnTypes.tokenType) {
                                        case INT:
                                            if (returnTypes.nextToken->tokenType == COMMA ||
                                                returnTypes.nextToken->tokenType == BRACKET_ROUND) {
                                                retTypesArray[count] = TYPE_INT;
                                                count++;
                                            } else {
                                                deleteList(parameters);
                                                destroyString(&funcIdent);
                                                return SYNTAX_ERROR;
                                            }
                                            break;
                                        case FLOAT:
                                            if (returnTypes.nextToken->tokenType == COMMA ||
                                                returnTypes.nextToken->tokenType == BRACKET_ROUND) {
                                                retTypesArray[count] = TYPE_FLOAT;
                                                count++;
                                            } else {
                                                deleteList(parameters);
                                                destroyString(&funcIdent);
                                                return SYNTAX_ERROR;
                                            }
                                            break;
                                        case STRING:
                                            if (returnTypes.nextToken->tokenType == COMMA ||
                                                returnTypes.nextToken->tokenType == BRACKET_ROUND) {
                                                retTypesArray[count] = TYPE_STRING;
                                                count++;
                                            } else {
                                                deleteList(parameters);
                                                destroyString(&funcIdent);
                                                return SYNTAX_ERROR;
                                            }
                                            break;
                                        case COMMA:
                                            if (returnTypes.nextToken->tokenType == INT ||
                                                returnTypes.nextToken->tokenType == FLOAT ||
                                                returnTypes.nextToken->tokenType == STRING) {

                                            } else {
                                                deleteList(parameters);
                                                destroyString(&funcIdent);
                                                return SYNTAX_ERROR;
                                            }
                                            break;

                                        default:
                                            deleteList(parameters);
                                            destroyString(&funcIdent);
                                            return SYNTAX_ERROR;

                                    }
                                    i++;
                                    getToken(tokenList, i, &returnTypes);
                                } else {
                                    deleteList(parameters);
                                    destroyString(&funcIdent);
                                    return SYNTAX_ERROR;
                                }

                            } // FUNC IDENT ( TYPE IDENT , ... ) ( RET_TYPE, ... *)*

                            if (returnTypes.nextToken != NULL && equalStrings("{", returnTypes.nextToken->tokenName.data)) {
                                i++;
                                getToken(tokenList, i, &returnTypes); // FUNC IDENT ( IDENT TYPE , ... ) ( RET_TYPE, ... ) *{*
                                if (returnTypes.nextToken != NULL && returnTypes.nextToken->tokenType == EOL) {
                                    if (copyNode(globalTable, funcIdent.data) == NULL) {
                                        insertNode(globalTable, funcIdent.data, retTypesArray, parameters, 0);
                                        destroyString(&funcIdent);
                                    } else {
                                        deleteList(parameters);
                                        destroyString(&funcIdent);
                                        return DEFINITION_ERROR;
                                    }
                                } else {
                                    deleteList(parameters);
                                    destroyString(&funcIdent);
                                    return SYNTAX_ERROR;
                                }
                            }

                        } else {
                            deleteList(parameters);
                            destroyString(&funcIdent);
                            return SYNTAX_ERROR;
                        }

                    } else {
                        if (param.nextToken != NULL && equalStrings("{", param.nextToken->tokenName.data)) {
                            i++;
                            getToken(tokenList, i, &param); // FUNC IDENT ( TYPE IDENT , ... ) *{*
                            if (param.nextToken != NULL && param.nextToken->tokenType == EOL) { // after { must be an eol
                                if (copyNode(globalTable, funcIdent.data) == NULL) {
                                    insertNode(globalTable, funcIdent.data, retTypesArray, parameters, 0);
                                    destroyString(&funcIdent);
                                } else {
                                    deleteList(parameters);
                                    destroyString(&funcIdent);
                                    return DEFINITION_ERROR;
                                }
                            } else {
                                deleteList(parameters);
                                destroyString(&funcIdent);
                                return SYNTAX_ERROR;
                            }
                        } else {
                            deleteList(parameters);
                            destroyString(&funcIdent);
                            return SYNTAX_ERROR;
                        }
                    }
                } else {
                    destroyString(&funcIdent);
                    return SYNTAX_ERROR;
                }

            } else {
                destroyString(&funcIdent);
                return SYNTAX_ERROR;
            }
        }
    }

    return OK;
}

//package main
errorCode blockA(list *tokenList) {
    token curToken;
    getToken(tokenList, 0, &curToken);

    if (curToken.tokenType != PACKAGE) return SYNTAX_ERROR;

    getToken(tokenList, 1, &curToken);

    if (strcmp(curToken.tokenName.data, "main") != 0) return SYNTAX_ERROR;

    return OK;

}

//function code
errorCode blockB(list *tokenList, token curToken) {

    int openBracketCount = 1;
    int closedBracketCount = 0;

    while (openBracketCount != closedBracketCount) {

        if (curToken.nextToken == NULL)
            if (openBracketCount != closedBracketCount) return SYNTAX_ERROR;
        curToken = *curToken.nextToken;


        if (curToken.tokenType == BRACKET_CURLY) {
            if (equalStrings(curToken.tokenName.data, "{")) {
                openBracketCount++;
            } else {
                closedBracketCount++;
            }
        }
    }


    if (openBracketCount != closedBracketCount) return SYNTAX_ERROR;

    return OK;

}

//expression
errorCode blockExpression(list *tokenList, token *curToken, bool forState, bool expressionList) {

    int openBracketCount = 0;
    int closedBracketCount = 0;

    while (curToken->tokenType != EOL) {

        if (forState == true && curToken->tokenType == SEMICOL) break;

        if (!expressionList && curToken->nextToken->tokenType == COMMA) return SYNTAX_ERROR;

        if (curToken->tokenType == COMMA)
            if (curToken->nextToken->tokenType != IDENT && curToken->nextToken->tokenType != INT_LIT &&
                curToken->nextToken->tokenType != FLOAT_LIT &&
                curToken->nextToken->tokenType != STRING_LIT)

                if (curToken->tokenType == BRACKET_ROUND && equalStrings(curToken->tokenName.data, "(")) {
                    openBracketCount++;
                    if (curToken->nextToken->tokenType != IDENT) return SYNTAX_ERROR;
                    curToken = curToken->nextToken;
                    continue;
                }

        if (curToken->tokenType == BRACKET_ROUND && equalStrings(curToken->tokenName.data, ")")) {
            closedBracketCount++;
            if (curToken->nextToken->tokenType != ARIT_OPERATOR && curToken->nextToken->tokenType != COMP_OPERATOR &&
                curToken->nextToken->tokenType != EOL)
                return SYNTAX_ERROR;
            curToken = curToken->nextToken;
            continue;
        }

        if (curToken->tokenType == IDENT || curToken->tokenType == INT_LIT || curToken->tokenType == FLOAT_LIT ||
            curToken->tokenType == STRING_LIT) {
            if (curToken->nextToken->tokenType != ARIT_OPERATOR && curToken->nextToken->tokenType != COMP_OPERATOR &&
                curToken->nextToken->tokenType != COMMA && curToken->nextToken->tokenType != EOL)
                return SYNTAX_ERROR;
            curToken = curToken->nextToken;
            continue;
        }

        if (curToken->tokenType == ARIT_OPERATOR || curToken->tokenType == COMP_OPERATOR) {
            if (curToken->nextToken->tokenType != IDENT && curToken->nextToken->tokenType != BRACKET_ROUND)
                return SYNTAX_ERROR;
            curToken = curToken->nextToken;
            continue;
        }

        return SYNTAX_ERROR;

    }

    if (openBracketCount != closedBracketCount) return SYNTAX_ERROR;

    return OK;
}

//asign
errorCode blockAssign(list *tokenList, token *curToken, bool forState) {

    int openBracketCount = 0;
    int closedBracketCount = 0;

    if (curToken->tokenType != IDENT) return SYNTAX_ERROR;

    while (curToken->tokenType != EOL) {

        if (curToken->tokenType == COMMA) {
            if (curToken->nextToken->tokenType != IDENT) return SYNTAX_ERROR;
            curToken = curToken->nextToken;
            continue;
        }


        if (forState == true && curToken->tokenType == SEMICOL) break;

        if (curToken->tokenType == IDENT || curToken->tokenType == INT_LIT || curToken->tokenType == FLOAT_LIT ||
            curToken->tokenType == STRING_LIT) {
            if (curToken->nextToken->tokenType == ASIGN_OPERATOR &&
                equalStrings(curToken->nextToken->tokenName.data, "="))
                return SYNTAX_ERROR;

            if (curToken->nextToken->tokenType != ARIT_OPERATOR && curToken->nextToken->tokenType != BRACKET_ROUND &&
                curToken->nextToken->tokenType != ASIGN_OPERATOR)
                return SYNTAX_ERROR;

            if (curToken->nextToken->tokenType == BRACKET_ROUND &&
                equalStrings(curToken->nextToken->tokenName.data, ")"))
                return SYNTAX_ERROR;

            curToken = curToken->nextToken;
            continue;
        }

        if (curToken->tokenType == ARIT_OPERATOR || curToken->tokenType == BRACKET_ROUND ||
            curToken->tokenType == ASIGN_OPERATOR) {

            if (equalStrings(curToken->tokenName.data, "("))
                openBracketCount++;
            if (equalStrings(curToken->tokenName.data, ")"))
                closedBracketCount++;

            if (curToken->tokenType == ASIGN_OPERATOR && equalStrings(curToken->tokenName.data, "="))
                return SYNTAX_ERROR;

            if (curToken->nextToken->tokenType != IDENT && curToken->nextToken->tokenType != INT_LIT &&
                curToken->nextToken->tokenType != FLOAT_LIT && curToken->nextToken->tokenType != STRING_LIT)
                return SYNTAX_ERROR;

            if (curToken->tokenType == BRACKET_ROUND && equalStrings(curToken->tokenName.data, ")"))
                return SYNTAX_ERROR;

            curToken = curToken->nextToken;
            continue;
        }

    }
    if (openBracketCount != closedBracketCount) return SYNTAX_ERROR;

    return OK;
}

//term
errorCode pregenerateDefvar(token curToken) {


    list localList;
    initList(&localList);


    int ifCount = 0;
    int forCount = 0;


    while (curToken.tokenType != FUNC) {

        if (curToken.nextToken == NULL) break;

        if (curToken.tokenType != EOL) {

            addToken(&localList, curToken.tokenType, curToken.tokenName.data);

        } else {

            for (int i = 0; i < localList.size; i++) {

                token localToken;
                initString(&localToken.tokenName);
                getToken(&localList, i, &localToken);

                if (localToken.tokenType == ASIGN_OPERATOR) {
                    deleteList(&localList);
                    break;
                }


                if (localToken.tokenType == IF) {
                    ifCount++;

                    char *name = malloc(50 * sizeof(char));
                    if (name == NULL) return INTERNAL_ERROR;

                    sprintf(name, "ifBool_%d", ifCount);
                    makeString(name, &localToken.tokenName);

                    free(name);


                }
                if (localToken.tokenType == FOR) {
                    forCount++;

                    char *name = malloc(50 * sizeof(char));
                    if (name == NULL) return INTERNAL_ERROR;

                    sprintf(name, "forBool_%d", ifCount);
                    makeString(name, &localToken.tokenName);

                    free(name);
                }

                generateDefvar(&localToken);


            }
        }
        curToken = *curToken.nextToken;
    }


    return OK;
}

//definition
errorCode blockDefinition(list *tokenList, token *curToken, bool forState) {

    if (curToken->tokenType != IDENT) return SYNTAX_ERROR;
    curToken = curToken->nextToken;

    while (curToken->tokenType != EOL || equalStrings(":=", curToken->tokenName.data)) {

        if (forState == true && curToken->tokenType == SEMICOL) break;

        if (curToken->tokenType == COMMA) {
            if (curToken->nextToken->tokenType != IDENT) return SYNTAX_ERROR;
            curToken = curToken->nextToken;
            continue;
        }

        if (curToken->tokenType == IDENT) {
            if (curToken->nextToken->tokenType != COMMA && !equalStrings(":=", curToken->tokenName.data))
                return SYNTAX_ERROR;
            curToken = curToken->nextToken;
            continue;
        }
        if (curToken->tokenType == EOL) return SYNTAX_ERROR;

    }

    if (curToken->tokenType != ASIGN_OPERATOR && !equalStrings(":=", curToken->tokenName.data)) return SYNTAX_ERROR;
    curToken = curToken->nextToken;

    return blockExpression(tokenList, curToken, forState, false);

}


errorCode parse(list *tokenList, string *code) {

    tableNodePtr globalTable;
    initTable(&globalTable);
    errorCode returnError = fillSymtable(globalTable, tokenList);

    tableNodePtr localTable;
    initTable(&localTable);

    size_t curLevel = 0;
    size_t forCount = 0;
    size_t ifCount = 0;
    size_t retCount = 0;


    list buffer;
    initList(&buffer);

    bool isIf = false;
    bool isFor = false;
    bool isFunc = false;

    if (returnError != OK) {
        return returnError;
    }

    returnError = blockA(tokenList);
    if (returnError != OK) return returnError;


    generatorInit();

    errorCode generatorError = generatorStart();
    if (generatorError != OK) return generatorError;


    token savedToken;

    for (int i = 2; i < tokenList->size; i++) {

        token curToken;
        getToken(tokenList, i, &curToken);

        if (curToken.tokenType == EOL)
            if (curToken.nextToken->tokenType != EOL) savedToken = *curToken.nextToken;


        if (curToken.tokenType == BRACKET_CURLY) {
            if (equalStrings(curToken.tokenName.data, "{")) {
                curLevel++;
                token tempToken;
                initString(&tempToken.tokenName);


                if (isIf || isFor) {

                    char *name = malloc(50 * sizeof(char));
                    if (name == NULL) return INTERNAL_ERROR;

                    if (isIf) {
                        isIf = false;
                        tempToken.tokenType = IF;
                        ifCount++;

                        sprintf(name, "%zu", ifCount);
                    } else {
                        isFor = false;
                        tempToken.tokenType = FOR;
                        isFor++;

                        sprintf(name, "%zu", forCount);
                    }

                    makeString(name, &tempToken.tokenName);
                    pushToken(&buffer, &tempToken);

                    free(name);

                }

            } else {
                if (buffer.first->tokenType == IF) {
                    int x = atoi(buffer.first->tokenName.data);
                    generateIfEnd(x);
                } else {
                    int x = atoi(buffer.first->tokenName.data);
                    generateForEndLabel(x);

                }
                popToken(&buffer);
                curLevel--;
            }
        }

        if (curToken.tokenType == FUNC) {

            list args;
            initList(&args);

            while (curToken.tokenType != BRACKET_CURLY) {
                if (curToken.tokenType != COMMA)
                    addToken(&args, curToken.tokenType, curToken.tokenName.data);
            }
            curToken = *savedToken.nextToken;
            token temp = *args.first;


            for(int j = 0; j < args.size; j++){
                dataType *type = malloc(sizeof(dataType));

                if(temp.tokenType == INT){
                    type[0] = TYPE_INT;
                    insertNode(&localTable, temp.tokenName.data, type , NULL, 0);
                }
                if(temp.tokenType == FLOAT){
                    type[0] = TYPE_FLOAT;
                    insertNode(&localTable, temp.tokenName.data, type , NULL, 0);
                }
                if(temp.tokenType == STRING){
                    type[0] = TYPE_STRING;
                    insertNode(&localTable, temp.tokenName.data, type , NULL, 0);
                }
                temp = *temp.nextToken;
            }
            deleteList(&args);
            curToken = savedToken;

            isFunc = true;

            //GENERATE FUNC START
            generatorError = generateFunctionStart(copyNode(&globalTable, curToken.nextToken->tokenName.data));


            if (generatorError != OK) return generatorError;

            //GENERATE FUNC DEFVAR

            generatorError = pregenerateDefvar(*curToken.nextToken);
            if (generatorError != OK) return generatorError;
        }

        if (equalStrings(curToken.tokenName.data, "{") && isFunc == true) {

            returnError = blockB(tokenList, curToken);
            if (returnError != OK) return returnError;
        }

        //GENERATE IF START
        if (curToken.tokenType == IF) {
            ifCount++;
            isIf = true;
            list condition;
            initList(&condition);

            token temp = curToken;

            while (temp.tokenType != EOL) {
                addToken(&condition, temp.tokenType, temp.tokenName.data);
            }

            generateIfStart(&condition, localTable, ifCount);


            returnError = blockExpression(tokenList, curToken.nextToken, false, false);
            if (returnError != OK) return returnError;

        }

        //CHECK IF IF HAS ELSE FOR SYNTAX
        //GENERATE ELSE
        if (curToken.tokenType == ELSE) {

            if (buffer.first->tokenType == FOR) return SYNTAX_ERROR;

            int x = atoi(buffer.first->tokenName.data);
            generateElse(x);
        }
        //GENERATE FOR
        if (curToken.tokenType == FOR) {

            forCount++;
            isFor = true;
            list condition;
            initList(&condition);

            list savedVars;
            initList(&savedVars);

            token firstTemp = curToken;
            token secondTemp;
            token thirdTemp;

            token appliedToken;
            token forVar;

            bool foundDef = false;

            forVar = firstTemp;

            returnError = blockDefinition(tokenList, curToken.nextToken, true);
            if (returnError != OK) return returnError;

            secondTemp = *curToken.nextToken;


            returnError = blockExpression(tokenList, curToken.nextToken, true, false);
            if (returnError != OK) return returnError;

            thirdTemp = curToken;

            returnError = blockAssign(tokenList, curToken.nextToken, true);
            if (returnError != OK) return returnError;

            generateForPrequel(forCount);

            while (firstTemp.tokenType != SEMICOL) {
                firstTemp = *firstTemp.nextToken;

                if (foundDef)
                    addToken(&condition, firstTemp.tokenType, firstTemp.tokenName.data);

                if (equalStrings(firstTemp.tokenName.data, ":=")) {
                    foundDef = true;
                }
            }
            if (foundDef) {
                generatorError = applyPrecedence(&condition, localTable);
                if (generatorError != OK) return generatorError;

                generatorError = generateMove(&forVar);
                if (generatorError != OK) return generatorError;
            } else {
                return SYNTAX_ERROR;
            }

            deleteList(&condition);
            initList(&condition);

            while (thirdTemp.tokenType != ASIGN_OPERATOR) {
                thirdTemp = *thirdTemp.nextToken;

                addToken(&savedVars, thirdTemp.tokenType, thirdTemp.tokenName.data);

            }
            while (thirdTemp.tokenType != BRACKET_CURLY) {
                thirdTemp = *thirdTemp.nextToken;

                addToken(&condition, thirdTemp.tokenType, thirdTemp.tokenName.data);
            }

            for (int j = 0; j < savedVars.size; j++) {
                token temp;
                getToken(&savedVars, j, &temp);
                applyPrecedence(&condition, localTable);
                generateMove(&temp);
            }
            deleteList(&condition);
            initList(&condition);
            deleteList(&savedVars);

            while (secondTemp.tokenType != SEMICOL) {
                addToken(&condition, secondTemp.tokenType, secondTemp.tokenName.data);
            }
            generateForStart(&condition, localTable, forCount);

            deleteList(&savedVars);
            deleteList(&condition);

        }

        if (curToken.tokenType == RETURN) {

            retCount++;
            if (curToken.nextToken->tokenType != EOL)
                returnError = blockExpression(tokenList, curToken.nextToken, false, true);
            if (returnError != OK) return returnError;

            curToken = savedToken;

            while (curToken.tokenType != EOL) {
                curToken = *curToken.nextToken;

                token temp;
                list condition;
                initList(&condition);
                token appliedToken;
                token retToken;
                retToken.tokenType = RETURN;
                initString(&retToken.tokenName);
                char *name = malloc(6 * sizeof(char));
                sprintf(name, "ret%zu", retCount);

                makeString(name, &retToken.tokenName);

                applyPrecedence(&condition, localTable);
                generateMove(&retToken);
                free(name);
                destroyString(&retToken.tokenName);
                deleteList(&condition);
            }

            //GENERATE FUNCTION END
            generatorError = generateFunctionEnd();
            if (generatorError != OK) return generatorError;


            isFunc = false;
        }

        if (curToken.tokenType == IDENT) {
            savedToken = curToken;

            bool foundDefAsign = false;

            list condition;
            initList(&condition);

            list savedVar;
            initList(&savedVar);

            list asignVar;
            initList(&asignVar);
            addToken(&asignVar, curToken.tokenType, curToken.tokenName.data);

            bool beforeAsignOperator = false;

            while (curToken.tokenType != EOL) {

                if (beforeAsignOperator)
                    addToken(&asignVar, curToken.tokenType, curToken.tokenName.data);

                if (curToken.tokenType == ASIGN_OPERATOR && equalStrings(curToken.tokenName.data, ":=")) {

                    while (curToken.tokenType != EOL) {
                        addToken(&condition, curToken.tokenType, curToken.tokenName.data);
                    }

                    beforeAsignOperator = true;
                    curToken = savedToken;
                    returnError = blockDefinition(tokenList, &curToken, false);
                    if (returnError != OK) return returnError;

                    curToken = savedToken;

                    //GENERATE ASSIGN
                    token temp;
                    generatorError = applyPrecedence(&condition, localTable);
                    if (generatorError != OK) return generatorError;

                    generatorError = generateMove(&savedToken);
                    if (generatorError != OK) return generatorError;

                    foundDefAsign = true;
                    break;
                } else if (curToken.tokenType == ASIGN_OPERATOR && equalStrings(curToken.tokenName.data, "=")) {
                    beforeAsignOperator = true;

                    data *curFunc = copyNode(&globalTable, curToken.nextToken->tokenName.data);
                    if (curFunc != NULL) {
                        list argValues;
                        initList(&argValues);
                        while (equalStrings(curToken.tokenName.data, ")")) {
                            curToken = *curToken.nextToken;
                            if (curToken.tokenType != COMMA)
                                addToken(&argValues, curToken.tokenType, curToken.tokenName.data);
                        }

                        //GENERATE FUNCTION CALLS AND RETURN
                        curToken = savedToken;
                        generatorError = generateFunctionCall(curFunc, &argValues);
                        if (generatorError != OK) return generatorError;

                        generatorError = generateFunctionReturn(curFunc, &asignVar);
                        if (generatorError != OK) return generatorError;


                    } else {
                        //GENERATE ASIGN 2

                        while (curToken.tokenType != EOL) {
                            addToken(&condition, curToken.tokenType, curToken.tokenName.data);
                        }

                        curToken = savedToken;

                        while (curToken.tokenType != ASIGN_OPERATOR) {
                            if (curToken.tokenType == IDENT)
                                addToken(&savedVar, curToken.tokenType, curToken.tokenName.data);
                        }

                        curToken = savedToken;

                        for (int j = 0; j < savedVar.size; j++) {
                            token temp;
                            token appliedToken;
                            getToken(&savedVar, j, &temp);

                            generatorError = applyPrecedence(&condition, localTable);
                            if (generatorError != OK) return generatorError;
                            generatorError = generateMove(&temp);
                            if (generatorError != OK) return generatorError;
                        }

                        deleteList(&condition);
                        deleteList(&savedVar);
                    }

                    returnError = blockAssign(tokenList, &curToken, false);
                    if (returnError != OK) return returnError;

                    foundDefAsign = true;
                    break;
                }

                curToken = *curToken.nextToken;
            }

            if (foundDefAsign) {
                foundDefAsign = false;
            } else {

                list argValues;
                initList(&argValues);

                curToken = savedToken;
                data *curFunc = copyNode(&globalTable, curToken.tokenName.data);

                if (curFunc != NULL) {
                    while (equalStrings(curToken.tokenName.data, ")")) {
                        curToken = *curToken.nextToken;
                        if (curToken.tokenType != COMMA)
                            addToken(&argValues, curToken.tokenType, curToken.tokenName.data);
                    }

                    generatorError = generateFunctionCall(curFunc, &argValues);
                    if (generatorError != OK) return generatorError;

                    generatorError = generateFunctionReturn(curFunc, NULL);
                    if (generatorError != OK) return generatorError;

                }


                returnError = blockExpression(tokenList, &curToken, false, false);
                if (returnError != OK) return returnError;
            }


        }

    }
    deleteList(&buffer);
    generatorWrite(stdout);
    generatorClear();
    return OK;
}