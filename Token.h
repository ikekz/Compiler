#pragma once
#include <string>
#include <map>
#include <vector>
#include <set>

enum State {
    AMPERSAND,
    ASTERISK,
    ASTERISK_EQUAL,
    ASTERISK_RIGHT_PARENTHESIS,
    AT,
    BIN_INTEGER_VALUE,
    COLON,
    COLON_EQUAL,
    COMMA,
    DOLLAR,
    DOUBLE_VALUE,
    DOUBLE_VALUE_DIGIT,
    DOUBLE_VALUE_DIGIT_SCALE,
    DOUBLE_VALUE_DIGIT_SCALE_SIGN,
    DOUBLE_VALUE_DIGIT_SCALE_SIGN_DIGIT,
    EQUAL, ERROR_INVALID_COMMENT,
    ERROR_INVALID_EXPRESSION,
    ERROR_INVALID_STRING,
    EXCLAMATION_POINT,
    GREATER_THAN,
    GREATER_THAN_EQUAL,
    GREATER_THAN_GREATER_THAN,
    GREATER_THAN_LESS_THAN,
    HASH, HEX_INTEGER_VALUE,
    IDENTIFIER,
    INTEGER_VALUE,
    INTEGER_VALUE_PERIOD_PERIOD,
    LEFT_BRACE,
    LEFT_BRACKET,
    LEFT_PARENTHESIS,
    LEFT_PARENTHESIS_ASTERISK,
    LEFT_PARENTHESIS_ASTERISK_ASTERISK,
    LESS_THAN, LESS_THAN_EQUAL,
    LESS_THAN_GREATER_THAN,
    LESS_THAN_LESS_THAN,
    MINUS,
    MINUS_EQUAL,
    NOT_TOKEN,
    OCTAL_INTEGER_VALUE,
    PERCENT,
    PERIOD,
    PERIOD_PERIOD,
    PLUS,
    PLUS_EQUAL,
    POINTER,
    QUOTE,
    RIGHT_BRACE,
    RIGHT_BRACKET,
    RIGHT_PARENTHESIS,
    SEMI_COLON,
    SLASH,
    SLASH_EQUAL,
    SLASH_SLASH,
    SPACE,
    ST_EOF,
    STRING,
    STRING_HASH,
    STRING_HASH_INTEGER_VALUE,
    VERTICAL_LINE,

    ABSOLUTE,
    AND,
    ARRAY,
    ASM,
    BEGIN,
    CASE,
    CONST,
    CONSTRUCTOR,
    DESTRUCTOR,
    DISPOSE,
    DIV,
    DO,
    DOWNTO,
    ELSE,
    END,
    EXIT,
    FALSE,
    ST_FILE,
    FOR,
    FUNCTION,
    GOTO,
    IF,
    IMPLEMENTATION,
    IN,
    INHERITED,
    INLINE,
    INTERFACE,
    LABEL,
    MOD,
    NEW,
    NIL,
    NOT,
    OBJECT,
    OF,
    OPERATOR,
    OR,
    PACKED,
    PROCEDURE,
    PROGRAM,
    RECORD,
    REINTRODUCE,
    REPEAT,
    SELF,
    SET,
    SHL,
    SHR,
    ST_STRING,
    THEN,
    TO,
    TRUE,
    TYPE,
    UNIT,
    UNTIL,
    USES,
    VAR,
    WHILE,
    WITH,
    XOR
};

static std::map<std::string, State> reservedWordState = {
        {"absolute", ABSOLUTE},
        {"and", AND},
        {"array", ARRAY},
        {"asm", ASM},
        {"begin", BEGIN},
        {"case", CASE},
        {"const", CONST},
        {"constructor", CONSTRUCTOR},
        {"destructor", DESTRUCTOR},
        {"dispose", DISPOSE},
        {"div", DIV},
        {"do", DO},
        {"downto", DOWNTO},
        {"else", ELSE},
        {"end", END},
        {"exit", EXIT},
        {"false", FALSE},
        {"file", ST_FILE},
        {"for", FOR},
        {"function", FUNCTION},
        {"goto", GOTO},
        {"if", IF},
        {"implementation", IMPLEMENTATION},
        {"in", IN},
        {"inherited", INHERITED},
        {"inline", INLINE},
        {"interface", INTERFACE},
        {"label", LABEL},
        {"mod", MOD},
        {"new", NEW},
        {"nil", NIL},
        {"not", NOT},
        {"object", OBJECT},
        {"of", OF},
        {"operator", OPERATOR},
        {"or", OR},
        {"packed", PACKED},
        {"procedure", PROCEDURE},
        {"program", PROGRAM},
        {"record", RECORD},
        {"reintroduce", REINTRODUCE},
        {"repeat", REPEAT},
        {"self", SELF},
        {"set", SET},
        {"shl", SHL},
        {"shr", SHR},
        {"string", ST_STRING},
        {"then", THEN},
        {"to", TO},
        {"true", TRUE},
        {"type", TYPE},
        {"unit", UNIT},
        {"until", UNTIL},
        {"uses", USES},
        {"var", VAR},
        {"while", WHILE},
        {"with", WITH},
        {"xor", XOR}
};

enum TokenType {
    TK_DOUBLE,
    TK_EOF,
    TK_IDENTIFIER,
    TK_INTEGER,
    TK_OPERATOR,
    TK_RESERVED_WORD,
    TK_SEPARATOR,
    TK_STRING
};

static std::map<TokenType, std::string> tokenName = {
        {TK_DOUBLE, "double"},
        {TK_EOF, "EOF"},
        {TK_IDENTIFIER, "identifier"},
        {TK_INTEGER, "integer"},
        {TK_OPERATOR, "operator"},
        {TK_RESERVED_WORD, "reserved word"},
        {TK_SEPARATOR, "separator"},
        {TK_STRING, "string"}
};

static std::map<State, TokenType> tokenType = {
        {DOUBLE_VALUE, TK_DOUBLE},
        {DOUBLE_VALUE_DIGIT, TK_DOUBLE},
        {DOUBLE_VALUE_DIGIT_SCALE_SIGN_DIGIT, TK_DOUBLE},

        {ST_EOF, TK_EOF},

        {IDENTIFIER, TK_IDENTIFIER},

        {BIN_INTEGER_VALUE, TK_INTEGER},
        {HEX_INTEGER_VALUE, TK_INTEGER},
        {INTEGER_VALUE, TK_INTEGER},
        {OCTAL_INTEGER_VALUE, TK_INTEGER},

        {ASTERISK, TK_OPERATOR},
        {ASTERISK_EQUAL, TK_OPERATOR},
        {AT, TK_OPERATOR},
        {COLON_EQUAL, TK_OPERATOR},
        {EQUAL, TK_OPERATOR},
        {GREATER_THAN, TK_OPERATOR},
        {GREATER_THAN_EQUAL, TK_OPERATOR},
        {GREATER_THAN_GREATER_THAN, TK_OPERATOR},
        {GREATER_THAN_LESS_THAN, TK_OPERATOR},
        {LESS_THAN, TK_OPERATOR},
        {LESS_THAN_EQUAL, TK_OPERATOR},
        {LESS_THAN_GREATER_THAN, TK_OPERATOR},
        {LESS_THAN_LESS_THAN, TK_OPERATOR},
        {MINUS, TK_OPERATOR},
        {MINUS_EQUAL, TK_OPERATOR},
        {PERIOD_PERIOD, TK_OPERATOR},
        {PLUS, TK_OPERATOR},
        {PLUS_EQUAL, TK_OPERATOR},
        {POINTER, TK_OPERATOR},
        {SLASH_EQUAL, TK_OPERATOR},
        {VERTICAL_LINE, TK_OPERATOR},

        {COLON, TK_SEPARATOR},
        {COMMA, TK_SEPARATOR},
        {LEFT_BRACKET, TK_SEPARATOR},
        {LEFT_PARENTHESIS, TK_SEPARATOR},
        {PERIOD, TK_SEPARATOR},
        {RIGHT_BRACKET, TK_SEPARATOR},
        {RIGHT_PARENTHESIS, TK_SEPARATOR},
        {SEMI_COLON, TK_SEPARATOR},
        {SPACE, TK_SEPARATOR},

        {STRING, TK_STRING}
};

class Token {
public:
    int GetLine() const;
    void SetLine(int line);
    int GetColumn() const;
    void SetColumn(int column);
    State GetState() const;
    void SetState(State state);
    const std::string &GetText() const;
    void SetText(const std::string &Text);
    void CalcValue();
    void CalcType();
    void SetType(TokenType type);
    TokenType GetType() const;
    const std::string &GetValue() const;
private:
    int line, column;
    State state;
    TokenType type;
    std::string text;
    std::string value;
    void DoubleValue();
    void StringValue();
    void IntegerValue();
    void IdentifierValue();
    void EofValue();
    std::string IntToValue(int base);
};