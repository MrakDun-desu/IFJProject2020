/******************************** Scanner.h **********************************/
/*  Predmet: IFJ a IAL						                                  */
/*  Lexikalny analyzator                                                      */
/*  Vytvoril: Martin Olsiak xolsia00                                          */
/* ************************************************************************** */

#include <stdio.h>
#include "List.h"
#include "DynamicString.h"

/**
 * @brief Analyzes code symbol by symbol and saves them into list.
 * @param sortedList List pointer where analyzed lexems will be stored.
 * @param code String of the code.
 */
errorCode CodeAnalyzer(list *sortedList, string code);



