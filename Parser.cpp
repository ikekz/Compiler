#include "Parser.h"
#include <iostream>
#include "Error.h"

using namespace std;

Node::Node(Token token, std::vector<PNode> &&children) : token(token), children(children) {}

void Parser::PrintTree(PNode pnode, int size) {
    for (int i = 0; i < size; i++) {
        cout << "\t";
    }
    if (pnode->token.GetState() == LEFT_BRACKET)
        cout << "[]" << endl;
    else
        cout << pnode->token.GetValue() << endl;
    for (int i = 0; i < pnode->children.size(); i++) {
        PrintTree(pnode->children[i], size + 1);
    }
}

Parser::Parser(const char* fileName) : scanner(fileName) {
    priorityTable = {
            {
                    NOT,
                    PLUS,
                    MINUS
            },
            {
                    ASTERISK,
                    SLASH,
                    DIV,
                    MOD,
                    AND
            },
            {
                    VERTICAL_LINE,
                    PLUS,
                    MINUS,
                    OR
            },
            {
                    EQUAL,
                    LESS_THAN_GREATER_THAN,
                    LESS_THAN,
                    LESS_THAN_EQUAL,
                    GREATER_THAN,
                    GREATER_THAN_EQUAL,
                    IN
            }
    };

    tree = Parse(3);
}

bool Parser::CheckCorrectToken(int priority) {
    return priorityTable[priority].find(scanner.GetToken().GetState()) != priorityTable[priority].end();
}

PNode Parser::Parse(int priority) {
    PNode pnode = ParseFactor();
    for (int i = 1; i <= priority; i++)
        pnode = ParseExpression(pnode, i);
    return pnode;
}

PNode Parser::ParseExpression(PNode left, int priority) {
    scanner.NextToken();
    Token token = scanner.GetToken();
    if (!CheckCorrectToken(priority)) {
        scanner.PrevToken();
        return left;
    }
    else {
        PNode right = Parse(priority - 1);
        PNode pnode = new Node(token, { left, right });
        return ParseExpression(pnode, priority);
    }
}

PNode Parser::ParseIdentifier(PNode left) {
    scanner.NextToken();
    Token token = scanner.GetToken();
    switch (token.GetState()) {
        case PERIOD: {
            scanner.NextToken();
            Token newtoken = scanner.GetToken();
            if (newtoken.GetType() == TK_IDENTIFIER) {
                PNode right = new Node(newtoken, {});
                PNode pnode = new Node(token, { left, right });
                return ParseIdentifier(pnode);
            }
            throw Error("Expected identifier", newtoken.GetLine(), newtoken.GetColumn());
        }
        case LEFT_BRACKET: {
            PNode pnode = new Node(token, { left });

            PNode right = Parse(3);
            pnode->children.push_back(right);
            scanner.NextToken();

            State state = scanner.GetToken().GetState();
            while (state != RIGHT_BRACKET) {
                if (scanner.GetToken().GetState() == COMMA) {
                    right = Parse(3);
                    pnode->children.push_back(right);
                }
                else
                    throw Error("Expected ']'", scanner.GetToken().GetLine(), scanner.GetToken().GetColumn());
                scanner.NextToken();
                state = scanner.GetToken().GetState();
            }
            scanner.NextToken();
            state = scanner.GetToken().GetState();
            if (state == PERIOD || state == LEFT_BRACKET) {
                scanner.PrevToken();
                return ParseIdentifier(pnode);
            }
            scanner.PrevToken();
            return pnode;
        }
        default: {
            break;
        }
    }
    scanner.PrevToken();
    return left;
}

PNode Parser::ParseUnOperator() {
    Token token = scanner.GetToken();
    if (CheckCorrectToken(0)) {
        PNode pnode = new Node(token, { ParseFactor() });
        return pnode;
    }
    throw Error("Unexpected expression", token.GetLine(), token.GetColumn());
}

PNode Parser::ParseParenthesis() {
    Token token = scanner.GetToken();
    if (token.GetState() == LEFT_PARENTHESIS) {
        PNode pnode = Parse(3);
        scanner.NextToken();
        token = scanner.GetToken();
        if (token.GetState() == RIGHT_PARENTHESIS)
            return pnode;
        else
            throw Error("Expected ')'", token.GetLine(), token.GetColumn());
    }
    throw Error("Unexpected expression", token.GetLine(), token.GetColumn());
}

PNode Parser::ParseFactor() {
    scanner.NextToken();
    Token token = scanner.GetToken();
    switch (token.GetType()) {
        case TK_INTEGER: {
            return new Node(token);
        }
        case TK_DOUBLE: {
            return new Node(token);
        }
        case TK_IDENTIFIER: {
            return ParseIdentifier(new Node(token));
        }
        case TK_OPERATOR: {
            return ParseUnOperator();
        }
        case TK_RESERVED_WORD: {
            return ParseUnOperator();
        }
        case TK_SEPARATOR: {
            return ParseParenthesis();
        }
        default: {
            break;
        }
    }
    throw Error("Unexpected expression", token.GetLine(), token.GetColumn());
}

const PNode Parser::GetTree() const {
    return tree;
}
