//
// Created by xolsia00 on 3. 11. 2020.
//

#include <stdio.h>
#include <ctype.h>
#include "List.h"
#include "Scanner.h"
#include <stdbool.h>

void removeChar(char *s, int c) {

    int j, n = strlen(s);
    for (int i = j = 0; i < n; i++)
        if (s[i] != c)
            s[j++] = s[i];

    s[j] = '\0';
}

int isIntLit(string *lexem) {
    if (lexem->len < 1) {
        return 1;
    }
    for (int i = 0; i < lexem->len; i++) {
        if (isdigit(lexem->data[i]) == 0)
            return 1;
    }

    return 0;
}

int isFloatLit(string *lexem) {
    if (lexem->len < 1) {
        return 1;
    }

    int dotCount = 0;
    for (int i = 0; i < lexem->len; i++) {
        if (isdigit(lexem->data[i]) == 0 && lexem->data[i] != '.') return 1;

        if (lexem->data[i] == '.') {
            dotCount++;
        }
        if (dotCount > 1)
            return 1;
    }
    return 0;
}

int isIdent(string *lexem) {
    for (int i = 0; i < lexem->len - 1; i++) {
        if (isalnum(lexem->data[i]) == 0)
            return 0;
    }
    return 1;
}


//this function will save lexems to list and sets their token values
errorCode LexemAutomat(list *sortedList, string *lexem) {

    if (strcmp(lexem->data, "int") == 0) {
        addToken(sortedList, INT, lexem->data);

        return 0;
    }
    if (strcmp(lexem->data, "float64") == 0) {
        addToken(sortedList, FLOAT, lexem->data);

        return 0;
    }
    if (strchr(lexem->data, '\n') != NULL) {
        addToken(sortedList, EOL, lexem->data);

        return 0;
    }
    if (strcmp(lexem->data, "string") == 0) {
        addToken(sortedList, STRING, lexem->data);

        return 0;
    }
    if (strcmp(lexem->data, "if") == 0) {
        addToken(sortedList, IF, lexem->data);

        return 0;
    }
    if (strcmp(lexem->data, "else") == 0) {
        addToken(sortedList, ELSE, lexem->data);

        return 0;
    }
    if (strcmp(lexem->data, "for") == 0) {
        addToken(sortedList, FOR, lexem->data);

        return 0;
    }
    if (strcmp(lexem->data, "package") == 0) {
        addToken(sortedList, PACKAGE, lexem->data);

        return 0;
    }
    if (strcmp(lexem->data, "return") == 0) {
        addToken(sortedList, RETURN, lexem->data);

        return 0;
    }
    if (strcmp(lexem->data, "func") == 0) {
        addToken(sortedList, FUNC, lexem->data);

        return 0;
    }
    if (strcmp(lexem->data, "{") == 0 || strcmp(lexem->data, "}") == 0) {
        addToken(sortedList, BRACKET_CURLY, lexem->data);

        return 0;
    }
    if (strcmp(lexem->data, "(") == 0 || strcmp(lexem->data, ")") == 0) {
        addToken(sortedList, BRACKET_ROUND, lexem->data);

        return 0;
    }
    if (lexem->data[0] == '"' && lexem->data[lexem->len - 1] == '"') {
        removeChar(lexem->data, '"');
        addToken(sortedList, STRING_LIT, lexem->data);

        return 0;
    }
    if (isIntLit(lexem) == 0) {
        addToken(sortedList, INT_LIT, lexem->data);

        return 0;
    }
    if (isFloatLit(lexem) == 0) {
        addToken(sortedList, FLOAT_LIT, lexem->data);

        return 0;
    }
    if (strcmp(lexem->data, "=") == 0 || strcmp(lexem->data, ":=") == 0) {
        addToken(sortedList, ASIGN_OPERATOR, lexem->data);

        return 0;
    }
    if (strcmp(lexem->data, "==") == 0 || strcmp(lexem->data, "<=") == 0 || strcmp(lexem->data, ">=") == 0 ||
        strcmp(lexem->data, "!=") == 0 || strcmp(lexem->data, ">") == 0 || strcmp(lexem->data, "<") == 0) {
        addToken(sortedList, COMP_OPERAtOR, lexem->data);

        return 0;
    }
    if (strcmp(lexem->data, "+") == 0 || strcmp(lexem->data, "-") == 0 || strcmp(lexem->data, "*") == 0 ||
        strcmp(lexem->data, "/") == 0) {
        addToken(sortedList, ARIT_OPERATOR, lexem->data);

        return 0;
    }
    if (strcmp(lexem->data, ",") == 0) {
        addToken(sortedList, COMMA, lexem->data);

        return 0;
    }
    if (strcmp(lexem->data, ";") == 0) {
        addToken(sortedList, SEMICOL, lexem->data);

        return 0;
    }
    if (isIdent(lexem) == 1) {
        addToken(sortedList, IDENT, lexem->data);

        return 0;
    }

    return LEXICAL_ERROR;


}


errorCode CodeAnalyzer(list *sortedList, string code) {
    char currentChar;

    string currentLexem;
    initString(&currentLexem);
    if (makeString("", &currentLexem) == INTERNAL_ERROR) {
        destroyString(&currentLexem);
        return INTERNAL_ERROR;
    }

    const char operators[] = "<>{}()*/-+.'=:";

    bool comment = false;
    bool stringLoaded = false;
    bool lineComment = false;


    //read code char by char
    for (int i = 0; i < code.len; i++) {

        currentChar = code.data[i];


        if (comment == true) {

            if (lineComment) {
                if (currentChar == '\n') {
                    lineComment = false;
                    comment = false;
                }
            } else {

                if (currentChar == '*') {
                    if (code.len >= i + 1) {
                        if (code.data[i + 1] == '/') {
                            comment = false;
                            i++;
                        }
                    }
                }
            }
            continue;
        }

        if (stringLoaded) {
            if (currentChar == '"') {
                addChar(&currentLexem, currentChar);
                stringLoaded = false;
                if (LexemAutomat(sortedList, &currentLexem) == LEXICAL_ERROR) {
                    destroyString(&currentLexem);
                    return LEXICAL_ERROR;
                }

                if (makeString("", &currentLexem) == INTERNAL_ERROR) {
                    destroyString(&currentLexem);
                    return INTERNAL_ERROR;
                }

                continue;
            }
            addChar(&currentLexem, currentChar);
        } else if (isalnum(currentChar) || currentChar == '_' || currentChar == '.') {
            if (currentChar != '\n')
                addChar(&currentLexem, currentChar);

        } else if (currentChar == '"') {
            if (currentLexem.len > 0) {

                if (LexemAutomat(sortedList, &currentLexem) == LEXICAL_ERROR) {
                    destroyString(&currentLexem);
                    return LEXICAL_ERROR;
                }

                if (makeString("", &currentLexem) == INTERNAL_ERROR) {
                    destroyString(&currentLexem);
                    return INTERNAL_ERROR;
                }
            }
            stringLoaded = true;
            addChar(&currentLexem, currentChar);
            continue;

        }
            //this will check for comments, spaces and special symbols and will end the word
        else {
            if (currentChar == '/') {
                if (code.len >= i + 1) {
                    if (code.data[i + 1] == '*') {
                        comment = true;
                        continue;
                    } else if (code.data[i + 1] == '/') {
                        comment = true;
                        lineComment = true;
                        continue;
                    }
                }
            }
            if (currentChar == ' ' && currentLexem.data != NULL) {

                if (currentLexem.len >= 1) {

                    if (LexemAutomat(sortedList, &currentLexem) == LEXICAL_ERROR) {
                        destroyString(&currentLexem);
                        return LEXICAL_ERROR;
                    }

                    if (makeString("", &currentLexem) == INTERNAL_ERROR) {
                        destroyString(&currentLexem);
                        return INTERNAL_ERROR;
                    }
                }
            }
            if (currentChar == '\n') {
                if (currentLexem.len >= 1) {

                    if (LexemAutomat(sortedList, &currentLexem) == LEXICAL_ERROR) {
                        destroyString(&currentLexem);
                        return LEXICAL_ERROR;
                    }

                    if (makeString("", &currentLexem) == INTERNAL_ERROR) {
                        destroyString(&currentLexem);
                        return INTERNAL_ERROR;
                    }
                }

                addChar(&currentLexem, currentChar);

                if (LexemAutomat(sortedList, &currentLexem) == LEXICAL_ERROR) {
                    destroyString(&currentLexem);
                    return LEXICAL_ERROR;
                }

                if (makeString("", &currentLexem) == INTERNAL_ERROR) {
                    destroyString(&currentLexem);
                    return INTERNAL_ERROR;
                }

            }
            if (currentChar == ';' || currentChar == ',') {
                if (currentLexem.len >= 1) {

                    if (LexemAutomat(sortedList, &currentLexem) == LEXICAL_ERROR) {
                        destroyString(&currentLexem);
                        return LEXICAL_ERROR;
                    }

                    if (makeString("", &currentLexem) == INTERNAL_ERROR) {
                        destroyString(&currentLexem);
                        return INTERNAL_ERROR;
                    }
                }
                addChar(&currentLexem, currentChar);

                if (LexemAutomat(sortedList, &currentLexem) == LEXICAL_ERROR) {
                    destroyString(&currentLexem);
                    return LEXICAL_ERROR;
                }

                if (makeString("", &currentLexem) == INTERNAL_ERROR) {
                    destroyString(&currentLexem);
                    return INTERNAL_ERROR;
                }

            }


            if (strchr(operators, currentChar) != NULL || currentChar == EOL) {

                if (currentLexem.len >= 1) {
                    if (LexemAutomat(sortedList, &currentLexem) == LEXICAL_ERROR) {
                        destroyString(&currentLexem);
                        return LEXICAL_ERROR;
                    }

                    if (makeString("", &currentLexem) == INTERNAL_ERROR) {
                        destroyString(&currentLexem);
                        return INTERNAL_ERROR;
                    }
                }

                addChar(&currentLexem, currentChar);

                if (currentChar == '=' && i != code.len) {
                    if (code.data[i + 1] == '<' || code.data[i + 1] == '>' || code.data[i + 1] == '=') {

                        addChar(&currentLexem, code.data[i + 1]);
                        i++;

                    }
                }
                if (currentChar == '<' || currentChar == '>') {
                    if (i != code.len) {
                        if (code.data[i + 1] == '=') {

                            addChar(&currentLexem, code.data[i + 1]);
                            i++;
                        }
                    }
                }
                if (currentChar == ':') {
                    if (i != code.len) {
                        if (code.data[i + 1] == '=') {

                            addChar(&currentLexem, code.data[i + 1]);
                            i++;
                        }
                    }
                }
                if (currentLexem.len >= 1) {
                    if (LexemAutomat(sortedList, &currentLexem) == LEXICAL_ERROR) {
                        destroyString(&currentLexem);
                        return LEXICAL_ERROR;
                    }

                    if (makeString("", &currentLexem) == INTERNAL_ERROR) {
                        destroyString(&currentLexem);
                        return INTERNAL_ERROR;
                    }
                }
            }
        }

    }

    if (currentLexem.len >= 1) {
        if (LexemAutomat(sortedList, &currentLexem) == LEXICAL_ERROR) {
            destroyString(&currentLexem);
            return LEXICAL_ERROR;
        }

        if (makeString("", &currentLexem) == INTERNAL_ERROR) {
            destroyString(&currentLexem);
            return INTERNAL_ERROR;
        }
    }


    destroyString(&currentLexem);
    return 0;
}


//this is "main" function that handles the entire scanner

errorCode ScannerHandler() {

    string tempCode; //THIS SHOULD BE CHANGED LATER
    initString(&tempCode);
    makeString(
            "if(Jakub location == doma){ \n } \n else{ \n Jakub smrdi while corona true} sranda() 1 * 1 pecka = 1 /* adasdadas*/ af //sdaasddsaad",
            &tempCode);

    list sortedList;
    initList(&sortedList);


    //tempCode is input static representation
    errorCode returnErrorCode;
    returnErrorCode = CodeAnalyzer(&sortedList, tempCode); //CHANGE TEMPCODE LATER

    /*
    token tempToken;
    printf("\nTOKEN TYPES: \n");
    for (int i = 0; i < sortedList.size; i++) {
        getToken(&sortedList, i, &tempToken);
        printf("%s ", tempToken.tokenName);
        printf("%d | ", tempToken.tokenType);
    }
*/
    destroyString(&tempCode);
    deleteList(&sortedList);

    return returnErrorCode;


}
/*
int main(int argc, char *argv[]) {
    return ScannerHandler();
}*/