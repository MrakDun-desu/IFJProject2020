//
// Created by xfudor00 on 28. 10. 2020.
//

#ifndef SRC_LIST_H
#define SRC_LIST_H
#include "DynamicString.h"
//                  0        1      2   3       4   5   6   7           8       9           10          11          12          13          14              15              16      17          18
typedef enum Type {STRING, ELSE, FLOAT, FOR, FUNC, IF, INT, PACKAGE, RETURN, VAR_ID,  BRACKET_CURLY, BRACKET_ROUND, EOL, ARIT_OPERATOR, ASIGN_OPERATOR, COMP_OPERAtOR, INT_LIT, STRING_LIT, FLOAT_LIT  } type;

typedef struct TToken {
    struct TToken *nextToken;
    type tokenType; /// type of the token
    string tokenName; /// string name of the token
} token;

typedef struct TList{
    token* first; /// pointer to first element in the list
    token* last; /// pointer to first element in the list
    size_t size; /// size of the list (empty list has size 0)
} list;

/**
 * @brief Initializes list so it can be used.
 * @param l List that is to be initialized.
 */
void initList(list* l);

/**
 * @brief Deletes list and frees all its allocated memory.
 * @param l List that is to be initialized.
 */
void deleteList(list* l);

/**
 * @brief Gets the value of token in the position pos. If pos is larger than list size, fills token with NULL.
 * @param l List from which the value will be taken.
 * @param pos Position of the token.
 * @param t Token that is used as return value.
 */
void getToken(list* l, size_t pos, token* t);

/**
 * @brief Adds token to the end of a list.
 * @param l List into which the token will be added.
 * @param t Token which will be added in to list l.
 * @return If item was added successfully, returns 0, otherwise returns 1.
 */
int addToken(list* l, type tType, char* tName);

#endif //SRC_LIST_H
