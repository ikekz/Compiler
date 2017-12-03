#include "Error.h"
#include <string>

using namespace std;

Error::Error() {}

Error::Error(ErrorType errorType, Token &token) {
    string str = errorName.at(errorType);
    char buff[minBuffSize + str.size()];
    sprintf(buff, "(%d,%d) Error: %s",
            token.GetLine(), token.GetColumn(), str.c_str());
    message = buff;
}

const std::string &Error::GetMessage() const {
    return message;
}

SyntaxError::SyntaxError(Token &token, State state, string text) {
    if (text[0] == '\0')
        text = "end of file";
    string stName = stateName.at(state);
    char buff[minBuffSize + stName.size() + text.size()];
    sprintf(buff, "(%d,%d) Error: Syntax error, \"%s\" expected but \"%s\" found",
            token.GetLine(), token.GetColumn(), stName.c_str(), text.c_str());
    message = buff;
}

SyntaxError::SyntaxError(Token &token, TK::TokenType type, string text) {
    if (text[0] == '\0')
        text = "end of file";
    string tkName = tokenName.at(type);
    char buff[minBuffSize + tkName.size() + text.size()];
    sprintf(buff, "(%d,%d) Error: Syntax error, \"%s\" expected but \"%s\" found",
            token.GetLine(), token.GetColumn(), tkName.c_str(), text.c_str());
    message = buff;
}

IncompatibleTypes::IncompatibleTypes(Token &token, std::string type1, std::string type2) {
    char buff[minBuffSize + type1.size() + type2.size()];
    sprintf(buff, "(%d,%d) Error: Incompatible types: got \"%s\" expected \"%s\"",
            token.GetLine(), token.GetColumn(), type1.c_str(), type2.c_str());
    message = buff;
}

IdentifierNotFound::IdentifierNotFound(Token &token, std::string ident) {
    char buff[minBuffSize + ident.size()];
    sprintf(buff, "(%d,%d) Error: Identifier not found \"%s\"",
            token.GetLine(), token.GetColumn(), ident.c_str());
    message = buff;
}

DuplicateIdentifier::DuplicateIdentifier(Token &token, std::string ident) {
    char buff[minBuffSize + ident.size()];
    sprintf(buff, "(%d,%d) Error: Duplicate identifier \"%s\"",
            token.GetLine(), token.GetColumn(), ident.c_str());
    message = buff;
}

BinOperatorIsNotOverloaded::BinOperatorIsNotOverloaded(Token &token, std::string type1,
                                                       std::string op, std::string type2) {
    char buff[minBuffSize + type1.size() + type2.size() + op.size()];
    sprintf(buff, "(%d,%d) Error: Operator is not overloaded: got \"%s\" %s \"%s\"",
            token.GetLine(), token.GetColumn(), type1.c_str(), op.c_str(), type2.c_str());
    message = buff;
}

UnOperatorIsNotOverloaded::UnOperatorIsNotOverloaded(Token &token, std::string op, std::string type) {
    char buff[minBuffSize + op.size() + type.size()];
    sprintf(buff, "(%d,%d) Error: Operator is not overloaded: %s \"%s\"",
            token.GetLine(), token.GetColumn(), op.c_str(), type.c_str());
    message = buff;
}

WrongCountParameters::WrongCountParameters(Token &token, std::string func) {
    char buff[minBuffSize];
    sprintf(buff, "(%d,%d) Error: Wrong number of parameters specified for call to \"%s\"",
            token.GetLine(), token.GetColumn(), func.c_str());
    message = buff;
}

IncompatibleTypesForArg::IncompatibleTypesForArg(Token &token, int num,
                                                 std::string type1, std::string type2) {
    char buff[minBuffSize + type1.size() + type2.size()];
    sprintf(buff, "(%d,%d) Error: Incompatible types for arg %d: got \"%s\" expected \"%s\"",
            token.GetLine(), token.GetColumn(), num, type1.c_str(), type2.c_str());
    message = buff;
}
