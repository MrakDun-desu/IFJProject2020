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

errorCode checkFunctionTypes(list* tokenList, data* func, size_t i, tableNodePtr localTable, dataType** back) {
    dataType* paramTypes = malloc(sizeof(dataType) * func->parameters->size/2);
    size_t typeCount = 0;

    // first, we need to check which parameter types function expects
    for (token* iter = func->parameters->first; iter != NULL; iter = iter->nextToken) {
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
    token* nextTok = NULL;
    getToken(tokenList, i+1, nextTok);
    for (size_t j = i + 1; nextTok != NULL; getToken(tokenList, ++j, nextTok)) {
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
        }
        else {
            if (inParams && !checkDatatype(paramTypes[nextCommaCount], nextTok, localTable)) {
                // if we have a term, we need to check for the right type. If it doesn't
                // correspond, return error
                free(paramTypes);
                return PARAMETER_ERROR;
            }
        }

    }
    if (typeCount != 0 && nextCommaCount != typeCount-1) {
        free(paramTypes);
        return PARAMETER_ERROR;
    }

    *back = func->types;
    return OK;
}

errorCode semanticAnalyser(list* tokenList, tableNodePtr globalTable, tableNodePtr localTable, data* function) {

    // ------------------------------ Checking datatype compatibility and zero division ------------------------------ //

    if (tokenList != NULL) {
        size_t tokCounter = 0;
        for (token* tok = tokenList->first; tok != NULL; tok = tok->nextToken) {
            if (tok->nextToken != NULL) { // first check if there is next token
                token* operator = tok->nextToken;
                if (operator->tokenType == ARIT_OPERATOR || operator->tokenType == COMP_OPERATOR) { // then check if it's operator
                    if (operator->nextToken != NULL) { // check if it has next token just to be sure (should be handled by syntax analysis)
                        if (equalStrings(operator->tokenName.data, "/") && // check if operator is division
                        (operator->nextToken->tokenType == INT_LIT || operator->nextToken->tokenType == FLOAT_LIT)) { // and check if next token is literal
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
                        size_t count = 0;
                        token* prevTok;
                        for (prevTok = tok; prevTok != NULL; getToken(tokenList, tokCounter-count, prevTok)) {
                            // run through tokens until we reach something which isn't ")"
                            if (equalStrings(prevTok->tokenName.data, ")")) // if token isn't ), that means we got variable or literal
                                break;
                            count++;
                        }
                        tokCounter+= count; // update i so it still represents current token
                        data* var = copyNode(&localTable, prevTok->tokenName.data);
                        bool varString = false;
                        bool varInt = false;
                        bool varFloat = false;
                        if (var != NULL) {
                            varString = (var->types[0] == TYPE_STRING);
                            varInt = (var->types[0] == TYPE_INT);
                            varFloat = (var->types[0] == TYPE_FLOAT);
                        }
                        if (prevTok->tokenType == STRING_LIT || varString) { // if first token was string, operator cannot be - * /
                            if (equalStrings(operator->tokenName.data, "-")) return TYPE_COMPATIBILITY_ERROR;
                            if (equalStrings(operator->tokenName.data, "*")) return TYPE_COMPATIBILITY_ERROR;
                            if (equalStrings(operator->tokenName.data, "/")) return TYPE_COMPATIBILITY_ERROR;
                            for (token* nextTok = operator->nextToken; nextTok != NULL; nextTok = nextTok->nextToken) {
                                if (!equalStrings(nextTok->tokenName.data, "(")) {
                                    // run through next tokens until variable or literal is reached
                                    if (!checkDatatype(TYPE_STRING, nextTok, localTable))
                                        return TYPE_COMPATIBILITY_ERROR;
                                }
                            }
                        }
                        else if (prevTok->tokenType == INT_LIT || varInt) {
                            for (token* nextTok = operator->nextToken; nextTok != NULL; nextTok = nextTok->nextToken) {
                                if (!equalStrings(nextTok->tokenName.data, "(")) {
                                    // run through next tokens until variable or literal is reached
                                    if (!checkDatatype(TYPE_INT, nextTok, localTable))
                                        return TYPE_COMPATIBILITY_ERROR;
                                }
                            }
                        }
                        else if (prevTok->tokenType == FLOAT_LIT || varFloat) {
                            for (token* nextTok = operator->nextToken; nextTok != NULL; nextTok = nextTok->nextToken) {
                                if (!equalStrings(nextTok->tokenName.data, "(")) {
                                    // run through next tokens until variable or literal is reached
                                    if (!checkDatatype(TYPE_FLOAT, nextTok, localTable))
                                        return TYPE_COMPATIBILITY_ERROR;
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
                if (tokenList->first->nextToken->tokenType == ELSE) { // else should always be second token, if it is there
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
                token* newSymbol;
                for (token *tok = tokenList->first->nextToken; // we need to begin from second one, since first token is just for keyword
                     tok != NULL; tok = tok->nextToken) {
                    if (tok->tokenType == SEMICOL) {
                        semicolonCount++;
                        if (semicolonCount == 2 && wasComparator == false) // if we are after second semicolon and there
                            // was no comparator, we aren't getting bool value, so return error.
                            return TYPE_COMPATIBILITY_ERROR;
                    }
                    else if (equalStrings(tok->tokenName.data, ":="))
                        wasDefinition = true;
                    else if (semicolonCount == 0) { // if there wasn't a semicolon, we are in definition part
                        if (tok->tokenType == IDENT && !wasDefinition) // if this is the identifier before definition token
                            newSymbol = tok; // store the token for future use
                        else if (wasDefinition && !decidedType) {

                            token* nextTok; // create next token, which should define the type of newSymbol
                            for (nextTok = tok; nextTok != NULL; nextTok = nextTok->nextToken) {
                                // run through next tokens until variable or literal is reached
                                if (!equalStrings(nextTok->tokenName.data, "("))
                                    break;
                            }

                            decidedType = true; // we are going to add this to symtable, so setting this so we don't do it again
                            dataType* type = malloc(sizeof(dataType));
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
                                data* lastVar;
                                if ((lastVar = copyNode(&localTable, nextTok->tokenName.data)) == NULL) {
                                    // if searching in local table yields no results, this variable was undefined
                                    free(type);
                                    return DEFINITION_ERROR;
                                }
                                else {
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
                    }
                    else if (semicolonCount == 1) { // if there was one semicolon, we are in the comparing part
                        if (tok->tokenType == COMP_OPERATOR) {
                            if (wasComparator || wasBracket) return SEMANTIC_ERROR;
                            // if there was more than one comparator or comparator is in brackets, return general semantic error
                            else wasComparator = true;
                        }
                        else if (equalStrings(tok->tokenName.data, "("))
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
                if (equalStrings(tok->nextToken->tokenName.data,
                                 ":=")) { // if this is the definition command, create new entry in symtable
                    data *thisSymbol;
                    if (equalStrings(tok->tokenName.data, "_")) return DEFINITION_ERROR;

                    thisSymbol = copyNode(&localTable, tok->tokenName.data);
                    bool isNull = thisSymbol == NULL;
                        if (isNull || scope > thisSymbol->scope || !thisSymbol->defined) {
                            dataType* newType = malloc(sizeof(dataType));
                            for (token* nextTok = tok->nextToken; nextTok != NULL; nextTok = nextTok->nextToken) {
                                // with this cycle, we decide which datatype the variable should be
                                if (nextTok->tokenType == IDENT) {
                                    data* var = copyNode(&localTable, nextTok->tokenName.data);
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
                                }
                                else if (nextTok->tokenType == INT_LIT) {
                                    newType[0] = TYPE_INT;
                                    break;
                                }
                                else if (nextTok->tokenType == STRING_LIT) {
                                    newType[0] = TYPE_STRING;
                                    break;
                                }
                                else if (nextTok->tokenType == FLOAT_LIT) {
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
                        }
                        else return DEFINITION_ERROR;
                    }

                else if (tok->tokenType == IDENT && (!equalStrings(tok->tokenName.data, "_") || wasAssign)) {
                    if (copyNode(&globalTable, tok->tokenName.data) == NULL &&
                        copyNode(&localTable, tok->tokenName.data) == NULL)
                        return DEFINITION_ERROR;
                }
            }

            // ------------------------------ Checking return semantics ------------------------------ //

            if (tokenList->first->tokenType == RETURN) { // if we are returning from the function
                size_t commaCounter = 0; // for knowing which return value are we checking
                size_t typeCount = 0; // for knowing how much types we have to control
                dataType* types = function->types;
                for (dataType type = types[0]; type != TYPE_UNDEFINED; type = types[++typeCount]);
                for (token* tok = tokenList->first; tok != NULL; tok = tok->nextToken) {
                    if (tok->tokenType == COMMA) { // if we are at a coma, just increase the counter
                        commaCounter++;
                        if (commaCounter >= typeCount) return PARAMETER_ERROR;
                        // if commaCounter is bigger than it should, jump out of function right away
                    }
                    else {

                        // return expressions must have the same data types as all variables/literals in expressions

                        if (types[commaCounter] == TYPE_INT) {
                            if (tok->tokenType != INT_LIT) { // if it's correct literal, we don't have to check
                                if (!checkDatatype(TYPE_INT, tok, localTable))
                                    return PARAMETER_ERROR;
                            }
                        }
                        else if (types[commaCounter] == TYPE_STRING) {
                            if (tok->tokenType != STRING_LIT) {
                                if (!checkDatatype(TYPE_STRING, tok, localTable))
                                    return PARAMETER_ERROR;
                            }
                        }
                        else if (types[commaCounter] == TYPE_FLOAT) {
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
                token* tok = NULL;
                getToken(tokenList, i, tok);
                if (equalStrings(tok->tokenName.data, "=")) { // if we encounter =, it means this is assign
                    size_t prevCommaCount = 0; // comma counts to check the same number of expressions
                    size_t nextCommaCount = 0;
                    token* prevTok = NULL;
                    getToken(tokenList, i-1, prevTok); // previous token to check before the = operator
                    token* nextTok = NULL;
                    getToken(tokenList, i+1, nextTok); // next token to check after the = operator

                    dataType* prevTypes = NULL; // types that are before operator
                    dataType* nextTypes = NULL; // types that are after the operator

                    for (size_t j = i-1; j != 0 && prevTok != NULL; getToken(tokenList, --j, prevTok)) {
                        // before the = operator, there should only be commas and variable identifiers.

                        if (prevTok->tokenType == COMMA)
                            prevCommaCount++;
                        else if (prevTok->tokenType != IDENT)
                            break;
                    }

                    data* func;
                    if ((func = copyNode(&globalTable, nextTok->tokenName.data)) == NULL) {
                        // after the = operator, there can be function or a list of
                        // expressions separated by commas.
                        for (size_t j = i + 1; nextTok != NULL; getToken(tokenList, ++j, nextTok)) {
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

                        nextTypes = malloc(sizeof(dataType) * (nextCommaCount+2));
                        for (size_t j = 0; j < nextCommaCount+2; j++)
                            nextTypes[j] = TYPE_UNDEFINED;
                        // if there was no function, we need to check next data types term after term
                        getToken(tokenList, i+1, nextTok); // begin the iteration from start again
                        nextCommaCount = 0;
                        for (size_t j = i + 1; nextTok != NULL; getToken(tokenList, ++j, nextTok)) {
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
                    prevTypes = malloc(sizeof(dataType) * (prevCommaCount+2));
                    for (size_t j = 0; j < prevCommaCount+2; j++)
                        prevTypes[j] = TYPE_UNDEFINED;
                    getToken(tokenList, i-1, prevTok); // begin the iteration from start again
                    prevCommaCount = 0;
                    for (size_t j = i - 1; prevTok != NULL; getToken(tokenList, --j, prevTok)) {
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

                    for (size_t j = 0; j < prevCommaCount+1; j++) { // at the end, iterate one last time to check if data types match
                        if (nextTypes[j] != prevTypes[j] || prevTypes[j] != TYPE_UNDEFINED)
                            // we need to add TYPE_UNDEFINED here in case one of prev identifiers was _ and undefined stayed with it
                            return TYPE_COMPATIBILITY_ERROR;
                    }

                    break;
                }
            }

            // ------------------------------ Checking function call semantics ------------------------------ //

            data* func;
            if ((func = copyNode(&globalTable, tokenList->first->tokenName.data)) == NULL) { // if we use function without assign
                dataType* types = NULL;
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