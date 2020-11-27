#include <stdio.h>
#include "DynamicString.h"

int main() {

    string a;
    initString(&a);
    makeString("This is my new string. " , &a); // adding content to new string a
    printf("%s\n", a.data); // should print "This is my new string. "

    makeString(a.data, &a); // trying to makeString with data from the same string
    printf("%s\n", a.data); // should print exactly the same as first since the content shouldnt change

    concatenate(&a, &a, &a); // trying to concat a with itself and store the value in a
    printf("%s\n", a.data); // should print "This is my new string. This is my new string. "

    string b;
    initString(&b);
    makeString("very nice ", &b);
    insertToString(&a, &b, 11, &b); // trying inserting string b into string a and returning the whole string in b

    printf("%s\n", b.data); // should print This is my very nice new string. This is my new string.

    makeLowercase(&b, &b); // making string b lowercase

    printf("%s\n", b.data);

    makeUppercase(&b, &b); // making string b uppercase

    printf("%s\n", b.data);

    string c;
    initString(&c);
    makeString("This is string c.", &c);

    char this[] = {' ', 'A', 'n', 'd', ' ', 'a', 'l', 's', 'o', ' ', 't', 'h', 'i', 's', '.'};

    addConstChar(&c, " And I can append this to it.");

    printf("%s\n", c.data);

    addConstChar(&c, this);

    printf("%s\n", c.data);

    destroyString(&a);
    destroyString(&b);
    destroyString(&c);

    return 0;
}
