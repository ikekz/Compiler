#include "Scanner.h"
#include "Error.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>

using namespace std;

Scanner::Scanner(const char* fileName) : line(1), column(0), token(new Token) {
    fin.open(fileName);
    InitStatesTable();
    FillStatesTable();
}

Scanner::~Scanner() {
    fin.close();
}

void Scanner::PrevToken() {
    if (token->GetType() != TK::EOFF) {
        gettedToken = true;
        UnGetChar((int) token->GetText().size());
    }
}

char Scanner::ReadChar() {
    char symb;
    fin.get(symb);
    if (fin.eof())
        symb = '\0';
    if (symb == '\n') {
        line++;
        prevColumn = column;
        column = 0;
    } else {
        prevColumn = column;
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
            column = prevColumn;
        } else {
            prevColumn = column;
            column--;
        }
    }
}

void Scanner::InitStatesTable() {
    for (auto const& state: states)
        statesTable[state].resize(128, NotToken);
}

void Scanner::SetState(State oldstate, int transition, State newstate) {
    statesTable[oldstate][transition] = newstate;
}

void Scanner::CheckError(State state) {
    std::string text;
    if (state == ErrorInvalidExpression)
        throw Error(ErrorType::InvalidExpression, *token);
    else if (state == ErrorInvalidString)
        throw Error(ErrorType::InvalidString, *token);
    else if (state == ErrorInvalidComment)
        throw Error(ErrorType::InvalidComment, *token);
}

bool Scanner::IsComment(State state) {
    return state == RightBrace || state == AsteriskRightParenthesis || state == SlashSlash;
}

void Scanner::CheckReservedWord() {
    string text = token->GetText();
    transform(text.begin(), text.end(), text.begin(), ::tolower);
    auto st = reservedWordState.find(text);
    if (st != reservedWordState.end()) {
        token->SetState(st->second);
        token->SetType(TK::ReservedWord);
    }
}

void Scanner::NextToken() {
    gettedToken = false;
    ClearToken();
    bool isToken = false;
    while (!IsEndOfFile()) {
        char symb = ReadChar();
        State state = statesTable[token->GetState()][symb];

        if (!isToken && state != NotToken) {
            isToken = true;
            token->SetColumn(column);
            token->SetLine(line);
        }

        CheckError(state);

        if (state == NotToken) {
            if (!isToken)
                continue;

            UnGetChar();

            State lastState = token->GetState();
            if (IsComment(lastState)) {
                ClearToken();
                isToken = false;
                continue;
            }
            if (lastState == IntegerValuePeriodPeriod) {
                UnGetChar(2);
                string text = token->GetText();
                token->SetText(text.substr(0, text.size() - 2));
                token->SetState(IntegerValue);
            }
            break;
        }
        token->SetState(state);
        token->SetText(token->GetText() + symb);
    }
    token->CalcValue();
    token->CalcType();
    if (token->GetState() == Identifier)
        CheckReservedWord();
}

PToken Scanner::GetNextToken() {
    NextToken();
    return GetToken();
}

void Scanner::PrintToken() {
    if (token->GetType() == TK::EOFF)
        return;

    cout << left << setw(lineWidth) <<  token->GetLine();
    cout << setw(columnWidth) << token->GetColumn();
    cout << setw(typeWidth) << tokenName[token->GetType()];
    cout << setw(textWidth) << token->GetText();
    cout << token->GetValue() << endl;
}

bool Scanner::IsEndOfFile() {
    return fin.eof();
}

void Scanner::ClearToken() {
    token->SetColumn(column);
    token->SetLine(line);
    token->SetText("");
    token->SetState(NotToken);
}

bool Scanner::IsGettedToken() {
    return gettedToken;
}

void Scanner::SetStates(State state, std::vector<std::pair<char, State>> ts) {
    for (auto const& i: ts)
        SetState(state, i.first, i.second);
}

void Scanner::SetStatesInRange(State state, std::vector<std::tuple<char, char, State>> ts) {
    for (int i = 0; i < ts.size(); i++)
        for (int j = get<0>(ts[i]); j <= (int)get<1>(ts[i]); j++)
            SetState(state, j, get<2>(ts[i]));
}

void Scanner::FillStatesTable() {
    SetStatesInRange(NotToken, {
            { 'a', 'z', Identifier   },
            { 'A', 'Z', Identifier   },
            { '0', '9', IntegerValue },
    });
    SetStates(NotToken, {
            { '*',  Pointer             },
            { ',',  Comma               },
            { '.',  Period              },
            { '@',  At                  },
            { '#',  Hash                },
            { '<',  LessThan            },
            { '>',  GreaterThan         },
            { ':',  Colon               },
            { ';',  SemiColon           },
            { '=',  Equal               },
            { '|',  VerticalLine        },
            { '!',  ExclamationPoint    },
            { '*',  Asterisk            },
            { '\'', Quote               },
            { '/',  Slash               },
            { '(',  LeftParenthesis     },
            { '{',  LeftBrace           },
            { '[',  LeftBracket         },
            { ')',  RightParenthesis    },
            { ']',  RightBracket        },
            { ' ',  NotToken            },
            { '\n', NotToken            },
            { '_',  Identifier          },
            { '$',  Dollar              },
            { '&',  Ampersand           },
            { '%',  Percent             },
            { '+',  Plus                },
            { '-',  Minus               },
            { '}',  ErrorInvalidComment },
            { '\0', EOFF                }
    });

    SetState(Period, '.', PeriodPeriod);

    SetStates(LessThan, {
            { '=', LessThanEqual       },
            { '>', LessThanGreaterThan },
            { '<', LessThanLessThan    }
    });

    SetStates(GreaterThan, {
            { '=', GreaterThanEqual       },
            { '<', GreaterThanLessThan    },
            { '>', GreaterThanGreaterThan }
    });

    SetState(Colon, '=', ColonEqual);

    SetStates(Asterisk, {
            { '=', AsteriskEqual       },
            { ')', ErrorInvalidComment }
    });

    SetStates(Slash, {
            { '/', SlashSlash },
            { '=', SlashEqual }
    });

    SetStatesInRange(SlashSlash, {
            { 0, 127, SlashSlash }
    });
    SetState(SlashSlash, '\n', NotToken);

    SetStatesInRange(Identifier, {
            { 'a', 'z', Identifier },
            { 'A', 'Z', Identifier },
            { '0', '9', Identifier }
    });
    SetState(Identifier, '_', Identifier);

    SetStatesInRange(IntegerValue, {
            { 'a', 'z', ErrorInvalidExpression },
            { 'A', 'Z', ErrorInvalidExpression },
            { '0', '9', IntegerValue           }
    });
    SetStates(IntegerValue, {
            { '_', ErrorInvalidExpression },
            { '.', DoubleValue            },
            { 'E', DoubleValueDigitScale  },
            { 'e', DoubleValueDigitScale  }
    });

    SetStatesInRange(Dollar, {
            { 'a', 'f', HexIntegerValue        },
            { 'g', 'a', ErrorInvalidExpression },
            { 'A', 'F', HexIntegerValue        },
            { 'G', 'Z', ErrorInvalidExpression },
            { '0', '9', HexIntegerValue        }
    });

    SetStatesInRange(Ampersand, {
            { 'a', 'z', Identifier             },
            { 'A', 'Z', Identifier             },
            { '0', '7', OctalIntegerValue      },
            { '8', '9', ErrorInvalidExpression }
    });
    SetState(Ampersand, '_', Identifier);

    SetStatesInRange(Percent, {
            { 'a', 'z', ErrorInvalidExpression },
            { 'A', 'Z', ErrorInvalidExpression },
            { '0', '1', BinIntegerValue        },
            { '2', '9', ErrorInvalidExpression }
    });

    SetStatesInRange(HexIntegerValue, {
            { 'a', 'f', HexIntegerValue        },
            { 'g', 'a', ErrorInvalidExpression },
            { 'A', 'F', HexIntegerValue        },
            { 'G', 'Z', ErrorInvalidExpression },
            { '0', '9', HexIntegerValue        }
    });
    SetState(HexIntegerValue, '_', ErrorInvalidExpression);

    SetStatesInRange(OctalIntegerValue, {
            { 'a', 'z', ErrorInvalidExpression },
            { 'A', 'Z', ErrorInvalidExpression },
            { '0', '7', OctalIntegerValue      },
            { '8', '9', ErrorInvalidExpression }
    });
    SetState(OctalIntegerValue, '_', ErrorInvalidExpression);

    SetStatesInRange(BinIntegerValue, {
            { 'a', 'z', ErrorInvalidExpression },
            { 'A', 'Z', ErrorInvalidExpression },
            { '0', '1', BinIntegerValue        },
            { '2', '9', ErrorInvalidExpression }
    });
    SetState(BinIntegerValue, '_', ErrorInvalidExpression);

    SetStatesInRange(DoubleValue, {
            { 'a', 'z', ErrorInvalidExpression },
            { 'A', 'Z', ErrorInvalidExpression },
            { '0', '9', DoubleValueDigit       }
    });
    SetStates(DoubleValue, {
            { '_', ErrorInvalidExpression   },
            { '.', IntegerValuePeriodPeriod }
    });

    SetStatesInRange(DoubleValueDigit, {
            { 'a', 'z', ErrorInvalidExpression },
            { 'A', 'Z', ErrorInvalidExpression },
            { '0', '9', DoubleValueDigit       }
    });
    SetStates(DoubleValueDigit, {
            { '_', ErrorInvalidExpression },
            { '.', ErrorInvalidExpression },
            { 'E', DoubleValueDigitScale  },
            { 'e', DoubleValueDigitScale  }
    });

    SetStatesInRange(DoubleValueDigitScale, {
            { 0, 127,   ErrorInvalidExpression         },
            { '0', '9', DoubleValueDigitScaleSignDigit },
    });
    SetStates(DoubleValueDigitScale, {
            { '+', DoubleValueDigitScaleSign },
            { '-', DoubleValueDigitScaleSign }
    });

    SetStatesInRange(DoubleValueDigitScaleSign, {
            { 0, 127,   ErrorInvalidExpression         },
            { '0', '9', DoubleValueDigitScaleSignDigit },
    });

    SetStatesInRange(DoubleValueDigitScaleSignDigit, {
            { 'a', 'z', ErrorInvalidExpression         },
            { 'A', 'Z', ErrorInvalidExpression         },
            { '0', '9', DoubleValueDigitScaleSignDigit },
    });
    SetStates(DoubleValueDigitScaleSignDigit, {
            { '.', ErrorInvalidExpression },
            { '_', ErrorInvalidExpression }
    });

    SetState(Minus, '=', MinusEqual);

    SetState(Plus, '=', PlusEqual);

    SetStatesInRange(LeftBrace, {
            { 0, 127, LeftBrace }
    });
    SetStates(LeftBrace, {
            { '}',  RightBrace          },
            { '\0', ErrorInvalidComment }
    });

    SetState(LeftParenthesis, '*', LeftParenthesisAsterisk);

    SetStatesInRange(LeftParenthesisAsterisk, {
            { 0, 127, LeftParenthesisAsterisk }
    });
    SetStates(LeftParenthesisAsterisk, {
            { '*',  LeftParenthesisAsteriskAsterisk },
            { '\0', ErrorInvalidComment              }
    });

    SetStatesInRange(LeftParenthesisAsteriskAsterisk, {
            { 0, 127, LeftParenthesisAsterisk }
    });
    SetStates(LeftParenthesisAsteriskAsterisk, {
            { '*',  LeftParenthesisAsteriskAsterisk },
            { ')',  AsteriskRightParenthesis        },
            { '\0', ErrorInvalidComment             }
    });

    SetStatesInRange(Quote, {
            { 0, 127, Quote }
    });
    SetStates(Quote, {
            { '\'', String             },
            { '\0', ErrorInvalidString },
            { '\n', ErrorInvalidString }
    });

    SetStates(String, {
            { '#',  StringHash },
            { '\'', Quote      }
    });

    SetStatesInRange(StringHash, {
            { 0, 127,   ErrorInvalidExpression },
            { '0', '9', StringHashIntegerValue }
    });

    SetStatesInRange(StringHashIntegerValue, {
            { 0, 127,   ErrorInvalidExpression },
            { '0', '9', StringHashIntegerValue }
    });
    SetStates(StringHashIntegerValue, {
            { '#',  StringHash },
            { '\'', Quote      }
    });
}

const PToken Scanner::GetToken() const {
    return token;
}
