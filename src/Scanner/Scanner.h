//
// Created by xolsia00 on 3. 11. 2020.
//

#include <stdio.h>
#include "List.h"
#include "../String/DynamicString.h"


/**
 * @brief Handles Scanner
 */

void ScannerHandler();


/**
 * @brief Analyzes code symbol by symbol and saves them into string
 * @param sortedList List pointer where analyzed lexems are stored
 * @param code List pointer where code is stored
 */

void CodeAnalyzer(list *sortedList, string code);

/**
 * @brief Analyzes lexems and saves them in list
 * @param sortedList List pointer where analyzed lexems are stored
 * @param code List pointer where analyzed lexems are stored
 */
void LexemAutomat(list* sortedList, string lexem);

void CharConcat(string target, char addition);

