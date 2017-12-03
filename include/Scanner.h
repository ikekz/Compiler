#pragma once
#include <string>
#include <fstream>
#include "Token.h"
#include <vector>

class Scanner {
public:
    Scanner(const char* fileName);
    ~Scanner();
    void NextToken();
    PToken GetNextToken();
    void PrintToken();
    bool IsEndOfFile();
    const PToken GetToken() const;
    void PrevToken();
    bool IsGettedToken();
private:
    std::map<State, std::vector<State>> statesTable;
    std::ifstream fin;
    PToken token;
    int line, column;
    int prevColumn;
    void FillStatesTable();
    void CheckReservedWord();
    void InitStatesTable();
    void SetState(State oldstate, int transition, State newstate);
    char ReadChar();
    void UnGetChar(int count = 1);
    void ClearToken();
    void CheckError(State state);
    bool IsComment(State state);
    void SetStates(State state, std::vector<std::pair<char, State>> ts);
    void SetStatesInRange(State state, std::vector<std::tuple<char, char, State>> ts);
    bool gettedToken;
    static const int lineWidth = 4;
    static const int columnWidth = 4;
    static const int typeWidth = 15;
    static const int textWidth = 15;
};

typedef std::shared_ptr<Scanner> PScanner;

const std::vector<State> states = {
        Ampersand,
        Asterisk,
        AsteriskEqual,
        AsteriskRightParenthesis,
        At,
        BinIntegerValue,
        Colon,
        ColonEqual,
        Comma,
        Dollar,
        DoubleValue,
        DoubleValueDigit,
        DoubleValueDigitScale,
        DoubleValueDigitScaleSign,
        DoubleValueDigitScaleSignDigit,
        Equal,
        ExclamationPoint,
        GreaterThan,
        GreaterThanEqual,
        GreaterThanGreaterThan,
        GreaterThanLessThan,
        Hash,
        HexIntegerValue,
        Identifier,
        IntegerValue,
        IntegerValuePeriodPeriod,
        LeftBrace,
        LeftBracket,
        LeftParenthesis,
        LeftParenthesisAsterisk,
        LeftParenthesisAsteriskAsterisk,
        LessThan,
        LessThanEqual,
        LessThanGreaterThan,
        LessThanLessThan,
        Minus,
        MinusEqual,
        NotToken,
        OctalIntegerValue,
        Percent,
        Period,
        PeriodPeriod,
        Plus,
        PlusEqual,
        Pointer,
        Quote,
        RightBrace,
        RightBracket,
        RightParenthesis,
        SemiColon,
        Slash,
        SlashEqual,
        SlashSlash,
        Space,
        String,
        StringHash,
        StringHashIntegerValue,
        VerticalLine
};