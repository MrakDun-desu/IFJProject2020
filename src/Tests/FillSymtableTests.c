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
    else {
        printf("Položky tabulky\n");
        printf("|id :%s type : %d", symtable->data.id.data);


        makeString("f", &testString);
        if (symtable->data.id.data == &testString) {

        } else {
            printf("chyba: zle zadené id\n");
        }
        if (*(symtable->data.types) == TYPE_INT) {
            printf("Typ bol zadaný dobre\n");

        } else {
            printf("Zle zadaný typ");
        }
        if (symtable->data.parameters->size == 0) {
            printf("chyba: v tabulke chýbaju parametre funkcie");
        }


        if (symtable->right != NULL) {
            printf("|id :%s type : %d", symtable->right->data.id.data);
            if (symtable->right->data.types == TYPE_UNDEFINED) {

            } else {
                printf("chyba: zlá navratová hodnota 2. nacitanej funkcie\n");
            }

            if (symtable->right->data.parameters->size != 0) {
                printf("chyba: funkcia nema mat parametre");
            }
        }


        if (symtable->left != NULL) {
            printf("|id :%s type : %d|\n", symtable->left->data.id.data);
            if (symtable->left->data.types == TYPE_UNDEFINED) {

            } else {
                printf("chyba: zlá navratová hodnota 2. nacitanej funkcie\n");
            }


            if (symtable->left && symtable->right == NULL)
                printf("chyba: funkcia nenacitala druhu runkciu\n");

            if (symtable->right->data.parameters->size != 0) {
                printf("chyba: funkcia nema mat parametre");
            }

        }


    }



}
