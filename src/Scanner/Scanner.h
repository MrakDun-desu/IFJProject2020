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
 * @brief Analyzes lexems and saves them in list
 * @param sortedList List pointer where analyzed lexems are stored
 * @param code List pointer where analyzed lexems are stored
 */

void Analyzer(TList *sortedList, String code);

void LexemAutomat(TList* sortedList, string lexem);

void CharConcat(string target, char addition);

