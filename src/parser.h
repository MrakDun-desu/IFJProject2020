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

size_t scope = 0;

/**
 * @brief Semantic analyser. Takes individual lines and checks for semantic errors. Adds variables to local symtable if needed.
 * @pre If command contains an expression with EOL-s, EOL-s should be removed before sending here.
 * @param tokenList List of tokens that is to be analysed (Always the whole line).
 * @param globalTable Global table of symbols (functions).
 * @param localTable Local table of symbols (variables).
 * @param data Function that we are currently in (for checking return types).
 * @return Error code that represents if discrepancy has been found. Can be OK or between 2 and 9.
 */
errorCode semanticAnalyser(list* tokenList, tableNodePtr globalTable, tableNodePtr localTable, data* function); /// xdanco00

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
errorCode parse(list* tokenList, string* code); /// xolsia00

#endif //SRC_PARSER_H
