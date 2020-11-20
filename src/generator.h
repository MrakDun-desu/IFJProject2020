//
// Created by xdanco00 on 12. 11. 2020.
//

#ifndef SRC_GENERATOR_H
#define SRC_GENERATOR_H

#include "symtable.h"
#include "Errors.h"
#include "DynamicString.h"

/// TODO make macros to add string to an end of another and check for errors

typedef struct tGenerator { /// struct that simplifies working with generator
    string* program; /// string that will be the final program
    size_t ifCounter; /// helper variable to count ifs for generating labels
    size_t forCounter; /// helper variable to count fors for generating labels
    size_t varCounter; /// helper variable to count additional variables (that are not specified in source code, but need to be in target code)
} generator;

generator gen; /// generator that all specified functions will use so program string doesn't have to be passed to each function.

// Input functions
#define FUNC_INPUTS /// TODO
#define FUNC_INPUTI /// TODO
#define FUNC_INPUTF /// TODO

// Print function
#define FUNC_PRINT /// TODO

// Number conversion functions
#define FUNC_INT2FLOAT /// TODO
#define FUNC_FLOAT2INT /// TODO

// String functions
#define FUNC_LEN /// TODO
#define FUNC_SUBSTR /// TODO
#define FUNC_ORD /// TODO
#define FUNC_CHR /// TODO

/**
 * @brief Initializes generator gen and sets all variables to default.
 */
void generatorInit();

/**
 * @brief Puts the head of program at start, including the macro functions. Also needs to include jump to main function.
 * @return OK if all allocation has been successful, corresponding error code otherwise.
 */
errorCode generatorStart();

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
 * @brief Starts main and creates frame.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateMainScopeStart();

/**
 * @brief Ends main (it should just jump to end of file, so no other commands are executed).
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateMainScopeEnd();

/**
 * @brief Generates start of function.
 * @param function Pointer to function data containing parameters and return types.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateFunctionStart(data* function);

/**
 * @brief Generates returns of function.
 * @param function Pointer to function data containing parameters and return types.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateFunctionReturn(data* function);

/**
 * @brief Generates end of function (pushframe and return).
 * @param function Pointer to function data containing parameters and return types.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateFunctionEnd(data* function);

/**
 * @brief Generates call of function and everything that goes before it.
 * @param function Pointer to function data containing parameters and return types.
 * @param args Pointer to array of arguments that are passed to function.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateFunctionCall(data* function, data* args[]);

/**
 * @brief Generates start of if else sequence.
 * @param level Level at which the if else sequence is (0 if directly in function). Should be used when naming jump labels to avoid duplicates.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateIfElseStart(size_t level);

/**
 * @brief Generates the else part of if else sequence.
 * @param level Level at which the if else sequence is (0 if directly in function). Should be used when naming jump labels to avoid duplicates.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateElse(size_t level);

/**
 * @brief Generates the end of if else sequence.
 * @param level Level at which the if else sequence is (0 if directly in function). Should be used when naming jump labels to avoid duplicates.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateIfElseEnd(size_t level);

/**
 * @brief Generates the start of for sequence.
 * @param level Level at which the for sequence is (0 if directly in function). Should be used when naming jump labels to avoid duplicates.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateForStart(size_t level);

/**
 * @brief Generates the end of for sequence.
 * @param level Level at which the for sequence is (0 if directly in function). Should be used when naming jump labels to avoid duplicates.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateForEnd(size_t level);

/**
 * @brief Generates definition of a variable.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateDefvar();

/**
 * @brief Generates assigning value to a variable.
 * @param type Datatype of the variable that is to be assigned (More parameters should probably be added).
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateMove(dataType type);

/**
 * @brief Generates breakpoint in program for debugging (not sure if necessary).
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateBreak();

/**
 * @brief Generates exit from program with value retval (must be between 0 and 49).
 * @param retval Return value of the program.
 * @return OK if allocation was successful, corresponding error code otherwise.
 */
errorCode generateExit(size_t retval);


#endif //SRC_GENERATOR_H
