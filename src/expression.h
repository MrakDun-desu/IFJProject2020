//
// Created by xdanco00 on 12. 11. 2020.
//

#ifndef SRC_EXPRESSION_H
#define SRC_EXPRESSION_H

#include "DynamicString.h"
#include "List.h"
#include "generator.h"

/**
 * @brief Function that parses expression and generates code with precedence.
 * @param tokenList List of tokens that is to be parsed.
 * @param program String that will get used as return value.
 * @return 0 if parsing was successful, corresponding nonzero error code value otherwise.
 */
token* applyPrecedence(list* tokenList, tableNodePtr varTable);

#endif //SRC_EXPRESSION_H
