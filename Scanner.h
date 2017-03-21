#include <string>
#include <fstream>
#include "Token.h"
#include <vector>

class Scanner {
public:
    Scanner(const char* fileName);
    ~Scanner();
    void NextToken();
    void PrintToken();
    bool IsEndOfFile();
    const Token &GetToken() const;
    void Previous();
private:
    std::ifstream fin;
    Token token;
    int column;
    int line;
    void FillStatesTable();
    void InitStatesTable();
    void SetState(State state1, int transition, State state2);
    char ReadChar();
    void UnGetChar(int count = 1);
    void ClearToken();
    void CheckError(State state);
    bool IsComment(State state);
    void SetStates(State state, std::vector<std::pair<char, State>> ts);
    void SetStatesInRange(State state, std::vector<std::tuple<char, char, State>> ts);
};

static std::vector<State> states = {
        AMPERSAND, ASTERISK, ASTERISK_EQUAL, ASTERISK_RIGHT_PARENTHESIS, AT, BIN_INTEGER_VALUE,
        COLON, COLON_EQUAL, COLON_EQUAL, COMMA, DOLLAR, DOUBLE_VALUE, DOUBLE_VALUE_DIGIT,
        DOUBLE_VALUE_DIGIT_SCALE, DOUBLE_VALUE_DIGIT_SCALE_SIGN, DOUBLE_VALUE_DIGIT_SCALE_SIGN_DIGIT,
        EQUAL, EXCLAMATION_POINT, GREATER_THAN, GREATER_THAN_EQUAL, GREATER_THAN_GREATER_THAN,
        GREATER_THAN_LESS_THAN, HASH, HEX_INTEGER_VALUE, IDENTIFIER, INTEGER_VALUE, INTEGER_VALUE_PERIOD_PERIOD,
        LEFT_BRACE, LEFT_BRACKET, LEFT_PARENTHESIS, LEFT_PARENTHESIS_ASTERISK, LEFT_PARENTHESIS_ASTERISK_ASTERISK,
        LESS_THAN, LESS_THAN_EQUAL, LESS_THAN_GREATER_THAN, LESS_THAN_LESS_THAN, MINUS, MINUS_EQUAL, NOT_TOKEN,
        OCTAL_INTEGER_VALUE, PERCENT, PERIOD, PERIOD_PERIOD, PLUS, PLUS_EQUAL, POINTER, QUOTE, RIGHT_BRACE,
        RIGHT_BRACKET, RIGHT_PARENTHESIS, SEMI_COLON, SLASH, SLASH_EQUAL, SLASH_SLASH, SPACE, STRING,
        STRING_HASH, STRING_HASH_INTEGER_VALUE, VERTICAL_LINE
};

static std::map<State, std::vector<State>> statesTable;