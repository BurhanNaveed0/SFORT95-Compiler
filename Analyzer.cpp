/*
Burhan Naveed
CS280
PA 1: Lexical Analyzer
*/

#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <cctype>
#include "lex.h"

std::map<Token, std::string> printMap = { {IF, "IF"}, {ELSE, "ELSE"}, {PRINT, "PRINT"} ,{INTEGER, "INTEGER"}, {REAL, "REAL"},
										 {CHARACTER, "CHARACTER"}, {END, "END"}, {THEN, "THEN"} ,{PROGRAM, "PROGRAM"}, {LEN, "LEN"},
										 {IDENT, "IDENT"}, {ICONST, "ICONST"}, {RCONST, "RCONST"}, {SCONST, "SCONST"}, {BCONST, "BCONST"},
										 {PLUS, "PLUS"}, {MINUS, "MINUS"}, {MULT, "MULT"}, {DIV, "DIV"}, {ASSOP, "ASSOP"}, {EQ, "EQ"},
										 {POW, "POW"}, {GTHAN, "GTHAN"}, {LTHAN, "LTHAN"}, {CAT, "CAT"}, {COMMA, "COMMA"}, {LPAREN, "LPAREN"},
										 {DOT, "DOT"}, {DCOLON, "DCOLON"}, {DOT, "DOT"}, {RPAREN, "RPAREN"}, {DEF, "DEF"}};


std::map<string, Token> kwMap = { {"IF", IF}, {"ELSE", ELSE}, {"PRINT", PRINT}, {"INTEGER", INTEGER}, {"REAL", REAL},
								 {"CHARACTER", CHARACTER}, {"END", END},   {"THEN", THEN},   {"PROGRAM", PROGRAM}, {"LEN", LEN} };

std::map<string, Token> opMap = { {"+", PLUS}, {"-", MINUS}, {"*", MULT}, {"/", DIV}, {"**", POW},
								 {"=", ASSOP}, {"==", EQ},   {"<", LTHAN},   {">", GTHAN}, {",", COMMA},
								 {"(", LPAREN}, {")", RPAREN}, {"::", DCOLON}, {".", DOT}, {"//", CAT}, {"*,", DEF}};

ostream& operator<<(ostream& out, const LexItem& tok) {
	Token type = tok.GetToken();
	string lexeme = tok.GetLexeme();

	if (type == ICONST || type == RCONST || type == BCONST) {
		out << printMap[type] << ": (" << lexeme << ")";
	}
	else if (type == SCONST) {
		std::replace(lexeme.begin(), lexeme.end(), '\'', '\"');
		out << printMap[type] << ": " << lexeme << "";
	}
	else if (type == ERR) {
		out << "Error in line " << tok.GetLinenum() << ": " << lexeme;
	}
	else if (type == IDENT) {
		out << printMap[type] << ": \'" << lexeme << "\'";
	}
	else {
		out << printMap[type];
	}

	return out;
}

LexItem id_or_kw(const string& lexeme, int linenum) {
	string lexeme_copy = lexeme;
	std::transform(lexeme_copy.begin(), lexeme_copy.end(), lexeme_copy.begin(), ::toupper);
	if (kwMap.find(lexeme_copy) != kwMap.end()) {
		return LexItem(kwMap[lexeme_copy], lexeme, linenum);
	}
	return LexItem(IDENT, lexeme, linenum);
}

LexItem currentItem;

LexItem getNextToken(istream& in, int& linenum) {
	enum TokenState { START, INID, INSTRING, ININT, INREAL, INCOMMENT, SIGN } lexstate = START;
	std::string lexeme;
	char current;

	while (in.get(current)) {
		switch (lexstate) {
		case START:
			// EOF 
			if (in.peek() == -1) {
				linenum++;
				return LexItem(DONE, lexeme, linenum);
			}

			// Next Line
			if (current == '\n') {
				linenum++;
				continue;
			}

			// Space
			if (std::isspace(current)) {
				continue;
			}

			// Identifier
			else if (std::isalpha(current) || current == '_') {
				lexstate = INID;
			}

			// String
			else if (current == '\"' || current == '\'') {
				lexstate = INSTRING;
			}

			// Integer
			else if (std::isdigit(current)) {
				lexstate = ININT;
			}

			// Sign
			else if (current == '+' || current == '-' || current == '*' || current == '/' ||
				current == '=' || current == '<' || current == '>' || current == ',' || current == '(' ||
				current == ')' || current == ':' || current == '.') {
				lexstate = SIGN;
			}

			// Comment
			else if (current == '!') {
				lexstate = INCOMMENT;
			}

			// Err --> None of the recognized symbols
			else {
				linenum++;
				std::string errMsg = "Unrecognized Lexeme {" + std::string(1, current) + "}";
				return LexItem(ERR, errMsg, linenum);
			}

			// Add Character to Lexeme
			lexeme += current;
			break;
		case INID:
			// End of Lexeme --> Return Identifier or Reserved Word 
			if (in.peek() == -1 || (!std::isalnum(current) && current != '_')) {
				lexstate = START;
				in.putback(current);
				return id_or_kw(lexeme, linenum);
			}

			// Add Character to Lexeme
			if (std::isalnum(current) || current == '_')
				lexeme += current;

			break;
		case INSTRING:
			// End of Lexeme " Case --> Return String
			if (lexeme.find("\"") != std::string::npos) {
				if (current == '\"') {
					lexstate = START;
					lexeme += current;
					return LexItem(SCONST, lexeme, linenum);
				}
				else if (current == '\'') {
					lexeme += current;
					linenum++;
					std::string errMsg = "Unrecognized Lexeme {" + lexeme + '}';
					return LexItem(ERR, errMsg, linenum);
				}
			}

			// End of Lexeme ' Case --> Return String
			if (lexeme.find("\'") != std::string::npos && current == '\'') {
				if (current == '\'') {
					lexstate = START;
					lexeme += current;
					return LexItem(SCONST, lexeme, linenum);
				}
				else if (current == '\"') {
					lexeme += current;
					linenum++;
					std::string errMsg = "Unrecognized Lexeme {" + lexeme + '}';
					return LexItem(ERR, errMsg, linenum);
				}
			}

			// Error --> End quotes not found
			if (in.peek() == -1 || current == '\n') {
				std::string errMsg = "Unrecognized Lexeme {" + lexeme + '}';
				linenum++;
				return LexItem(ERR, errMsg, linenum);
			}

			// Add to lexeme
			lexeme += current;
			break;

		case ININT:
			// Set state to REAL if . encountered
			if (current == '.') {
				lexstate = INREAL;
			}

			// End of Lexeme --> non digit value --> Return Int  
			else if (!std::isdigit(current)) {
				lexstate = START;
				in.putback(current);
				return LexItem(ICONST, lexeme, linenum);
			}

			else if (in.peek() == -1) {
				linenum++;
				lexeme += current;
				return LexItem(ICONST, lexeme, linenum);
			}

			// Add Character to Lexeme
			lexeme += current;
			break;

		case INREAL:
			// Check if . encountered again
			if (current == '.') {
				lexeme += current; 
				linenum++;
				std::string errMsg = "Unrecognized Lexeme {" + lexeme + '}';
				return LexItem(ERR, errMsg, linenum);
			}


			// End of Lexeme --> non digit value --> Return Real
			if (!std::isdigit(current)) {
				lexstate = START;
				in.putback(current);

				return LexItem(RCONST, lexeme, linenum);
			}

			if (in.peek() == -1) {
				linenum++;
				lexeme += current;

				return LexItem(RCONST, lexeme, linenum);
			}

			// Add Character to Lexeme
			lexeme += current;
			break;


		case INCOMMENT:
			// End of Lexeme --> Return Comment
			if (in.peek() == -1 || current == '\n') {
				lexstate = START;
				lexeme = "";
				linenum++;
			}

			break;
		case SIGN:
			// Handle .
			if (lexeme == "." && std::isdigit(current)) {
				lexstate = INREAL;
			}

			// Handle doulble =
			else if (lexeme == "=" && current == '=') {
				lexstate = START;
				lexeme += current;
				return LexItem(EQ, lexeme, linenum);
			}

			// Handle * and = 
			else if (lexeme == "*" && current == ',') {
				lexstate = START;
				in.putback(current);
				return LexItem(DEF, lexeme, linenum);
			}

			// Handle * and **
			else if (lexeme == "*" && current == '*') {
				lexstate = START;
				lexeme += current;
				return LexItem(POW, lexeme, linenum);
			}

			// // Edge Case
			else if (lexeme == "/" && current == '/') {
				lexstate = START;
				lexeme += current;
				return LexItem(CAT, lexeme, linenum);
			}

			// :: Edge Case --> if single : --> Throw Err
			else if (lexeme == ":") {
				if (current == ':') {
					lexstate = START;
					lexeme += current;
					return LexItem(DCOLON, lexeme, linenum);
				}
				else {
					std::string errMsg = "Unrecognized Lexeme {" + lexeme + '}';
					linenum++;
					return LexItem(ERR, errMsg, linenum);
				}
			}

			// Handle Other Cases
			else if (opMap.find(lexeme) != opMap.end()) {
				lexstate = START;
				in.putback(current);
				return LexItem(opMap[lexeme], lexeme, linenum);
			}

			lexeme += current;
			break;
		}
	}

	return LexItem(DONE, "", linenum);
}

