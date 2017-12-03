#pragma once
#include <string>
#include <map>
#include <vector>
#include <set>
#include <memory>

enum State { 
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
    ErrorInvalidComment,
    ErrorInvalidExpression,
    ErrorInvalidString,
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
    EOFF,
    String,
    StringHash,
    StringHashIntegerValue,
    VerticalLine,

    Absolute,
    And,
    Array,
    Asm,
    Begin,
    Case,
    Const,
    Constructor,
    Destructor,
    Dispose,
    Div,
    Do,
    Downto,
    Else,
    End,
    Exit,
    False,
    Filee,
    For,
    Function,
    Goto,
    If,
    Implementation,
    In,
    Inherited,
    Inline,
    Interface,
    Label,
    Mod,
    New,
    Nil,
    Not,
    Object,
    Of,
    Operator,
    Or,
    Packed,
    Procedure,
    Program,
    Record,
    Reintroduce,
    Repeat,
    Self,
    Set,
    Shl,
    Shr,
    Stringg,
    Then,
    To,
    True,
    Type,
    Unit,
    Until,
    Uses,
    Var,
    While,
    With,
    Xor
};

static std::map<std::string, State> reservedWordState = { 
        { "absolute",       Absolute       },
        { "and",            And            },
        { "array",          Array          },
        { "asm",            Asm            },
        { "begin",          Begin          },
        { "case",           Case           },
        { "const",          Const          },
        { "constructor",    Constructor    },
        { "destructor",     Destructor     },
        { "dispose",        Dispose        },
        { "div",            Div            },
        { "do",             Do             },
        { "downto",         Downto         },
        { "else",           Else           },
        { "end",            End            },
        { "exit",           Exit           },
        { "false",          False          },
        { "file",           Filee          },
        { "for",            For            },
        { "function",       Function       },
        { "goto",           Goto           },
        { "if",             If             },
        { "implementation", Implementation },
        { "in",             In             },
        { "inherited",      Inherited      },
        { "inline",         Inline         },
        { "interface",      Interface      },
        { "label",          Label          },
        { "mod",            Mod            },
        { "new",            New            },
        { "nil",            Nil            },
        { "not",            Not            },
        { "object",         Object         },
        { "of",             Of             },
        { "operator",       Operator       },
        { "or",             Or             },
        { "packed",         Packed         },
        { "procedure",      Procedure      },
        { "program",        Program        },
        { "record",         Record         },
        { "reintroduce",    Reintroduce    },
        { "repeat",         Repeat         },
        { "self",           Self           },
        { "set",            Set            },
        { "shl",            Shl            },
        { "shr",            Shr            },
        { "string",         Stringg        },
        { "then",           Then           },
        { "to",             To             },
        { "true",           True           },
        { "type",           Type           },
        { "unit",           Unit           },
        { "until",          Until          },
        { "uses",           Uses           },
        { "var",            Var            },
        { "while",          While          },
        { "with",           With           },
        { "xor",            Xor            }
};

namespace TK {
    enum TokenType {
        Double,
        EOFF,
        Identifier,
        Integer,
        Operator,
        ReservedWord,
        Separator,
        String
    };
}

static std::map<TK::TokenType, std::string> tokenName = {
        { TK::Double,       "double"        },
        { TK::EOFF,         "EOF"           },
        { TK::Identifier,   "identifier"    },
        { TK::Integer,      "integer"       },
        { TK::Operator,     "operator"      },
        { TK::ReservedWord, "reserved word" },
        { TK::Separator,    "separator"     },
        { TK::String,       "string"        }
};

static std::map<State, std::string> stateName = {
        { Begin,            "begin" },
        { End,              "end"   },
        { Period,           "."     },
        { Equal,            "="     },
        { SemiColon,        ";"     },
        { Of,               "of"    },
        { RightBracket,     "]"     },
        { Colon,            ":"     },
        { RightParenthesis, ")"     },
        { Then,             "then"  },
        { To,               "to"    },
        { ColonEqual,       ":="    },
        { Do,               "do"    }
};

static std::map<State, TK::TokenType> tokenType = {
        { DoubleValue,                    TK::Double },
        { DoubleValueDigit,               TK::Double },
        { DoubleValueDigitScaleSignDigit, TK::Double },

        { EOFF, TK::EOFF },

        { Identifier, TK::Identifier },

        { BinIntegerValue,   TK::Integer },
        { HexIntegerValue,   TK::Integer },
        { IntegerValue,      TK::Integer },
        { OctalIntegerValue, TK::Integer },

        { Asterisk,               TK::Operator },
        { AsteriskEqual,          TK::Operator },
        { At,                     TK::Operator },
        { ColonEqual,             TK::Operator },
        { Equal,                  TK::Operator },
        { GreaterThan,            TK::Operator },
        { GreaterThanEqual,       TK::Operator },
        { GreaterThanGreaterThan, TK::Operator },
        { GreaterThanLessThan,    TK::Operator },
        { LessThan,               TK::Operator },
        { LessThanEqual,          TK::Operator },
        { LessThanGreaterThan,    TK::Operator },
        { LessThanLessThan,       TK::Operator },
        { Minus,                  TK::Operator },
        { MinusEqual,             TK::Operator },
        { PeriodPeriod,           TK::Operator },
        { Plus,                   TK::Operator },
        { PlusEqual,              TK::Operator },
        { Pointer,                TK::Operator },
        { Slash,                  TK::Operator },
        { SlashEqual,             TK::Operator },
        { VerticalLine,           TK::Operator },

        { Colon,            TK::Separator },
        { Comma,            TK::Separator },
        { LeftBracket,      TK::Separator },
        { LeftParenthesis,  TK::Separator },
        { Period,           TK::Separator },
        { RightBracket,     TK::Separator },
        { RightParenthesis, TK::Separator },
        { SemiColon,        TK::Separator },
        { Space,            TK::Separator },

        { String, TK::String }
};

static std::set<State> assignmentOperators = {
        ColonEqual,
        PlusEqual,
        MinusEqual,
        AsteriskEqual,
        SlashEqual
};

class Token;
typedef std::shared_ptr<Token> PToken;

class Token {
public:
    int GetLine() const;
    void SetLine(int line);
    int GetColumn() const;
    void SetColumn(int column);
    State GetState() const;
    void SetState(State);
    const std::string &GetText() const;
    void SetText(const std::string &Text);
    void CalcValue();
    void CalcType();
    void SetType(TK::TokenType type);
    TK::TokenType GetType() const;
    const std::string &GetValue() const;
private:
    int line, column;
    State state;
    TK::TokenType type;
    std::string text;
    std::string value;
    void DoubleValue();
    void StringValue();
    void IntegerValue();
    void IdentifierValue();
    void EofValue();
    std::string IntToValue(int base);
};