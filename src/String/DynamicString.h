//
// Created by xdanco00 on 3. 10. 2020.
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
    size_t len;
} string;

/**
 * @brief Function that initializes string. Should be ALWAYS called before anything else.
 */
void initString(string* newString);

/**
 * @brief Function that fills string with data from a string literal. If used with data from the string back does nothing.
 * Can also be called with already allocated strings to change their content.
 * @param content String literal that should be converted to string.
 * @param back String that will be newly allocated.
 * @return Pointer to string struct.
 */
void makeString(const char* content, string *back);

/**
 * @brief Function that frees memory which was allocated for string data.
 * @param s String that should be freed.
 */
void destroyString(string* s);

/**
 * @brief Function that appends string to another string and creates new string in the process. First, second and back can be all the same string.
 * @param first String that will be first in new string.
 * @param second String that will be second in new string.
 * @param back New string that is first succeeded by second.
 */
void concatenate(string* first, string* second, string* back);

/**
 * @brief Function that inserts string into the middle of another string based on the position. First, second and back can be all the same string.
 * @param first String into which the other string will be inserted.
 * @param second String that will be inserted into the first string.
 * @param pos Position in the first string from where the second one will begin. If position is greater than length of first string, empty string will be returned.
 * @param back New string that is first pos characters from the first string, then entire second string and then the rest of first string.
 */
void insertToString(string* first, string* second, size_t pos, string* back);

/**
 * @brief Function that returns string as all lowercase.
 * @param input String that will be converted.
 * @param back New string that will be same as first, only lowercase.
 */
void makeLowercase(string* input, string* back);

/**
 * @brief Function that returns string as all uppercase.
 * @param input String that will be converted.
 * @param back New string that will be same as first, only uppercase.
 */
void makeUppercase(string* input, string* back);

#endif //SRC_DYNAMICSTRING_H
