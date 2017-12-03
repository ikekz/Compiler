#pragma once
#include <string>
#include <map>
#include "Token.h"

enum class ErrorType {
    IllegalExpression,
    IllegalQualifier,
    IllegalStatement,
    InvalidExpression,
    InvalidString,
    InvalidComment,
    InvalidIdentifier,
    InvalidDeclaration,
    IllegalType,
    InvalidOrdinalType,
    IllegalTypeDefinition,
    IllegalRange,
    UnexpectedEOF,
    InvalidVariable
};

static std::map<ErrorType, std::string> errorName = {
        { ErrorType::IllegalExpression,     "Illegal expression"                   },
        { ErrorType::IllegalStatement,      "Illegal statement"                    },
        { ErrorType::IllegalQualifier,      "Illegal qualifier"                    },
        { ErrorType::IllegalType,           "Illegal type"                         },
        { ErrorType::InvalidExpression,     "Invalid expression"                   },
        { ErrorType::InvalidString,         "Invalid string"                       },
        { ErrorType::InvalidComment,        "Invalid comment"                      },
        { ErrorType::InvalidIdentifier,     "Invalid identifier"                   },
        { ErrorType::InvalidDeclaration,    "Only one variable can be initialized" },
        { ErrorType::InvalidOrdinalType,    "Ordinal expression expected"          },
        { ErrorType::IllegalTypeDefinition, "Error in type definition"             },
        { ErrorType::IllegalRange,          "High range limit < low range limit"   },
        { ErrorType::UnexpectedEOF,         "Unexpected end of file"               },
        { ErrorType::InvalidVariable,       "Variable identifier expected"         }
};

class Error {
public:
    Error(ErrorType, Token&);
    Error();
    virtual const std::string &GetMessage() const;
protected:
    std::string message;
    static const int minBuffSize = 100;
};

class SyntaxError: public Error {
public:
    SyntaxError(Token&, State, std::string);
    SyntaxError(Token&, TK::TokenType, std::string);
};

class IncompatibleTypes: public Error {
public:
    IncompatibleTypes(Token&, std::string, std::string);
};

class IncompatibleTypesForArg: public Error {
public:
    IncompatibleTypesForArg(Token&, int, std::string, std::string);
};

class IdentifierNotFound: public Error {
public:
    IdentifierNotFound(Token&, std::string);
};

class DuplicateIdentifier: public Error {
public:
    DuplicateIdentifier(Token&, std::string);
};

class BinOperatorIsNotOverloaded: public Error {
public:
    BinOperatorIsNotOverloaded(Token&, std::string, std::string, std::string);
};

class UnOperatorIsNotOverloaded: public Error {
public:
    UnOperatorIsNotOverloaded(Token&, std::string, std::string);
};

class WrongCountParameters: public Error {
public:
    WrongCountParameters(Token&, std::string);
};