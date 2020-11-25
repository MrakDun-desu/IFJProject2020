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

    // ------------------------------ Checking datatype compatibility and zero division ------------------------------ //

    if (tokenList != NULL) {
        size_t i = 0;
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
                        for (prevTok = tok; prevTok != NULL; getToken(tokenList, i-count, prevTok)) {
                            // run through tokens until we reach something which isn't ")"
                            if (equalStrings(prevTok->tokenName.data, ")")) // if token isn't ), that means we got variable or literal
                                break;
                            count++;
                        }
                        i+= count; // update i so it still represents current token
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
            i++;
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

            for (token *tok = tokenList->first; tok != NULL; tok = tok->nextToken) {
                if (equalStrings(tok->nextToken->tokenName.data,
                                 ":=")) { // if this is the definition command, create new entry in symtable
                    data *thisSymbol;
                    if (copyNode(&globalTable, tok->tokenName.data) !=
                        NULL) // if this symbol has same name as a function, return definition error
                        return DEFINITION_ERROR;
                    if ((thisSymbol = copyNode(&localTable, tok->tokenName.data)) !=
                        NULL) { // if this symbol has same name as variable
                        // look at the level of scope we are currently in
                        if (scope >
                            thisSymbol->scope) {  // if current scope is more nested, then we can override the variable
                        } else return
                    }
                }
                if (tok->tokenType == IDENT && !equalStrings(tok->tokenName.data, "_")) {
                    if (copyNode(&globalTable, tok->tokenName.data) == NULL &&
                        copyNode(&localTable, tok->tokenName.data) == NULL)
                        return DEFINITION_ERROR;
                }
            }

            // ------------------------------ Checking return semantics ------------------------------ //

            if (tokenList->first->tokenType == RETURN) {
            }

            // ------------------------------ Checking assign semantics ------------------------------ //
        }
    }
    return OK;

}