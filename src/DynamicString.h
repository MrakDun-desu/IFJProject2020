//
// Created by Marek on 3. 10. 2020.
//

#ifndef SRC_DYNAMICSTRING_H
#define SRC_DYNAMICSTRING_H
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Struct for storing strings and their length
 */
typedef struct String {
    char* data;
    int len;
} string;

/**
 * @brief Function that creates string from a string literal
 * @param content String literal that should be converted to string
 * @return Pointer to string struct
 */
string* makeString(const char* content);

/**
 * @brief Function that frees memory which was allocated for string data
 * @param s String that should be freed
 */
void destroyString(string* s);

/**
 * @brief Function that changes string data to different string literal and changes the length too
 * @param content String literal which should be new data of a string
 * @param s Pointer to string which will recieve the new data
 */
void changeString(const char* content, string* s);

/**
 * @brief Function that appends string to another string and creates new string in the process
 * @param first String that will be first in new string
 * @param second String that will be second in new string
 * @return New string that is first succeeded by second
 */
string* addStrings(string* first, string* second);

/**
 * @brief Function that inserts string into the middle of another string based on the position
 * @param first String into which the other string will be inserted
 * @param second String that will be inserted into the first string
 * @param pos Position in the first string from where the second one will begin. If position is greater than length of first string, empty string will be returned
 * @return New string that is first pos characters from the first string, then entire second string and then the rest of first string
 */
string* insertToString(string* first, string* second, int pos);

#endif //SRC_DYNAMICSTRING_H
