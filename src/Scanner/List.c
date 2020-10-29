//
// Created by praca on 28. 10. 2020.
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
        free(tToken);
        tToken = l->first;
    }
    l->last = NULL;
    l->size = 0;
}

void getToken(list* l, size_t pos, token* t){
    if(pos >= l->size){
        t = NULL;
    }else{
        t = l->first;
        for(int i = 0; i < pos; i++){
            t = t->nextToken;
        }
    }
}

int addToken(list* l, token* t){

    t = malloc(sizeof(token));
    if(t == NULL){
        return 1;
    }
    if(l->last != NULL){
        l->last->nextToken = t;
    }
    t->nextToken = NULL;
    l->last = t;
    if(l->size == 0){
        l->first = t;
    }
    l->size++;
    return 0;
}