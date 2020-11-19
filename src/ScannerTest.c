//
// Created by xdrobe01 on 5. 11. 2020.
//
#include "Scanner.h"
main(){
    string testString;
    string testString2;
    list testList;
    token testToken;
    int listSize;
    int end = 0;
    FILE *in_file;
    FILE *output;
    in_file = fopen("test.txt", "r");
    output = fopen("output.txt", "w");
    initList(&testList);
    initString(&testString);
    char line[200];
    do
        {

            fgets( line, 200, in_file );
            makeString(line,&testString);
            CodeAnalyzer(&testList,testString);

        }while(feof(in_file) == 0);




    listSize = testList.size-1;
    printf("\n");
    for(int i = 0; i<=listSize; i++){
        getToken(&testList,i,&testToken);
        switch (testToken.tokenType) {
            case 0 : fprintf(output,"STRING : %s\n ",testToken.tokenName.data);
            break;
            case 1 : fprintf(output,"ELSE : %s\n ",testToken.tokenName.data);
            break;
            case 2 : fprintf(output,"FLOAT : %s\n ",testToken.tokenName.data);
            break;
            case 3 : fprintf(output,"FOR : %s\n ",testToken.tokenName.data);
            break;
            case 4 : fprintf(output,"FUNC : %s\n ",testToken.tokenName.data);
            break;
            case 5 : fprintf(output,"IF : %s\n ",testToken.tokenName.data);
            break;
            case 6 : fprintf(output,"INT : %s\n ",testToken.tokenName.data);
            break;
            case 7 : fprintf(output,"PACKAGE : %s\n ",testToken.tokenName.data);
            break;
            case 8 : fprintf(output,"RETURN : %s\n ",testToken.tokenName.data);
            break;
            case 9 : fprintf(output,"VAR_ID : %s\n ",testToken.tokenName.data);
            break;
            case 10 : fprintf(output,"BRACKET_CURLY : %s\n ",testToken.tokenName.data);
            break;
            case 11 : fprintf(output,"BRACKET_ROUND : %s\n ",testToken.tokenName.data);
            break;
            case 12 : fprintf(output,"EOL : %s\n ",testToken.tokenName.data);
            break;
            case 13 : fprintf(output,"ARIT_OPERATOR : %s\n ",testToken.tokenName.data);
            break;
            case 14 : fprintf(output,"ASIGN_OPERATOR : %s\n ",testToken.tokenName.data);
            break;
            case 15 : fprintf(output,"COMP_OPERATOR : %s\n ",testToken.tokenName.data);
            break;
            case 16 : fprintf(output,"INT_LIT : %s\n ",testToken.tokenName.data);
            break;
            case 17 : fprintf(output,"STRING_LIT : %s\n ",testToken.tokenName.data);
            break;
            case 18 : fprintf(output,"FLOAT_LIT : %s\n ",testToken.tokenName.data);
            break;

        }

    }




}
