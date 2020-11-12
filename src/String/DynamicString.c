#include "DynamicString.h"

void initString(string* newString) {

    newString->len = 0;
    newString->data = NULL;

}

void makeString(const char *content, string* back) {

    if (back->data != NULL) {
        if (back->data == content)
            return;

        free(back->data);
    }

    size_t length = strlen(content);
    back->data = malloc(sizeof(char) * (length + 1));
    strcpy(back->data, content);
    back->len = length;

}

void destroyString(string* s) {

    if (s->data != NULL)
        free(s->data);

    s->len = 0;

}

void concatenate(string* first, string* second, string* back) {

    char* newData = malloc(sizeof(char) * (first->len + second->len + 1));
    strcpy(newData, first->data);
    strcpy(&newData[first->len], second->data);
    makeString(newData, back);
    free(newData);

}

void addConstChar (string* s, const char* c) {

    char* newData = malloc(sizeof(char) * (s->len + strlen(c) + 1));
    strcpy(newData, s->data);
    strcpy(&newData[s->len], c);
    makeString(newData, s);
    free(newData);

}

void insertToString(string* first, string* second, size_t pos, string* back) {

    if (pos > first->len) {
        makeString(first->data, back);
        return;
    }

    char* newData = malloc(sizeof(char) * (first->len + second->len + 1));
    if (pos != 0)
        strcpy(newData, first->data);

    strcpy(&newData[pos], second->data);
    strcpy(&newData[pos+second->len], &first->data[pos]);
    makeString(newData, back);
    free(newData);

}

void addChar(string* s, char c) {
    char pChar[2];
    pChar[0] = c;
    pChar[1] = '\0';

    string temp;
    initString(&temp);
    makeString(pChar, &temp);

    concatenate(s, &temp, s);

    destroyString(&temp);
}

void makeLowercase(string* input, string* back) {

    makeString(input->data, back);

    for (size_t i = 0; i < back->len; i++) {
        if (back->data[i] > 64 && back->data[i] < 91)
            back->data[i] = back->data[i] + ' ';
    }
}

void makeUppercase(string* input, string* back) {

    makeString(input->data, back);

    for (size_t i = 0; i < back->len; i++) {
        if (back->data[i] > 96 && back->data[i] < 123)
            back->data[i] = back->data[i] - ' ';
    }
}

