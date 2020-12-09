/******************************** DynamicString.c **********************************/
/*  Predmet: IFJ a IAL						                                  */
/*  Dynamicky alokovany string                                                */
/*  Vytvoril: Marek Danco xdanco00	                                          */
/* ************************************************************************** */


#include "DynamicString.h"

void initString(string* newString) {

    newString->len = 0;
    newString->data = NULL;

}

errorCode makeString(const char *content, string* back) {

    if (content != NULL) {
        if (back->data != NULL) {
            if (back->data == content)
                return OK;

            free(back->data);
        }

        size_t length = strlen(content);
        back->data = malloc(sizeof(char) * (length + 1));

        if (back->data) {
            strcpy(back->data, content);
            back->len = length;
            return OK;
        }
    }
    return INTERNAL_ERROR;

}

void destroyString(string* s) {

    if (s != NULL) {
        if (s->data != NULL)
            free(s->data);
        s->data = NULL;

        s->len = 0;
    }

}

errorCode concatenate(string* first, string* second, string* back) {

    char* newData;
    if ((newData = malloc(sizeof(char) * (first->len + second->len + 1))) != NULL) {
        strcpy(newData, first->data);
        strcpy(&newData[first->len], second->data);
        errorCode error;
        error = makeString(newData, back);
        free(newData);
        return error;
    }
    return INTERNAL_ERROR;

}

errorCode addConstChar (string* s, const char* c) {

    char* newData;
    if ((newData = malloc(sizeof(char) * (s->len + strlen(c) + 1))) != NULL) {
        strcpy(newData, s->data);
        strcpy(&newData[s->len], c);
        errorCode error;
        error = makeString(newData, s);
        free(newData);
        return error;
    }
    return INTERNAL_ERROR;

}

errorCode insertToString(string* first, string* second, size_t pos, string* back) {

    if (pos > first->len)
        return makeString(first->data, back);

    char* newData;
    if ((newData = malloc(sizeof(char) * (first->len + second->len + 1))) != NULL) {
        if (pos != 0)
            strcpy(newData, first->data);

        strcpy(&newData[pos], second->data);
        strcpy(&newData[pos + second->len], &first->data[pos]);
        errorCode error;
        error = makeString(newData, back);
        free(newData);
        return error;
    }
    return INTERNAL_ERROR;

}

errorCode addChar(string* s, char c) {
    char pChar[2];
    pChar[0] = c;
    pChar[1] = '\0';

    string temp;
    initString(&temp);
    errorCode error;
    error = makeString(pChar, &temp);
    if (error == OK) {
        error = concatenate(s, &temp, s);
        destroyString(&temp);
    }

    return error;
}

errorCode makeLowercase(string* input, string* back) {

    errorCode error = makeString(input->data, back);

    if (error == OK) {
        for (size_t i = 0; i < back->len; i++) {
            if (back->data[i] > 64 && back->data[i] < 91)
                back->data[i] = back->data[i] + ' ';
        }
    }
    return error;
}

errorCode makeUppercase(string* input, string* back) {

    errorCode error = makeString(input->data, back);

    if (error == OK) {
        for (size_t i = 0; i < back->len; i++) {
            if (back->data[i] > 96 && back->data[i] < 123)
                back->data[i] = back->data[i] - ' ';
        }
    }
    return error;
}

bool equalStrings(char* s1, char* s2) {
    return (strcmp(s1, s2) == 0);
}
