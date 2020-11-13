//
// Created by xdanco00 on 12. 11. 2020.
//

#ifndef SRC_PARSER_H
#define SRC_PARSER_H

#include "symtable.h"
#include "expression.h"
#include "generator.h"
#include "List.h"

/**
 * @brief Parses list of tokens and returns it as a translated string. If any errors occur, returns corresponding error code.
 * @param tokenList Token list that is to be parsed.
 * @return Returns code OK if parsing and translation was successful, returns nonzero corresponding error code otherwise.
 */
errorCode parse(list* tokenList);

#endif //SRC_PARSER_H
