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

const std::string &Token::GetStr() const {
    return str;
}

void Token::SetStr(const string &str) {
    Token::str = str;
}

void Token::CalcValue() {
    if (tokenType[state] == TK_DOUBLE) {
        DoubleValue();
    }
    else if (tokenType[state] == TK_IDENTIFIER || tokenType[state] == TK_OPERATOR ||
            tokenType[state] == TK_SEPARATOR) {
        IdentifierValue();
    }
    else if (tokenType[state] == TK_INTEGER) {
        IntegerValue();
    }
    else if (tokenType[state] == TK_STRING) {
        StringValue();
    }
}

const std::string &Token::GetValue() const {
    return value;
}

void Token::DoubleValue() {
    value = to_string(atof(str.c_str()));
}

void Token::StringValue() {
    value = str.substr(1, str.size() - 2);

    if (value == "") {
        return;
    }

    regex rx1("\'((#[0-9]+)+)\'");
    smatch m1;
    while (regex_search(value, m1, rx1)) {
        regex rx2("#([0-9]+)");
        smatch m2;
        string tmp = m1[1].str();
        while (regex_search(tmp, m2, rx2)) {
            tmp = m2.prefix().str() + (char)atoi(m2[1].str().c_str()) + m2.suffix().str();
        }
        value = m1.prefix().str() + tmp + m1.suffix().str();
    }

    regex rx3("\'\'");
    value = regex_replace(value, rx3, "'");
}

void Token::IntegerValue() {
    char* tmp;
    if (state == HEX_INTEGER_VALUE)
        value = IntToValue(16);
    else if (state == INTEGER_VALUE)
        value = IntToValue(10);
    else if (state == OCTAL_INTEGER_VALUE)
        value = IntToValue(8);
    else if (state == BIN_INTEGER_VALUE)
        value = IntToValue(2);
}

void Token::IdentifierValue() {
    value = str;
}

string Token::IntToValue(int base) {
    char* tmp;
    const char* buf;
    if (base == 10)
        buf = str.c_str();
    else
        buf = str.substr(1, str.size() - 1).c_str();

    return to_string(strtol(buf, &tmp, base));
}