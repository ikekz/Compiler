#include "Scanner.h"
#include <iostream>
#include <iomanip>
#include "Error.h"

using namespace std;

Scanner::Scanner(const char* fileName) : token(), line(1), column(0) {
    fin.open(fileName);
    InitStatesTable();
    FillStatesTable();
}

Scanner::~Scanner() {
    fin.close();
}

void Scanner::PrevToken() {
    if (token.GetType() != TK_EOF)
        UnGetChar((int)token.GetText().size());
}

char Scanner::ReadChar() {
    char symb;
    fin.get(symb);
    if (fin.eof())
        symb = '\0';
    if (symb == '\n') {
        line++;
        column = 0;
    }
    else {
        column++;
    }
    return symb;
}

void Scanner::UnGetChar(int count) {
    for (int i = 0; i < count; i++) {
        if (fin.eof())
            fin.clear();
        else
            fin.unget();
        if (column == 0) {
            line--;
        } else {
            column--;
        }
    }
}

void Scanner::InitStatesTable() {
    for (int i = 0; i < states.size(); i++) {
        statesTable[states[i]].resize(128, NOT_TOKEN);
    }
}

void Scanner::SetState(State oldstate, int transition, State newstate) {
    statesTable[oldstate][transition] = newstate;
}

void Scanner::CheckError(State state) {
    std::string text;
    if (state == ERROR_INVALID_EXPRESSION)
        text = "Invalid expression";
    else if (state == ERROR_INVALID_STRING)
        text = "Invalid string";
    else if (state == ERROR_INVALID_COMMENT)
        text = "Invalid comment";

    if (!text.empty())
        throw Error(text, token.GetLine(), token.GetColumn());
}

bool Scanner::IsComment(State state) {
    return state == RIGHT_BRACE || state == ASTERISK_RIGHT_PARENTHESIS || state == SLASH_SLASH;
}

void Scanner::CheckReservedWord() {
    auto st = reservedWordState.find(token.GetText());
    if (st != reservedWordState.end()) {
        token.SetState(st->second);
        token.SetType(TK_RESERVED_WORD);
    }
}

void Scanner::NextToken() {
    ClearToken();
    bool isToken = false;

    while (!IsEndOfFile()) {
        char symb = ReadChar();
        State state = statesTable[token.GetState()][symb];

        if (!isToken && state != NOT_TOKEN) {
            isToken = true;
            token.SetColumn(column);
            token.SetLine(line);
        }

        CheckError(state);

        if (state == NOT_TOKEN) {
            if (isToken) {
                UnGetChar();
                State lastState = token.GetState();
                if (IsComment(lastState)) {
                    ClearToken();
                    isToken = false;
                    continue;
                }
                if (lastState == INTEGER_VALUE_PERIOD_PERIOD) {
                    UnGetChar(2);
                    string text = token.GetText();
                    token.SetText(text.substr(0, text.size() - 2));
                    token.SetState(INTEGER_VALUE);
                }
                break;
            }
            else {
                continue;
            }
        }
        token.SetState(state);
        token.SetText(token.GetText() + symb);
    }
    token.CalcValue();
    token.CalcType();
    if (token.GetState() == IDENTIFIER)
        CheckReservedWord();
}

void Scanner::PrintToken() {
    if (token.GetType() == TK_EOF)
        return;

    cout << left << setw(4) <<  token.GetLine();
    cout << setw(4) << token.GetColumn();
    cout << setw(15) << tokenName[token.GetType()];
    cout << setw(15) << token.GetText();
    cout << token.GetValue() << endl;
}

bool Scanner::IsEndOfFile() {
    return fin.eof();
}

void Scanner::ClearToken() {
    token.SetColumn(column);
    token.SetLine(line);
    token.SetText("");
    token.SetState(NOT_TOKEN);
}

void Scanner::SetStates(State state, std::vector<std::pair<char, State>> ts) {
    for (int i = 0; i < ts.size(); i++)
        SetState(state, ts[i].first, ts[i].second);
}

void Scanner::SetStatesInRange(State state, std::vector<std::tuple<char, char, State>> ts) {
    for (int i = 0; i < ts.size(); i++)
        for (int j = get<0>(ts[i]); j <= (int)get<1>(ts[i]); j++)
            SetState(state, j, get<2>(ts[i]));
}

void Scanner::FillStatesTable() {
    SetStatesInRange(NOT_TOKEN, {
            {'a', 'z', IDENTIFIER},
            {'A', 'Z', IDENTIFIER},
            {'0', '9', INTEGER_VALUE},
    });
    SetStates(NOT_TOKEN, {
            {'*', POINTER},
            {',', COMMA},
            {'.', PERIOD},
            {'@', AT},
            {'#', HASH},
            {'<', LESS_THAN},
            {'>', GREATER_THAN},
            {':', COLON},
            {';', SEMI_COLON},
            {'=', EQUAL},
            {'|', VERTICAL_LINE},
            {'!', EXCLAMATION_POINT},
            {'*', ASTERISK},
            {'\'', QUOTE},
            {'/', SLASH},
            {'(', LEFT_PARENTHESIS},
            {'{', LEFT_BRACE},
            {'[', LEFT_BRACKET},
            {')', RIGHT_PARENTHESIS},
            {']', RIGHT_BRACKET},
            {' ', NOT_TOKEN},
            {'\n', NOT_TOKEN},
            {'_', IDENTIFIER},
            {'$', DOLLAR},
            {'&', AMPERSAND},
            {'%', PERCENT},
            {'+', PLUS},
            {'-', MINUS},
            {'}', ERROR_INVALID_COMMENT},
            {'\0', ST_EOF}
    });

    SetState(PERIOD, '.', PERIOD_PERIOD);

    SetStates(LESS_THAN, {
            {'=', LESS_THAN_EQUAL},
            {'>', LESS_THAN_GREATER_THAN},
            {'<', LESS_THAN_LESS_THAN}
    });

    SetStates(GREATER_THAN, {
            {'=', GREATER_THAN_EQUAL},
            {'>', GREATER_THAN_LESS_THAN},
            {'<', GREATER_THAN_GREATER_THAN}
    });

    SetState(COLON, '=', COLON_EQUAL);

    SetStates(ASTERISK, {
            {'=', ASTERISK_EQUAL},
            {')', ERROR_INVALID_COMMENT}
    });

    SetStates(SLASH, {
            {'/', SLASH_SLASH},
            {'=', SLASH_EQUAL}
    });

    SetStatesInRange(SLASH_SLASH, {
            {0, 127, SLASH_SLASH}
    });
    SetState(SLASH_SLASH, '\n', NOT_TOKEN);

    SetStatesInRange(IDENTIFIER, {
            {'a', 'z', IDENTIFIER},
            {'A', 'Z', IDENTIFIER},
            {'0', '9', IDENTIFIER}
    });
    SetState(IDENTIFIER, '_', IDENTIFIER);

    SetStatesInRange(INTEGER_VALUE, {
            {'a', 'z', ERROR_INVALID_EXPRESSION},
            {'A', 'Z', ERROR_INVALID_EXPRESSION},
            {'0', '9', INTEGER_VALUE}
    });
    SetStates(INTEGER_VALUE, {
            {'_', ERROR_INVALID_EXPRESSION},
            {'.', DOUBLE_VALUE},
            {'E', DOUBLE_VALUE_DIGIT_SCALE},
            {'e', DOUBLE_VALUE_DIGIT_SCALE}
    });

    SetStatesInRange(DOLLAR, {
            {'a', 'f', HEX_INTEGER_VALUE},
            {'g', 'a', ERROR_INVALID_EXPRESSION},
            {'A', 'F', HEX_INTEGER_VALUE},
            {'G', 'Z', ERROR_INVALID_EXPRESSION},
            {'0', '9', HEX_INTEGER_VALUE}
    });

    SetStatesInRange(AMPERSAND, {
            {'a', 'z', IDENTIFIER},
            {'A', 'Z', IDENTIFIER},
            {'0', '7', OCTAL_INTEGER_VALUE},
            {'8', '9', ERROR_INVALID_EXPRESSION}
    });
    SetState(AMPERSAND, '_', IDENTIFIER);

    SetStatesInRange(PERCENT, {
            {'a', 'z', ERROR_INVALID_EXPRESSION},
            {'A', 'Z', ERROR_INVALID_EXPRESSION},
            {'0', '1', BIN_INTEGER_VALUE},
            {'2', '9', ERROR_INVALID_EXPRESSION}
    });

    SetStatesInRange(HEX_INTEGER_VALUE, {
            {'a', 'f', HEX_INTEGER_VALUE},
            {'g', 'a', ERROR_INVALID_EXPRESSION},
            {'A', 'F', HEX_INTEGER_VALUE},
            {'G', 'Z', ERROR_INVALID_EXPRESSION},
            {'0', '9', HEX_INTEGER_VALUE}
    });
    SetState(HEX_INTEGER_VALUE, '_', ERROR_INVALID_EXPRESSION);

    SetStatesInRange(OCTAL_INTEGER_VALUE, {
            {'a', 'z', ERROR_INVALID_EXPRESSION},
            {'A', 'Z', ERROR_INVALID_EXPRESSION},
            {'0', '7', OCTAL_INTEGER_VALUE},
            {'8', '9', ERROR_INVALID_EXPRESSION}
    });
    SetState(OCTAL_INTEGER_VALUE, '_', ERROR_INVALID_EXPRESSION);

    SetStatesInRange(BIN_INTEGER_VALUE, {
            {'a', 'z', ERROR_INVALID_EXPRESSION},
            {'A', 'Z', ERROR_INVALID_EXPRESSION},
            {'0', '1', BIN_INTEGER_VALUE},
            {'2', '9', ERROR_INVALID_EXPRESSION}
    });
    SetState(BIN_INTEGER_VALUE, '_', ERROR_INVALID_EXPRESSION);

    SetStatesInRange(DOUBLE_VALUE, {
            {'a', 'z', ERROR_INVALID_EXPRESSION},
            {'A', 'Z', ERROR_INVALID_EXPRESSION},
            {'0', '9', DOUBLE_VALUE_DIGIT}
    });
    SetStates(DOUBLE_VALUE, {
            {'_', ERROR_INVALID_EXPRESSION},
            {'.', INTEGER_VALUE_PERIOD_PERIOD}
    });

    SetStatesInRange(DOUBLE_VALUE_DIGIT, {
            {'a', 'z', ERROR_INVALID_EXPRESSION},
            {'A', 'Z', ERROR_INVALID_EXPRESSION},
            {'0', '9', DOUBLE_VALUE_DIGIT}
    });
    SetStates(DOUBLE_VALUE_DIGIT, {
            {'_', ERROR_INVALID_EXPRESSION},
            {'.', ERROR_INVALID_EXPRESSION},
            {'E', DOUBLE_VALUE_DIGIT_SCALE},
            {'e', DOUBLE_VALUE_DIGIT_SCALE}
    });

    SetStatesInRange(DOUBLE_VALUE_DIGIT_SCALE, {
            {0, 127, ERROR_INVALID_EXPRESSION},
            {'0', '9', DOUBLE_VALUE_DIGIT_SCALE_SIGN_DIGIT},
    });
    SetStates(DOUBLE_VALUE_DIGIT_SCALE, {
            {'+', DOUBLE_VALUE_DIGIT_SCALE_SIGN},
            {'-', DOUBLE_VALUE_DIGIT_SCALE_SIGN}
    });

    SetStatesInRange(DOUBLE_VALUE_DIGIT_SCALE_SIGN, {
            {0, 127, ERROR_INVALID_EXPRESSION},
            {'0', '9', DOUBLE_VALUE_DIGIT_SCALE_SIGN_DIGIT},
    });

    SetStatesInRange(DOUBLE_VALUE_DIGIT_SCALE_SIGN_DIGIT, {
            {'a', 'z', ERROR_INVALID_EXPRESSION},
            {'A', 'Z', ERROR_INVALID_EXPRESSION},
            {'0', '9', DOUBLE_VALUE_DIGIT_SCALE_SIGN_DIGIT},
    });
    SetStates(DOUBLE_VALUE_DIGIT_SCALE_SIGN_DIGIT, {
            {'.', ERROR_INVALID_EXPRESSION},
            {'_', ERROR_INVALID_EXPRESSION}
    });

    SetState(MINUS, '=', MINUS_EQUAL);

    SetState(PLUS, '=', PLUS_EQUAL);

    SetStatesInRange(LEFT_BRACE, {
            {0, 127, LEFT_BRACE}
    });
    SetStates(LEFT_BRACE, {
            {'}', RIGHT_BRACE},
            {'\0', ERROR_INVALID_COMMENT}
    });

    SetState(LEFT_PARENTHESIS, '*', LEFT_PARENTHESIS_ASTERISK);

    SetStatesInRange(LEFT_PARENTHESIS_ASTERISK, {
            {0, 127, LEFT_PARENTHESIS_ASTERISK}
    });
    SetStates(LEFT_PARENTHESIS_ASTERISK, {
            {'*', LEFT_PARENTHESIS_ASTERISK_ASTERISK},
            {'\0', ERROR_INVALID_COMMENT}
    });

    SetStatesInRange(LEFT_PARENTHESIS_ASTERISK_ASTERISK, {
            {0, 127, LEFT_PARENTHESIS_ASTERISK}
    });
    SetStates(LEFT_PARENTHESIS_ASTERISK_ASTERISK, {
            {'*', LEFT_PARENTHESIS_ASTERISK_ASTERISK},
            {')', ASTERISK_RIGHT_PARENTHESIS},
            {'\0', ERROR_INVALID_COMMENT}
    });

    SetStatesInRange(QUOTE, {
            {0, 127, QUOTE}
    });
    SetStates(QUOTE, {
            {'\'', STRING},
            {'\0', ERROR_INVALID_STRING},
            {'\n', ERROR_INVALID_STRING}
    });

    SetStates(STRING, {
            {'#', STRING_HASH},
            {'\'', QUOTE}
    });

    SetStatesInRange(STRING_HASH, {
            {0, 127, ERROR_INVALID_EXPRESSION},
            {'0', '9', STRING_HASH_INTEGER_VALUE}
    });

    SetStatesInRange(STRING_HASH_INTEGER_VALUE, {
            {0, 127, ERROR_INVALID_EXPRESSION},
            {'0', '9', STRING_HASH_INTEGER_VALUE}
    });
    SetStates(STRING_HASH_INTEGER_VALUE, {
            {'#', STRING_HASH},
            {'\'', QUOTE}
    });
}

const Token &Scanner::GetToken() const {
    return token;
}
