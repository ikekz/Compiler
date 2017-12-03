#include "Token.h"
#include <string>
#include <cstdlib>
#include <regex>
#include <iostream>

using namespace std;

int Token::GetLine() const {
    return line;
}

void Token::SetLine(int line) {
    Token::line = line;
}

int Token::GetColumn() const {
    return column;
}

void Token::SetColumn(int column) {
    Token::column = column;
}

State Token::GetState() const {
    return state;
}

void Token::SetState(State state) {
    Token::state = state;
}

const std::string &Token::GetText() const {
    return text;
}

void Token::SetText(const string &text) {
    Token::text = text;
}

void Token::CalcType() {
    if (tokenType.find(GetState()) != tokenType.end())
        type = tokenType[GetState()];
}

void Token::CalcValue() {
    if (tokenType[state] == TK::Double)
        DoubleValue();
    else if (tokenType[state] == TK::Identifier || tokenType[state] == TK::Operator ||
            tokenType[state] == TK::Separator)
        IdentifierValue();
    else if (tokenType[state] == TK::Integer)
        IntegerValue();
    else if (tokenType[state] == TK::String)
        StringValue();
    else if (tokenType[state] == TK::EOFF)
        EofValue();
}

void Token::EofValue() {
    value = "EOF";
}

const std::string &Token::GetValue() const {
    return value;
}

void Token::DoubleValue() {
    value = to_string(atof(text.c_str()));
}

void Token::StringValue() {
    value = text.substr(1, text.size() - 2);

    if (value == "")
        return;

    regex rx1("\'((#[0-9]+)+)\'");
    smatch m1;
    while (regex_search(value, m1, rx1)) {
        regex rx2("#([0-9]+)");
        smatch m2;
        string tmp = m1[1].str();
        while (regex_search(tmp, m2, rx2))
            tmp = m2.prefix().str() + (char)atoi(m2[1].str().c_str()) + m2.suffix().str();
        value = m1.prefix().str() + tmp + m1.suffix().str();
    }

    regex rx3("\'\'");
    value = regex_replace(value, rx3, "'");
}

void Token::IntegerValue() {
    char* tmp;
    if (state == HexIntegerValue)
        value = IntToValue(16);
    else if (state == State::IntegerValue)
        value = IntToValue(10);
    else if (state == OctalIntegerValue)
        value = IntToValue(8);
    else if (state == BinIntegerValue)
        value = IntToValue(2);
}

void Token::IdentifierValue() {
    value = text;
}

string Token::IntToValue(int base) {
    char* tmp;
    const char* buf;
    if (base == 10)
        buf = text.c_str();
    else
        buf = text.substr(1, text.size() - 1).c_str();

    return to_string(strtol(buf, &tmp, base));
}

void Token::SetType(TK::TokenType type) {
    Token::type = type;
}

TK::TokenType Token::GetType() const {
    return type;
}