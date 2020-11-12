//
// Created by xfudor00 on 29. 10. 2020.
//

#include <stdio.h>
#include "List.h"
#include "../String/DynamicString.h"

int main (){
    list TestList;
    initList(&TestList);
    printf("-------------------------\n");
    printf("List size = %d\nList first = %p\nList last = %p\n",TestList.size,TestList.first, TestList.last);
    printf("-------------------------\n");


    string a;
    initString(&a);
    makeString("First", &a);
    addToken(&TestList, ELSE, a.data);
    destroyString(&a);
    printf("Token added\n");
    printf("List size = %d\nList first = %p\nList last = %p\n",TestList.size,TestList.first, TestList.last);
    token t1;
    getToken(&TestList, TestList.size-1, &t1);
    printf("Token name = %s\n",t1.tokenName.data);
    printf("Token type = %d\n",t1.tokenType);
    printf("-------------------------\n");

    string b;
    initString(&b);
    makeString("Second", &b);
    addToken(&TestList, STRING, b.data);
    destroyString(&b);
    printf("Token added\n");
    printf("List size = %d\nList first = %p\nList last = %p\n",TestList.size,TestList.first, TestList.last);
    token t2;
    getToken(&TestList, TestList.size-1, &t2);
    printf("Token name = %s\n",t2.tokenName.data);
    printf("Token type = %d\n",t2.tokenType);
    printf("-------------------------\n");
    token t3;
    getToken(&TestList, 0, &t3);
    printf("Token on position 0 \n");
    printf("Token name on pos 0 = %s\n",t3.tokenName.data);
    printf("Token type on pos 0 = %d\n",t3.tokenType);
    printf("-------------------------\n");

    deleteList(&TestList);
    printf("Clearing list\n");
    printf("List size = %d\nList first = %p\nList last = %p\n",TestList.size,TestList.first, TestList.last);
    printf("-------------------------\n");

    printf("Everything should be 0 / 00000000\n");
    destroyString(&t1.tokenName);
    destroyString(&t3.tokenName);
    destroyString(&t2.tokenName);


    return 0;
}