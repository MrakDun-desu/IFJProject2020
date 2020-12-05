//
// Created by xdanco00 on 12. 11. 2020.
//

#include "generator.h"

#define ADDCHAR(des_str,src_char) {if(addConstChar(des_str, src_char)) return INTERNAL_ERROR;}

bool equalStrings(char* s1, char* s2) {
    return (strcmp(s1, s2) == 0);
}

errorCode transformString(string* str){

    string tmp;
    initString(&tmp);
    char tmpStr[100];
    for(size_t i = 0; i < str->len; i++){
        char c = str->data[i];
        if(c < 33){
            addChar(&tmp, 92);
            addChar(&tmp, '0');
            if(c < 10){
                addChar(&tmp, '0');
            }
            sprintf(tmpStr,"%d", c);
            addConstChar(&tmp, tmpStr);
        }else if(c == 35){
            addChar(&tmp, 92);
            addChar(&tmp, '0');
            sprintf(tmpStr,"%d", c);
            addConstChar(&tmp, tmpStr);
        }else if(c == 92){
            addChar(&tmp, 92);
            addChar(&tmp, '0');
            sprintf(tmpStr,"%d", c);
            addConstChar(&tmp, tmpStr);
        }else{
            addChar(&tmp, c);
        }
    }
    makeString(tmp.data,str);
    destroyString(&tmp);
    return OK;
}

///VOID FUNCTIONS

void generatorInit(){
    initString(gen.program);
    gen.forCounter = 0;
    gen.ifCounter = 0;
}

void generatorClear(){
    destroyString(gen.program);
}

///BASIC CODE

errorCode generatorStart(){
    char str[100];
    sprintf(str, ".IFJcode20\n");
    ADDCHAR(gen.program, str);

    for(size_t i = 0; i < 10; i++){
        sprintf(str, "DEFVAR GF@expVar%d\n", i);
        ADDCHAR(gen.program, str);
    }

    ADDCHAR(gen.program, FUNC_INPUTS);
    ADDCHAR(gen.program, FUNC_INPUTI);
    ADDCHAR(gen.program, FUNC_INPUTF);

    ADDCHAR(gen.program, FUNC_INT2FLOAT);
    ADDCHAR(gen.program, FUNC_FLOAT2INT);

    ADDCHAR(gen.program, FUNC_LEN);

    ADDCHAR(gen.program, FUNC_SUBSTR);
    ADDCHAR(gen.program, FUNC_ORD);
    ADDCHAR(gen.program, FUNC_CHR);

    sprintf(str, "JUMP main\n");
    ADDCHAR(gen.program, str);
    return OK;
}

errorCode generateDefvar(token* var){
    char str[100];
    sprintf(str, "DEFVAR TF@%s\n", var->tokenName.data);
    ADDCHAR(gen.program, str);
    return OK;
}

errorCode generatorMove(token* var, token* value){

    char str[100];

    switch(var->tokenType){
        case INT_LIT:
            sprintf(str,"MOVE TF@%s int@%s\n", var->tokenName.data, value->tokenName.data);
            break;
        case STRING_LIT:
            string tmp;
            initString(&tmp);
            makeString(value->tokenName.data,&tmp);
            transformString(&tmp);
            sprintf(str,"MOVE TF@%s string@%s\n", var->tokenName.data, tmp.data);
            destroyString(&tmp);
            break;
        case FLOAT_LIT:
            sprintf(str,"MOVE TF@%s float@%s\n", var->tokenName.data, value->tokenName.data);
            break;
        case IDENT:
            sprintf(str,"MOVE TF@%s GF@%s\n", var->tokenName.data, value->tokenName.data);
            break;
        default:
            break;
    }

    ADDCHAR(gen.program,str);

    return OK;
}

///ARITHMETIC FUNCTIONS

errorCode generateArithmetic(token* var, token* symb1, token* symb2, char* frames, char operation){
    char str[100];
    switch(operation){
        case '+':
            if(symb1->tokenType == INT_LIT){
                if(symb2->tokenType == INT_LIT){
                    if(frames[0] == 't'){
                        sprintf(str, "ADD TF@%s int@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g'){
                        sprintf(str, "ADD GF@%s int@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == IDENT){
                    if(frames[0] == 't' && frames[2] == 't'){
                        sprintf(str, "ADD TF@%s int@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 't'){
                        sprintf(str, "ADD GF@%s int@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[2] == 'g'){
                        sprintf(str, "ADD TF@%s int@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 'g'){
                        sprintf(str, "ADD GF@%s int@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
            }

            if(symb1->tokenType == FLOAT_LIT){
                if(symb2->tokenType == FLOAT_LIT){
                    if(frames[0] == 't'){
                        sprintf(str, "ADD TF@%s float@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g'){
                        sprintf(str, "ADD GF@%s float@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == IDENT){
                    if(frames[0] == 't' && frames[2] == 't'){
                        sprintf(str, "ADD TF@%s float@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 't'){
                        sprintf(str, "ADD GF@%s float@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[2] == 'g'){
                        sprintf(str, "ADD TF@%s float@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 'g'){
                        sprintf(str, "ADD GF@%s float@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
            }

            if(symb1->tokenType == IDENT){
                if(symb2->tokenType == IDENT){
                    if(equalStrings("ttt", frames)){
                        sprintf(str, "ADD TF@%s TF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("ttg", frames)){
                        sprintf(str, "ADD TF@%s TF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("tgt", frames)){
                        sprintf(str, "ADD TF@%s GF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("tgg", frames)){
                        sprintf(str, "ADD TF@%s GF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("gtt", frames)){
                        sprintf(str, "ADD GF@%s TF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("gtg", frames)){
                        sprintf(str, "ADD GF@%s TF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("ggg", frames)){
                        sprintf(str, "ADD GF@%s GF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == INT_LIT){

                    if(frames[0] == 't' && frames[1] == 't'){
                        sprintf(str, "ADD TF@%s TF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[1] == 'g'){
                        sprintf(str, "ADD TF@%s GF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 't'){
                        sprintf(str, "ADD GF@%s TF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 'g'){
                        sprintf(str, "ADD GF@%s GF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == FLOAT_LIT){
                    if(frames[0] == 't' && frames[1] == 't'){
                        sprintf(str, "ADD TF@%s TF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[1] == 'g'){
                        sprintf(str, "ADD TF@%s GF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 't'){
                        sprintf(str, "ADD GF@%s TF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 'g'){
                        sprintf(str, "ADD GF@%s GF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
            }
            break;
        case '-':
            if(symb1->tokenType == INT_LIT){
                if(symb2->tokenType == INT_LIT){
                    if(frames[0] == 't'){
                        sprintf(str, "SUB TF@%s int@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g'){
                        sprintf(str, "SUB GF@%s int@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == IDENT){
                    if(frames[0] == 't' && frames[2] == 't'){
                        sprintf(str, "SUB TF@%s int@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 't'){
                        sprintf(str, "SUB GF@%s int@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[2] == 'g'){
                        sprintf(str, "SUB TF@%s int@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 'g'){
                        sprintf(str, "SUB GF@%s int@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
            }

            if(symb1->tokenType == FLOAT_LIT){
                if(symb2->tokenType == FLOAT_LIT){
                    if(frames[0] == 't'){
                        sprintf(str, "SUB TF@%s float@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g'){
                        sprintf(str, "SUB GF@%s float@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == IDENT){
                    if(frames[0] == 't' && frames[2] == 't'){
                        sprintf(str, "SUB TF@%s float@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 't'){
                        sprintf(str, "SUB GF@%s float@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[2] == 'g'){
                        sprintf(str, "SUB TF@%s float@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 'g'){
                        sprintf(str, "SUB GF@%s float@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
            }

            if(symb1->tokenType == IDENT){
                if(symb2->tokenType == IDENT){
                    if(equalStrings("ttt", frames)){
                        sprintf(str, "SUB TF@%s TF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("ttg", frames)){
                        sprintf(str, "SUB TF@%s TF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("tgt", frames)){
                        sprintf(str, "SUB TF@%s GF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("tgg", frames)){
                        sprintf(str, "SUB TF@%s GF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("gtt", frames)){
                        sprintf(str, "SUB GF@%s TF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("gtg", frames)){
                        sprintf(str, "SUB GF@%s TF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("ggg", frames)){
                        sprintf(str, "SUB GF@%s GF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == INT_LIT){

                    if(frames[0] == 't' && frames[1] == 't'){
                        sprintf(str, "SUB TF@%s TF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[1] == 'g'){
                        sprintf(str, "SUB TF@%s GF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 't'){
                        sprintf(str, "SUB GF@%s TF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 'g'){
                        sprintf(str, "SUB GF@%s GF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == FLOAT_LIT){
                    if(frames[0] == 't' && frames[1] == 't'){
                        sprintf(str, "SUB TF@%s TF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[1] == 'g'){
                        sprintf(str, "SUB TF@%s GF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 't'){
                        sprintf(str, "SUB GF@%s TF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 'g'){
                        sprintf(str, "SUB GF@%s GF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
            }
            break;
        case '*':
            if(symb1->tokenType == INT_LIT){
                if(symb2->tokenType == INT_LIT){
                    if(frames[0] == 't'){
                        sprintf(str, "MUL TF@%s int@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g'){
                        sprintf(str, "MUL GF@%s int@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == IDENT){
                    if(frames[0] == 't' && frames[2] == 't'){
                        sprintf(str, "MUL TF@%s int@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 't'){
                        sprintf(str, "MUL GF@%s int@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[2] == 'g'){
                        sprintf(str, "MUL TF@%s int@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 'g'){
                        sprintf(str, "MUL GF@%s int@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
            }

            if(symb1->tokenType == FLOAT_LIT){
                if(symb2->tokenType == FLOAT_LIT){
                    if(frames[0] == 't'){
                        sprintf(str, "MUL TF@%s float@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g'){
                        sprintf(str, "MUL GF@%s float@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == IDENT){
                    if(frames[0] == 't' && frames[2] == 't'){
                        sprintf(str, "MUL TF@%s float@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 't'){
                        sprintf(str, "MUL GF@%s float@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[2] == 'g'){
                        sprintf(str, "MUL TF@%s float@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 'g'){
                        sprintf(str, "MUL GF@%s float@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
            }

            if(symb1->tokenType == IDENT){
                if(symb2->tokenType == IDENT){
                    if(equalStrings("ttt", frames)){
                        sprintf(str, "MUL TF@%s TF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("ttg", frames)){
                        sprintf(str, "MUL TF@%s TF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("tgt", frames)){
                        sprintf(str, "MUL TF@%s GF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("tgg", frames)){
                        sprintf(str, "MUL TF@%s GF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("gtt", frames)){
                        sprintf(str, "MUL GF@%s TF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("gtg", frames)){
                        sprintf(str, "MUL GF@%s TF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("ggg", frames)){
                        sprintf(str, "MUL GF@%s GF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == INT_LIT){

                    if(frames[0] == 't' && frames[1] == 't'){
                        sprintf(str, "MUL TF@%s TF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[1] == 'g'){
                        sprintf(str, "MUL TF@%s GF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 't'){
                        sprintf(str, "MUL GF@%s TF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 'g'){
                        sprintf(str, "MUL GF@%s GF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == FLOAT_LIT){
                    if(frames[0] == 't' && frames[1] == 't'){
                        sprintf(str, "MUL TF@%s TF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[1] == 'g'){
                        sprintf(str, "MUL TF@%s GF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 't'){
                        sprintf(str, "MUL GF@%s TF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 'g'){
                        sprintf(str, "MUL GF@%s GF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
            }
            break;
        case '/':
            if(symb1->tokenType == INT_LIT){
                if(symb2->tokenType == INT_LIT){
                    if(frames[0] == 't'){
                        sprintf(str, "IDIV TF@%s int@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g'){
                        sprintf(str, "IDIV GF@%s int@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == IDENT){
                    if(frames[0] == 't' && frames[2] == 't'){
                        sprintf(str, "IDIV TF@%s int@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 't'){
                        sprintf(str, "IDIV GF@%s int@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[2] == 'g'){
                        sprintf(str, "IDIV TF@%s int@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 'g'){
                        sprintf(str, "IDIV GF@%s int@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
            }

            if(symb1->tokenType == IDENT){
                if(symb2->tokenType == IDENT){
                    if(equalStrings("ttt", frames)){
                        sprintf(str, "IDIV TF@%s TF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("ttg", frames)){
                        sprintf(str, "IDIV TF@%s TF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("tgt", frames)){
                        sprintf(str, "IDIV TF@%s GF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("tgg", frames)){
                        sprintf(str, "IDIV TF@%s GF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("gtt", frames)){
                        sprintf(str, "IDIV GF@%s TF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("gtg", frames)){
                        sprintf(str, "IDIV GF@%s TF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("ggg", frames)){
                        sprintf(str, "IDIV GF@%s GF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == INT_LIT){

                    if(frames[0] == 't' && frames[1] == 't'){
                        sprintf(str, "IDIV TF@%s TF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[1] == 'g'){
                        sprintf(str, "IDIV TF@%s GF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 't'){
                        sprintf(str, "IDIV GF@%s TF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 'g'){
                        sprintf(str, "IDIV GF@%s GF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
            }
            break;
        case '%':
            if(symb1->tokenType == FLOAT_LIT){
                if(symb2->tokenType == FLOAT_LIT){
                    if(frames[0] == 't'){
                        sprintf(str, "DIV TF@%s float@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g'){
                        sprintf(str, "DIV GF@%s float@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == IDENT){
                    if(frames[0] == 't' && frames[2] == 't'){
                        sprintf(str, "DIV TF@%s float@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 't'){
                        sprintf(str, "DIV GF@%s float@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[2] == 'g'){
                        sprintf(str, "DIV TF@%s float@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 'g'){
                        sprintf(str, "DIV GF@%s float@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
            }

            if(symb1->tokenType == IDENT){
                if(symb2->tokenType == IDENT){
                    if(equalStrings("ttt", frames)){
                        sprintf(str, "DIV TF@%s TF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("ttg", frames)){
                        sprintf(str, "DIV TF@%s TF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("tgt", frames)){
                        sprintf(str, "DIV TF@%s GF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("tgg", frames)){
                        sprintf(str, "DIV TF@%s GF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("gtt", frames)){
                        sprintf(str, "DIV GF@%s TF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("gtg", frames)){
                        sprintf(str, "DIV GF@%s TF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("ggg", frames)){
                        sprintf(str, "DIV GF@%s GF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == FLOAT_LIT){

                    if(frames[0] == 't' && frames[1] == 't'){
                        sprintf(str, "DIV TF@%s TF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[1] == 'g'){
                        sprintf(str, "DIV TF@%s GF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 't'){
                        sprintf(str, "DIV GF@%s TF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 'g'){
                        sprintf(str, "DIV GF@%s GF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
            }
            break;
        default:
            break;

    }

    ADDCHAR(gen.program,str);

    return OK;

}

///COMPARING FUNCTIONS

errorCode generateLT(token* var, token* symb1, token* symb2, char* frames){


    char str[100];
    if(symb1->tokenType == INT_LIT){
        if(symb2->tokenType == INT_LIT){
            sprintf(str,"LT GF@%s int@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
        }
        if(symb2->tokenType == IDENT){
            if(frames[1] == 't' ){
                sprintf(str,"LT GF@%s int@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[1] == 'g' ){
                sprintf(str,"LT GF@%s int@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }
    }

    if(symb1->tokenType == FLOAT_LIT){
        if(symb2->tokenType == FLOAT_LIT){
            sprintf(str,"LT GF@%s float@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
        }
        if(symb2->tokenType == IDENT){
            if(frames[1] == 't'){
                sprintf(str,"LT GF@%s float@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[1] == 'g'){
                sprintf(str,"LT GF@%s float@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }
    }

    if(symb1->tokenType == STRING_LIT){

        string tmp1;
        initString(&tmp1);
        makeString(symb1->tokenName.data, &tmp1);
        transformString(&tmp1);

        if(symb2->tokenType == STRING_LIT){

            string tmp2;
            initString(&tmp2);
            makeString(symb2->tokenName.data, &tmp2);
            transformString(&tmp2);

            sprintf(str,"LT GF@%s string@%s string@%s\n",var->tokenName.data, tmp1.data, tmp2.data);

            destroyString(&tmp2);
        }
        if(symb2->tokenType == IDENT){
            if(frames[1] == 't'){
                sprintf(str,"LT GF@%s string@%s TF@%s\n",var->tokenName.data, tmp1.data, symb2->tokenName.data);
            }
            if(frames[1] == 'g'){
                sprintf(str,"LT GF@%s string@%s GF@%s\n",var->tokenName.data, tmp1.data, symb2->tokenName.data);
            }
        }
        destroyString(&tmp1);
    }

    if(symb1->tokenType == IDENT){
        if(symb2->tokenType == FLOAT_LIT){
            if(frames[0] == 't'){
                sprintf(str,"LT GF@%s TF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[0] == 'g'){
                sprintf(str,"LT GF@%s GF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }

        if(symb2->tokenType ==  STRING_LIT){
            string tmp;
            initString(&tmp);
            makeString(symb2->tokenName.data,&tmp);
            transformString(&tmp);

            if(frames[0] == 't'){
                sprintf(str,"LT GF@%s TF@%s string@%s\n",var->tokenName.data, symb1->tokenName.data, tmp.data);
            }
            if(frames[0] == 'g'){
                sprintf(str,"LT GF@%s GF@%s string@%s\n",var->tokenName.data, symb1->tokenName.data, tmp.data);
            }
            destroyString(&tmp);
        }

        if(symb2->tokenType == INT_LIT){
            if(frames[0] == 't'){
                sprintf(str,"LT GF@%s TF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[0] == 'g'){
                sprintf(str,"LT GF@%s GF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }

        if(symb2->tokenType == IDENT){
            if(equalStrings("tt", frames)){
                sprintf(str,"LT GF@%s TF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(equalStrings("tg", frames)){
                sprintf(str,"LT GF@%s TF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(equalStrings("gt", frames)){
                sprintf(str,"LT GF@%s GF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(equalStrings("gg", frames)){
                sprintf(str,"LT GF@%s GF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }
    }
    ADDCHAR(gen.program,str);
    return OK;
}

errorCode generateGT(token* var, token* symb1, token* symb2, char* frames){

    char str[100];
    if(symb1->tokenType == INT_LIT){
        if(symb2->tokenType == INT_LIT){
            sprintf(str,"GT GF@%s int@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
        }
        if(symb2->tokenType == IDENT){
            if(frames[1] == 't' ){
                sprintf(str,"GT GF@%s int@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[1] == 'g' ){
                sprintf(str,"GT GF@%s int@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }
    }

    if(symb1->tokenType == FLOAT_LIT){
        if(symb2->tokenType == FLOAT_LIT){
            sprintf(str,"GT GF@%s float@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
        }
        if(symb2->tokenType == IDENT){
            if(frames[1] == 't'){
                sprintf(str,"GT GF@%s float@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[1] == 'g'){
                sprintf(str,"GT GF@%s float@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }
    }

    if(symb1->tokenType == STRING_LIT){

        string tmp1;
        initString(&tmp1);
        makeString(symb1->tokenName.data, &tmp1);
        transformString(&tmp1);

        if(symb2->tokenType == STRING_LIT){

            string tmp2;
            initString(&tmp2);
            makeString(symb2->tokenName.data, &tmp2);
            transformString(&tmp2);

            sprintf(str,"GT GF@%s string@%s string@%s\n",var->tokenName.data, tmp1.data, tmp2.data);

            destroyString(&tmp2);
        }
        if(symb2->tokenType == IDENT){
            if(frames[1] == 't'){
                sprintf(str,"GT GF@%s string@%s TF@%s\n",var->tokenName.data, tmp1.data, symb2->tokenName.data);
            }
            if(frames[1] == 'g'){
                sprintf(str,"GT GF@%s string@%s GF@%s\n",var->tokenName.data, tmp1.data, symb2->tokenName.data);
            }
        }
        destroyString(&tmp1);
    }

    if(symb1->tokenType == IDENT){
        if(symb2->tokenType == FLOAT_LIT){
            if(frames[0] == 't'){
                sprintf(str,"GT GF@%s TF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[0] == 'g'){
                sprintf(str,"GT GF@%s GF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }

        if(symb2->tokenType ==  STRING_LIT){
            string tmp;
            initString(&tmp);
            makeString(symb2->tokenName.data,&tmp);
            transformString(&tmp);

            if(frames[0] == 't'){
                sprintf(str,"GT GF@%s TF@%s string@%s\n",var->tokenName.data, symb1->tokenName.data, tmp.data);
            }
            if(frames[0] == 'g'){
                sprintf(str,"GT GF@%s GF@%s string@%s\n",var->tokenName.data, symb1->tokenName.data, tmp.data);
            }
            destroyString(&tmp);
        }

        if(symb2->tokenType == INT_LIT){
            if(frames[0] == 't'){
                sprintf(str,"GT GF@%s TF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[0] == 'g'){
                sprintf(str,"GT GF@%s GF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }

        if(symb2->tokenType == IDENT){
            if(equalStrings("tt", frames)){
                sprintf(str,"GT GF@%s TF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(equalStrings("tg", frames)){
                sprintf(str,"GT GF@%s TF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(equalStrings("gt", frames)){
                sprintf(str,"GT GF@%s GF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(equalStrings("gg", frames)){
                sprintf(str,"GT GF@%s GF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }
    }
    ADDCHAR(gen.program,str);
    return OK;
}

errorCode generateEQ(token* var, token* symb1, token* symb2, char* frames){

    char str[100];
    if(symb1->tokenType == INT_LIT){
        if(symb2->tokenType == INT_LIT){
            sprintf(str,"EQ GF@%s int@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
        }
        if(symb2->tokenType == IDENT){
            if(frames[1] == 't' ){
                sprintf(str,"EQ GF@%s int@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[1] == 'g' ){
                sprintf(str,"EQ GF@%s int@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }
    }

    if(symb1->tokenType == FLOAT_LIT){
        if(symb2->tokenType == FLOAT_LIT){
            sprintf(str,"EQ GF@%s float@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
        }
        if(symb2->tokenType == IDENT){
            if(frames[1] == 't'){
                sprintf(str,"EQ GF@%s float@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[1] == 'g'){
                sprintf(str,"EQ GF@%s float@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }
    }

    if(symb1->tokenType == STRING_LIT){

        string tmp1;
        initString(&tmp1);
        makeString(symb1->tokenName.data, &tmp1);
        transformString(&tmp1);

        if(symb2->tokenType == STRING_LIT){

            string tmp2;
            initString(&tmp2);
            makeString(symb2->tokenName.data, &tmp2);
            transformString(&tmp2);

            sprintf(str,"EQ GF@%s string@%s string@%s\n",var->tokenName.data, tmp1.data, tmp2.data);

            destroyString(&tmp2);
        }
        if(symb2->tokenType == IDENT){
            if(frames[1] == 't'){
                sprintf(str,"EQ GF@%s string@%s TF@%s\n",var->tokenName.data, tmp1.data, symb2->tokenName.data);
            }
            if(frames[1] == 'g'){
                sprintf(str,"EQ GF@%s string@%s GF@%s\n",var->tokenName.data, tmp1.data, symb2->tokenName.data);
            }
        }
        destroyString(&tmp1);
    }

    if(symb1->tokenType == IDENT){
        if(symb2->tokenType == FLOAT_LIT){
            if(frames[0] == 't'){
                sprintf(str,"EQ GF@%s TF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[0] == 'g'){
                sprintf(str,"EQ GF@%s GF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }

        if(symb2->tokenType ==  STRING_LIT){
            string tmp;
            initString(&tmp);
            makeString(symb2->tokenName.data,&tmp);
            transformString(&tmp);

            if(frames[0] == 't'){
                sprintf(str,"EQ GF@%s TF@%s string@%s\n",var->tokenName.data, symb1->tokenName.data, tmp.data);
            }
            if(frames[0] == 'g'){
                sprintf(str,"EQ GF@%s GF@%s string@%s\n",var->tokenName.data, symb1->tokenName.data, tmp.data);
            }
            destroyString(&tmp);
        }

        if(symb2->tokenType == INT_LIT){
            if(frames[0] == 't'){
                sprintf(str,"EQ GF@%s TF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[0] == 'g'){
                sprintf(str,"EQ GF@%s GF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }

        if(symb2->tokenType == IDENT){
            if(equalStrings("tt", frames)){
                sprintf(str,"EQ GF@%s TF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(equalStrings("tg", frames)){
                sprintf(str,"EQ GF@%s TF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(equalStrings("gt", frames)){
                sprintf(str,"EQ GF@%s GF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(equalStrings("gg", frames)){
                sprintf(str,"EQ GF@%s GF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }
    }
    ADDCHAR(gen.program,str);
    return OK;
}

///LOGICAL FUNCTIONS

errorCode generateOR(token* var, token* symb1, token* symb2, char* frames){

    char str[100];
    if(symb1->tokenType == INT_LIT){
        if(symb2->tokenType == INT_LIT){
            sprintf(str,"OR GF@%s int@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
        }
        if(symb2->tokenType == IDENT){
            if(frames[1] == 't'){
                sprintf(str,"OR GF@%s int@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[1] == 'g'){
                sprintf(str,"OR GF@%s int@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }
    }

    if(symb1->tokenType == FLOAT_LIT){
        if(symb2->tokenType == FLOAT_LIT){
            sprintf(str,"OR GF@%s float@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
        }
        if(symb2->tokenType == IDENT){
            if(frames[1] == 't'){
                sprintf(str,"OR GF@%s float@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[1] == 'g'){
                sprintf(str,"OR GF@%s float@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }
    }

    if(symb1->tokenType == STRING_LIT){
        string tmp1;
        initString(&tmp1);
        makeString(symb1->tokenName.data,&tmp1);
        transformString(&tmp1);

        if(symb2->tokenType == STRING_LIT){
            string tmp2;
            initString(&tmp2);
            makeString(symb2->tokenName.data,&tmp2);
            transformString(&tmp2);
            sprintf(str,"OR GF@%s string@%s string@%s\n",var->tokenName.data, tmp1.data, tmp2.data);
            destroyString(&tmp2);
        }
        if(symb2->tokenType == IDENT){
            if(frames[1] == 't'){
                sprintf(str,"OR GF@%s string@%s TF@%s\n",var->tokenName.data, tmp1.data, symb2->tokenName.data);
            }
            if(frames[1] == 'g'){
                sprintf(str,"OR GF@%s string@%s GF@%s\n",var->tokenName.data, tmp1.data, symb2->tokenName.data);
            }
        }
        destroyString(&tmp1);
    }

    if(symb1->tokenType == IDENT){
        if(symb2->tokenType == STRING_LIT){
            string tmp;
            initString(&tmp);
            makeString(symb2->tokenName.data,&tmp);
            transformString(&tmp);
            if(frames[0] == 't'){
                sprintf(str,"OR GF@%s TF@%s string@%s\n",var->tokenName.data, symb1->tokenName.data, tmp.data);
            }
            if(frames[0] == 'g'){
                sprintf(str,"OR GF@%s GF@%s string@%s\n",var->tokenName.data, symb1->tokenName.data, tmp.data);
            }
            destroyString(&tmp);
        }

        if(symb2->tokenType == INT_LIT){
            if(frames[0] == 't'){
                sprintf(str,"OR GF@%s TF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[0] == 'g'){
                sprintf(str,"OR GF@%s GF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }

        if(symb2->tokenType == FLOAT_LIT){
            if(frames[0] == 't'){
                sprintf(str,"OR GF@%s TF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[0] == 'g'){
                sprintf(str,"OR GF@%s GF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }

        if(symb2->tokenType == IDENT){
            if(equalStrings("tt",frames)){
                sprintf(str,"OR GF@%s TF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(equalStrings("tg",frames)){
                sprintf(str,"OR GF@%s TF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(equalStrings("gt",frames)){
                sprintf(str,"OR GF@%s GF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(equalStrings("gg",frames)){
                sprintf(str,"OR GF@%s GF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }
    }

    ADDCHAR(gen.program,str);
    return OK;
}

errorCode generateNOT(token* var, token* symb){

    char str[100];
    if(symb->tokenType == INT_LIT){
        sprintf(str,"NOT GF@%s int@%s\n",var->tokenName.data, symb->tokenName.data);
    }
    if(symb->tokenType == FLOAT_LIT){
        sprintf(str,"NOT GF@%s float@%s\n",var->tokenName.data, symb->tokenName.data);
    }
    if(symb->tokenType == STRING_LIT){
        string tmp;
        initString(&tmp);
        makeString(symb->tokenName.data,&tmp);
        transformString(&tmp);
        sprintf(str,"NOT GF@%s string@%s\n",var->tokenName.data, tmp.data);
        destroyString(&tmp);
    }
    if(symb->tokenType == IDENT){
        sprintf(str,"NOT GF@%s GF@%s\n",var->tokenName.data, symb->tokenName.data);
    }

    ADDCHAR(gen.program,str);
    return OK;
}

///STRING FUNCTIONS

errorCode generateConcat(token* var, token* symb1, token* symb2, char* frames){

    char str[100];
    if(symb1->tokenType == STRING_LIT && symb2->tokenType == STRING_LIT){
        string tmp1;
        initString(&tmp1);
        makeString(symb1->tokenName.data,&tmp1);
        transformString(&tmp1);
        string tmp2;
        initString(&tmp2);
        makeString(symb2->tokenName.data,&tmp2);
        transformString(&tmp2);

        sprintf(str, "CONCAT GF%s string@%s string@%s\n",var->tokenName.data, tmp1.data, tmp2.data);

    }
    if(symb1->tokenType == IDENT && symb2->tokenType == IDENT){
        string tmp1;
        initString(&tmp1);
        makeString(symb1->tokenName.data,&tmp1);
        transformString(&tmp1);
        string tmp2;
        initString(&tmp2);
        makeString(symb2->tokenName.data,&tmp2);
        transformString(&tmp2);

        if(equalStrings("tt",frames)){
            sprintf(str, "CONCAT GF%s TF@%s TF@%s\n",var->tokenName.data, tmp1.data, tmp2.data);
        }
        if(equalStrings("tg",frames)){
            sprintf(str, "CONCAT GF%s TF@%s GF@%s\n",var->tokenName.data, tmp1.data, tmp2.data);
        }
        if(equalStrings("gt",frames)){
            sprintf(str, "CONCAT GF%s GF@%s TF@%s\n",var->tokenName.data, tmp1.data, tmp2.data);
        }
        if(equalStrings("gg",frames)){
            sprintf(str, "CONCAT GF%s GF@%s GF@%s\n",var->tokenName.data, tmp1.data, tmp2.data);
        }
        destroyString(&tmp1);
        destroyString(&tmp2);
    }
    if(symb1->tokenType == IDENT && symb2->tokenType == STRING_LIT){
        string tmp1;
        initString(&tmp1);
        makeString(symb1->tokenName.data,&tmp1);
        transformString(&tmp1);
        string tmp2;
        initString(&tmp2);
        makeString(symb2->tokenName.data,&tmp2);
        transformString(&tmp2);

        if(frames[0] == 't'){
            sprintf(str, "CONCAT GF%s TF@%s string@%s\n",var->tokenName.data, tmp1.data, tmp2.data);
        }
        if(frames[0] == 'g'){
            sprintf(str, "CONCAT GF%s GF@%s string@%s\n",var->tokenName.data, tmp1.data, tmp2.data);
        }
        destroyString(&tmp1);
        destroyString(&tmp2);
    }
    if(symb1->tokenType == STRING_LIT && symb2->tokenType == IDENT){
        string tmp1;
        initString(&tmp1);
        makeString(symb1->tokenName.data,&tmp1);
        transformString(&tmp1);
        string tmp2;
        initString(&tmp2);
        makeString(symb2->tokenName.data,&tmp2);
        transformString(&tmp2);

        if(frames[1] == 't'){
            sprintf(str, "CONCAT GF%s string@%s TF@%s\n",var->tokenName.data, tmp1.data, tmp2.data);
        }
        if(frames[1] == 'g'){
            sprintf(str, "CONCAT GF%s string@%s GF@%s\n",var->tokenName.data, tmp1.data, tmp2.data);
        }
        destroyString(&tmp1);
        destroyString(&tmp2);
    }

    ADDCHAR(gen.program,str);
    return OK;
}

///FUNCTION FUNCTIONS

errorCode generateFunctionStart(data* function){
    char str[100];

    if(function == NULL){
        sprintf(str, "LABEL main\nCREATEFRAME\n");
        ADDCHAR(gen.program, str);
        return OK;

    }else{
        sprintf(str, "LABEL %s\n", function->id.data);
        ADDCHAR(gen.program, str);
        sprintf(str, "CREATEFRAME\n");
        ADDCHAR(gen.program, str);
        for(size_t i = 0; i < function->parameters->size; i++){
            token tmp;
            getToken(function->parameters, i, &tmp);
            sprintf(str, "DEFVAR TF@%s\n", tmp.tokenName.data);
            ADDCHAR(gen.program, str);
            sprintf(str, "POPS TF@%s\n", tmp.tokenName.data);
            ADDCHAR(gen.program, str);
        }
        size_t  i = 0;
        dataType tmpType;
        tmpType = function->types[i];

        while(tmpType != TYPE_UNDEFINED){
            sprintf(str, "DEFVAR TF@ret%d\n", i);
            ADDCHAR(gen.program, str);
            i++;
            tmpType = function->types[i];
        }
    }

    return OK;
}

errorCode generateFunctionCall(data* function, list* argValues){
    char str[100];

    for(size_t i = 0; i < argValues->size; i++){
        token tmp;
        getToken(argValues, i, &tmp);
        sprintf(str, "PUSHS TF@%s\n", tmp.tokenName.data);
        ADDCHAR(gen.program, str);
    }
    sprintf(str, "PUSHFRAME\n");
    ADDCHAR(gen.program, str);

    sprintf(str, "CALL %s\n",function->id.data);
    ADDCHAR(gen.program, str);

    return OK;
}

errorCode generateFunctionReturn(data* function, list* assignVariables){
    char str[100];

    for(size_t i = 0; i < assignVariables->size; i++){
        token tmp;
        getToken(function->parameters, i, &tmp);
        sprintf(str, "MOVE LF@%s TF@ret%d\n", tmp.tokenName.data, i);
        ADDCHAR(gen.program, str);
    }
    sprintf(str, "POPFRAME\n");
    ADDCHAR(gen.program, str);

    return OK;
}

errorCode generateFunctionEnd(){
    char str[100];
    sprintf(str, "RETURN\n");
    ADDCHAR(gen.program, str);
    return OK;
}

///OUTPUT FUNCTIONS

errorCode generatorWrite(FILE* dest){
    fprintf(dest, "%s", gen.program->data);
    return OK;
}

///MAIN FUNCTIONS

errorCode generateMainScopeEnd(){
    ADDCHAR(gen.program, "EXIT int@0\n");
    return OK;
}

///IF FUNCTIONS

errorCode generateIfStart(list* condition, tableNodePtr varTable, size_t ifCount){
    char str[100];
    token out;

    applyPrecedence(condition, varTable, &out);

    sprintf(str, "MOVE TF@bool%d GF@%s\n", ifCount, out.tokenName.data);
    ADDCHAR(gen.program, str);

    sprintf(str, "JUMPIFNEQ else%d TF@bool%d bool@true\n", ifCount, ifCount);
    ADDCHAR(gen.program, str);

    return OK;
}

errorCode generateElse(size_t ifCount){
    char str[100];
    sprintf(str,"JUMP ifEnd%d\n", ifCount);
    ADDCHAR(gen.program, str);
    sprintf(str,"LABEL else%d\n", ifCount);
    ADDCHAR(gen.program, str);
    return OK;
}

errorCode generateIfEnd(size_t ifCount){
    char str[100];
    sprintf(str, "LABEL ifEnd%d\n", ifCount);
    ADDCHAR(gen.program, str);
    return OK;
}

///FOR FUNCTIONS

errorCode generateForPrequel(size_t forCount){
    char str[100];
    sprintf(str,"JUMP forFirst%d\n", forCount);
    ADDCHAR(gen.program, str);
    sprintf(str,"LABEL forStart%d\n", forCount);
    ADDCHAR(gen.program, str);
    return OK;
}

errorCode generateForStart(list* condition, tableNodePtr varTable, size_t forCount){
    char str[100];
    sprintf(str,"LABEL forFirst%d\n", forCount);
    ADDCHAR(gen.program, str);

    token out;
    applyPrecedence(condition, varTable, &out);

    sprintf(str,"MOVE TF@bool%d GF@%s\n", forCount, out.tokenName.data);
    ADDCHAR(gen.program, str);

    sprintf(str,"JUMPIFNEQ forEnd%d TF@bool%d bool@true\n", forCount, forCount);
    ADDCHAR(gen.program, str);

    return OK;
}

errorCode generateForEndLabel(size_t forCount){
    char str[100];
    sprintf(str, "LABEL forEnd%d\n", forCount);
    ADDCHAR(gen.program, str);
    return OK;
}
