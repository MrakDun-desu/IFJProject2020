#include "DynamicString.h"

void initString(string* newString) {

    newString->len = 0;
    newString->data = NULL;

}

void makeString(const char *content, string* back) {

    if (back->len != 0) {
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

void insertToString(string* first, string* second, size_t pos, string* back) {

    if (pos > first->len) {
        makeString(first->data, back);
        return;
    }
    if (pos == 0) {
        makeString("", back);
        return;
    }

    char* newData = malloc(sizeof(char) * (first->len + second->len + 1));
    strcpy(newData, first->data);
    strcpy(&newData[pos], second->data);
    strcpy(&newData[pos+second->len], &first->data[pos]);
    makeString(newData, back);
    free(newData);

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

