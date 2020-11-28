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

    for(size_t i = 0; i < str->len; i++){
        char c = str->data[i];
        if(c == ' '){
            c = 92;
            addChar(&tmp,c);
            c = '0';
            addChar(&tmp,c);
            c = '3';
            addChar(&tmp,c);
            c = '2';
            addChar(&tmp,c);
        }else{
            if(c == '\n'){
                c = 92;
                addChar(&tmp,c);
                c = '0';
                addChar(&tmp,c);
                c = '1';
                addChar(&tmp,c);
                c = '0';
                addChar(&tmp,c);
            }else{
                addChar(&tmp,c);
            }
        }
    }
    makeString(tmp.data,str);
    destroyString(&tmp);
}

void generatorInit(){
    initString(gen.program);
    gen.forCounter = 0;
    gen.ifCounter = 0;
}

void generatorClear(){
    destroyString(gen.program);
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

errorCode generateArithmetic(token* var, token* symb1, token* symb2, char* frames, char operation){
    char str[30];
    switch(operation){
        case '+':
            if(symb1->tokenType == INT_LIT){
                if(symb2->tokenType == INT_LIT){
                    if(frames[0] == 't'){
                        sprintf(str, "ADD TF@%s int@%s int@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g'){
                        sprintf(str, "ADD GF@%s int@%s int@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == IDENT){
                    if(frames[0] == 't' && frames[2] == 't'){
                        sprintf(str, "ADD TF@%s int@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 't'){
                        sprintf(str, "ADD GF@%s int@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[2] == 'g'){
                        sprintf(str, "ADD TF@%s int@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 'g'){
                        sprintf(str, "ADD GF@%s int@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
            }

            if(symb1->tokenType == FLOAT_LIT){
                if(symb2->tokenType == FLOAT_LIT){
                    if(frames[0] == 't'){
                        sprintf(str, "ADD TF@%s float@%s float@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g'){
                        sprintf(str, "ADD GF@%s float@%s float@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == IDENT){
                    if(frames[0] == 't' && frames[2] == 't'){
                        sprintf(str, "ADD TF@%s float@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 't'){
                        sprintf(str, "ADD GF@%s float@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[2] == 'g'){
                        sprintf(str, "ADD TF@%s float@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 'g'){
                        sprintf(str, "ADD GF@%s float@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
            }

            if(symb1->tokenType == IDENT){
                if(symb2->tokenType == IDENT){
                    if(equalStrings("ttt", frames)){
                        sprintf(str, "ADD TF@%s TF@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("ttg", frames)){
                        sprintf(str, "ADD TF@%s TF@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("tgt", frames)){
                        sprintf(str, "ADD TF@%s GF@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("tgg", frames)){
                        sprintf(str, "ADD TF@%s GF@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("gtt", frames)){
                        sprintf(str, "ADD GF@%s TF@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("gtg", frames)){
                        sprintf(str, "ADD GF@%s TF@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("ggg", frames)){
                        sprintf(str, "ADD GF@%s GF@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == INT_LIT){

                    if(frames[0] == 't' && frames[1] == 't'){
                        sprintf(str, "ADD TF@%s TF@%s int@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[1] == 'g'){
                        sprintf(str, "ADD TF@%s GF@%s int@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 't'){
                        sprintf(str, "ADD GF@%s TF@%s int@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 'g'){
                        sprintf(str, "ADD GF@%s GF@%s int@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == FLOAT_LIT){
                    if(frames[0] == 't' && frames[1] == 't'){
                        sprintf(str, "ADD TF@%s TF@%s float@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[1] == 'g'){
                        sprintf(str, "ADD TF@%s GF@%s float@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 't'){
                        sprintf(str, "ADD GF@%s TF@%s float@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 'g'){
                        sprintf(str, "ADD GF@%s GF@%s float@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
            }
            break;
        case '-':
            if(symb1->tokenType == INT_LIT){
                if(symb2->tokenType == INT_LIT){
                    if(frames[0] == 't'){
                        sprintf(str, "SUB TF@%s int@%s int@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g'){
                        sprintf(str, "SUB GF@%s int@%s int@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == IDENT){
                    if(frames[0] == 't' && frames[2] == 't'){
                        sprintf(str, "SUB TF@%s int@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 't'){
                        sprintf(str, "SUB GF@%s int@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[2] == 'g'){
                        sprintf(str, "SUB TF@%s int@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 'g'){
                        sprintf(str, "SUB GF@%s int@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
            }

            if(symb1->tokenType == FLOAT_LIT){
                if(symb2->tokenType == FLOAT_LIT){
                    if(frames[0] == 't'){
                        sprintf(str, "SUB TF@%s float@%s float@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g'){
                        sprintf(str, "SUB GF@%s float@%s float@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == IDENT){
                    if(frames[0] == 't' && frames[2] == 't'){
                        sprintf(str, "SUB TF@%s float@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 't'){
                        sprintf(str, "SUB GF@%s float@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[2] == 'g'){
                        sprintf(str, "SUB TF@%s float@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 'g'){
                        sprintf(str, "SUB GF@%s float@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
            }

            if(symb1->tokenType == IDENT){
                if(symb2->tokenType == IDENT){
                    if(equalStrings("ttt", frames)){
                        sprintf(str, "SUB TF@%s TF@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("ttg", frames)){
                        sprintf(str, "SUB TF@%s TF@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("tgt", frames)){
                        sprintf(str, "SUB TF@%s GF@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("tgg", frames)){
                        sprintf(str, "SUB TF@%s GF@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("gtt", frames)){
                        sprintf(str, "SUB GF@%s TF@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("gtg", frames)){
                        sprintf(str, "SUB GF@%s TF@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("ggg", frames)){
                        sprintf(str, "SUB GF@%s GF@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == INT_LIT){

                    if(frames[0] == 't' && frames[1] == 't'){
                        sprintf(str, "SUB TF@%s TF@%s int@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[1] == 'g'){
                        sprintf(str, "SUB TF@%s GF@%s int@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 't'){
                        sprintf(str, "SUB GF@%s TF@%s int@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 'g'){
                        sprintf(str, "SUB GF@%s GF@%s int@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == FLOAT_LIT){
                    if(frames[0] == 't' && frames[1] == 't'){
                        sprintf(str, "SUB TF@%s TF@%s float@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[1] == 'g'){
                        sprintf(str, "SUB TF@%s GF@%s float@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 't'){
                        sprintf(str, "SUB GF@%s TF@%s float@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 'g'){
                        sprintf(str, "SUB GF@%s GF@%s float@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
            }
            break;
        case '*':
            if(symb1->tokenType == INT_LIT){
                if(symb2->tokenType == INT_LIT){
                    if(frames[0] == 't'){
                        sprintf(str, "MUL TF@%s int@%s int@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g'){
                        sprintf(str, "MUL GF@%s int@%s int@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == IDENT){
                    if(frames[0] == 't' && frames[2] == 't'){
                        sprintf(str, "MUL TF@%s int@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 't'){
                        sprintf(str, "MUL GF@%s int@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[2] == 'g'){
                        sprintf(str, "MUL TF@%s int@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 'g'){
                        sprintf(str, "MUL GF@%s int@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
            }

            if(symb1->tokenType == FLOAT_LIT){
                if(symb2->tokenType == FLOAT_LIT){
                    if(frames[0] == 't'){
                        sprintf(str, "MUL TF@%s float@%s float@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g'){
                        sprintf(str, "MUL GF@%s float@%s float@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == IDENT){
                    if(frames[0] == 't' && frames[2] == 't'){
                        sprintf(str, "MUL TF@%s float@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 't'){
                        sprintf(str, "MUL GF@%s float@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[2] == 'g'){
                        sprintf(str, "MUL TF@%s float@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 'g'){
                        sprintf(str, "MUL GF@%s float@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
            }

            if(symb1->tokenType == IDENT){
                if(symb2->tokenType == IDENT){
                    if(equalStrings("ttt", frames)){
                        sprintf(str, "MUL TF@%s TF@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("ttg", frames)){
                        sprintf(str, "MUL TF@%s TF@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("tgt", frames)){
                        sprintf(str, "MUL TF@%s GF@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("tgg", frames)){
                        sprintf(str, "MUL TF@%s GF@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("gtt", frames)){
                        sprintf(str, "MUL GF@%s TF@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("gtg", frames)){
                        sprintf(str, "MUL GF@%s TF@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("ggg", frames)){
                        sprintf(str, "MUL GF@%s GF@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == INT_LIT){

                    if(frames[0] == 't' && frames[1] == 't'){
                        sprintf(str, "MUL TF@%s TF@%s int@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[1] == 'g'){
                        sprintf(str, "MUL TF@%s GF@%s int@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 't'){
                        sprintf(str, "MUL GF@%s TF@%s int@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 'g'){
                        sprintf(str, "MUL GF@%s GF@%s int@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == FLOAT_LIT){
                    if(frames[0] == 't' && frames[1] == 't'){
                        sprintf(str, "MUL TF@%s TF@%s float@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[1] == 'g'){
                        sprintf(str, "MUL TF@%s GF@%s float@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 't'){
                        sprintf(str, "MUL GF@%s TF@%s float@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 'g'){
                        sprintf(str, "MUL GF@%s GF@%s float@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
            }
            break;
        case '/':
            if(symb1->tokenType == INT_LIT){
                if(symb2->tokenType == INT_LIT){
                    if(frames[0] == 't'){
                        sprintf(str, "IDIV TF@%s int@%s int@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g'){
                        sprintf(str, "IDIV GF@%s int@%s int@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == IDENT){
                    if(frames[0] == 't' && frames[2] == 't'){
                        sprintf(str, "IDIV TF@%s int@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 't'){
                        sprintf(str, "IDIV GF@%s int@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[2] == 'g'){
                        sprintf(str, "IDIV TF@%s int@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 'g'){
                        sprintf(str, "IDIV GF@%s int@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
            }

            if(symb1->tokenType == IDENT){
                if(symb2->tokenType == IDENT){
                    if(equalStrings("ttt", frames)){
                        sprintf(str, "IDIV TF@%s TF@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("ttg", frames)){
                        sprintf(str, "IDIV TF@%s TF@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("tgt", frames)){
                        sprintf(str, "IDIV TF@%s GF@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("tgg", frames)){
                        sprintf(str, "IDIV TF@%s GF@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("gtt", frames)){
                        sprintf(str, "IDIV GF@%s TF@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("gtg", frames)){
                        sprintf(str, "IDIV GF@%s TF@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("ggg", frames)){
                        sprintf(str, "IDIV GF@%s GF@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == INT_LIT){

                    if(frames[0] == 't' && frames[1] == 't'){
                        sprintf(str, "IDIV TF@%s TF@%s int@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[1] == 'g'){
                        sprintf(str, "IDIV TF@%s GF@%s int@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 't'){
                        sprintf(str, "IDIV GF@%s TF@%s int@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 'g'){
                        sprintf(str, "IDIV GF@%s GF@%s int@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
            }
            break;
        case '%':
            if(symb1->tokenType == FLOAT_LIT){
                if(symb2->tokenType == FLOAT_LIT){
                    if(frames[0] == 't'){
                        sprintf(str, "DIV TF@%s float@%s float@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g'){
                        sprintf(str, "DIV GF@%s float@%s float@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == IDENT){
                    if(frames[0] == 't' && frames[2] == 't'){
                        sprintf(str, "DIV TF@%s float@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 't'){
                        sprintf(str, "DIV GF@%s float@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[2] == 'g'){
                        sprintf(str, "DIV TF@%s float@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[2] == 'g'){
                        sprintf(str, "DIV GF@%s float@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
            }

            if(symb1->tokenType == IDENT){
                if(symb2->tokenType == IDENT){
                    if(equalStrings("ttt", frames)){
                        sprintf(str, "DIV TF@%s TF@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("ttg", frames)){
                        sprintf(str, "DIV TF@%s TF@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("tgt", frames)){
                        sprintf(str, "DIV TF@%s GF@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("tgg", frames)){
                        sprintf(str, "DIV TF@%s GF@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("gtt", frames)){
                        sprintf(str, "DIV GF@%s TF@%s TF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("gtg", frames)){
                        sprintf(str, "DIV GF@%s TF@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(equalStrings("ggg", frames)){
                        sprintf(str, "DIV GF@%s GF@%s GF@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                }
                if(symb2->tokenType == FLOAT_LIT){

                    if(frames[0] == 't' && frames[1] == 't'){
                        sprintf(str, "DIV TF@%s TF@%s float@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 't' && frames[1] == 'g'){
                        sprintf(str, "DIV TF@%s GF@%s float@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 't'){
                        sprintf(str, "DIV GF@%s TF@%s float@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
                    }
                    if(frames[0] == 'g' && frames[1] == 'g'){
                        sprintf(str, "DIV GF@%s GF@%s float@%s",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
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

errorCode generateLT(token* var, token* symb1, token* symb2, char* frames){


    char str[100];
    if(symb1->tokenType == INT_LIT){
        if(symb2->tokenType == INT_LIT){
            if(frames[0] == 't'){
                sprintf(str,"LT TF@%s int@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[0] == 'g'){
                sprintf(str,"LT GF@%s int@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }
        if(symb2->tokenType == IDENT){
            if(frames[0] == 't' && frames[2] == 't' ){
                sprintf(str,"LT TF@%s int@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[0] == 't' && frames[2] == 'g' ){
                sprintf(str,"LT TF@%s int@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[0] == 'g' && frames[2] == 't' ){
                sprintf(str,"LT GF@%s int@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[0] == 'g' && frames[2] == 't' ){
                sprintf(str,"LT GF@%s int@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }
    }

    if(symb1->tokenType == FLOAT_LIT){
        if(symb2->tokenType == FLOAT_LIT){
            if(frames[0] == 't'){
                sprintf(str,"LT TF@%s float@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[0] == 'g'){
                sprintf(str,"LT GF@%s float@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }
        if(symb2->tokenType == IDENT){
            if(frames[0] == 't' && frames[2] == 't'){
                sprintf(str,"LT TF@%s float@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[0] == 't' && frames[2] == 'g'){
                sprintf(str,"LT TF@%s float@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[0] == 'g' && frames[2] == 't'){
                sprintf(str,"LT GF@%s float@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[0] == 'g' && frames[2] == 'g'){
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

            if(frames[0] == 't'){
                sprintf(str,"LT TF@%s string@%s string@%s\n",var->tokenName.data, tmp1.data, tmp2.data);
            }
            if(frames[0] == 'g'){
                sprintf(str,"LT GF@%s string@%s string@%s\n",var->tokenName.data, tmp1.data, tmp2.data);
            }
            destroyString(&tmp2);
        }
        if(symb2->tokenType == IDENT){
            if(frames[0] == 't' && frames[2] == 't'){
                sprintf(str,"LT TF@%s string@%s TF@%s\n",var->tokenName.data, tmp1.data, symb2->tokenName.data);
            }
            if(frames[0] == 't' && frames[2] == 'g'){
                sprintf(str,"LT TF@%s string@%s GF@%s\n",var->tokenName.data, tmp1.data, symb2->tokenName.data);
            }
            if(frames[0] == 'g' && frames[2] == 't'){
                sprintf(str,"LT GF@%s string@%s TF@%s\n",var->tokenName.data, tmp1.data, symb2->tokenName.data);
            }
            if(frames[0] == 'g' && frames[2] == 'g'){
                sprintf(str,"LT GF@%s string@%s GF@%s\n",var->tokenName.data, tmp1.data, symb2->tokenName.data);
            }
        }
        destroyString(&tmp1);
    }

    if(symb1->tokenType == IDENT){
        if(symb2->tokenType == FLOAT_LIT){
            if(frames[0] == 't' && frames[1] == 't'){
                sprintf(str,"LT TF@%s TF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[0] == 't' && frames[1] == 'g'){
                sprintf(str,"LT TF@%s GF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[0] == 'g' && frames[1] == 't'){
                sprintf(str,"LT GF@%s TF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[0] == 'g' && frames[1] == 'g'){
                sprintf(str,"LT GF@%s GF@%s float@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }

        if(symb2->tokenType ==  STRING_LIT){
            string tmp;
            initString(&tmp);
            makeString(symb2->tokenName.data,&tmp);
            transformString(&tmp);

            if(frames[0] == 't' && frames[1] == 't'){
                sprintf(str,"LT TF@%s TF@%s string@%s\n",var->tokenName.data, symb1->tokenName.data, tmp.data);
            }
            if(frames[0] == 't' && frames[1] == 'g'){
                sprintf(str,"LT TF@%s GF@%s string@%s\n",var->tokenName.data, symb1->tokenName.data, tmp.data);
            }
            if(frames[0] == 'g' && frames[1] == 't'){
                sprintf(str,"LT GF@%s TF@%s string@%s\n",var->tokenName.data, symb1->tokenName.data, tmp.data);
            }
            if(frames[0] == 'g' && frames[1] == 'g'){
                sprintf(str,"LT GF@%s GF@%s string@%s\n",var->tokenName.data, symb1->tokenName.data, tmp.data);
            }
            destroyString(&tmp);
        }

        if(symb2->tokenType == TYPE_INT){
            if(frames[0] == 't' && frames[1] == 't'){
                sprintf(str,"LT TF@%s TF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[0] == 't' && frames[1] == 'g'){
                sprintf(str,"LT TF@%s GF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[0] == 'g' && frames[1] == 't'){
                sprintf(str,"LT GF@%s TF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(frames[0] == 'g' && frames[1] == 'g'){
                sprintf(str,"LT GF@%s GF@%s int@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }

        if(symb2->tokenType == IDENT){
            if(equalStrings("ttt", frames)){
                sprintf(str,"LT TF@%s TF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(equalStrings("ttg", frames)){
                sprintf(str,"LT TF@%s TF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(equalStrings("tgt", frames)){
                sprintf(str,"LT TF@%s GF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(equalStrings("tgg", frames)){
                sprintf(str,"LT TF@%s GF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(equalStrings("gtt", frames)){
                sprintf(str,"LT GF@%s TF@%s TF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(equalStrings("gtg", frames)){
                sprintf(str,"LT GF@%s TF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
            if(equalStrings("ggg", frames)){
                sprintf(str,"LT GF@%s GF@%s GF@%s\n",var->tokenName.data, symb1->tokenName.data, symb2->tokenName.data);
            }
        }
    }



    return OK;
}