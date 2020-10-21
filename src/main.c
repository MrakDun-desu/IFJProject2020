#include <stdio.h>
#include "DynamicString.h"

int main() {

    string* s = makeString("This is my string");
    string* t = makeString(" and it is mine!");

    printf("%s\n", s->data);

    changeString("This is my other string", s);

    printf("%s\n", s->data);

    string* whole = addStrings(s, t);

    printf("%s, %d, %lu\n", whole->data, whole->len, strlen(whole->data));

    string* mid = insertToString(whole, t, 42);

    printf("%s, string\n", mid->data);

    destroyString(s);
    destroyString(t);
    destroyString(whole);
    destroyString(mid);
    free(mid);
    free(s);
    free(t);
    free(whole);

    return 0;
}
