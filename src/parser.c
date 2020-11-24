//
// Created by xdanco00 on 12. 11. 2020.
//

#include "parser.h"

bool equalStrings(char* s1, char* s2) {
    return (strcmp(s1, s2) == 0);
}

errorCode fillSymtable(tableNodePtr globalTable, list* tokenList){

    size_t length = tokenList->size;

    for(size_t i = 0; i < length; i++){

        string funcIdent;
        initString(&funcIdent);
        token func;
        getToken(tokenList, i, &func);

       if(func.tokenType == FUNC){  // *FUNC* ...

            if(func.nextToken != NULL && func.nextToken->tokenType == IDENT){
                i++;
                token ident;
                getToken(tokenList, i, &ident); // FUNC *IDENT* ...
                makeString(ident.tokenName.data, &funcIdent); //uloží identifikator do stringu

                if(equalStrings(funcIdent.data,"main")){
                    destroyString(&funcIdent);
                    continue;
                }

                if((ident.nextToken != NULL) && (ident.nextToken->tokenType == BRACKET_ROUND) && equalStrings("(", ident.tokenName.data)){
                    i++;
                    token param;
                    getToken(tokenList, i, &param); // FUNC IDENT *(* ...

                    list* parameters = malloc(sizeof(list));
                    initList(parameters); //list parametrov

                    i++;
                    getToken(tokenList, i, &param); // FUNC IDENT ( *IDENT* ...

                    while(param.tokenType != BRACKET_ROUND && !equalStrings(")", param.tokenName.data)){ //naplní list parametrov
                        if(param.nextToken != NULL){
                            switch (param.tokenType) {
                                case INT:
                                    if(param.nextToken->tokenType == COMMA || (param.nextToken->tokenType == BRACKET_ROUND && equalStrings(")", param.nextToken->tokenName.data))){
                                        addToken(parameters, param.tokenType, param.tokenName.data);
                                        i++;
                                        getToken(tokenList, i, &param);
                                    }else{
                                        deleteList(parameters);
                                        destroyString(&funcIdent);
                                        return SYNTAX_ERROR;
                                    }
                                    break;
                                case FLOAT:
                                    if(param.nextToken->tokenType == COMMA || (param.nextToken->tokenType == BRACKET_ROUND && equalStrings(")", param.nextToken->tokenName.data))){
                                        addToken(parameters, param.tokenType, param.tokenName.data);
                                        i++;
                                        getToken(tokenList, i, &param);
                                    }else{
                                        deleteList(parameters);
                                        destroyString(&funcIdent);
                                        return SYNTAX_ERROR;
                                    }
                                    break;
                                case STRING:
                                    if(param.nextToken->tokenType == COMMA || (param.nextToken->tokenType == BRACKET_ROUND && equalStrings(")", param.nextToken->tokenName.data))){
                                        addToken(parameters, param.tokenType, param.tokenName.data);
                                        i++;
                                        getToken(tokenList, i, &param);
                                    }else{
                                        deleteList(parameters);
                                        destroyString(&funcIdent);
                                        return SYNTAX_ERROR;
                                    }
                                    break;

                                case IDENT:
                                    if(param.nextToken->tokenType == INT || param.nextToken->tokenType == STRING || param.nextToken->tokenType == FLOAT ){
                                        addToken(parameters, param.tokenType, param.tokenName.data);
                                        i++;
                                        getToken(tokenList, i, &param);
                                    }else{
                                        deleteList(parameters);
                                        destroyString(&funcIdent);
                                        return SYNTAX_ERROR;
                                    }
                                    break;

                                case COMMA:
                                    if(param.nextToken->tokenType == IDENT || param.nextToken->tokenType == EOL){
                                        addToken(parameters, param.tokenType, param.tokenName.data);
                                        i++;
                                        getToken(tokenList, i, &param);
                                    }else{
                                        deleteList(parameters);
                                        destroyString(&funcIdent);
                                        return SYNTAX_ERROR;
                                    }
                                    break;
                                case EOL:
                                    if(param.nextToken->tokenType == EOL || param.nextToken->tokenType == IDENT){
                                        i++;
                                        getToken(tokenList, i, &param);
                                    }else{
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
                        }else{
                            deleteList(parameters);
                            destroyString(&funcIdent);
                            return SYNTAX_ERROR;
                        }

                    } // po while  ->  FUNC IDENT ( TYPE IDENT , ... *)*


                    if(param.nextToken != NULL && param.nextToken->tokenType == BRACKET_ROUND && equalStrings("(", param.nextToken->tokenName.data)) {

                        i++;
                        token returnTypes;
                        getToken(tokenList, i, &returnTypes); // FUNC IDENT ( TYPE IDENT , ... ) *(* ...
                        if(returnTypes.nextToken != NULL){
                            i++;
                            getToken(tokenList, i, &returnTypes); // FUNC IDENT ( TYPE IDENT , ... ) ( *RET_TYPE* ...

                            int count = 0;
                            dataType* retTypesArray = malloc(100*sizeof(dataType));


                            while(returnTypes.tokenType != BRACKET_ROUND && equalStrings(")", returnTypes.tokenName.data)){
                                if(returnTypes.nextToken != NULL){
                                    switch (returnTypes.tokenType) {
                                        case INT:
                                            if(returnTypes.nextToken->tokenType == COMMA || returnTypes.nextToken->tokenType == BRACKET_ROUND ){
                                                retTypesArray[count] = TYPE_INT;
                                                count++;
                                            }else{
                                                deleteList(parameters);
                                                destroyString(&funcIdent);
                                                return SYNTAX_ERROR;
                                            }
                                            break;
                                        case FLOAT:
                                            if(returnTypes.nextToken->tokenType == COMMA || returnTypes.nextToken->tokenType == BRACKET_ROUND ){
                                                retTypesArray[count] = TYPE_FLOAT;
                                                count++;
                                            }else{
                                                deleteList(parameters);
                                                destroyString(&funcIdent);
                                                return SYNTAX_ERROR;
                                            }
                                            break;
                                        case STRING:
                                            if(returnTypes.nextToken->tokenType == COMMA || returnTypes.nextToken->tokenType == BRACKET_ROUND ){
                                                retTypesArray[count] = TYPE_STRING;
                                                count++;
                                            }else{
                                                deleteList(parameters);
                                                destroyString(&funcIdent);
                                                return SYNTAX_ERROR;
                                            }
                                            break;
                                        case COMMA:
                                            if(returnTypes.nextToken->tokenType == INT || returnTypes.nextToken->tokenType == FLOAT || returnTypes.nextToken->tokenType == STRING){

                                            }else{
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
                                }else{
                                    deleteList(parameters);
                                    destroyString(&funcIdent);
                                    return SYNTAX_ERROR;
                                }

                            } // FUNC IDENT ( TYPE IDENT , ... ) ( RET_TYPE, ... *)*

                            retTypesArray[count] = TYPE_UNDEFINED;

                            if(returnTypes.nextToken != NULL && returnTypes.nextToken->tokenType == BRACKET_CURLY && equalStrings("{", returnTypes.nextToken->tokenName.data)){
                                i++;
                                getToken(tokenList, i, &returnTypes); // FUNC IDENT ( IDENT TYPE , ... ) ( RET_TYPE, ... ) *{*
                                if(returnTypes.nextToken != NULL && returnTypes.nextToken->tokenType == EOL){
                                    if(copyNode(&globalTable, funcIdent.data) == NULL){
                                        insertNode(&globalTable, funcIdent.data, retTypesArray, parameters,0);
                                        destroyString(&funcIdent);
                                    }else{
                                        deleteList(parameters);
                                        destroyString(&funcIdent);
                                        return DEFINITION_ERROR;
                                    }
                                }else{
                                    deleteList(parameters);
                                    destroyString(&funcIdent);
                                    return SYNTAX_ERROR;
                                }
                            }

                        }else{
                            deleteList(parameters);
                            destroyString(&funcIdent);
                            return SYNTAX_ERROR;
                        }

                    }else{
                        if(param.nextToken != NULL && param.nextToken->tokenType == BRACKET_CURLY && equalStrings("{", param.nextToken->tokenName.data)){
                            i++;
                            getToken(tokenList, i, &param); // FUNC IDENT ( TYPE IDENT , ... ) *{*
                            if(param.nextToken != NULL && param.nextToken->tokenType == EOL){
                                if(copyNode(&globalTable, funcIdent.data) == NULL){
                                    insertNode(&globalTable, funcIdent.data, NULL, parameters,0);
                                    destroyString(&funcIdent);
                                }else{
                                    deleteList(parameters);
                                    destroyString(&funcIdent);
                                    return DEFINITION_ERROR;
                                }
                            }else{
                                deleteList(parameters);
                                destroyString(&funcIdent);
                                return SYNTAX_ERROR;
                            }
                        }else{
                            deleteList(parameters);
                            destroyString(&funcIdent);
                            return SYNTAX_ERROR;
                        }
                    }
                }else{
                    destroyString(&funcIdent);
                    return SYNTAX_ERROR;
                }

            }else{
                destroyString(&funcIdent);
                return SYNTAX_ERROR;
            }
       }
    }

    return OK;
}
