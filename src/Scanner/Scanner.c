//
// Created by xolsia00 on 3. 11. 2020.
//

#include <stdio.h>
#include <ctype.h>
#include "List.h"
#include "../String/DynamicString.h"
#include <stdbool.h>

int isIntLit(string *lexem) {
    for (int i = 0; i < lexem->len; i++) {
        if (isdigit(lexem->data[i]) == 0)
            return 0;
    }
    return 1;
}

int isIdent(string *lexem) {
    for (int i = 0; i < lexem->len-1; i++) {
        if (isalnum(lexem->data[i]) == 0)
            return 0;
    }
    return 1;
}


//this function will save lexems to list and sets their token values
void LexemAutomat(list *sortedList, string *lexem) {
   printf("%s , ", lexem->data);
    if (strcmp(lexem->data, "int") == 0) {
        addToken(sortedList, INT, lexem->data);

        return;
    }
    if (strcmp(lexem->data, "float64") == 0) {
        addToken(sortedList, FLOAT, lexem->data);

        return;
    }
    if (strcmp(lexem->data, "string") == 0) {
        addToken(sortedList, STRING, lexem->data);

        return;
    }
    if (strcmp(lexem->data, "if") == 0) {
        addToken(sortedList, IF, lexem->data);

        return;
    }
    if (strcmp(lexem->data, "else") == 0) {
        addToken(sortedList, ELSE, lexem->data);

        return;
    }
    if (strcmp(lexem->data, "for") == 0) {
        addToken(sortedList, FOR, lexem->data);

        return;
    }
    if (strcmp(lexem->data, "package") == 0) {
        addToken(sortedList, PACKAGE, lexem->data);

        return;
    }
    if (strcmp(lexem->data, "return") == 0) {
        addToken(sortedList, RETURN, lexem->data);

        return;
    }
    if (strcmp(lexem->data, "func") == 0) {
        addToken(sortedList, FUNC, lexem->data);

        return;
    }
    if (strcmp(lexem->data, "{") == 0 || strcmp(lexem->data, "}") == 0) {
        addToken(sortedList, BRACKET_CURLY, lexem->data);

        return;
    }
    if (strcmp(lexem->data, "(") == 0 || strcmp(lexem->data, ")") == 0) {
        addToken(sortedList, BRACKET_ROUND, lexem->data);

        return;
    }
    if (lexem->data[0] == '\0' && lexem->len == 1) {
        addToken(sortedList, EOL, lexem->data);

        return;
    }
    if (lexem->data[0] == '"' && lexem->data[lexem->len] == '"') {
        addToken(sortedList, STRING_LIT, lexem->data);

        return;
    }
    if (isIntLit(lexem) == 1) {
        addToken(sortedList, INT_LIT, lexem->data);

        return;
    }
    if (strcmp(lexem->data, "=") == 0) {
        addToken(sortedList, ASIGN_OPERATOR, lexem->data);

        return;
    }
    if(strcmp(lexem->data, "==") == 0 || strcmp(lexem->data, "<=") == 0 || strcmp(lexem->data, ">=") == 0 ){
        addToken(sortedList, COMP_OPERAtOR, lexem->data);

        return;
    }
    if(strcmp(lexem->data, "+") == 0 || strcmp(lexem->data, "-") == 0 || strcmp(lexem->data, "*") == 0 ||strcmp(lexem->data, "/") == 0 ){
        addToken(sortedList, ARIT_OPERATOR, lexem->data);

        return;
    }
    if (isIdent(lexem) == 1) {
        addToken(sortedList, VAR_ID, lexem->data);

        return;
    }

    //CALL ERROR


}


void CodeAnalyzer(list *sortedList, string code) {
    printf("Code Analyzer\n");
    char currentChar;

    string currentLexem;
    initString(&currentLexem);
    makeString("", &currentLexem);

    const char operators[] = "<>{}()*/-+.'=";

    bool comment = false;
    bool lineComment = false;


    //read code char by char
    for (int i = 0; i < code.len; i++) {

        currentChar = code.data[i];

        if(comment == true){
            printf("COMMENT");
        }
        //comment skip
        if (comment == true) {
            //printf("Comment\n");
            if (lineComment) {
                if (currentChar == '\n') {
                    lineComment = false;
                    comment = false;
                }
                if (currentChar == '*') {
                    if (code.len >= ++i) {
                        if (code.data[++i] == '/') {
                            comment = false;
                        }
                    }
                }
            }
            continue;
        }
            //check for identificator, literals
        else if (isalnum(currentChar) || currentChar == '_') {
            //printf("pismeno/cislo\n");
            addChar(&currentLexem, currentChar);

        }
            //this will check for comments, spaces and special symbols and will end the word
        else {
            if (currentChar == '/') {
                printf("OUTER");
                if (code.len >= ++i) {
                    printf("INNER");
                    if (code.data[++i] == '*') {
                        printf("COMENT * ");
                        comment = true;
                        continue;
                    } else if (code.data[++i] == '/') {
                        printf("LINECOMMENT ");
                        comment = true;
                        lineComment = true;
                        continue;
                    }
                }
            }
            if (currentChar == ' ' && currentLexem.data != NULL) {
                // printf("space\n");
                if (currentLexem.len >= 1) {

                    LexemAutomat(sortedList, &currentLexem);

                    makeString("", &currentLexem);
                }
            }
            if (strchr(operators, currentChar) != NULL || currentChar == EOL) {
                //printf("else\n");
                if (currentLexem.len >= 1) {
                    LexemAutomat(sortedList,&currentLexem);

                    makeString("", &currentLexem);
                }

                addChar(&currentLexem, currentChar);

                if (currentChar == '=' && i != code.len) {
                    if (code.data[i + 1] == '<' || code.data[i + 1] == '>' || code.data[i + 1] == '=') {

                        addChar(&currentLexem, code.data[i + 1]);
                        i++;

                    }
                }
                if (currentChar == '<' || currentChar == '>') {
                    if (i != code.len){
                        if (code.data[++i] == '=') {

                            addChar(&currentLexem, code.data[i+1]);
                            i++;
                        }
                    }
                }
                LexemAutomat(sortedList, &currentLexem);

                makeString("", &currentLexem);
            }
        }

    }
    LexemAutomat(sortedList, &currentLexem);

    makeString("", &currentLexem);

    destroyString(&currentLexem);
}

// YOU HAVE TO APPEND . AND TWO NUMBERS NEXT TO IT
// ALSO IF + / - IS BEFORE A NUMBER AND THERE IS NO NUMBER BEFORE IT APPEND THAT TOO
// ALSO APPEND LITERALS BETWEEN "" or ''




//this is "main" function that handles the entire scanner

void ScannerHandler() {

    string tempCode; //THIS SHOULD BE CHANGED LATER
    initString(&tempCode);
    makeString("if(Jakub location == doma){ } else{ Jakub smrdi while corona true} sranda() 1 * 1 pecka = 1", &tempCode);

    list sortedList;
    initList(&sortedList);


    //tempCode is input static representation
    CodeAnalyzer(&sortedList, tempCode); //CHANGE TEMPCODE LATER

    token tempToken;
    printf("\nTOKEN TYPES: \n");
    for (int i = 0; i < sortedList.size; i++) {
        getToken(&sortedList, i, &tempToken);
        printf("%s ", tempToken.tokenName);
        printf("%d | ", tempToken.tokenType);
    }

    destroyString(&tempCode);
    deleteList(&sortedList);
}

int main(int argc, char *argv[]) {
    ScannerHandler();
}