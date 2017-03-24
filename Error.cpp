#include "Error.h"
#include <string>

Error::Error(std::string error, int line, int column) {
    message = error + " at line " + std::to_string(line) + ", column " + std::to_string(column);
}

const std::string &Error::GetMessage() const {
    return message;
}
