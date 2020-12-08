//
// Created by xdanco00 on 12. 11. 2020.
//

#ifndef SRC_GENERATOR_H
#define SRC_GENERATOR_H

#include "symtable.h"
#include "Errors.h"
#include "DynamicString.h"
#include "expression.h"

typedef struct tGenerator { /// struct that simplifies working with generator
    string* program; /// string that will be the final program
} generator;

generator gen; /// generator that all specified functions will use so program string doesn't have to be passed to each function.

// Input functions

#define FUNC_INPUTS "LABEL inputs \n\
    CREATEFRAME \n\
    DEFVAR TF@ret0 \n\
    DEFVAR TF@ret1 \n\
    MOVE TF@ret1 int@0 \n\
    READ TF@ret0 string \n\
    JUMPIFNEQ inputs_succ TF@ret0 nil@nil \n\
    MOVE TF@ret1 int@1 \n\
    LABEL inputs_succ \n\
    RETURN \n\
     \n\
    "
#define FUNC_INPUTI "LABEL inputi \n\
    CREATEFRAME \n\
    DEFVAR TF@ret0 \n\
    DEFVAR TF@ret1 \n\
    MOVE TF@ret1 int@0 \n\
    READ TF@ret0 int \n\
    DEFVAR TF@type0 \n\
    TYPE TF@type0 TF@ret0 \n\
    JUMPIFEQ inputi_err TF@ret0 nil@nil \n\
    JUMPIFEQ inputi_succ TF@type0 string@int \n\
    JUMP inputi_err \n\
    LABEL inputi_succ \n\
    RETURN \n\
    LABEL inputi_err \n\
    MOVE TF@ret1 int@1 \n\
    RETURN \n\
     \n\
    "
#define FUNC_INPUTF "LABEL inputf \n\
    CREATEFRAME \n\
    DEFVAR TF@ret0 \n\
    DEFVAR TF@ret1 \n\
    MOVE TF@ret1 int@0 \n\
    READ TF@ret0 float \n\
    DEFVAR TF@type0 \n\
    TYPE TF@type0 TF@ret0 \n\
    JUMPIFEQ inputf_err TF@ret0 nil@nil \n\
    JUMPIFEQ inputf_succ TF@type0 string@float \n\
    JUMP inputf_err \n\
    LABEL inputf_succ \n\
    RETURN \n\
    LABEL inputf_err \n\
    MOVE TF@ret1 int@1 \n\
    RETURN \n\
     \n\
    "


// Number conversion functions
#define FUNC_INT2FLOAT "LABEL int2float \n\
    CREATEFRAME \n\
    DEFVAR TF@p \n\
    POPS TF@p \n\
    DEFVAR TF@ret \n\
    INT2FLOAT TF@ret TF@p \n\
    RETURN \n\
     \n\
    "
#define FUNC_FLOAT2INT "LABEL float2int \n\
    CREATEFRAME \n\
    DEFVAR TF@p \n\
    POPS TF@p \n\
    DEFVAR TF@ret \n\
    FLOAT2INT TF@ret TF@p \n\
    RETURN \n\
     \n\
    "

// String functions
#define FUNC_LEN "LABEL funclen \n\
    CREATEFRAME \n\
    DEFVAR TF@s \n\
    POPS TF@s \n\
    DEFVAR TF@ret \n\
    STRLEN TF@ret TF@s \n\
    RETURN \n\
     \n\
    "
#define FUNC_SUBSTR "LABEL substr \n\
    CREATEFRAME \n\
    DEFVAR TF@n \n\
    POPS TF@n \n\
    DEFVAR TF@i \n\
    POPS TF@i \n\
    DEFVAR TF@s \n\
    POPS TF@s \n\
    DEFVAR TF@temp \n\
    DEFVAR TF@lenght \n\
    DEFVAR TF@maxIndex \n\
    DEFVAR TF@ret0 \n\
    DEFVAR TF@ret1 \n\
    \n\
    DEFVAR TF@out \n\
    STRLEN TF@lenght TF@s \n\
    MOVE TF@maxIndex TF@lenght \n\
    SUB TF@maxIndex TF@maxIndex int@1 \n\
    MOVE TF@ret1 int@0 \n\
    MOVE TF@out bool@false \n\
    LT TF@out TF@i int@0 \n\
    GT TF@out TF@i TF@maxIndex \n\
    JUMPIFEQ substr_err TF@out bool@true \n\
    MOVE TF@out bool@false \n\
    LT TF@out TF@n int@0 \n\
    JUMPIFEQ substr_err TF@out bool@true \n\
    \n\
    MOVE TF@ret0 string@\\035 \n\
    LABEL substr_cyklus \n\
    GETCHAR TF@temp TF@s TF@i \n\
    JUMPIFEQ substr_greater TF@i TF@maxIndex \n\
    ADD TF@i TF@i int@1 \n\
    SUB TF@n TF@n int@1 \n\
    LABEL substr_cyklus_back \n\
    CONCAT TF@ret0 TF@ret0 TF@temp \n\
    MOVE TF@temp nil@nil \n\
    JUMPIFNEQ substr_cyklus TF@n int@0 \n\
    RETURN \n\
    \n\
    LABEL substr_greater \n\
    MOVE TF@i int@0 \n\
    SUB TF@n TF@n int@1 \n\
    JUMP substr_cyklus_back \n\
    LABEL substr_err \n\
    MOVE TF@ret1 int@1 \n\
    MOVE TF@ret0 string@OUT_OF_STRING \n\
    RETURN \n\
     \n\
    "

#define FUNC_ORD "LABEL funcord \n\
    CREATEFRAME \n\
    DEFVAR TF@i \n\
    POPS TF@i \n\
    DEFVAR TF@s \n\
    POPS TF@s \n\
    DEFVAR TF@l \n\
    DEFVAR TF@out \n\
    STRLEN TF@l TF@s \n\
    SUB TF@l TF@l int@1 \n\
    GT TF@out TF@i TF@l \n\
    DEFVAR TF@ret0 \n\
    DEFVAR TF@ret1 \n\
    MOVE TF@ret1 int@0 \n\
    JUMPIFEQ funcord_err TF@out bool@true \n\
    STRI2INT TF@ret0 TF@s TF@i \n\
    RETURN \n\
    \n\
    LABEL funcord_err \n\
    MOVE TF@ret0 nil@nil \n\
    MOVE TF@ret1 int@1 \n\
    RETURN \n\
     \n\
    "

#define FUNC_CHR "LABEL chr \n\
    CREATEFRAME \n\
    DEFVAR TF@ascii \n\
    POPS TF@ascii \n\
    DEFVAR TF@ret0 \n\
    DEFVAR TF@ret1 \n\
    DEFVAR TF@out \n\
    MOVE TF@out bool@false \n\
    MOVE TF@ret0 int@0 \n\
    JUMPIFEQ chr_err TF@ascii nil@nil \n\
    GT TF@out TF@ascii int@255 \n\
    \n\
    LT TF@out TF@ascii int@1 \n\
    JUMPIFEQ chr_err TF@out bool@true \n\
    INT2CHAR TF@ret0 TF@ascii \n\
    RETURN \n\
    LABEL chr_err \n\
    MOVE TF@ret1 int@1 \n\
    MOVE TF@ret0 string@OUT_OF_RANGE_(0;255) \n\
    RETURN \n\
     \n\
    "

/**
 * @brief Initializes generator gen and sets all variables to default.
 */
void generatorInit();

errorCode generatorHandle(list* currentLine, list* tokenList, tableNodePtr globalTable, tableNodePtr localTable, list* ifStack, data* currentFunc);

/**
 * @brief Puts the head of program at start, including the macro functions. Also needs to include jump to main function.
 * @return OK if all allocation has been successful, corresponding error code otherwise.
 */
errorCode generatorStart();
/*
 * IFJ CODE 20
 *
 * Takisto vygeneruje aj 10 pomocnych premennych do expressionov. Budu vyzerat takto:
 * DEFVAR GF@expVar{0-9}
 * Toto zabezpeci, ze sa nebudu musiet pouzivat ziadne defvary mimo premennych aj v zdrojovom kode.
 * Aby sa vyuzilo vsetkych 10 premennych, je treba, aby bolo 9 vnorenych zatvoriek (co sa dufam testovat nebude)
 *
 * KUBOVE funkcie
 *
 * JUMP MAIN
 */

/**
 * @brief Empties the program string and sets all the helper variables to zero.
 */
void generatorClear();

errorCode generatorHandle(list* currentLine, list* tokenList, tableNodePtr globalTable, tableNodePtr localTable, list* ifStack, data* currentFunc);


/**
 * @brief Writes program string into a specified file.
 * @param dest File that string will be written to.
 * @return OK if writing was successful, corresponding error code otherwise.
 */
errorCode generatorWrite(FILE* dest);


/**
 * @brief Ends main (it should just jump to end of file, so no other commands are executed).
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateMainScopeEnd();

/**
 * @brief Generates start of function (label, creation and popping of parameters and defvar for return variables).
 * @param function Pointer to function data containing parameters and return types.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateFunctionStart(data* function);

/* priklad functionStart
 * LABEL factorial // label na zaciatok
 * CREATEFRAME // frame na oddelenie premennych
 * DEFVAR TF@n // definicia a popovanie parametrov funkcie
 * POPS TF@n
 * DEFVAR TF@ret0 // definicia return hodnot
 */

/**
 * @brief Generates end of function (just return command).
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateFunctionEnd();

/**
 * @brief Generates call of function and everything that goes before it.
 * @param function Pointer to function data.
 * @param argValues Pointer to list of tokens, that are passed as arguments to function.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateFunctionCall(data* function, list* argValues);

/*
 * volanie funkcie priklad:
 * PUSHS TF@a // pushuje sa argument z argvalues                                    | o tuto cast sa stara functionCall
 * PUSHFRAME                                                                        |
 * CALL factorial // callne sa                                                      |
 *
 * MOVE LF@vysl TF@ret0 // return hodnoty funkcie sa vratia                         | o tuto cast sa stara function return
 * POPFRAME                                                                         |
 */

/**
 * @brief Generates returning from the function (moving the values from TF@ ret variables to assignVariables).
 * @param function Pointer to function data (so generator knows how many return types there are).
 * @param assignVariables Pointer to list of tokens that will receive return data from functions
 * @return
 */
errorCode generateFunctionReturn(data* function, list* assignVariables);

/**
 * @brief Generates start of if else sequence (condition should be send to expression.c for parsing and generation of code).
 * @param condition Pointer to list of tokens that are an if condition.
 * @param ifCount Number of the if, used for label generation.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateIfStart(list* condition, tableNodePtr varTable, size_t ifCount);

/*
 * ifElse priklad:
 *
 * DEFVAR TF@bool1 // vytvorenie bool premennej, v ktorej bude vysledok | toto riesi tato funkcia (ale tento riadok expression)
 * DEFVAR TF@bool2
 * GT TF@bool2 TF@a int@42                                              | (tento riadok riesi expression)
 * MOVE TF@bool1 TF@bool2                                               |
 * JUMPIFNEQ else0 TF@bool1 bool@true                                   |
 *      // prikazy vnutri ifu
 * JUMP ifEnd0                                                          | toto riesi generateElse
 * LABEL else0                                                          |
 *     // prikazy vnutri else
 * LABEL ifEnd0                                                         | toto riesi ifEnd
 */

/**
 * @brief Generates else label and jump to if end.
 * @param ifCount Number of the if, used for label generation.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateElse(size_t ifCount);

/**
 * @brief Generates the end of if else sequence.
 * @param ifCount Number of the if, used for label generation.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateIfEnd(size_t ifCount);

/**
 * @brief Generates prequel to for (first jump and label for start).
 * @param forCount Number of the for, used for label generation.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateForPrequel(size_t forCount);

/**
 * @brief Generates the start of for sequence (creating bools, creating two starting labels, checking condition and jump).
 * @param level Level at which the for sequence is (0 if directly in function). Should be used when naming jump labels to avoid duplicates.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateForStart(list* condition, tableNodePtr varTable, size_t forCount);
/*
 * Pozn. na condition spravi xdanco00 v expression.c funkciu, ktora ju vyriesi a vygeneruje pripadne potrebne frasy
 * Priklad na for:
 * DEFVAR TF@bool1 // vytvorenie bool premennej, v ktorej bude vysledok            |
 * JUMP forFirst0 // jumpuje sa pri prvom prechode                                 |   Tuto cast riesi forPrequel
 * LABEL forStart0                                                                 |
 *
 * SUB TF@a TF@a int@1 // vykonanie priradenia po ukonceni jednej iteracie         |   Toto by mal riesit syntax analyser pomocou inych funkcii
 *
 * LABEL forFirst0                                                                 |   Tuto cast riesi tato funkcia
 * DEFVAR TF@bool2                                                                 | (tento riadok riesi expression)
 * GT TF@bool2 TF@a int@0 // porovnanie (robi sa aj na zaciatku aj pocas foru)     | (tento riadok riesi expression - moze byt aj viac riadkov)
 * MOVE TF@bool1 TF@bool2                                                          |
 * JUMPIFNEQ forEnd0 TF@bool1 bool@true // jump na koniec, ak je treba             |
 *
 *      // prikazy vnutri foru
 *
 * JUMP forStart0 // jump na zaciatok, aby sa cyklilo                              |   Tuto cast riesi forEnd
 * LABEL forEnd0                                                                   |
 */

/**
 * @brief Generates the end of for sequence.
 * @param level Level at which the for sequence is (0 if directly in function). Should be used when naming jump labels to avoid duplicates.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateForEndLabel(size_t level);

/**
 * @brief Generates definition of a variable.
 * @param var Token representing the variable which should be created.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateDefvar(token* var);

/**
 * @brief Generates assigning value to a variable.
 * @param var Token representing the variable to which the value will be assigned.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateMove(token* var);

errorCode generatePrint(list* argValues);

/**
 * @brief Generates the arithmetic command (command is determined by char).
 * @param var Variable that should store the result of addition.
 * @param symb1 First symbol in the addition.
 * @param symb2 Second symbol in the addition.
 * @param operation Operation which is to be performed. (Can be + - * / %). / is for idiv, % is for div.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateArithmetic(token* var, token* symb1, token* symb2, char* frames, char operation);

/**
 * @brief Generates the "less than" command.
 * @param var Variable that should store the result of LT.
 * @param symb1 First symbol in the LT.
 * @param symb2 Second symbol in the LT.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateLT(token* var, token* symb1, token* symb2, char* frames);

/**
 * @brief Generates the "greater than" command.
 * @param var Variable that should store the result of GT.
 * @param symb1 First symbol in the GT.
 * @param symb2 Second symbol in the GT.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateGT(token* var, token* symb1, token* symb2, char* frames);

/**
 * @brief Generates the "equal" command.
 * @param var Variable that should store the result of EQ.
 * @param symb1 First symbol in the EQ.
 * @param symb2 Second symbol in the EQ.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateEQ(token* var, token* symb1, token* symb2, char* frames);

/**
 * @brief Generates the "not" command.
 * @param var Variable that should be negated.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateNOT(token* var);

/**
 * @brief Generates the "concatenation" command.
 * @param var Variable that should store the result of concatenation.
 * @param symb1 First string in the concatenation.
 * @param symb2 Second string in the concatenation.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateConcat(token* var, token* symb1, token* symb2, char* frames);

/**
 * @brief Generates expression from rightly ordered token list.
 * @param expression Token list which contains expression.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateExpression(list* expression, tableNodePtr varTable); // funkcia pre xdanco00

/*
 * riesenie expressions:
 *
 * dostaneme napriklad prikaz
 * a, b, c = x+y, y+2, 42
 * Najskor syntakticka analyza posudi, ci dany prikaz nie je uplny bullshit.
 * Posle semantickej analyze zistit, ci je spravny pocet vyrazov, ci sa typy zhoduju atd.
 *
 * Ak je vsetko v pohode, posle najskor x+y do applyPrecedence, ktory da tokeny do spravneho poradia,
 * odstrani zatvorky a nakoniec zavola tuto generateExpression.
 *
 * V applyPrecedence sa vrati token, ktory bude reprezentovat vysledok expressionu.
 * V kode sa budu tvorit nove premenne a parser potrebuje vediet, ktoru z nich ma priradit
 * do ktorej premennej na lavej strane.
 *
 * Takze, applyPrecedence vygeneruje kod na x+y a vrati token int4 (lebo momentalny varCounter bol 4).
 * Takze hodnota x+y sa ulozila do premennej TF@int4.
 *
 * Syntakticka analyza dostane toto ako token a vie, ze ma zavolat funkciu generateMove s tymto
 * tokenom ako parametrom value.
 *
 * Tento proces sa bude opakovat pre vyrazy y+2 a 42 a applyPrecedence vzdy vrati iny token vdaka varCounteru.
 *
 */


#endif //SRC_GENERATOR_H
