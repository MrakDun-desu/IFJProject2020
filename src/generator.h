//
// Created by xdanco00 on 12. 11. 2020.
//

#ifndef SRC_GENERATOR_H
#define SRC_GENERATOR_H

#include "symtable.h"
#include "Errors.h"
#include "DynamicString.h"
#include "expression.h"

/// TODO make macros to add string to an end of another and check for errors

typedef struct tGenerator { /// struct that simplifies working with generator
    string* program; /// string that will be the final program
    size_t ifCounter; /// helper variable to count ifs for generating labels
    size_t forCounter; /// helper variable to count fors for generating labels
    size_t varCounter; /// helper variable to count additional variables (that are not specified in source code, but need to be in target code)
} generator;

generator gen; /// generator that all specified functions will use so program string doesn't have to be passed to each function.

// Input functions
#define FUNC_INPUTS "LABEL inputs\nCREATEFRAME\nDEFVAR TF@ret0\nDEFVAR TF@ret1\nMOVE TF@ret1 int@0\nREAD TF@ret0 string\nJUMPIFNEQ inputs_succ TF@ret0 nil@nil\nMOVE TF@ret1 int@1\nLABEL inputs_succ\nRETURN\n\n"
#define FUNC_INPUTI "LABEL inputi\nCREATEFRAME\nDEFVAR TF@ret0\nDEFVAR TF@ret1\nMOVE TF@ret1 int@0\nREAD TF@ret0 int\nDEFVAR TF@type0\nTYPE TF@type0 TF@ret0\nJUMPIFEQ inputi_err TF@ret0 nil@nil\nJUMPIFEQ inputi_succ TF@type0 string@int\nJUMP inputi_err\nLABEL inputi_succ\nRETURN\nLABEL inputi_err\nMOVE TF@ret1 int@1\nRETURN\n\n"
#define FUNC_INPUTF "LABEL inputf\nCREATEFRAME\nDEFVAR TF@ret0\nDEFVAR TF@ret1\nMOVE TF@ret1 int@0\nREAD TF@ret0 float\nDEFVAR TF@type0\nTYPE TF@type0 TF@ret0\nJUMPIFEQ inputf_err TF@ret0 nil@nil\nJUMPIFEQ inputf_succ TF@type0 string@float\nJUMP inputf_err\nLABEL inputf_succ\nRETURN\nLABEL inputf_err\nMOVE TF@ret1 int@1\nRETURN\n\n"


// Number conversion functions
#define FUNC_INT2FLOAT "LABEL int2float\nCREATEFRAME\nDEFVAR TF@p\nPOPS TF@p\nDEFVAR TF@ret\nINT2FLOAT TF@ret TF@p\nRETURN\n\n"
#define FUNC_FLOAT2INT "LABEL float2int\nCREATEFRAME\nDEFVAR TF@p\nPOPS TF@p\nDEFVAR TF@ret\nFLOAT2INT TF@ret TF@p\nRETURN\n\n"

// String functions
#define FUNC_LEN "LABEL funclen\nCREATEFRAME\nDEFVAR TF@s\nPOPS TF@s\nDEFVAR TF@ret\nSTRLEN TF@ret TF@s\nRETURN\n\n"
#define FUNC_SUBSTR "LABEL substr\nCREATEFRAME\nDEFVAR TF@n\nPOPS TF@n\nDEFVAR TF@i\nPOPS TF@i\nDEFVAR TF@s\nPOPS TF@s\nDEFVAR TF@temp\nDEFVAR TF@lenght\nDEFVAR TF@maxIndex\nDEFVAR TF@ret0\nDEFVAR TF@ret1\n\
DEFVAR TF@out\nSTRLEN TF@lenght TF@s\nMOVE TF@maxIndex TF@lenght\nSUB TF@maxIndex TF@maxIndex int@1\nMOVE TF@ret1 int@0\nMOVE TF@out bool@false\nLT TF@out TF@i int@0\nGT TF@out TF@i TF@maxIndex\nJUMPIFEQ substr_err TF@out bool@true\nMOVE TF@out bool@false\nLT TF@out TF@n int@0\nJUMPIFEQ substr_err TF@out bool@true\n\
MOVE TF@ret0 string@#\nLABEL substr_cyklus\nGETCHAR TF@temp TF@s TF@i\nJUMPIFEQ substr_greater TF@i TF@maxIndex\nADD TF@i TF@i int@1\nSUB TF@n TF@n int@1\nLABEL substr_cyklus_back\nCONCAT TF@ret0 TF@ret0 TF@temp\nMOVE TF@temp nil@nil\nJUMPIFNEQ substr_cyklus TF@n int@0\nRETURN\n\
LABEL substr_greater\nMOVE TF@i int@0\nSUB TF@n TF@n int@1\nJUMP substr_cyklus_back\nLABEL substr_err\nMOVE TF@ret1 int@1\nMOVE TF@ret0 string@OUT_OF_STRING\nRETURN\n\n"

#define FUNC_ORD "LABEL funcord\nCREATEFRAME\nDEFVAR TF@i\nPOPS TF@i\nDEFVAR TF@s\nPOPS TF@s\nDEFVAR TF@l\nDEFVAR TF@out\nSTRLEN TF@l TF@s\nSUB TF@l TF@l int@1\nGT TF@out TF@i TF@l\nDEFVAR TF@ret0\nDEFVAR TF@ret1\nMOVE TF@ret1 int@0\nJUMPIFEQ funcord_err TF@out bool@true\nSTRI2INT TF@ret0 TF@s TF@i\nRETURN\n\
LABEL funcord_err\nMOVE TF@ret0 nil@nil\nMOVE TF@ret1 int@1\nRETURN\n\n"

#define FUNC_CHR "LABEL chr\nCREATEFRAME\nDEFVAR TF@ascii\nPOPS TF@ascii\nDEFVAR TF@ret0\nDEFVAR TF@ret1\nDEFVAR TF@out\nMOVE TF@out bool@false\nMOVE TF@ret0 int@0\nJUMPIFEQ chr_err TF@ascii nil@nil\nGT TF@out TF@ascii int@255\n\
LT TF@out TF@ascii int@1\nJUMPIFEQ chr_err TF@out bool@true\nINT2CHAR TF@ret0 TF@ascii\nRETURN\nLABEL chr_err\nMOVE TF@ret1 int@1\nMOVE TF@ret0 string@OUT_OF_RANGE_(0;255)\nRETURN\n\n"

/**
 * @brief Initializes generator gen and sets all variables to default.
 */
void generatorInit();

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
 * @param type Datatype of the variable that is to be assigned (More parameters should probably be added).
 * @param var Token representing the variable to which the value will be assigned.
 * @param value Token representing the value which will be assigned to variable.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateMove(token* var, token* value);

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
 * @brief Generates the "or" command.
 * @param var Variable that should store the result of OR.
 * @param symb1 First symbol in the OR.
 * @param symb2 Second symbol in the OR.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateOR(token* var, token* symb1, token* symb2, char* frames);

/**
 * @brief Generates the "not" command.
 * @param var Variable that should store the result of NOT.
 * @param symb Bool value that is to be negated.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateNOT(token* var, token* symb, char* frames);

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
errorCode generateExpression(list* expression, tableNodePtr varTable, token* lastToken); // funkcia pre xdanco00

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
