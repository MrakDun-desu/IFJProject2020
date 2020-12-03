//
// Created by xdanco00 on 27. 11. 2020.
//

#include "../parser.h"
#include "../DynamicString.h"
#include "../List.h"
#include "../Scanner.h"

/// TODO Otestovat funkciu fillSymtable//prve
int main() {

    string testString;
    list testList;
    tableNodePtr symtable;
    initString(&testString);
    printf("Test 1:\n");
    makeString("func factorial(n int) (int) {}" , &testString);
    printf("beriem: %s\n",testString.data);
    initList(&testList);
    initTable(&symtable);
    CodeAnalyzer(&testList,testString);
    fillSymtable(symtable,&testList);
    if(symtable == NULL)
    {
        printf("chyba funkcia symtable nenapnila tabulku\n");
    }
    else
    {
        printf("|id :%s type : %d",symtable->data.id.data);
        makeString("factorial",&testString);
        if(symtable->data.id.data == &testString)
        {

        } else
        {
            printf("chyba: zle zadené id\n");
        }
        if(*(symtable->data.types) == TYPE_INT){
            printf("Typ bol zadaný dobre\n");

        } else
        {
            printf("Zle zadaný typ");
        }
        if(symtable->data.parameters->size == 0){
            printf("chyba: v tabulke chýbaju parametre funkcie");
        }

    }
    deleteTable(&symtable);
    deleteList(&testList);
    initTable(&symtable);
    initList(&testList);
    printf("\n");
    printf("Test 2:\n");
    makeString("func f(i int) (int) { return i }" , &testString);
    printf("beriem :%s\n",testString.data);
    CodeAnalyzer(&testList,testString);
    makeString("func main() {}" , &testString);
    printf("beriem :%s\n",testString.data);
    CodeAnalyzer(&testList,testString);
    fillSymtable(symtable,&testList);
    if(symtable == NULL)
    {
        printf("chyba funkcia symtable nenapnila tabulku\n");
    }
    else
    {
        printf("Položky tabulky");
        printf("|id :%s type : %d",symtable->data.id.data);
        if(symtable->right !=NULL)
            printf("|id :%s type : %d",symtable->right->data.id.data);
        else
            printf("|id :%s type : %d",symtable->left->data.id.data);

    }






}
