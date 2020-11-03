//
// Created by xolsia00 on 3. 11. 2020.
//

#include <stdio.h>
#include <ctype.h>
#include "List.h"
#include "../String/DynamicString.h"



void ScannerHandler() {

	string tempCode; //THIS SHOULD BE CHANGED LATER
	initString(tempCode);
	makeString("if(Jakub.location == doma){ } else{ Jakub smirdi while corona true}", &tempCode);

	list sortedList;
	initList(sortedList);

	Analyzer(&sortedList, tempCode); //CHANGE TEMPCODE LATER
}

void Analyzer(TList* sortedList, string code) {

	char currentChar;
	string currentLexem;
	initString(currentLexem);

	char operators[] = "{}()*/-+.'"";

		bool comment = false;
	bool lineComment = false;


	//read code char by char
	for (int i = 0; i < code.len; i++) {

		currentChar = code.data[i];

		//comment check
		if (comment == true) {
			if (lineComment) {
				if (currentChar = '\n') {
					lineComment = false;
					comment = false;
				}
				if (currentChar = '*') {
					if (code.len >= ++i) {
						if (code.data[++i] == '/') {
							comment = false;
						}
					}
				}
			}
			continue;
		}
		//check for identificator, literals
		else if (isalnum(currentChar) || currentChar == '_') {

			CharConcat(currentLexem, currentChar);

		}

		else {
			if (currentChar == '/') {

				if (code.len >= ++i) {
					if (code.data[++i] == '*') {
						comment = true;
					}
					else if (code.data[++i] == '/') {
						comment = true;
						lineComment = true;
					}
					else if (code.data[++i] == '/t') {
						//SAVE THIS AS NEW LEXEM
					}
					else if (code.data[++i] == '/"') {
						//SAVE THIS AS NEW LEXEM
					}
					else {

						//CALL LEXICAL ERROR
					}
				}

				CharConcat(currentLexem, '\0');

				LexemAutomat(&sortedList, currentLexem);

				destroyString(currentLexem);
				initString(currentLexem);

				if (strchr(operators, currentChar) != NULL)
				{

					makeString(currentChar, &currentLexem);

					LexemAutomat(&sortedList, currentLexem);

					destroyString(&currentLexem);
					initString(&currentLexem);

				}


			}

		}

	}
}

// YOU HAVE TO APPEND . AND TWO NUMBERS NEXT TO IT
// ALSO IF + / - IS BEFORE A NUMBER AND THERE IS NO NUMBER BEFORE IT APPEND THAT TOO
// ALSO APPEND LITERALS BETWEEN "" or ''

void LexemAutomat(TList* sortedList, string lexem) {

}

void LexemAppend() {

}

void CharConcat(string target, char addition) {
	string temp;
	initString(temp);
	makeString(addition, &temp);
	concatenate(&target, &temp, &target);
	destroyString(&temp;)
}

