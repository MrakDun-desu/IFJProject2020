//
// Created by xdanco00 on 27. 11. 2020.
//

#include "parser.h"
#include "Scanner.h"

void drawTree(tableNodePtr symtable){

    if(symtable == NULL) {
        printf("Symtable is empty\n");
        return;
    }

    if (symtable->left != NULL)
        drawTree(symtable->left);

    printf("Identifier: %s\n", symtable->data.id.data);
    string types;
    initString(&types);
    int counter = 0;
    for (dataType type = symtable->data.types[counter]; type != TYPE_UNDEFINED; type = symtable->data.types[++counter]){
        printf("Datatype %d: %d\n", counter, type);
    }

    list* parameters;
    parameters = symtable->data.parameters;
    if (parameters != NULL) {
        printf("This symbol is a function. Parameters:\n");
        for (int i = 1; i <= symtable->data.parameters->size; i++) {
            printf("%s ", parameters->first->tokenName.data);
            parameters->first = parameters->first->nextToken;
        }
    } else {
        printf("This symbol is a variable.");
    }

    printf("\nScope: %zu\n", symtable->data.scope);
    if (symtable->data.defined)
        printf("Is defined.\n");
    else
        printf("Is not defined\n");

    if (symtable->right != NULL)
        drawTree(symtable->right);

}

void testCommand(char* testStr, tableNodePtr* globalTable, tableNodePtr* localTable, data* func) {
    string testString;
    initString(&testString);
    list testList;
    initList(&testList);

    makeString(testStr,&testString);
    CodeAnalyzer(&testList,testString);
    errorCode code;
    code = semanticAnalyser(&testList, globalTable, localTable, func);
    switch (code) {
        case OK:
            printf("Command:\n%s\nResult: OK", testStr);
            break;
        case LEXICAL_ERROR:
            printf("Command:\n%s\nResult: LEXICAL_ERROR", testStr);
            break;
        case SYNTAX_ERROR:
            printf("Command:\n%s\nResult: SYNTAX_ERROR", testStr);
            break;
        case DEFINITION_ERROR:
            printf("Command:\n%s\nResult: DEFINITION_ERROR", testStr);
            break;
        case DATATYPE_ERROR:
            printf("Command:\n%s\nResult: DATATYPE_ERROR", testStr);
            break;
        case TYPE_COMPATIBILITY_ERROR:
            printf("Command:\n%s\nResult: TYPE_COMPATIBILITY_ERROR", testStr);
            break;
        case PARAMETER_ERROR:
            printf("Command:\n%s\nResult: PARAMETER_ERROR", testStr);
            break;
        case SEMANTIC_ERROR:
            printf("Command:\n%s\nResult: SEMANTIC_ERROR", testStr);
            break;
        case ZERO_DIVISION_ERROR:
            printf("Command:\n%s\nResult: ZERO_DIVISION_ERROR", testStr);
            break;
        case INTERNAL_ERROR:
            printf("Command:\n%s\nResult: INTERNAL_ERROR", testStr);
            break;
        default:
            break;
    }
    printf("\n");
}

/// TODO Otestovat funkciu semanticAnalysis//cely jeden riadok a bere 2 sem tabulky//drhue
int main() {
    string out;
    gen.program = &out;
    generatorInit();
    tableNodePtr globalTable;
    initTable(&globalTable);
    tableNodePtr localTable;
    initTable(&localTable);
    string testString;
    initString(&testString);

    // current function mockup
    data func;
    func.types = NULL;
    func.parameters = NULL;
    func.defined = true;
    func.scope = 0;
    initString(&func.id);
    makeString("main", &func.id);


    printf("----Datatype compatibility and zero division testing----\n");

    testCommand("64+2", &globalTable, &localTable, &func);
    testCommand("42.42 - 12.5",&globalTable, &localTable, &func);
    testCommand("\"Hello there\"+\"42\"", &globalTable, &localTable, &func);
    testCommand("12/0", &globalTable, &localTable, &func);
    testCommand("14.3/0.0000", &globalTable, &localTable, &func);
    testCommand("\"Hello there\" * \"18\"", &globalTable, &localTable, &func);
    testCommand("16+12.4", &globalTable, &localTable, &func);
    testCommand("\"Hello there\" + 42", &globalTable, &localTable, &func);
    testCommand("a := 42-45", &globalTable, &localTable, &func);
    testCommand("b := a-42", &globalTable, &localTable, &func);
    testCommand("c := b-a", &globalTable, &localTable, &func);
    testCommand("d := 16.5-a", &globalTable, &localTable, &func);
    testCommand("e := 15.4", &globalTable, &localTable, &func);
    testCommand("f := e-a", &globalTable, &localTable, &func);
    testCommand("g := 42<a", &globalTable, &localTable, &func);
    printf("\n");

    deleteTable(&localTable); // refreshing table so variables don't create errors they shouldn't

    printf("----Testing correct scope ending----\n");
    scope += 2;
    testCommand("a :=42", &globalTable, &localTable, &func);
    testCommand("b:= 18", &globalTable, &localTable, &func);
    testCommand("}", &globalTable, &localTable, &func);
    drawTree(localTable);

    testCommand("d := 12.5", &globalTable, &localTable, &func);
    testCommand("}", &globalTable, &localTable, &func);
    printf("\n");

    drawTree(localTable);


    printf("----Testing if semantics----\n");
    testCommand("if 42<18 {", &globalTable, &localTable, &func);
    testCommand("}", &globalTable, &localTable, &func);
    testCommand("a:=42", &globalTable, &localTable, &func);
    drawTree(localTable);
    testCommand("if a<45 {", &globalTable, &localTable, &func);
    testCommand("b:=18", &globalTable, &localTable, &func);
    testCommand("a:=98.5", &globalTable, &localTable, &func);
    drawTree(localTable);
    testCommand("}", &globalTable, &localTable, &func);
    testCommand("if 12.5 + 48.2 {", &globalTable, &localTable, &func);
    testCommand("if 4+5*7-(42+5)/5<18+(8-2*(16-5)+781) {", &globalTable, &localTable, &func);
    drawTree(localTable);

    printf("\n");

    scope = 0;
    deleteTable(&localTable);

    printf("----Testing for semantics----\n");
    testCommand("for c:= 18 ;42<18; c = c +1 {", &globalTable, &localTable, &func);
    testCommand("}", &globalTable, &localTable, &func);
    testCommand("a:=42", &globalTable, &localTable, &func);
    drawTree(localTable);
    testCommand("for ;a<45; {", &globalTable, &localTable, &func);
    testCommand("b:=18", &globalTable, &localTable, &func);
    testCommand("a:=98.5", &globalTable, &localTable, &func);
    drawTree(localTable);
    testCommand("}", &globalTable, &localTable, &func);
    testCommand("for ;12.5 + 48.2; {", &globalTable, &localTable, &func);
    testCommand("for ;4+5*7-(42+5)/5<18+(8-2*(16-5)+781); {", &globalTable, &localTable, &func);
    testCommand("for c:= 18 ;42<c; c = c +1 {", &globalTable, &localTable, &func);

    drawTree(localTable);

    printf("\n");

    scope = 0;
    deleteTable(&localTable);

    printf("----Testing assign semantics----\n");
    testCommand("_ = 42", &globalTable, &localTable, &func);
    testCommand("a = 42", &globalTable, &localTable, &func);
    testCommand("a := 42", &globalTable, &localTable, &func);
    testCommand("a = 17", &globalTable, &localTable, &func);
    testCommand("a = 17.4", &globalTable, &localTable, &func);
    testCommand("b := 13.5", &globalTable, &localTable, &func);
    testCommand("a, b = 11, 16.5", &globalTable, &localTable, &func);
    testCommand("a, b = 11.5, 17.8", &globalTable, &localTable, &func);
    testCommand("_, b = 11.5, 17.8", &globalTable, &localTable, &func);
    // testing functions
    dataType* types = malloc(sizeof(dataType)*3);
    types[0] = TYPE_INT;
    types[1] = TYPE_FLOAT;
    types[2] = TYPE_UNDEFINED;
    list* testFuncParams = malloc(sizeof(list));
    initList(testFuncParams);
    insertNode(&globalTable, "testFunc", types, testFuncParams, 0);
    testCommand("a, b = testFunc()", &globalTable, &localTable, &func);
    testCommand("testFunc()", &globalTable, &localTable, &func);
    globalTable->data.types[0] = TYPE_UNDEFINED;
    testCommand("a, b = testFunc()", &globalTable, &localTable, &func);
    testCommand("testFunc()", &globalTable, &localTable, &func);


    func.types = malloc(sizeof(dataType));
    func.types[0] = TYPE_UNDEFINED;
    func.parameters = NULL;
    func.defined = true;
    func.scope = 0;
    testCommand("return 42", &globalTable, &localTable, &func);
    testCommand("return", &globalTable, &localTable, &func);
    realloc(func.types, sizeof(dataType)*3);
    func.types[0] = TYPE_INT;
    func.types[1] = TYPE_FLOAT;
    func.types[2] = TYPE_UNDEFINED;
    func.parameters = NULL;
    func.defined = true;
    func.scope = 0;
    testCommand("return a, b", &globalTable, &localTable, &func);
    testCommand("return 14, 5.4", &globalTable, &localTable, &func);

    testCommand("print(a)", &globalTable, &localTable, &func);

    deleteTable(&localTable);

    generatorClear();

    destroyString(&testString);
    deleteTable(&globalTable);

    return 0;
}
