//
// Created by xfudor00 on 28. 10. 2020.
//

#ifndef SRC_LIST_H
#define SRC_LIST_H
#include "DynamicString.h"

typedef enum Type {STRING,         // 0
                   ELSE,           // 1
                   FLOAT,          // 2
                   FOR,            // 3
                   FUNC,           // 4
                   IF,             // 5
                   INT,            // 6
                   PACKAGE,        // 7
                   RETURN,         // 8
                   IDENT,          // 9
                   BRACKET_CURLY,  // 10
                   BRACKET_ROUND,  // 11
                   EOL,            // 12
                   ARIT_OPERATOR,  // 13
                   ASIGN_OPERATOR, // 14
                   COMP_OPERATOR,  // 15
                   INT_LIT,        // 16
                   STRING_LIT,     // 17
                   FLOAT_LIT,      // 18
                   SEMICOL,        // 19
                   COMMA           // 20
} type;

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
errorCode addToken(list* l, type tType, char* tName);

/**
 * @brief Pushes token to the start of list as if to a stack.
 * @param l List into which the token will be pushed.
 * @param tok Token which contains data that will be pushed (will be copied by value).
 * @return Error code OK if pushing was successful, INTERNAL_ERROR otherwise.
 */
errorCode pushToken(list *l, token *tok);

/**
 * @brief Returns first token from the list as if from stack and deletes it from it.
 * @param l List from which the token will be popped.
 * @return Token which was formerly first in the list.
 */
token* popToken(list *l);

/**
 * @brief Copies the pointer to token in position pos.
 * @param l List from which the token will be copied.
 * @param pos Position in the list from which the token will be copied.
 * @return Pointer to the token.
 */
token* copyToken(list* l, size_t pos);

#endif //SRC_LIST_H
