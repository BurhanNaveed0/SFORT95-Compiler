/*
Burhan Naveed
CS280
PA 1: Lexical Analyzer
*/


#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <unordered_set>
#include <set>
#include "lex.h"

int set_arguments(std::set<std::string>& flags_called, int argc, char** argv) {
    std::unordered_set<std::string> flag_set({ "-all", "-int", "-real", "-str", "-id", "-kw" });
    int invalid = -1;

    for (int i = 2; i < argc; i++) {
        if (flag_set.find(argv[i]) != flag_set.end()) {
            flags_called.insert(argv[i]);
        }
        else {
            invalid = i;
            break;
        }
    }

    return invalid;
}

void update_map(std::map<std::string, int>& counter, std::string lexeme) {
    if (counter.find(lexeme) == counter.end()) {
        counter.insert({ lexeme, 1 });
    }
    else {
        counter[lexeme]++;
    }
}

void update_map(std::map<int, int>& counter, int lexeme) {
    if (counter.find(lexeme) == counter.end()) {
        counter.insert({ lexeme, 1 });
    }
    else {
        counter[lexeme]++;
    }
}

int main(int argc, char** argv)
{

    // Check if no file name provided
    if (argc < 2) {
        std::cout << "NO SPECIFIED INPUT FILE." << std::endl;
        return 1;
    }


    // File stream
    std::ifstream file;
    file.open(argv[1]);

    // Arguments
    std::set<std::string> flags;
    int failed_arg = set_arguments(flags, argc, argv);

    // Check Multiple Filenames
    for (int i = 2; i < argc; i++) {
        if (argv[i][0] != '-') {
            std::cout << "ONLY ONE FILE NAME IS ALLOWED." << std::endl;
            return 1;
        }
    }

    // Check Invalid Argument
    if (failed_arg != -1) {
        std::cout << "UNRECOGNIZED FLAG {" << argv[failed_arg] << "}" << std::endl;
        return 1;
    }

    // Token Data
    LexItem token;
    std::vector<LexItem> tokens;
    std::map<std::string, int> idents;
    std::map<std::string, int> strings;
    std::map<int, int> ints;
    std::map<std::string, int> reals;
    std::map<std::string, int> keywords;

    std::set<Token> kwSet({ IF, ELSE, PRINT, INTEGER, REAL, CHARACTER, END, THEN, PROGRAM, LEN });

    int lineNumber = 0;
    int tokenCount = 0;
    bool error = false;

    bool flag_all = flags.find("-all") != flags.end();
    bool flag_id = flags.find("-id") != flags.end();
    bool flag_int = flags.find("-int") != flags.end();
    bool flag_real = flags.find("-real") != flags.end();
    bool flag_string = flags.find("-str") != flags.end();
    bool flag_kw = flags.find("-kw") != flags.end();

    if (file.is_open()) {

        // Check if file open
        if (file.peek() == std::ifstream::traits_type::eof()) {
            std::cout << "Empty File." << std::endl;
            return 1;
        }


        // Loop through tokens
        while (true) {
            token = getNextToken(file, lineNumber);

            if (token == DONE) {
                break;
            }

            else if (token == IDENT) {
                update_map(idents, token.GetLexeme());
            }

            else if (token == ICONST) {
                update_map(ints, std::stoi(token.GetLexeme()));
            }

            else if (token == RCONST) {
                update_map(reals, token.GetLexeme());
            }

            else if (token == SCONST) {
                update_map(strings, token.GetLexeme());
            }

            else if (flag_kw && kwSet.find(token.GetToken()) != kwSet.end()) {
                update_map(keywords, token.GetLexeme());
            }
            tokenCount++;

            // Add tokens to token vector
            tokens.push_back(token);

            // Stop program if error reached
            if (token == ERR) {
                error = true;
                break;
            }
        } 

        // -all flag
        if (flag_all) {
            for (int i = 0; i < tokens.size(); i++) {
                std::cout << tokens[i] << std::endl;
            }
        }

        // no flag data
        if (!error) {
            std::cout << std::endl;
            std::cout << "Lines: " << lineNumber << std::endl;
            std::cout << "Total Tokens: " << tokenCount << std::endl;
            std::cout << "Identifiers: " << idents.size() << std::endl;
            std::cout << "Integers: " << ints.size() << std::endl;
            std::cout << "Reals: " << reals.size() << std::endl;
            std::cout << "Strings: " << strings.size() << std::endl;


            // -id flag
            if (flag_id && idents.size() != 0) {
                std::cout << "IDENTIFIERS:" << std::endl;
                auto it = idents.begin();
                while (it != idents.end()) {
                    std::cout << it->first << " (" << it->second << ")";
                    ++it;

                    if (it != idents.end()) {
                        std::cout << ", ";
                    }
                }
                std::cout << std::endl;
            }

            // -kw flag
            if (flag_kw && keywords.size() != 0) {
                std::cout << "KEYWORDS:" << std::endl;
                auto it = keywords.begin();
                while (it != keywords.end()) {
                    std::cout << it->first << " (" << it->second << ")";
                    ++it;

                    if (it != keywords.end()) {
                        std::cout << ", ";
                    }
                }
                std::cout << std::endl;
            }

            // -int flag
            if (flag_int && ints.size() != 0) {
                std::cout << "INTEGERS:" << std::endl;
                auto it = ints.begin();
                while (it != ints.end()) {
                    std::cout << it->first;
                    ++it;

                    if (it != ints.end()) {
                        std::cout << ", ";
                    }
                }
                std::cout << std::endl;
            }

            // -real flag
            if (flag_real && reals.size() != 0) {
                std::cout << "REALS:" << std::endl;
                auto it = reals.begin();
                while (it != reals.end()) {

                    std::string num = it->first;
                    if (std::stod(num) < 1 && num[0] != '0') {
                        num.insert(num.begin(), '0');
                    }

                    std::cout << num;
                    ++it;

                    if (it != reals.end()) {
                        std::cout << ", ";
                    }
                }
                std::cout << std::endl;
            }

            // -str flag
            if (flag_string && strings.size() != 0) {
                std::cout << "STRINGS:" << std::endl;

                auto it = strings.begin();
                while (it != strings.end()) {
                    std::string str = it->first;
                    std::replace(str.begin(), str.end(), '\'', '\"');

                    std::cout << str;

                    ++it;

                    if (it != strings.end()) {
                        std::cout << ", ";
                    }
                }

                std::cout << endl;
            }
        }
    }
    else {
        // Invalid file directory path
        std::cout << "CANNOT OPEN THE FILE " << argv[1] << std::endl;
    }

    return 0;
}