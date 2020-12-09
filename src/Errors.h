/******************************** Errors.h **********************************/
/*  Predmet: IFJ a IAL						                                  */
/*  Pomocny enum na typy chyb ktore mozu nastat                               */
/*  Vytvoril: Marek Danco xdanco00	                                          */
/* ************************************************************************** */

#ifndef SRC_ERRORS_H
#define SRC_ERRORS_H

typedef enum tErrorCode {
    OK = 0,
    LEXICAL_ERROR,
    SYNTAX_ERROR,
    DEFINITION_ERROR,
    DATATYPE_ERROR,
    TYPE_COMPATIBILITY_ERROR,
    PARAMETER_ERROR,
    SEMANTIC_ERROR,
    ZERO_DIVISION_ERROR = 9,
    INTERNAL_ERROR = 99
    } errorCode;


#endif //SRC_ERRORS_H