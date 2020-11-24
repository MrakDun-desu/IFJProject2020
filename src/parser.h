//
// Created by xdanco00 on 12. 11. 2020.
//

#ifndef SRC_PARSER_H
#define SRC_PARSER_H

#include "symtable.h"
#include "expression.h"
#include "generator.h"
#include "List.h"
#include "Errors.h"

/**
 * @brief Syntax analyser. Checks if tokenList has same datatype returns as types and if it has same count.
 * @param types Data types that are expected to be returned from token list
 * @param tokenList List of tokens that is to be analysed.
 * @return Error code that represents if discrepancy has been found. Can be OK or between 2 and 9.
 */
errorCode syntaxAnalyser(dataType* types, list tokenList, tableNodePtr globalTable, tableNodePtr localTable); /// xdrobe01

/**
 * @brief First run through token list. Only needs to identify functions and add them to global symtable.
 * @param globalTable Symtable into which the functions will be inserted.
 * @param tokenList List of tokens that is to be parsed.
 * @return Error code that represents if finding functions has been successful (can return OK, SYNTAX_ERROR or INTERNAL_ERROR)
 */
errorCode fillSymtable(tableNodePtr globalTable, list* tokenList); /// xfudor00

/**
 * @brief Parses list of tokens and returns it as a translated string. If any errors occur, returns corresponding error code.
 * @param tokenList Token list that is to be parsed.
 * @return Returns code OK if parsing and translation was successful, returns nonzero corresponding error code otherwise.
 */
errorCode parse(list* tokenList); /// xolsia00

#endif //SRC_PARSER_H
