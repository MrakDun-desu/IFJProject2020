//
// Created by xdanco00 on 3. 10. 2020.
//

#ifndef SRC_DYNAMICSTRING_H
#define SRC_DYNAMICSTRING_H
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Errors.h"

/**
 * @brief Struct for storing strings and their length
 */
typedef struct String {
    char* data;
    size_t len;
} string;

/**
 * @brief Function that initializes string. Should be ALWAYS called before anything else and only once.
 */
void initString(string* newString);

/**
 * @brief Function that fills string with data from a string literal. If used with data from the string back does nothing.
 * Can also be called with already allocated strings to change their content.
 * @param content String literal that should be converted to string.
 * @param back String that will be newly allocated.
 * @return Error code OK if allocation was successful, INTERNAL_ERROR otherwise.
 */
errorCode makeString(const char* content, string* back);

/**
 * @brief Function that frees memory which was allocated for string data.
 * @param s String that should be freed.
 */
void destroyString(string* s);

/**
 * @brief Function that appends string to another string and creates new string in the process. First, second and back can be all the same string.
 * @pre All strings must have had init function used on them.
 * @param first String that will be first in new string.
 * @param second String that will be second in new string.
 * @param back New string, that is first succeeded by second.
 * @return Error code OK if reallocation of data was successful, INTERNAL_ERROR otherwise.
 */
errorCode concatenate(string* first, string* second, string* back);

/**
 * @brief Function that adds const char* to the string.
 * @pre S must have had init function used on it.
 * @param s String that will have char appended.
 * @param c Char that will be appended.
 * @return Error code OK if reallocation of data was successful, INTERNAL_ERROR otherwise.
 */
errorCode addConstChar(string* s, const char* c);

/**
 * @brief Function that inserts string into the middle of another string based on the position. First, second and back can be all the same string.
 * @pre All strings must have had init function used on them.
 * @param first String into which the other string will be inserted.
 * @param second String that will be inserted into the first string.
 * @param pos Position in the first string from where the second one will begin. If position is greater than length of first string, back will be only first string.
 * @param back New string that is first pos characters from the first string, then entire second string and then the rest of first string.
 * @return Error code OK if reallocation of data was successful, INTERNAL_ERROR otherwise.
 */
errorCode insertToString(string* first, string* second, size_t pos, string* back);

/**
 * @brief Function that appends char at the end of a string.
 * @pre S must have had init function used on it.
 * @param s String that will have the char appended.
 * @param c Char that will be appended.
 * @return Error code OK if reallocation of data was successful, INTERNAL_ERROR otherwise.
 */
errorCode addChar(string* s, char c);

/**
 * @brief Function that returns string as all lowercase.
 * @pre All strings must have had init function used on them.
 * @param input String that will be converted.
 * @param back New string that will be same as first, only lowercase.
 * @return Error code OK if allocation of data was successful, INTERNAL_ERROR otherwise (only matters if input and back are not the same string).
 */
errorCode makeLowercase(string* input, string* back);

/**
 * @brief Function that returns string as all uppercase.
 * @pre All strings must have had init function used on them.
 * @param input String that will be converted.
 * @param back New string that will be same as first, only uppercase.
 * @return Error code OK if allocation of data was successful, INTERNAL_ERROR otherwise (only matters if input and back are not the same string).
 */
errorCode makeUppercase(string* input, string* back);

#endif //SRC_DYNAMICSTRING_H
