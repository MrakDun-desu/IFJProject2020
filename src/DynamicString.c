#include "DynamicString.h"

string *makeString(const char *content) {

    string* back = malloc(sizeof(string));

    int length = strlen(content);
    back->data = malloc(sizeof(char) * (length + 1));
    strcpy(back->data, content);
    back->len = length;

    return back;

}

void destroyString(string* s) {
    free(s->data);
    s->len = 0;
}

void changeString(const char* content, string* s) {

    int length = strlen(content);
    free(s->data);
    s->data = malloc(sizeof(char) * (length + 1));
    strcpy(s->data, content);
    s->len = length;

}

string* addStrings(string* first, string* second) {

    char* newData = malloc(sizeof(char) * (first->len + second->len + 1));
    strcpy(newData, first->data);
    strcpy(&newData[first->len], second->data);
    string* back = makeString(newData);
    free(newData);

    return back;

}

string* insertToString(string* first, string* second, int pos) {

    string* back;
    if (pos > first->len) {
        back = makeString("");
        return back;
    }
    char* newData = malloc(sizeof(char) * (first->len + second->len + 1));
    strcpy(newData, first->data);
    strcpy(&newData[pos], second->data);
    strcpy(&newData[pos+second->len], &first->data[pos]);
    back = makeString(newData);
    free(newData);

    return back;

}

