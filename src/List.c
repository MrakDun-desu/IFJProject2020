//
// Created by xfudor00 on 28. 10. 2020.
//

#include "List.h"

void initList (list* l) {
   l->size = 0;
   l->first = NULL;
   l->last = NULL;
}

void deleteList(list* l){
    token *tToken = l->first;
    while(l->first != NULL){
        l->first = l->first->nextToken;
        destroyString(&tToken->tokenName);
        free(tToken);
        tToken = l->first;
    }
    l->last = NULL;
    l->size = 0;
}

void getToken(list* l, size_t pos, token* t){
    if (t == NULL) {
        return;
    }
    if(pos >= l->size)
        t = NULL;
    else {
        *t = *l->first;
        for (size_t i = 0; i < pos; i++)
            *t = *t->nextToken;
    }
}

errorCode addToken(list* l, type tType, char* tName ){

    token *tNew;
    tNew = malloc(sizeof(token));
    if(!tNew){
        return INTERNAL_ERROR;
    }
    tNew->tokenType = tType;
    initString(&tNew->tokenName);
    if (makeString(tName, &tNew->tokenName)) return INTERNAL_ERROR;
    tNew->nextToken = NULL;

    if(l->last != NULL){
        l->last->nextToken = tNew;
    }
    l->last = tNew;
    if(l->size == 0){
        l->first = tNew;
    }
    l->size++;
    return OK;
}

token* copyToken(list* l, size_t pos){
    if(pos >= l->size)
        return NULL;
    else {
        token* temp = l->first;
        for (size_t i = 0; i < pos; i++)
           temp = temp->nextToken;
        return temp;
    }
}

errorCode pushToken(list *l, token *tok) {
    if (l != NULL) {
        token *newToken;
        if ((newToken = malloc(sizeof(token))) == NULL) return INTERNAL_ERROR;
        initString(&newToken->tokenName);
        if (makeString(tok->tokenName.data, &newToken->tokenName)) return INTERNAL_ERROR;
        newToken->tokenType = tok->tokenType;
        token* temp = l->first;
        l->first = newToken;
        if (temp == NULL)
            l->last = newToken;
        l->first = newToken;
        l->size++;
        newToken->nextToken = temp;
    }
    return OK;
}

token* popToken(list *l) {
    if (l != NULL) {
        token *back = l->first;
        if (l->first != NULL) {
            l->first = l->first->nextToken;
            l->size--;
        }
        return back;
    }
    return NULL;
}

