#pragma once
#include "Scanner.h"
#include <vector>
#include <set>

class Node;

typedef Node* PNode;

class Node {
public:
    Token token;
    std::vector<PNode> children;
    Node(Token token, std::vector<PNode> &&children = {});
    Node() {};
};

class Parser {
public:
    Parser(const char* fileName);
    const PNode GetTree() const;
    void PrintTree(PNode pnode, int size);
private:
    PNode tree;
    PNode ParseFactor();
    PNode ParseExpression(PNode left, int priority);
    PNode Parse(int priority);
    PNode ParseUnOperator();
    PNode ParseIdentifier(PNode left);
    PNode ParseParenthesis();
    Scanner scanner;
    std::vector<std::set<State>> priorityTable;
    bool CheckCorrectToken(int priority);
};