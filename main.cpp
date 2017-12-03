#include <iostream>
#include <fstream>
#include <cstring>
#include "Scanner.h"
#include "Parser.h"
#include "Error.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (!strcmp(argv[1], "-s")) {
        try {
            Scanner scanner(argv[2]);
            while (!scanner.IsEndOfFile()) {
                scanner.NextToken();
                scanner.PrintToken();
            }
        }
        catch (Error error) {
            cout << error.GetMessage();
        }
    }
    else if (!strcmp(argv[1], "-pe")) {
        try {
            Parser parser(argv[2], ParserConfig::ParseExpression);
            parser.PrintTree();
        }
        catch (Error error) {
            cout << error.GetMessage();
        }
    }
    else if (!strcmp(argv[1], "-pd")) {
        try {
            Parser parser(argv[2], ParserConfig::ParseProgram);
            parser.PrintStack();
        }
        catch (Error error) {
            cout << error.GetMessage();
        }
    }
    else if (!strcmp(argv[1], "-ps")) {
        try {
            Parser parser(argv[2], ParserConfig::ParseProgram);
            parser.PrintTree();
        }
        catch (Error error) {
            cout << error.GetMessage();
        }
    }

    return 0;
}