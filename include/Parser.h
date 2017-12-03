#pragma once
#include <vector>
#include <set>
#include <vector>
#include "Scanner.h"
#include "Error.h"
#include "Symbols.h"
#include "Node.h"

enum class ExprType {
    Const,
    Var
};

enum class ParserConfig {
    ParseExpression,
    ParseProgram
};

enum class Priority {
    Zero   = 0,
    First  = 1,
    Second = 2,
    Third  = 3
};

class Parser {
public:
    Parser(const char* fileName, ParserConfig);
    const PNode GetTree() const;
    void PrintTree();
    void PrintStack();
private:
    PScanner scanner;
    void PrintTree(PNode node);
    ParserConfig parserConfig;
    PNode tree;
    PSymbolTableStack tableStack;
    PNodeOp ParseFactor(ExprType);
    void AddBaseTypesToTable(PSymbolTable);
    void CreateGlobalTable();
    void Run();
    PNodeOp ParseExpression(PNodeOp left, int priority, ExprType);
    PNodeOp ParseExpression(int priority, ExprType);
    PNodeOp ParseExpression(ExprType = ExprType::Var);
    PNodeOp ParseUnOperator(ExprType);
    PNode ParseForStatement();
    PNodeOp ParseExprIdentifier(PNodeOp left, ExprType);
    PNodeOp ParseParenthesis(ExprType exprType);
    void ParseProgram();
    PNode ParseStatement();
    PSymbolComplex ParseExistingIdentifier();
    std::string ParseIdentifier(PSymbolTable);
    std::set<std::string> ParseIdentifiers(PSymbolTable);
    PNodeAssignmentOp ParseAssignmentOperator();
    void ParseFunctionDeclaration(PSymbolTable);
    PNodeOp ParseExprParameters(PNodeStructed node, State stopState, ExprType);
    void ParseSemiColons();
    void ParseSemiColon();
    PNode ParseRepeatStatement();
    PNode ParseWhileStatement();
    PNode ParseProcedureStatement();
    PSymbolBase ParseSimpleType();
    void ParseConstDeclaration(PSymbolTable table);
    PNode ParseComplexStatement();
    PNode ParseCompoundStatement();
    PNode ParseLocalCompoundStatement();
    PNode ParseGlobalCompoundStatement();
    PNode ParseIfStatement();
    PNodeAssignmentOp ParseAssignmentStatement(PNodeOp);
    PSymbolSubRange ParseSubRange();
    void ParseVarDeclaration(PSymbolTable table);
    void ParseTypeDeclaration(PSymbolTable table);
    PSymbolRecord ParseRecord();
    void ParseDeclaration(PSymbolTable table);
    void ParseProcedureDeclaration(PSymbolTable table);
    void ParseExpParameterList(PSymbolTable table);
    void ParseParameterList(PSymbolTable table);
    void ParseValueParameters(PSymbolTable table);
    PSymbolBase ParseType();
    PSymbolArray ParseArray();
    PSymbolStaticArray ParseStaticArray(std::vector<PSymbolSubRange> &dimensions);
    PSymbolDynamicArray ParseDynamicArray();
    std::vector<PSymbolSubRange> ParseArrayBrackets();
    void CalcNodeType(PNodeOp);
    std::any ParseValue(PSymbolBase type, int size);
    PSymbolType ParseExistingType();
    std::vector<std::set<State>> priorityTable;
    bool CheckNodeType(PNodeOp, std::string);
    void SetCorrectType(PNodeOp, std::string);
    bool CheckCorrectToken(int priority);
    bool CheckTokenType(PToken, TK::TokenType);
    bool CheckTokenState(PToken, State);
    bool CheckTokenState(PToken, State, State);
};