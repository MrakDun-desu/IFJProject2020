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
        for (token* tok = tokenList->first; tok != NULL; tok = tok->nextToken) {
            if (tok->nextToken != NULL) { // first check if there is next token
                token* operator = tok->nextToken;
                if (operator->tokenType == ARIT_OPERATOR || operator->tokenType == COMP_OPERATOR) { // then check if it's operator
                    if (operator->nextToken != NULL) { // check if it has next token just to be sure (should be handled by syntax analysis)
                        if (equalStrings(operator->tokenName.data, "/") && // check if operator is division
                        operator->nextToken->tokenType != IDENT) { // and check if next token is literal
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

        if (tokenList->first != NULL) { // all the next checks require first token, so checking if it's not null

            // ------------------------------ Checking if scope has ended ------------------------------ //

            if (tokenList->first->tokenType == BRACKET_CURLY && equalStrings(tokenList->first->tokenName.data, "}")) {
                if (scope != 0) {
                    invalidateScope(&localTable, scope);
                    scope--;
                    invalidateScope(&localTable, scope);
                    scope--;
                } else
                    deleteTable(&localTable);
            }

            // ------------------------------ Checking if semantics ------------------------------ //

            if (tokenList->first->tokenType == IF) {
                bool wasComparator = false;
                for (token *tok = tokenList->first;
                     tok != NULL; tok = tok->nextToken) {
                    if (tok->tokenType == COMP_OPERATOR) {
                        if (wasComparator) return SEMANTIC_ERROR; // if there was more than one comparator, return general semantic error
                        else wasComparator = true;
                    }
                }
                if (!wasComparator) return TYPE_COMPATIBILITY_ERROR; // if there was no comparator, return compatibility error
                scope++; // increasing the scope twice since we are entering if
                scope++;
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
                bool wasDefinition = false;
                token* newSymbol;
                for (token *tok = tokenList->first->nextToken; // we need to begin from second one, since first token is just for
                     tok != NULL; tok = tok->nextToken) {
                    if (tok->tokenType == SEMICOL)
                        semicolonCount++;
                    else if (equalStrings(tok->tokenName.data, ":="))
                        wasDefinition = true;
                    else if (semicolonCount == 0) { // if there wasn't a semicolon, we are in definition part
                        if (tok->tokenType == IDENT && !wasDefinition) // if this is the identifier before definition token
                            newSymbol = tok; // store the token for future use
                        else if (tok->nextToken->tokenType == SEMICOL && wasDefinition) { // if we are just before the semicolon
                            dataType* type = malloc(sizeof(dataType));
                            bool isVar = false;
                            switch (tok->tokenType) { // token type just before the semicolon should only be a literal or identifier
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
                                default:
                                    break;
                            }
                            if (isVar) { // if it was variable, further checking is needed
                                data* lastVar;
                                if ((lastVar = copyNode(&localTable, tok->tokenName.data)) == NULL) {
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
                            if (wasComparator) return SEMANTIC_ERROR; // if there was more than one comparator, return general semantic error
                            else wasComparator = true;
                        }
                    }
                    else if (semicolonCount == 2) { // if there were two semicolons, we are in the end command part
                        if (tok->tokenType != )
                    }
                }
                scope++; // increasing the scope again after first line
            }

            /// TODO vymysliet jak u foru znizit scope 2x

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
                if (tok->tokenType == IDENT) {
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