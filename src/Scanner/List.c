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
    while(tToken != NULL){
        l->first = l->first->nextToken;
        destroyString(&tToken->tokenName);
        free(tToken);
        tToken = l->first;
    }
    l->last = NULL;
    l->size = 0;
}

void getToken(list* l, size_t pos, token* t){
    if(pos >= l->size){
        t = NULL;
        t->nextToken= NULL;
    }else{
        int i = 0;
        for(token* temp = l->first; i <= pos; temp = temp->nextToken ){
            initString(&t->tokenName);
            makeString(temp->tokenName.data, &t->tokenName);
            t->nextToken = temp->nextToken;
            t->tokenType = temp->tokenType;
            i++;
        }
    }
}

int addToken(list* l, type tType, char* tName ){

    token *tNew;
    tNew = malloc(sizeof(token));
    if(tNew == NULL){
        printf("Allocation went wrong!\n");
        return 1;
    }
    tNew->tokenType = tType;
    initString(&tNew->tokenName);
    makeString(tName, &tNew->tokenName);
    tNew->nextToken = NULL;

    if(l->last != NULL){
        l->last->nextToken = tNew;
    }
    l->last = tNew;
    if(l->size == 0){
        l->first = tNew;
    }
    l->size++;
    return 0;
}