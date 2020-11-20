//
// Created by xolsia00 on 3. 11. 2020.
//

#include <stdio.h>
#include "List.h"
#include "DynamicString.h"


/**
 * @brief Handles Scanner
 */

errorCode ScannerHandler();


/**
 * @brief Analyzes code symbol by symbol and saves them into string
 * @param sortedList List pointer where analyzed lexems are stored
 * @param code List pointer where code is stored
 */

errorCode CodeAnalyzer(list *sortedList, string code);

/**
 * @brief Analyzes lexems and saves them in list
 * @param sortedList List pointer where analyzed lexems are stored
 * @param code List pointer where analyzed lexems are stored
 */
errorCode LexemAutomat(list* sortedList, string *lexem);



