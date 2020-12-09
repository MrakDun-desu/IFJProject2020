/******************************** generator.c **********************************/
/*  Predmet: IFJ a IAL						                                  */
/*  Kod na generovanie a pomocne funkcie                                      */
/*  Vytvorili: Marek Danco xdanco00 Frantisek Fudor xfudor00                  */
/* ************************************************************************** */

#include "generator.h"

errorCode pregenerateDefvar(list *tokenList, token curToken, size_t i) {

    list localList;
    initList(&localList);

    for (; curToken.nextToken != NULL && curToken.tokenType != FUNC; curToken = *curToken.nextToken) {

        if (equalStrings(curToken.tokenName.data, ":=")) {
            token localToken;
            getToken(tokenList, i - 1, &localToken);
            bool wasDefined = false;
            for (token *tmp = localList.first; tmp != NULL; tmp = tmp->nextToken) {
                if (equalStrings(tmp->tokenName.data, localToken.tokenName.data)) {
                    wasDefined = true;
                    break;
                }
            }
            if (!wasDefined && localToken.tokenType == IDENT) {
                if (addToken(&localList, localToken.tokenType, localToken.tokenName.data)) {
                    deleteList(&localList);
                    return INTERNAL_ERROR;
                }
                if (generateDefvar(&localToken)) {
                    deleteList(&localList);
                    return INTERNAL_ERROR;
                }
            }
        }

        i++;
    }
    deleteList(&localList);


    return OK;
}

errorCode assignHandle(list *assign, tableNodePtr globalTable, tableNodePtr localTable) {

    if (assign->first != NULL) {
        list vars;
        initList(&vars);
        list expression;
        initList(&expression);
        errorCode code;

        bool wasAssign = false;
        bool wasFunction = false;
        int commaCounter = 0;

        for (token *tok = assign->first; tok != NULL; tok = tok->nextToken) {
            if (tok->tokenType == ASIGN_OPERATOR) {
                wasAssign = true;
            } else if (!wasAssign && tok->tokenType == IDENT && !equalStrings(tok->tokenName.data, "_")) {
                if ((code = addToken(&vars, tok->tokenType, tok->tokenName.data)) != OK) {
                    deleteList(&vars);
                    deleteList(&expression);
                    return code;
                }
            } else if (wasAssign && tok->tokenType == COMMA) {
                if (!equalStrings(copyToken(&vars, commaCounter)->tokenName.data, "_")) {
                    if ((code = applyPrecedence(&expression, localTable)) != OK) {
                        deleteList(&vars);
                        deleteList(&expression);
                        return code;
                    }
                    if ((code = generateMove(copyToken(&vars, commaCounter))) != OK) {
                        deleteList(&vars);
                        deleteList(&expression);
                        return code;
                    }
                }
                deleteList(&expression);
                commaCounter++;
            } else if (wasAssign && copyNode(&globalTable, tok->tokenName.data) == NULL) {
                if ((code = addToken(&expression, tok->tokenType, tok->tokenName.data)) != OK) {
                    deleteList(&vars);
                    deleteList(&expression);
                    return code;
                }
            } else if (wasAssign && copyNode(&globalTable, tok->tokenName.data) != NULL) {
                for (token *arg = tok->nextToken; arg != NULL; arg = arg->nextToken) {
                    if (arg->tokenType != COMMA && arg->tokenType != BRACKET_ROUND) {
                        if ((code = addToken(&expression, arg->tokenType, arg->tokenName.data)) != OK) {
                            deleteList(&vars);
                            deleteList(&expression);
                            return code;
                        }
                    }
                }
                if ((code = generateFunctionCall(copyNode(&globalTable, tok->tokenName.data), &expression)) != OK) {
                    deleteList(&vars);
                    deleteList(&expression);
                    return code;
                }
                if ((code = generateFunctionReturn(&vars)) != OK) {
                    deleteList(&vars);
                    deleteList(&expression);
                    return code;
                }
                wasFunction = true;
                break;
            }
        }

        if (!wasFunction) {
            if (!equalStrings(copyToken(&vars, commaCounter)->tokenName.data, "_")) {
                if ((code = applyPrecedence(&expression, localTable)) != OK) {
                    deleteList(&vars);
                    deleteList(&expression);
                    return code;
                }
                if ((generateMove(copyToken(&vars, commaCounter))) != OK) {
                    deleteList(&vars);
                    deleteList(&expression);
                    return code;
                }
            }
            deleteList(&expression);
        }

        deleteList(&expression);
        deleteList(&vars);
    }

    return OK;
}

errorCode
generatorHandle(list *currentLine, list *tokenList, tableNodePtr globalTable, tableNodePtr localTable, list *ifStack,
                data *currentFunc) {

    errorCode code;
    if (currentLine->first != NULL) {
        if (currentLine->first->tokenType == PACKAGE) {
            generatorInit();
            code = generatorStart();
            if (code) return code;
        } else if (currentLine->first->tokenType == FUNC) {
            token *funcIdent = currentLine->first->nextToken;
            data *func = copyNode(&globalTable, funcIdent->tokenName.data);
            code = generateFunctionStart(func);
            if (code) return code;
            token *curToken;
            size_t i = 1;
            for (curToken = tokenList->first; curToken != NULL; curToken = curToken->nextToken) {
                if (curToken->tokenType == FUNC &&
                    equalStrings(curToken->nextToken->tokenName.data, funcIdent->tokenName.data)) {
                    break;
                }
                i++;
            }

            code = pregenerateDefvar(tokenList, *curToken->nextToken, i);
            if (code) return code;
        } else if (currentLine->first->tokenType == BRACKET_CURLY &&
                   !equalStrings(currentLine->first->tokenName.data, "{")) {
            if (ifStack->first == NULL) {
                if (currentFunc != NULL)
                    code = generateFunctionEnd();
                else
                    code = generateMainScopeEnd();
                if (code) return code;
            } else if (ifStack->first->tokenType == IF) {
                if (currentLine->first->nextToken->tokenType == ELSE) {
                    int ifCount = atoi(ifStack->first->tokenName.data);
                    code = generateElse(ifCount);
                    if (code) return code;
                } else {
                    int ifCount = atoi(ifStack->first->tokenName.data);
                    code = generateIfEnd(ifCount);
                    if (code) return code;
                    token *temp = popToken(ifStack);
                    destroyString(&temp->tokenName);
                    free(temp);
                }
            } else {
                int forCount = atoi(ifStack->first->tokenName.data);
                code = generateForEnd(forCount);
                if (code) return code;
                token *temp = popToken(ifStack);
                destroyString(&temp->tokenName);
                free(temp);
            }
        } else if (currentLine->first->tokenType == IF) {
            list condition;
            initList(&condition);
            for (token *temp = currentLine->first->nextToken;
                 temp != NULL && temp->tokenType != BRACKET_CURLY; temp = temp->nextToken) {
                code = addToken(&condition, temp->tokenType, temp->tokenName.data);
                if (code) {
                    deleteList(&condition);
                    return code;
                }
            }
            int ifCount = atoi(ifStack->first->tokenName.data);
            code = generateIfStart(&condition, localTable, ifCount);
            if (code) {
                deleteList(&condition);
                return code;
            }
            deleteList(&condition);

        } else if (currentLine->first->tokenType == FOR) {
            int forCounter = atoi(ifStack->first->tokenName.data);

            list assign;
            initList(&assign);
            list condition;
            initList(&condition);
            int semicolCount = 0;
            for (token *tok = currentLine->first;
                 tok != NULL && tok->tokenType != BRACKET_CURLY; tok = tok->nextToken) {
                if (tok->tokenType == SEMICOL) {
                    if (semicolCount == 0) {
                        code = assignHandle(&assign, globalTable, localTable);
                        if (code) {
                            deleteList(&assign);
                            deleteList(&condition);
                            return code;
                        }
                        deleteList(&assign);
                        generateForPrequel(forCounter);
                    }
                    semicolCount++;
                } else if (semicolCount == 0) {
                    if (tok->tokenType != FOR) {
                        code = addToken(&assign, tok->tokenType, tok->tokenName.data);
                        if (code) {
                            deleteList(&assign);
                            deleteList(&condition);
                            return code;
                        }
                    }
                } else if (semicolCount == 1) {
                    code = addToken(&condition, tok->tokenType, tok->tokenName.data);
                    if (code) {
                        deleteList(&assign);
                        deleteList(&condition);
                        return code;
                    }
                } else if (semicolCount == 2) {
                    code = addToken(&assign, tok->tokenType, tok->tokenName.data);
                    if (code) {
                        deleteList(&assign);
                        deleteList(&condition);
                        return code;
                    }
                }
            }
            code = assignHandle(&assign, globalTable, localTable);
            if (code) {
                deleteList(&assign);
                deleteList(&condition);
                return code;
            }

            deleteList(&assign);

            code = generateForStart(&condition, localTable, forCounter);
            if (code) {
                deleteList(&assign);
                deleteList(&condition);
                return code;
            }
            deleteList(&condition);
            deleteList(&assign);

        } else if (copyNode(&globalTable, currentLine->first->tokenName.data) != NULL) {
            list args;
            initList(&args);
            for (token *tok = currentLine->first->nextToken; tok != NULL && !equalStrings(tok->tokenName.data, ")");) {
                if (tok->tokenType == IDENT || tok->tokenType == INT_LIT || tok->tokenType == STRING_LIT ||
                    tok->tokenType == FLOAT_LIT) {
                    code = addToken(&args, tok->tokenType, tok->tokenName.data);
                    if (code) {
                        deleteList(&args);
                        return code;
                    }
                }
            }
            code = generateFunctionCall(copyNode(&globalTable, currentLine->first->tokenName.data), &args);
            if (code) {
                deleteList(&args);
                return code;
            }
            deleteList(&args);
        } else if (equalStrings(currentLine->first->tokenName.data, "print")) {
            for (token *arg = currentLine->first->nextToken; arg != NULL; arg = arg->nextToken) {
                if (equalStrings(arg->tokenName.data, ")")) break;
                else if (arg->tokenType == INT_LIT || arg->tokenType == STRING_LIT || arg->tokenType == FLOAT_LIT ||
                         arg->tokenType == IDENT) {
                    code = generatePrint(arg);
                    if (code) return code;
                }
            }
        } else if (currentLine->first->tokenType == RETURN) {
            size_t commaCounter = 0;
            list exp;
            initList(&exp);
            token ret;
            initString(&ret.tokenName);
            ret.tokenType = IDENT;
            ret.nextToken = NULL;
            char retName[50];
            for (token *tok = currentLine->first->nextToken; tok != NULL; tok = tok->nextToken) {
                if (tok->tokenType == COMMA) {
                    code = applyPrecedence(&exp, localTable);
                    if (code) {
                        deleteList(&exp);
                        return code;
                    }
                    deleteList(&exp);
                    sprintf(retName, "ret%zu", commaCounter);
                    code = makeString(retName, &ret.tokenName);
                    if (code) {
                        destroyString(&ret.tokenName);
                        deleteList(&exp);
                        return code;
                    }
                    code = generateMove(&ret);
                    if (code) {
                        deleteList(&exp);
                        destroyString(&ret.tokenName);
                        return code;
                    }
                    commaCounter++;
                } else if (tok->tokenType != EOL) {
                    code = addToken(&exp, tok->tokenType, tok->tokenName.data);
                    if (code) {
                        destroyString(&ret.tokenName);
                        deleteList(&exp);
                        return code;
                    }
                }
            }
            code = applyPrecedence(&exp, localTable);
            if (code) {
                destroyString(&ret.tokenName);
                deleteList(&exp);
                return code;
            }
            deleteList(&exp);
            sprintf(retName, "ret%zu", commaCounter);
            code = makeString(retName, &ret.tokenName);
            if (code) {
                destroyString(&ret.tokenName);
                deleteList(&exp);
                return code;
            }
            code = generateMove(&ret);
            if (code) {
                destroyString(&ret.tokenName);
                deleteList(&exp);
                return code;
            }

            destroyString(&ret.tokenName);
            deleteList(&exp);
        } else if (currentLine->first->tokenType != EOL) {
            code = assignHandle(currentLine, globalTable, localTable);
            if (code) return code;
        }

    }

    return OK;
}

errorCode transformFloat(string *str) {
    double number = strtod(str->data, NULL);

    char newFloat[100];
    sprintf(newFloat, "%a", number);
    if (makeString(newFloat, str)) return INTERNAL_ERROR;

    return OK;
}

errorCode transformString(string *str) {

    string tmp;
    initString(&tmp);
    if (makeString("", &tmp)) return INTERNAL_ERROR;
    string tmp2;
    initString(&tmp2);
    if (makeString("", &tmp2)) {
        destroyString(&tmp);
        return INTERNAL_ERROR;
    }
    errorCode out;
    char tmpStr[100];
    for (size_t i = 0; i < str->len; i++) {
        char c = str->data[i];
        if (c == '\\') {
            int escape;
            switch (str->data[i + 1]) {
                case 'n':
                    if ((out = addChar(&tmp, '\n')) != OK) {
                        destroyString(&tmp);
                        destroyString(&tmp2);
                        return out;
                    }
                    i++;
                    break;
                case '\\':
                    if ((out = addChar(&tmp, '\\')) != OK) {
                        destroyString(&tmp);
                        destroyString(&tmp2);
                        return out;
                    }
                    i++;
                    break;
                case 't':
                    if ((out = addChar(&tmp, '\t')) != OK) {
                        destroyString(&tmp);
                        destroyString(&tmp2);
                        return out;
                    }
                    i++;
                    break;
                case '\"':
                    if ((out = addChar(&tmp, '\"')) != OK) {
                        destroyString(&tmp);
                        destroyString(&tmp2);
                        return out;
                    }
                    i++;
                    break;
                case 'x':
                    escape = strtol(&str->data[i + 2], NULL, 16);
                    if ((out = addChar(&tmp, (char) escape)) != OK) {
                        destroyString(&tmp);
                        destroyString(&tmp2);
                        return out;
                    }
                    i += 3;
                    break;
                default:
                    return LEXICAL_ERROR;
            }
        } else {
            if ((out = addChar(&tmp, c)) != OK) {
                destroyString(&tmp);
                destroyString(&tmp2);
                return out;
            }
        }
    }

    for (size_t i = 0; i < tmp.len; i++) {
        char c = tmp.data[i];
        if (c < 33) {
            if ((out = addChar(&tmp2, 92)) != OK) {
                destroyString(&tmp);
                destroyString(&tmp2);
                return out;
            }
            if ((out = addChar(&tmp2, '0')) != OK) {
                destroyString(&tmp);
                destroyString(&tmp2);
                return out;
            }
            if (c < 10) {
                if ((out = addChar(&tmp2, '0')) != OK) {
                    destroyString(&tmp);
                    destroyString(&tmp2);
                    return out;
                }
            }
            sprintf(tmpStr, "%d", c);
            if ((out = addConstChar(&tmp2, tmpStr)) != OK) {
                destroyString(&tmp);
                destroyString(&tmp2);
                return out;
            }
        } else if (c == 35 || c == 92) {
            if ((out = addChar(&tmp2, 92)) != OK) {
                destroyString(&tmp);
                destroyString(&tmp2);
                return out;
            }
            if ((out = addChar(&tmp2, '0')) != OK) {
                destroyString(&tmp);
                destroyString(&tmp2);
                return out;
            }
            sprintf(tmpStr, "%d", c);
            if ((out = addConstChar(&tmp2, tmpStr)) != OK) {
                destroyString(&tmp);
                destroyString(&tmp2);
                return out;
            }
        } else {
            if ((out = addChar(&tmp2, c)) != OK) {
                destroyString(&tmp);
                destroyString(&tmp2);
                return out;
            }
        }
    }
    if ((out = makeString(tmp2.data, str)) != OK) {
        destroyString(&tmp);
        destroyString(&tmp2);
        return out;
    }
    destroyString(&tmp2);
    destroyString(&tmp);
    return OK;
}

///VOID FUNCTIONS

void generatorInit() {
    initString(gen.program);
    makeString(".IFJcode20\n", gen.program);
}

void generatorClear() {
    destroyString(gen.program);
}

///BASIC CODE

errorCode generatorStart() {
    char str[100];

    for (size_t i = 0; i < 10; i++) {
        sprintf(str, "DEFVAR GF@expVar%zu\n", i);
        if (addConstChar(gen.program, str)) return INTERNAL_ERROR;
    }

    sprintf(str, "JUMP main\n");
    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;

    if (addConstChar(gen.program, FUNC_INPUTS)) return INTERNAL_ERROR;
    if (addConstChar(gen.program, FUNC_INPUTI)) return INTERNAL_ERROR;
    if (addConstChar(gen.program, FUNC_INPUTF)) return INTERNAL_ERROR;

    if (addConstChar(gen.program, FUNC_INT2FLOAT)) return INTERNAL_ERROR;
    if (addConstChar(gen.program, FUNC_FLOAT2INT)) return INTERNAL_ERROR;

    if (addConstChar(gen.program, FUNC_LEN)) return INTERNAL_ERROR;

    if (addConstChar(gen.program, FUNC_SUBSTR)) return INTERNAL_ERROR;
    if (addConstChar(gen.program, FUNC_ORD)) return INTERNAL_ERROR;
    if (addConstChar(gen.program, FUNC_CHR)) return INTERNAL_ERROR;

    return OK;
}

errorCode generateDefvar(token *var) {
    char str[100];
    sprintf(str, "DEFVAR TF@%s\n", var->tokenName.data);
    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;

    return OK;
}

errorCode generateMove(token *var) {

    char str[100];

    sprintf(str, "MOVE TF@%s GF@expVar0\n", var->tokenName.data);

    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;

    return OK;
}

///ARITHMETIC FUNCTIONS

errorCode generateArithmetic(token *var, token *symb1, token *symb2, char *frames, char operation) {
    char str[100];
    errorCode out;

    if (symb1->tokenType == FLOAT_LIT) {
        out = transformFloat(&symb1->tokenName);
        if (out) return out;
    }
    if (symb2->tokenType == FLOAT_LIT) {
        out = transformFloat(&symb1->tokenName);
        if (out) return out;
    }

    switch (operation) {
        case '+':
            if (symb1->tokenType == INT_LIT || symb2->tokenType == INT_LIT) {
                if (symb1->tokenType == INT_LIT && symb2->tokenType == INT_LIT) {
                    sprintf(str, "ADD GF@%s int@%s int@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                } else if (symb1->tokenType == IDENT) {
                    if (frames[0] == 't') {
                        sprintf(str, "ADD GF@%s TF@%s int@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                    if (frames[0] == 'g') {
                        sprintf(str, "ADD GF@%s GF@%s int@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                } else if (symb2->tokenType == IDENT) {
                    if (frames[1] == 't') {
                        sprintf(str, "ADD GF@%s int@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                    if (frames[1] == 'g') {
                        sprintf(str, "ADD GF@%s int@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                }
            } else if (symb1->tokenType == FLOAT_LIT || symb2->tokenType == FLOAT_LIT) {
                if (symb1->tokenType == FLOAT_LIT && symb2->tokenType == FLOAT_LIT) {
                    sprintf(str, "ADD GF@%s float@%s float@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                } else if (symb1->tokenType == IDENT) {
                    if (frames[0] == 't') {
                        sprintf(str, "ADD GF@%s TF@%s float@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                    if (frames[0] == 'g') {
                        sprintf(str, "ADD GF@%s GF@%s float@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                } else if (symb2->tokenType == IDENT) {
                    if (frames[1] == 't') {
                        sprintf(str, "ADD GF@%s float@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                    if (frames[1] == 'g') {
                        sprintf(str, "ADD GF@%s float@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                }
            } else if (symb1->tokenType == IDENT && symb2->tokenType == IDENT) {
                if (equalStrings("tt", frames)) {
                    sprintf(str, "ADD GF@%s TF@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                } else if (equalStrings("gt", frames)) {
                    sprintf(str, "ADD GF@%s GF@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                } else if (equalStrings("tg", frames)) {
                    sprintf(str, "ADD GF@%s TF@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                } else if (equalStrings("gg", frames)) {
                    sprintf(str, "ADD GF@%s GF@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                }
            }

            break;
        case '-':
            if (symb1->tokenType == INT_LIT || symb2->tokenType == INT_LIT) {
                if (symb1->tokenType == INT_LIT && symb2->tokenType == INT_LIT) {
                    sprintf(str, "SUB GF@%s int@%s int@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                } else if (symb1->tokenType == IDENT) {
                    if (frames[0] == 't') {
                        sprintf(str, "SUB GF@%s TF@%s int@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                    if (frames[0] == 'g') {
                        sprintf(str, "SUB GF@%s GF@%s int@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                } else if (symb2->tokenType == IDENT) {
                    if (frames[1] == 't') {
                        sprintf(str, "SUB GF@%s int@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                    if (frames[1] == 'g') {
                        sprintf(str, "SUB GF@%s int@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                }
            } else if (symb1->tokenType == FLOAT_LIT || symb2->tokenType == FLOAT_LIT) {
                if (symb1->tokenType == FLOAT_LIT && symb2->tokenType == FLOAT_LIT) {
                    sprintf(str, "SUB GF@%s float@%s float@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                } else if (symb1->tokenType == IDENT) {
                    if (frames[0] == 't') {
                        sprintf(str, "SUB GF@%s TF@%s float@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                    if (frames[0] == 'g') {
                        sprintf(str, "SUB GF@%s GF@%s float@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                } else if (symb2->tokenType == IDENT) {
                    if (frames[1] == 't') {
                        sprintf(str, "SUB GF@%s float@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                    if (frames[1] == 'g') {
                        sprintf(str, "SUB GF@%s float@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                }
            } else if (symb1->tokenType == IDENT && symb2->tokenType == IDENT) {
                if (equalStrings("tt", frames)) {
                    sprintf(str, "SUB GF@%s TF@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                } else if (equalStrings("gt", frames)) {
                    sprintf(str, "SUB GF@%s GF@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                } else if (equalStrings("tg", frames)) {
                    sprintf(str, "SUB GF@%s TF@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                } else if (equalStrings("gg", frames)) {
                    sprintf(str, "SUB GF@%s GF@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                }
            }
            break;
        case '*':
            if (symb1->tokenType == INT_LIT || symb2->tokenType == INT_LIT) {
                if (symb1->tokenType == INT_LIT && symb2->tokenType == INT_LIT) {
                    sprintf(str, "MUL GF@%s int@%s int@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                } else if (symb1->tokenType == IDENT) {
                    if (frames[0] == 't') {
                        sprintf(str, "MUL GF@%s TF@%s int@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                    if (frames[0] == 'g') {
                        sprintf(str, "MUL GF@%s GF@%s int@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                } else if (symb2->tokenType == IDENT) {
                    if (frames[1] == 't') {
                        sprintf(str, "MUL GF@%s int@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                    if (frames[1] == 'g') {
                        sprintf(str, "MUL GF@%s int@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                }
            } else if (symb1->tokenType == FLOAT_LIT || symb2->tokenType == FLOAT_LIT) {
                if (symb1->tokenType == FLOAT_LIT && symb2->tokenType == FLOAT_LIT) {
                    sprintf(str, "MUL GF@%s float@%s float@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                } else if (symb1->tokenType == IDENT) {
                    if (frames[0] == 't') {
                        sprintf(str, "MUL GF@%s TF@%s float@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                    if (frames[0] == 'g') {
                        sprintf(str, "MUL GF@%s GF@%s float@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                } else if (symb2->tokenType == IDENT) {
                    if (frames[1] == 't') {
                        sprintf(str, "MUL GF@%s float@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                    if (frames[1] == 'g') {
                        sprintf(str, "MUL GF@%s float@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                }
            } else if (symb1->tokenType == IDENT && symb2->tokenType == IDENT) {
                if (equalStrings("tt", frames)) {
                    sprintf(str, "MUL GF@%s TF@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                } else if (equalStrings("gt", frames)) {
                    sprintf(str, "MUL GF@%s GF@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                } else if (equalStrings("tg", frames)) {
                    sprintf(str, "MUL GF@%s TF@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                } else if (equalStrings("gg", frames)) {
                    sprintf(str, "MUL GF@%s GF@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                }
            }
            break;
        case '/':
            if (symb1->tokenType == INT_LIT || symb2->tokenType == INT_LIT) {
                if (symb1->tokenType == INT_LIT && symb2->tokenType == INT_LIT) {
                    sprintf(str, "IDIV GF@%s int@%s int@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                } else if (symb1->tokenType == IDENT) {
                    if (frames[0] == 't') {
                        sprintf(str, "IDIV GF@%s TF@%s int@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                    if (frames[0] == 'g') {
                        sprintf(str, "IDIV GF@%s GF@%s int@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                } else if (symb2->tokenType == IDENT) {
                    if (frames[1] == 't') {
                        sprintf(str, "IDIV GF@%s int@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                    if (frames[1] == 'g') {
                        sprintf(str, "IDIV GF@%s int@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                }
            } else if (symb1->tokenType == IDENT && symb2->tokenType == IDENT) {
                if (equalStrings("tt", frames)) {
                    sprintf(str, "IDIV GF@%s TF@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                } else if (equalStrings("gt", frames)) {
                    sprintf(str, "IDIV GF@%s GF@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                } else if (equalStrings("tg", frames)) {
                    sprintf(str, "IDIV GF@%s TF@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                } else if (equalStrings("gg", frames)) {
                    sprintf(str, "IDIV GF@%s GF@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                }
            }
            break;
        case '%':
            if (symb1->tokenType == FLOAT_LIT || symb2->tokenType == FLOAT_LIT) {
                if (symb1->tokenType == FLOAT_LIT && symb2->tokenType == FLOAT_LIT) {
                    sprintf(str, "DIV GF@%s float@%s float@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                } else if (symb1->tokenType == IDENT) {
                    if (frames[0] == 't') {
                        sprintf(str, "DIV GF@%s TF@%s float@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                    if (frames[0] == 'g') {
                        sprintf(str, "DIV GF@%s GF@%s float@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                } else if (symb2->tokenType == IDENT) {
                    if (frames[1] == 't') {
                        sprintf(str, "DIV GF@%s float@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                    if (frames[1] == 'g') {
                        sprintf(str, "DIV GF@%s float@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                                symb2->tokenName.data);
                    }
                }
            } else if (symb1->tokenType == IDENT && symb2->tokenType == IDENT) {
                if (equalStrings("tt", frames)) {
                    sprintf(str, "DIV GF@%s TF@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                } else if (equalStrings("gt", frames)) {
                    sprintf(str, "DIV GF@%s GF@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                } else if (equalStrings("tg", frames)) {
                    sprintf(str, "DIV GF@%s TF@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                } else if (equalStrings("gg", frames)) {
                    sprintf(str, "DIV GF@%s GF@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                            symb2->tokenName.data);
                }
            }
            break;
        default:
            break;

    }

    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;

    return OK;

}

///COMPARING FUNCTIONS

errorCode generateLT(token *var, token *symb1, token *symb2, char *frames) {

    errorCode out;
    char str[100];
    if (symb1->tokenType == INT_LIT) {
        if (symb2->tokenType == INT_LIT) {
            sprintf(str, "LT GF@%s int@%s int@%s\n", var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
        }
        if (symb2->tokenType == IDENT) {
            if (frames[1] == 't') {
                sprintf(str, "LT GF@%s int@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (frames[1] == 'g') {
                sprintf(str, "LT GF@%s int@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
        }
    }

    if (symb1->tokenType == FLOAT_LIT) {

        out = transformFloat(&symb1->tokenName);
        if (out) return out;

        if (symb2->tokenType == FLOAT_LIT) {
            transformFloat(&symb2->tokenName);
            sprintf(str, "LT GF@%s float@%s float@%s\n", var->tokenName.data, symb1->tokenName.data,
                    symb2->tokenName.data);
        }
        if (symb2->tokenType == IDENT) {
            if (frames[1] == 't') {
                sprintf(str, "LT GF@%s float@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (frames[1] == 'g') {
                sprintf(str, "LT GF@%s float@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
        }
    }

    if (symb1->tokenType == STRING_LIT) {

        out = transformString(&symb1->tokenName);
        if (out) return out;

        if (symb2->tokenType == STRING_LIT) {

            out = transformString(&symb2->tokenName);
            if (out) return out;

            sprintf(str, "LT GF@%s string@%s string@%s\n", var->tokenName.data, symb1->tokenName.data,
                    symb2->tokenName.data);

        }
        if (symb2->tokenType == IDENT) {
            if (frames[1] == 't') {
                sprintf(str, "LT GF@%s string@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (frames[1] == 'g') {
                sprintf(str, "LT GF@%s string@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
        }
    }

    if (symb1->tokenType == IDENT) {
        if (symb2->tokenType == FLOAT_LIT) {

            out = transformFloat(&symb2->tokenName);
            if (out) return out;

            if (frames[0] == 't') {
                sprintf(str, "LT GF@%s TF@%s float@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (frames[0] == 'g') {
                sprintf(str, "LT GF@%s GF@%s float@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
        }

        if (symb2->tokenType == STRING_LIT) {
            out = transformString(&symb2->tokenName);
            if (out) return out;

            if (frames[0] == 't') {
                sprintf(str, "LT GF@%s TF@%s string@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (frames[0] == 'g') {
                sprintf(str, "LT GF@%s GF@%s string@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
        }

        if (symb2->tokenType == INT_LIT) {
            if (frames[0] == 't') {
                sprintf(str, "LT GF@%s TF@%s int@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (frames[0] == 'g') {
                sprintf(str, "LT GF@%s GF@%s int@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
        }

        if (symb2->tokenType == IDENT) {
            if (equalStrings("tt", frames)) {
                sprintf(str, "LT GF@%s TF@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (equalStrings("tg", frames)) {
                sprintf(str, "LT GF@%s TF@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (equalStrings("gt", frames)) {
                sprintf(str, "LT GF@%s GF@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (equalStrings("gg", frames)) {
                sprintf(str, "LT GF@%s GF@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
        }
    }
    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;
    return OK;
}

errorCode generateGT(token *var, token *symb1, token *symb2, char *frames) {

    errorCode out;
    char str[100];
    if (symb1->tokenType == INT_LIT) {
        if (symb2->tokenType == INT_LIT) {
            sprintf(str, "GT GF@%s int@%s int@%s\n", var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
        }
        if (symb2->tokenType == IDENT) {
            if (frames[1] == 't') {
                sprintf(str, "GT GF@%s int@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (frames[1] == 'g') {
                sprintf(str, "GT GF@%s int@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
        }
    }

    if (symb1->tokenType == FLOAT_LIT) {

        out = transformFloat(&symb1->tokenName);
        if (out) return out;

        if (symb2->tokenType == FLOAT_LIT) {
            out = transformFloat(&symb2->tokenName);
            if (out) return out;

            sprintf(str, "GT GF@%s float@%s float@%s\n", var->tokenName.data, symb1->tokenName.data,
                    symb2->tokenName.data);
        }
        if (symb2->tokenType == IDENT) {
            if (frames[1] == 't') {
                sprintf(str, "GT GF@%s float@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (frames[1] == 'g') {
                sprintf(str, "GT GF@%s float@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
        }
    }

    if (symb1->tokenType == STRING_LIT) {

        out = transformString(&symb1->tokenName);
        if (out) return out;

        if (symb2->tokenType == STRING_LIT) {

            out = transformString(&symb2->tokenName);
            if (out) return out;

            sprintf(str, "GT GF@%s string@%s string@%s\n", var->tokenName.data, symb1->tokenName.data,
                    symb2->tokenName.data);

        }
        if (symb2->tokenType == IDENT) {
            if (frames[1] == 't') {
                sprintf(str, "GT GF@%s string@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (frames[1] == 'g') {
                sprintf(str, "GT GF@%s string@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
        }
    }

    if (symb1->tokenType == IDENT) {
        if (symb2->tokenType == FLOAT_LIT) {

            out = transformFloat(&symb2->tokenName);
            if (out) return out;
            if (frames[0] == 't') {
                sprintf(str, "GT GF@%s TF@%s float@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (frames[0] == 'g') {
                sprintf(str, "GT GF@%s GF@%s float@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
        }

        if (symb2->tokenType == STRING_LIT) {

            out = transformString(&symb2->tokenName);
            if (out) return out;

            if (frames[0] == 't') {
                sprintf(str, "GT GF@%s TF@%s string@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (frames[0] == 'g') {
                sprintf(str, "GT GF@%s GF@%s string@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
        }

        if (symb2->tokenType == INT_LIT) {
            if (frames[0] == 't') {
                sprintf(str, "GT GF@%s TF@%s int@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (frames[0] == 'g') {
                sprintf(str, "GT GF@%s GF@%s int@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
        }

        if (symb2->tokenType == IDENT) {
            if (equalStrings("tt", frames)) {
                sprintf(str, "GT GF@%s TF@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (equalStrings("tg", frames)) {
                sprintf(str, "GT GF@%s TF@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (equalStrings("gt", frames)) {
                sprintf(str, "GT GF@%s GF@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (equalStrings("gg", frames)) {
                sprintf(str, "GT GF@%s GF@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
        }
    }
    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;
    return OK;
}

errorCode generateEQ(token *var, token *symb1, token *symb2, char *frames) {

    errorCode out;
    char str[100];
    if (symb1->tokenType == INT_LIT) {
        if (symb2->tokenType == INT_LIT) {
            sprintf(str, "EQ GF@%s int@%s int@%s\n", var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
        }
        if (symb2->tokenType == IDENT) {
            if (frames[1] == 't') {
                sprintf(str, "EQ GF@%s int@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (frames[1] == 'g') {
                sprintf(str, "EQ GF@%s int@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
        }
    }

    if (symb1->tokenType == FLOAT_LIT) {

        out = transformFloat(&symb1->tokenName);
        if (out) return out;
        if (symb2->tokenType == FLOAT_LIT) {

            out = transformFloat(&symb2->tokenName);
            if (out) return out;
            sprintf(str, "EQ GF@%s float@%s float@%s\n", var->tokenName.data, symb1->tokenName.data,
                    symb2->tokenName.data);
        }
        if (symb2->tokenType == IDENT) {
            if (frames[1] == 't') {
                sprintf(str, "EQ GF@%s float@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (frames[1] == 'g') {
                sprintf(str, "EQ GF@%s float@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
        }
    }

    if (symb1->tokenType == STRING_LIT) {

        out = transformString(&symb1->tokenName);
        if (out) return out;

        if (symb2->tokenType == STRING_LIT) {

            out = transformString(&symb2->tokenName);
            if (out) return out;

            sprintf(str, "EQ GF@%s string@%s string@%s\n", var->tokenName.data, symb1->tokenName.data,
                    symb2->tokenName.data);

        }
        if (symb2->tokenType == IDENT) {
            if (frames[1] == 't') {
                sprintf(str, "EQ GF@%s string@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (frames[1] == 'g') {
                sprintf(str, "EQ GF@%s string@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
        }
    }

    if (symb1->tokenType == IDENT) {
        if (symb2->tokenType == FLOAT_LIT) {

            out = transformFloat(&symb2->tokenName);
            if (out) return out;
            if (frames[0] == 't') {
                sprintf(str, "EQ GF@%s TF@%s float@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (frames[0] == 'g') {
                sprintf(str, "EQ GF@%s GF@%s float@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
        }

        if (symb2->tokenType == STRING_LIT) {
            out = transformString(&symb2->tokenName);
            if (out) return out;

            if (frames[0] == 't') {
                sprintf(str, "EQ GF@%s TF@%s string@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (frames[0] == 'g') {
                sprintf(str, "EQ GF@%s GF@%s string@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
        }

        if (symb2->tokenType == INT_LIT) {
            if (frames[0] == 't') {
                sprintf(str, "EQ GF@%s TF@%s int@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (frames[0] == 'g') {
                sprintf(str, "EQ GF@%s GF@%s int@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
        }

        if (symb2->tokenType == IDENT) {
            if (equalStrings("tt", frames)) {
                sprintf(str, "EQ GF@%s TF@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (equalStrings("tg", frames)) {
                sprintf(str, "EQ GF@%s TF@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (equalStrings("gt", frames)) {
                sprintf(str, "EQ GF@%s GF@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
            if (equalStrings("gg", frames)) {
                sprintf(str, "EQ GF@%s GF@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                        symb2->tokenName.data);
            }
        }
    }
    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;
    return OK;
}

///LOGICAL FUNCTIONS

errorCode generateNOT(token *var) {

    char str[100];
    sprintf(str, "NOT GF@%s GF@%s\n", var->tokenName.data, var->tokenName.data);

    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;
    return OK;
}

///STRING FUNCTIONS

errorCode generateConcat(token *var, token *symb1, token *symb2, char *frames) {

    errorCode out;
    char str[100];
    if (symb1->tokenType == STRING_LIT && symb2->tokenType == STRING_LIT) {

        out = transformString(&symb1->tokenName);
        if (out) return out;
        out = transformString(&symb2->tokenName);
        if (out) return out;

        sprintf(str, "CONCAT GF@%s string@%s string@%s\n", var->tokenName.data, symb1->tokenName.data,
                symb2->tokenName.data);

    }
    if (symb1->tokenType == IDENT && symb2->tokenType == IDENT) {

        if (equalStrings("tt", frames)) {
            sprintf(str, "CONCAT GF@%s TF@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                    symb2->tokenName.data);
        }
        if (equalStrings("tg", frames)) {
            sprintf(str, "CONCAT GF@%s TF@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                    symb2->tokenName.data);
        }
        if (equalStrings("gt", frames)) {
            sprintf(str, "CONCAT GF@%s GF@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                    symb2->tokenName.data);
        }
        if (equalStrings("gg", frames)) {
            sprintf(str, "CONCAT GF@%s GF@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                    symb2->tokenName.data);
        }
    }
    if (symb1->tokenType == IDENT && symb2->tokenType == STRING_LIT) {
        out = transformString(&symb2->tokenName);
        if (out) return out;

        if (frames[0] == 't') {
            sprintf(str, "CONCAT GF@%s TF@%s string@%s\n", var->tokenName.data, symb1->tokenName.data,
                    symb2->tokenName.data);
        }
        if (frames[0] == 'g') {
            sprintf(str, "CONCAT GF@%s GF@%s string@%s\n", var->tokenName.data, symb1->tokenName.data,
                    symb2->tokenName.data);
        }
    }
    if (symb1->tokenType == STRING_LIT && symb2->tokenType == IDENT) {
        out = transformString(&symb1->tokenName);
        if (out) return out;

        if (frames[1] == 't') {
            sprintf(str, "CONCAT GF@%s string@%s TF@%s\n", var->tokenName.data, symb1->tokenName.data,
                    symb2->tokenName.data);
        }
        if (frames[1] == 'g') {
            sprintf(str, "CONCAT GF@%s string@%s GF@%s\n", var->tokenName.data, symb1->tokenName.data,
                    symb2->tokenName.data);
        }
    }

    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;
    return OK;
}

///FUNCTION FUNCTIONS

errorCode generateFunctionStart(data *function) {
    char str[100];

    if (function == NULL) {
        sprintf(str, "LABEL main\nCREATEFRAME\n");
        if (addConstChar(gen.program, str)) return INTERNAL_ERROR;
        return OK;

    } else {
        sprintf(str, "LABEL %s\n", function->id.data);
        if (addConstChar(gen.program, str)) return INTERNAL_ERROR;
        sprintf(str, "CREATEFRAME\n");
        if (addConstChar(gen.program, str)) return INTERNAL_ERROR;
        for (size_t i = 0; i < function->parameters->size; i++) {
            token tmp;
            getToken(function->parameters, i, &tmp);
            if (tmp.tokenType == IDENT) {
                sprintf(str, "DEFVAR TF@%s\n", tmp.tokenName.data);
                if (addConstChar(gen.program, str)) return INTERNAL_ERROR;
                sprintf(str, "POPS TF@%s\n", tmp.tokenName.data);
                if (addConstChar(gen.program, str)) return INTERNAL_ERROR;
            }
        }
        size_t i = 0;
        dataType tmpType;
        tmpType = function->types[i];

        while (tmpType != TYPE_UNDEFINED) {
            sprintf(str, "DEFVAR TF@ret%zu\n", i);
            if (addConstChar(gen.program, str)) return INTERNAL_ERROR;
            tmpType = function->types[++i];
        }
    }

    return OK;
}

errorCode generateFunctionCall(data *function, list *argValues) {
    char str[100];

    for (size_t i = 0; i < argValues->size; i++) {
        token *tmp = copyToken(argValues, i);
        if (tmp != NULL) {
            switch (tmp->tokenType) {
                case INT_LIT:
                    sprintf(str, "PUSHS int@%s\n", tmp->tokenName.data);
                    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;
                    break;
                case STRING_LIT:
                    if (transformString(&tmp->tokenName)) return INTERNAL_ERROR;
                    sprintf(str, "PUSHS string@%s\n", tmp->tokenName.data);
                    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;
                    break;
                case FLOAT_LIT:
                    transformFloat(&tmp->tokenName);
                    sprintf(str, "PUSHS float@%s\n", tmp->tokenName.data);
                    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;
                    break;
                case IDENT:
                    sprintf(str, "PUSHS TF@%s\n", tmp->tokenName.data);
                    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;
                    break;
                default:
                    break;
            }
        }
    }
    sprintf(str, "PUSHFRAME\n");
    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;

    sprintf(str, "CALL %s\n", function->id.data);
    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;

    return OK;
}

errorCode generateFunctionReturn(list *assignVariables) {
    char str[100];

    for (size_t i = 0; i < assignVariables->size; i++) {
        token *tmp = copyToken(assignVariables, i);
        if (tmp != NULL) {
            sprintf(str, "MOVE LF@%s TF@ret%zu\n", tmp->tokenName.data, i);
            if (addConstChar(gen.program, str)) return INTERNAL_ERROR;
        }
    }
    sprintf(str, "POPFRAME\n");
    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;

    return OK;
}

errorCode generateFunctionEnd() {

    if (addConstChar(gen.program, "RETURN\n\n")) return INTERNAL_ERROR;
    return OK;
}

errorCode generatePrint(token *tok) {
    char str[100];
    errorCode out;

    switch (tok->tokenType) {
        case INT_LIT:
            sprintf(str, "WRITE int@%s\n", tok->tokenName.data);
            break;
        case STRING_LIT:
            out = transformString(&tok->tokenName);
            if (out) return out;
            sprintf(str, "WRITE string@%s\n", tok->tokenName.data);
            break;
        case FLOAT_LIT:
            out = transformFloat(&tok->tokenName);
            if (out) return out;
            sprintf(str, "WRITE float@%s\n", tok->tokenName.data);
            break;
        case IDENT:
            sprintf(str, "WRITE TF@%s\n", tok->tokenName.data);
            break;
        default:
            break;
    }


    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;

    return OK;
}

///OUTPUT FUNCTIONS

errorCode generatorWrite() {
    printf("%s", gen.program->data);
    return OK;
}

///MAIN FUNCTIONS

errorCode generateMainScopeEnd() {
    if (addConstChar(gen.program, "EXIT int@0\n")) return INTERNAL_ERROR;
    return OK;
}

///IF FUNCTIONS

errorCode generateIfStart(list *condition, tableNodePtr varTable, size_t ifCount) {
    char str[100];
    errorCode out;

    out = applyPrecedence(condition, varTable);
    if (out) return out;

    sprintf(str, "JUMPIFNEQ else%zu GF@expVar0 bool@true\n", ifCount);
    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;

    return OK;
}

errorCode generateElse(size_t ifCount) {
    char str[100];
    sprintf(str, "JUMP ifEnd%zu\n", ifCount);
    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;
    sprintf(str, "LABEL else%zu\n", ifCount);
    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;
    return OK;
}

errorCode generateIfEnd(size_t ifCount) {
    char str[100];
    sprintf(str, "LABEL ifEnd%zu\n", ifCount);
    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;
    return OK;
}

///FOR FUNCTIONS

errorCode generateForPrequel(size_t forCount) {
    char str[100];
    sprintf(str, "JUMP forFirst%zu\n", forCount);
    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;
    sprintf(str, "LABEL forStart%zu\n", forCount);
    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;
    return OK;
}

errorCode generateForStart(list *condition, tableNodePtr varTable, size_t forCount) {
    char str[100];
    errorCode out;
    sprintf(str, "LABEL forFirst%zu\n", forCount);
    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;

    out = applyPrecedence(condition, varTable);
    if (out) return out;

    sprintf(str, "JUMPIFNEQ forEnd%zu GF@expVar0 bool@true\n", forCount);
    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;

    return OK;
}

errorCode generateForEnd(size_t forCount) {
    char str[100];
    sprintf(str, "JUMP forStart%zu\n", forCount);
    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;
    sprintf(str, "LABEL forEnd%zu\n", forCount);
    if (addConstChar(gen.program, str)) return INTERNAL_ERROR;
    return OK;
}

errorCode
decideOperation(token *var, token *first, token *second, token *operation, char *scopes, dataType expressionType) {

    errorCode out = OK;
    if (equalStrings(operation->tokenName.data, "+")) {
        if (expressionType != TYPE_STRING) {
            out = generateArithmetic(var, first, second, scopes, '+');
        } else {
            out = generateConcat(var, first, second, scopes);
        }
    } else if (equalStrings(operation->tokenName.data, "-")) {
        out = generateArithmetic(var, first, second, scopes, '-');
    } else if (equalStrings(operation->tokenName.data, "*")) {
        out = generateArithmetic(var, first, second, scopes, '*');
    } else if (equalStrings(operation->tokenName.data, "/")) {
        if (expressionType == TYPE_INT) {
            out = generateArithmetic(var, first, second, scopes, '/');
        } else {
            out = generateArithmetic(var, first, second, scopes, '%');
        }
    } else if (equalStrings(operation->tokenName.data, ">")) {
        out = generateGT(var, first, second, scopes);
    } else if (equalStrings(operation->tokenName.data, "<")) {
        out = generateLT(var, first, second, scopes);
    } else if (equalStrings(operation->tokenName.data, ">=")) {
        out = generateLT(var, first, second, scopes);
        if (out) return out;
        out = generateNOT(var);
    } else if (equalStrings(operation->tokenName.data, "<=")) {
        out = generateGT(var, first, second, scopes);
        if (out) return out;
        out = generateNOT(var);
    } else if (equalStrings(operation->tokenName.data, "==")) {
        out = generateEQ(var, first, second, scopes);
    } else if (equalStrings(operation->tokenName.data, "!=")) {
        out = generateEQ(var, first, second, scopes);
        if (out) return out;
        out = generateNOT(var);
    }
    return out;
}

void freeVarVals(token *varVals[10]) {
    for (size_t i = 0; i < 10; i++) {
        if (varVals[i] != NULL) {
            destroyString(&varVals[i]->tokenName);
            free(varVals[i]);
        }
    }
}

errorCode generateExpression(list *expression, tableNodePtr varTable) {

    errorCode out;
    bool usedTokens[expression->size];
    for (size_t i = 0; i < expression->size; i++) usedTokens[i] = false;
    dataType expressionType;
    token *varVals[10];
    for (size_t i = 0; i < 10; i++) varVals[i] = NULL;
    size_t usedVars = 0;
    data *node = NULL;

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
            token *tok = copyToken(expression, i);
            token *first = NULL;
            token *second = NULL;
            bool wasSecond = false;
            if (tok->tokenType == ARIT_OPERATOR || tok->tokenType == COMP_OPERATOR) {
                usedTokens[i] = true;
                wasOperator = true;
                for (int j = (int) i - 1; j >= 0; j--) {
                    if (!wasSecond) {
                        if (!usedTokens[j]) {
                            second = copyToken(expression, j);
                            if (second->tokenType != ARIT_OPERATOR && second->tokenType != COMP_OPERATOR) {
                                usedTokens[j] = true;
                                wasSecond = true;
                            }
                            if (j != (int) i - 1 && usedTokens[j + 1]) break;
                        }
                    } else {
                        if (usedTokens[j]) break;
                        else {
                            first = copyToken(expression, j);
                            if (first->tokenType != ARIT_OPERATOR && first->tokenType != COMP_OPERATOR) {
                                usedTokens[j] = true;

                                if ((varVals[usedVars] = malloc(sizeof(token))) == NULL) {
                                    freeVarVals(varVals);
                                    return INTERNAL_ERROR;
                                }
                                varVals[usedVars]->tokenType = IDENT;
                                initString(&varVals[usedVars]->tokenName);
                                char *newName = NULL;
                                if ((newName = malloc(sizeof(char) * 8)) == NULL) {
                                    freeVarVals(varVals);
                                    return INTERNAL_ERROR;
                                }
                                sprintf(newName, "expVar%zu", usedVars);
                                if (makeString(newName, &varVals[usedVars]->tokenName)) {
                                    freeVarVals(varVals);
                                    return INTERNAL_ERROR;
                                }
                                free(newName);

                                out = decideOperation(varVals[usedVars], first, second, tok, "tt", expressionType);
                                if (out) {
                                    freeVarVals(varVals);
                                    return out;
                                }
                                usedVars++;
                                break;
                            }
                        }
                    }
                }
                if (second == NULL) {
                    usedVars--;
                    out = decideOperation(varVals[usedVars - 1], varVals[usedVars - 1], varVals[usedVars], tok, "gg",
                                        expressionType);
                    if (out) {
                        freeVarVals(varVals);
                        return out;
                    }
                    destroyString(&varVals[usedVars]->tokenName);
                    free(varVals[usedVars]);
                    varVals[usedVars] = NULL;
                } else if (first == NULL) {
                    first = copyToken(expression, i - 1);
                    if (second == first) {
                        out = decideOperation(varVals[usedVars - 1], varVals[usedVars - 1], second, tok, "gt",
                                            expressionType);
                        if (out) {
                            freeVarVals(varVals);
                            return out;
                        }
                    } else {
                        out = decideOperation(varVals[usedVars - 1], second, varVals[usedVars - 1], tok, "tg",
                                            expressionType);
                        if (out) {
                            freeVarVals(varVals);
                            return out;
                        }
                    }
                }
            }
        }

        freeVarVals(varVals);

        if (!wasOperator) {
            char str[100];
            switch (expression->first->tokenType) {
                case INT_LIT:
                    sprintf(str, "MOVE GF@expVar0 int@%s\n", expression->first->tokenName.data);
                    break;
                case STRING_LIT:
                    out = transformString(&expression->first->tokenName);
                    if (out) return out;
                    sprintf(str, "MOVE GF@expVar0 string@%s\n", expression->first->tokenName.data);
                    break;
                case FLOAT_LIT:
                    out = transformFloat(&expression->first->tokenName);
                    if (out) return out;
                    sprintf(str, "MOVE GF@expVar0 float@%s\n", expression->first->tokenName.data);
                    break;
                case IDENT:
                    sprintf(str, "MOVE GF@expVar0 TF@%s\n", expression->first->tokenName.data);
                    break;
                default:
                    break;
            }

            if (addConstChar(gen.program, str)) return INTERNAL_ERROR;

        }

    }

    return OK;
}