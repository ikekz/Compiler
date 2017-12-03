#include "Evaluate.h"
#include "Token.h"

using namespace std;

any Evaluate::Calc(State state, any value, string type, string exprType) {
    switch (state) {
        case Plus:
            if (type == baseType.at(BaseType::Double))
                return Cast(+ any_cast<double>(value), type, exprType);
            return Cast(+ any_cast<int>(value), type, exprType);
        case Minus:
            if (type == baseType.at(BaseType::Double))
                return Cast(- any_cast<double>(value), type, exprType);
            return Cast(- any_cast<int>(value), type, exprType);
        case Not:
            return Cast(~ any_cast<int>(value), type, exprType);
    }
}

any Evaluate::Cast(any value, string type, string newType) {
    if (newType == baseType.at(BaseType::Double) &&
        type == baseType.at(BaseType::Integer))
        return static_cast<double>(any_cast<int>(value));
    if (newType == baseType.at(BaseType::Integer) &&
        type == baseType.at(BaseType::Double))
        return static_cast<int>(any_cast<double >(value));
    return value;
}

any Evaluate::IntArgs(any first, any second, string firstType, string secondType,
                           string exprType, function<int(int, int)> op) {
    if (firstType == baseType.at(BaseType::Integer) &&
        secondType == baseType.at(BaseType::Integer))
        return Cast(op(any_cast<int>(first), any_cast<int>(second)), firstType, exprType);
    return any();
}

any Evaluate::DoubleArgs(any first, any second, string firstType, string secondType,
                              string exprType, function<double(double, double)> op) {
    any value = IntArgs(first, second, firstType, secondType, exprType, op);
    if (value.has_value())
        return value;
    string intType = baseType.at(BaseType::Integer);
    string doubleType = baseType.at(BaseType::Double);
    if (firstType == intType && secondType == doubleType)
        return Cast(op(any_cast<int>(first), any_cast<double>(second)), doubleType, exprType);
    if (firstType == doubleType && secondType == intType)
        return Cast(op(any_cast<double>(first), any_cast<int>(second)), doubleType, exprType);
    if (firstType == doubleType && secondType == doubleType)
        return Cast(op(any_cast<double>(first), any_cast<double>(second)), doubleType, exprType);
    return any();
}

any Evaluate::Calc(string value, string type) {
    if (type == baseType.at(BaseType::Integer))
        return stoi(value);
    else if (type == baseType.at(BaseType::Double))
        return stod(value);
    else if (type == baseType.at(BaseType::Char))
        return value;
}

any Evaluate::Calc(any value, string type, string newType) {
    return Cast(value, type, newType);
}

any Evaluate::Calc(State state, any first, any second, string firstType,
                        string secondType, string exprType) {
    switch (state) {
        case Plus: {
            if (firstType == baseType.at(BaseType::Char))
                return any_cast<string>(first) + any_cast<string>(second);
            return DoubleArgs(first, second, firstType, secondType, exprType, plus<double>());
        }
        case Minus:
            return DoubleArgs(first, second, firstType, secondType, exprType, minus<double>());
        case Asterisk:
            return DoubleArgs(first, second, firstType, secondType, exprType, multiplies<double>());
        case Slash:
            return DoubleArgs(first, second, firstType, secondType, exprType, divides<double>());
        case Mod:
            return Cast(any_cast<int>(first) % any_cast<int>(second), firstType, exprType);
        case Div:
            return Cast(any_cast<int>(first) / any_cast<int>(second), firstType, exprType);
        case And:
            return Cast(any_cast<int>(first) & any_cast<int>(second), firstType, exprType);
        case Or:
            return Cast(any_cast<int>(first) | any_cast<int>(second), firstType, exprType);
        case Xor:
            return Cast(any_cast<int>(first) ^ any_cast<int>(second), firstType, exprType);
        case Shl:
            return Cast(any_cast<int>(first) << any_cast<int>(second), firstType, exprType);
        case Shr:
            return Cast(any_cast<int>(first) >> any_cast<int>(second), firstType, exprType);
        case LessThanLessThan:
            return Cast(any_cast<int>(first) << any_cast<int>(second), firstType, exprType);
        case GreaterThanGreaterThan:
            return Cast(any_cast<int>(first) >> any_cast<int>(second), firstType, exprType);
        case Equal:
            return DoubleArgs(first, second, firstType, secondType, exprType, equal_to<double>());
        case LessThanGreaterThan:
            return DoubleArgs(first, second, firstType, secondType, exprType, not_equal_to<double>());
        case LessThan:
            return DoubleArgs(first, second, firstType, secondType, exprType, less<double>());
        case LessThanEqual:
            return DoubleArgs(first, second, firstType, secondType, exprType, less_equal<double>());
        case GreaterThan:
            return DoubleArgs(first, second, firstType, secondType, exprType, greater<double>());
        case GreaterThanEqual:
            return DoubleArgs(first, second, firstType, secondType, exprType, greater_equal<double>());
    }
}
