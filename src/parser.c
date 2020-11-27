//
// Created by xdanco00 on 12. 11. 2020.
//

#include "parser.h"

bool equalStrings(char* s1, char* s2) {
    return (strcmp(s1, s2) == 0);
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
            if (curToken.tokenName.data == "{") {
                openBracketCount++;
            } else {
                closedBracketCount++;
            }
        }
    }

    return OK;

}

//expression
errorCode blockExpression(list *tokenList, token *curToken, bool forState) {

    int openBracketCount = 0;
    int closedBracketCount = 0;

    while (curToken->tokenType != EOL) {

        if(forState == true && curToken->tokenType == SEMICOL) break;


        if (curToken->tokenType == BRACKET_ROUND && curToken->tokenName.data == "(") {
            openBracketCount++;
            if (curToken->nextToken->tokenType != IDENT) return SYNTAX_ERROR;
            curToken = curToken->nextToken;
            continue;
        }

        if (curToken->tokenType == BRACKET_ROUND && curToken->tokenName.data == ")") {
            closedBracketCount++;
            if (curToken->nextToken->tokenType != ARIT_OPERATOR && curToken->nextToken->tokenType != COMP_OPERAtOR && curToken->nextToken->tokenType != EOL) return SYNTAX_ERROR;
            curToken = curToken->nextToken;
            continue;
        }

        if (curToken->tokenType == IDENT || curToken->tokenType == INT_LIT || curToken->tokenType == FLOAT_LIT || curToken->tokenType == STRING_LIT) {
            if (curToken->nextToken->tokenType != ARIT_OPERATOR && curToken->nextToken->tokenType != COMP_OPERAtOR && curToken->nextToken->tokenType != EOL ) return SYNTAX_ERROR;
            curToken = curToken->nextToken;
            continue;
        }

        if(curToken->tokenType == ARIT_OPERATOR || curToken->tokenType == COMP_OPERAtOR){
            if(curToken->nextToken->tokenType != IDENT && curToken->nextToken->tokenType != BRACKET_ROUND) return SYNTAX_ERROR;
            curToken = curToken->nextToken;
            continue;
        }

        return SYNTAX_ERROR;

    }

    if(openBracketCount != closedBracketCount) return SYNTAX_ERROR;

    return OK;
}

//asign
errorCode blockAsign(list *tokenList, token *curToken, bool forState) {

    int openBracketCount = 0;
    int closedBracketCount = 0;

    if (curToken->tokenType != IDENT && curToken->tokenType != INT_LIT && curToken->tokenType != FLOAT_LIT && curToken->tokenType != STRING_LIT) return SYNTAX_ERROR;

    while(curToken->tokenType != EOL){

        if(forState == true && curToken->tokenType == SEMICOL) break;

        if (curToken->tokenType == IDENT || curToken->tokenType == INT_LIT || curToken->tokenType == FLOAT_LIT || curToken->tokenType == STRING_LIT){
            if(curToken->nextToken->tokenType == ASIGN_OPERATOR && curToken->nextToken->tokenName.data == "=") return SYNTAX_ERROR;

            if(curToken->nextToken->tokenType != ARIT_OPERATOR &&  curToken->nextToken->tokenType != BRACKET_ROUND && curToken->nextToken->tokenType != ASIGN_OPERATOR) return SYNTAX_ERROR;

            if(curToken->nextToken->tokenType == BRACKET_ROUND && curToken->nextToken->tokenName.data == "(") return SYNTAX_ERROR;

            curToken = curToken->nextToken;
            continue;
        }

        if(curToken->tokenType == ARIT_OPERATOR || curToken->tokenType == BRACKET_ROUND || curToken->tokenType == ASIGN_OPERATOR){

            if(curToken->tokenType == ASIGN_OPERATOR && curToken->tokenName.data == "=") return SYNTAX_ERROR;

            if(curToken->nextToken->tokenType != IDENT && curToken->nextToken->tokenType != INT_LIT && curToken->nextToken->tokenType != FLOAT_LIT &&curToken->nextToken->tokenType != STRING_LIT )
                return SYNTAX_ERROR;

            if(curToken->tokenType == BRACKET_ROUND && curToken->tokenName.data == ")") return SYNTAX_ERROR;

            curToken = curToken->nextToken;
            continue;
        }

    }

}

//term
errorCode blockD() {

}

//definition
errorCode blockDefinition(list *tokenList, token *curToken, bool forState) {

    if(curToken->tokenType != IDENT) return SYNTAX_ERROR;
    curToken = curToken->nextToken;

    if(curToken->tokenType != ASIGN_OPERATOR && curToken != ":=") return SYNTAX_ERROR;
    curToken = curToken->nextToken;

    blockExpression(tokenList, &curToken, forState);

}


errorCode parse(list *tokenList, string *code) {

    tableNodePtr globalTable;
    errorCode returnError = fillSymtable(globalTable, tokenList);
    bool isFunc = false;

    if (returnError != OK) {
        return returnError;
    }

    returnError = blockA(tokenList);
    if (returnError != OK) return returnError;

    generatorInit();
    generatorStart();

    token savedToken;

    for (int i = 2; i < tokenList->size; i++) {

        token curToken;
        getToken(tokenList, i, &curToken);

        if (curToken.tokenType == EOL)
            if(curToken.nextToken->tokenType != EOL) savedToken = *curToken.nextToken;


        if (curToken.tokenType == FUNC) isFunc = true;

        if (curToken.tokenName.data == "{" && isFunc == true) {
            returnError = blockB(tokenList, curToken);
            if (returnError != OK) return returnError;
        }

        if (curToken.tokenType == IF) {
            returnError = blockExpression(tokenList, &curToken.nextToken, false);
            if (returnError != OK) return returnError;
        }

        if (curToken.tokenType == FOR) {
            returnError = blockDefinition(tokenList, &curToken.nextToken, true);
            if (returnError != OK) return returnError;


            returnError = blockExpression(tokenList, &curToken.nextToken, true);
            if (returnError != OK) return returnError;

            returnError = blockAsign(tokenList, &curToken.nextToken, true);
            if (returnError != OK) return returnError;
        }

    }


}