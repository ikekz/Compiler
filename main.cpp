#include <iostream>
#include <fstream>
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
    else if (!strcmp(argv[1], "-p")) {
        try {
            Parser parser(argv[2]);
            parser.PrintTree(parser.GetTree(), 0);
        }
        catch (Error error) {
            cout << error.GetMessage();
        }
    }

    return 0;
}