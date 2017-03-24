#pragma once

#include <string>

class Error {
public:
    Error(std::string error, int line, int column);
    const std::string &GetMessage() const;
private:
    std::string message;
};