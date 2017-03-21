#include <iostream>
#include "Scanner.h"

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
        catch (string error) {
            cout << error;
        }
    }

    return 0;
}