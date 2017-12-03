#pragma once
#include <any>
#include <functional>
#include "Token.h"
#include "Symbols.h"

class Evaluate {
public:
    static std::any Calc(State state, std::any first, std::any second, std::string firstType,
                         std::string secondType, std::string exprType);
    static std::any Calc(std::string value, std::string type);
    static std::any Calc(std::any value, std::string type, std::string newType);
    static std::any Calc(State state, std::any elem, std::string type, std::string exprType);
private:
    static std::any IntArgs(std::any first, std::any second, std::string firstType, std::string secondType,
                            std::string exprType, std::function<int(int, int)> op);
    static std::any Cast(std::any value, std::string type, std::string newType);
    static std::any DoubleArgs(std::any first, std::any second, std::string firstType, std::string secondType,
                               std::string exprType, std::function<double(double, double)> op);
};
