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
    in_file = fopen("test.txt", "r");
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
            case 0 : printf("STRING : %s\n ",testToken.tokenName.data);
            break;
            case 1 : printf("ELSE : %s\n ",testToken.tokenName.data);
            break;
            case 2 : printf("FLOAT : %s\n ",testToken.tokenName.data);
            break;
            case 3 : printf("FOR : %s\n ",testToken.tokenName.data);
            break;
            case 4 : printf("FUNC : %s\n ",testToken.tokenName.data);
            break;
            case 5 : printf("IF : %s\n ",testToken.tokenName.data);
            break;
            case 6 : printf("INT : %s\n ",testToken.tokenName.data);
            break;
            case 7 : printf("PACKAGE : %s\n ",testToken.tokenName.data);
            break;
            case 8 : printf("RETURN : %s\n ",testToken.tokenName.data);
            break;
            case 9 : printf("VAR_ID : %s\n ",testToken.tokenName.data);
            break;
            case 10 : printf("BRACKET_CURLY : %s\n ",testToken.tokenName.data);
            break;
            case 11 : printf("BRACKET_ROUND : %s\n ",testToken.tokenName.data);
            break;
            case 12 : printf("EOL : %s\n ",testToken.tokenName.data);
            break;
            case 13 : printf("ARIT_OPERATOR : %s\n ",testToken.tokenName.data);
            break;
            case 14 : printf("ASIGN_OPERATOR : %s\n ",testToken.tokenName.data);
            break;
            case 15 : printf("COMP_OPERATOR : %s\n ",testToken.tokenName.data);
            break;
            case 16 : printf("INT_LIT : %s\n ",testToken.tokenName.data);
            break;
            case 17 : printf("STRING_LIT : %s\n ",testToken.tokenName.data);
            break;
            case 18 : printf("FLOAT_LIT : %s\n ",testToken.tokenName.data);
            break;

        }

    }




}
