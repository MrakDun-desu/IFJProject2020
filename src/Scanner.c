/******************************** Scanner.c **********************************/
/*  Predmet: IFJ a IAL						                                  */
/*  Lexikalny analyzator                                                      */
/*  Vytvoril: Martin Olsiak xolsia00                                          */
/* ************************************************************************** */

#include <stdio.h>
#include <ctype.h>
#include "List.h"
#include "Scanner.h"
#include <stdbool.h>

void removeChar(char *s, int c) {

    if (s != NULL) {
        size_t j, n = strlen(s);
        for (size_t i = j = 0; i < n; i++) {
            if (i != 0) {
                if (s[i] != c || s[i - 1] == '\\')
                    s[j++] = s[i];
            }
        }

        s[j] = '\0';
    }
}

int isIntLit(string *lexem) {
    if (lexem->len < 1) {
        return 1;
    }
    for (size_t i = 0; i < lexem->len; i++) {
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
    for (size_t i = 0; i < lexem->len; i++) {
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
    if (isdigit(lexem->data[0]) != 0)
        return 0;

    for (size_t i = 0; i < lexem->len - 1; i++) {
        if (isalnum(lexem->data[i]) == 0 && lexem->data[i] != '_')
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
        addToken(sortedList, COMP_OPERATOR, lexem->data);

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

    const char operators[] = "<>{}()*/-+.'=:!";

    bool comment = false;
    bool stringLoaded = false;
    bool lineComment = false;


    //read code char by char
    for (size_t i = 0; i < code.len; i++) {

        currentChar = code.data[i];

        //handle comments
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
        //handle \" in strings
        if (currentChar == '\\')
            if (code.len > i + 1)
                if (code.data[i + 1] == '\"') {
                    addChar(&currentLexem, '\\');
                    addChar(&currentLexem, '\"');
                    i++;
                    continue;
                }
        //handle strings
        if (stringLoaded) {
            if (currentChar == '"') {
                if (addChar(&currentLexem, currentChar)) {
                    destroyString(&currentLexem);
                    return INTERNAL_ERROR;
                }
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
            if (addChar(&currentLexem, currentChar)) {
                destroyString(&currentLexem);
                return INTERNAL_ERROR;
            }
            //handle identifiers
        } else if (isalnum(currentChar) || currentChar == '_' || currentChar == '.') {
            if (currentChar != '\n')
                if (addChar(&currentLexem, currentChar)) {
                    destroyString(&currentLexem);
                    return INTERNAL_ERROR;
                }

        }
        //begins strings
        else if (currentChar == '"') {
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
            if (addChar(&currentLexem, currentChar)) {
                destroyString(&currentLexem);
                return INTERNAL_ERROR;
            }
            continue;

        }
            //this will check for comments, spaces and special symbols and will end the word
        else {
            //check if is a comment and what sort of comment it is
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
            //check for spaces and end the current lexem
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
            //check for endline and ends current lexem
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

                if (addChar(&currentLexem, currentChar)) {
                    destroyString(&currentLexem);
                    return INTERNAL_ERROR;
                }

                if (LexemAutomat(sortedList, &currentLexem) == LEXICAL_ERROR) {
                    destroyString(&currentLexem);
                    return LEXICAL_ERROR;
                }

                if (makeString("", &currentLexem) == INTERNAL_ERROR) {
                    destroyString(&currentLexem);
                    return INTERNAL_ERROR;
                }

            }
            //checks for semicollons and commas and ends current lexem and creates a new one with semicollon/comma
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
                if (addChar(&currentLexem, currentChar)) {
                    destroyString(&currentLexem);
                    return INTERNAL_ERROR;
                }

                if (LexemAutomat(sortedList, &currentLexem) == LEXICAL_ERROR) {
                    destroyString(&currentLexem);
                    return LEXICAL_ERROR;
                }

                if (makeString("", &currentLexem) == INTERNAL_ERROR) {
                    destroyString(&currentLexem);
                    return INTERNAL_ERROR;
                }

            }

            //check for operators and save them in separate lexem
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

                if (addChar(&currentLexem, currentChar)) {
                    destroyString(&currentLexem);
                    return INTERNAL_ERROR;
                }

                if (currentChar == '=' && i != code.len) {
                    if (code.data[i + 1] == '<' || code.data[i + 1] == '>' || code.data[i + 1] == '=') {

                        if (addChar(&currentLexem, code.data[i + 1])) {
                            destroyString(&currentLexem);
                            return INTERNAL_ERROR;
                        }
                        i++;

                    }
                }
                if (currentChar == '<' || currentChar == '>' || currentChar == '!') {
                    if (i != code.len) {
                        if (code.data[i + 1] == '=') {

                            if (addChar(&currentLexem, code.data[i + 1])) {
                                destroyString(&currentLexem);
                                return INTERNAL_ERROR;
                            }
                            i++;
                        }
                    }
                }
                if (currentChar == ':') {
                    if (i != code.len) {
                        if (code.data[i + 1] == '=') {

                            if (addChar(&currentLexem, code.data[i + 1])) {
                                destroyString(&currentLexem);
                                return INTERNAL_ERROR;
                            }
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

    //pass the current lexem to lexem automaton
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
