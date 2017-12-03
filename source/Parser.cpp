#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include "Parser.h"
#include "Error.h"

using namespace std;

void Parser::PrintTree() {
    PrintTree(tree);
}

void Parser::PrintTree(PNode node) {
    node->Print();
}

Parser::Parser(const char* fileName, ParserConfig parserConfig) :
        scanner(new Scanner(fileName)), parserConfig(parserConfig),
        tableStack(new SymbolTableStack()) {
    priorityTable = {
            {
                    Not,
                    Plus,
                    Minus
            },
            {
                    Asterisk,
                    Slash,
                    Div,
                    Mod,
                    And,
                    Shr,
                    GreaterThanGreaterThan,
                    Shl,
                    LessThanLessThan
            },
            {
                    Plus,
                    Minus,
                    Or,
                    Xor
            },
            {
                    Equal,
                    LessThanGreaterThan,
                    LessThan,
                    GreaterThan,
                    LessThanEqual,
                    GreaterThanEqual
            }
    };

    CreateGlobalTable();
    Run();
}

void Parser::Run() {
    if (parserConfig == ParserConfig::ParseExpression)
        tree = ParseExpression(ExprType::Var);
    else if (parserConfig == ParserConfig::ParseProgram)
        ParseProgram();
}

bool Parser::CheckCorrectToken(int priority) {
    return priorityTable[priority].find(scanner->GetToken()->GetState()) != priorityTable[priority].end();
}

PNodeOp Parser::ParseExpression(int priority, ExprType exprType) {
    PNodeOp node = ParseFactor(exprType);
    for (int i = static_cast<int>(Priority::First); i <= priority; i++)
        node = ParseExpression(node, i, exprType);
    return node;
}

PNodeOp Parser::ParseExpression(ExprType exprType) {
    PNodeOp node = ParseExpression(static_cast<int>(Priority::Third), exprType);
    if (scanner->IsGettedToken())
        scanner->NextToken();
    return node;
}

PNodeOp Parser::ParseExpression(PNodeOp left, int priority, ExprType exprType) {
    PToken token = scanner->GetNextToken();
    if (!CheckCorrectToken(priority)) {
        scanner->PrevToken();
        return left;
    }
    PNodeBinOp node(new NodeBinOp(token));
    PNodeOp right = ParseExpression(priority - 1, exprType);
    node->SetLeft(left);
    node->SetRight(right);
    CalcNodeType(node);
    return ParseExpression(node, priority, exprType);
}

PNodeOp Parser::ParseExprIdentifier(PNodeOp left, ExprType exprType) {
    PToken token = scanner->GetToken();
    scanner->NextToken();
    switch (token->GetState()) {
        case Period: {
            PNodePeriod node(new NodePeriod(token));
            scanner->NextToken();
            CheckTokenType(token, TK::Identifier);
            node->SetName(left);
            node->SetField(token);
            CalcNodeType(node);
            return ParseExprIdentifier(node, exprType);
        }
        case LeftBracket: {
            PNodeBrackets node(new NodeBrackets(token, left));
            return ParseExprParameters(node, RightBracket, exprType);
        }
        case LeftParenthesis: {
            PNodeParenthesiss node(new NodeParenthesiss(token, left));
            return ParseExprParameters(node, RightParenthesis, exprType);
        }
        default: {
            scanner->PrevToken();
            return left;
        }
    }
}

PNodeOp Parser::ParseExprParameters(PNodeStructed node, State stopState, ExprType exprType) {
    PToken token = scanner->GetToken();
    scanner->NextToken();
    if (token->GetState() == stopState) {
        CalcNodeType(node);
        return node;
    }
    scanner->PrevToken();
    PNodeOp right = ParseExpression(static_cast<int>(Priority::Third), exprType);
    node->AddParameter(right);
    scanner->NextToken();
    while (token->GetState() != stopState) {
        CheckTokenState(token, Comma, stopState);
        right = ParseExpression(static_cast<int>(Priority::Third), exprType);
        node->AddParameter(right);
        scanner->NextToken();
    }
    CalcNodeType(node);
    token = scanner->GetNextToken();
    State state = token->GetState();
    if (state == Period || state == LeftBracket || state == LeftParenthesis) {
        scanner->PrevToken();
        return ParseExprIdentifier(node, exprType);
    }
    scanner->PrevToken();
    return node;
}

PNodeOp Parser::ParseUnOperator(ExprType exprType) {
    PToken token = scanner->GetToken();
    PNodeUnOp node(new NodeUnOp(token));
    if (CheckCorrectToken(static_cast<int>(Priority::Zero))) {
        node->SetNode(ParseFactor(exprType));
        CalcNodeType(node);
        return node;
    }
    token = scanner->GetToken();
    throw Error(ErrorType::IllegalExpression, *token);
}

PNodeOp Parser::ParseParenthesis(ExprType exprType) {
    CheckTokenState(scanner->GetToken(), LeftParenthesis);
    PNodeOp node = ParseExpression(static_cast<int>(Priority::Third), exprType);
    PToken token = scanner->GetNextToken();
    CheckTokenState(token, RightParenthesis);
    return node;
}

PNodeOp Parser::ParseFactor(ExprType exprType) {
    PToken token = scanner->GetToken();
    scanner->NextToken();
    switch (token->GetType()) {
        case TK::Integer: {
            return PNodeValue(new NodeValue(token, basicSymbol.at(BaseType::Integer)));
        }
        case TK::Double: {
            return PNodeValue(new NodeValue(token, basicSymbol.at(BaseType::Double)));
        }
        case TK::String: {
            return PNodeValue(new NodeValue(token, basicSymbol.at(BaseType::Char)));
        }
        case TK::Identifier: {
            PNodeValue node(new NodeValue(token));
            if (parserConfig == ParserConfig::ParseProgram) {
                PSymbolComplex symbol = tableStack->FindSymbol(token->GetText());
                if (symbol == nullptr)
                    throw IdentifierNotFound(*token, token->GetText());
                if (exprType == ExprType::Const && symbol->GetSymType() != SymType::Const)
                    throw Error(ErrorType::IllegalExpression, *token);
                if (symbol->GetSymType() == SymType::Type)
                    throw Error(ErrorType::IllegalExpression, *token);
                node->SetType(symbol->GetType());
            }
            return ParseExprIdentifier(node, exprType);
        }
        case TK::Operator: {
            return ParseUnOperator(exprType);
        }
        case TK::ReservedWord: {
            return ParseUnOperator(exprType);
        }
        case TK::Separator: {
            if (token->GetState() == LeftParenthesis)
                return ParseParenthesis(exprType);
            else
                throw Error(ErrorType::IllegalExpression, *token);
        }
        default: {
            throw Error(ErrorType::IllegalExpression, *token);
        }
    }
}

void Parser::CalcNodeType(PNodeOp node) {
    if (parserConfig == ParserConfig::ParseProgram)
        node->CalcType();
}

const PNode Parser::GetTree() const {
    return tree;
}

void Parser::ParseTypeDeclaration(PSymbolTable table) {
    PToken token = scanner->GetToken();
    CheckTokenState(token, Type);
    scanner->NextToken();
    do {
        string identifier = ParseIdentifier(table);
        scanner->NextToken();
        CheckTokenState(token, Equal);
        scanner->NextToken();
        PSymbolBase type = ParseType();
        PSymbolTypeAlias typeAlias(new SymbolTypeAlias(identifier, type));
        tableStack->Top()->AddSymbol(typeAlias);
        ParseSemiColon();
    } while (token->GetType() == TK::Identifier);
}

void Parser::ParseConstDeclaration(PSymbolTable table) {
    PToken token = scanner->GetToken();
    CheckTokenState(token, Const);
    scanner->NextToken();
    do {
        string identifier = ParseIdentifier(table);
        scanner->NextToken();
        PSymbolType type;
        PNodeOp expr;
        switch (token->GetState()) {
            case Equal: {
                expr = ParseExpression(ExprType::Const);
                break;
            }
            case Colon: {
                scanner->NextToken();
                type = ParseExistingType();
                CheckTokenState(token, Equal);
                expr = ParseExpression(ExprType::Const);
                break;
            }
            default: {
                throw SyntaxError(*token, Equal, token->GetText());
            }
        }
        if (type == nullptr)
            type = dynamic_pointer_cast<SymbolType>(tableStack->FindSymbol(expr->GetTypeName()));
        SetCorrectType(expr, type->GetTypeName());
        any value = expr->CalcValue(tableStack);
        PSymbolConst symbolConst(new SymbolConst(identifier, type->GetType(), value));
        tableStack->Top()->AddSymbol(symbolConst);
        ParseSemiColon();
    } while (token->GetType() == TK::Identifier);
}

void Parser::ParseVarDeclaration(PSymbolTable table) {
    PToken token = scanner->GetToken();
    CheckTokenState(token, Var);
    scanner->NextToken();
    do {
        set<string> identifiers = ParseIdentifiers(table);
        CheckTokenState(token, Colon);
        scanner->NextToken();
        PSymbolBase type = ParseType();
        any value = ParseValue(type, identifiers.size());
        for (const auto &identifier: identifiers) {
            PSymbolVar var(new SymbolVar(identifier, type, value));
            tableStack->Top()->AddSymbol(var);
        }
        ParseSemiColon();
    } while (token->GetType() == TK::Identifier);
}

void Parser::ParseDeclaration(PSymbolTable table) {
    PToken token = scanner->GetToken();
    while (true) {
        switch (token->GetState()) {
            case Type: {
                ParseTypeDeclaration(table);
                break;
            }
            case Const: {
                ParseConstDeclaration(table);
                break;
            }
            case Var: {
                ParseVarDeclaration(table);
                break;
            }
            case Procedure: {
                ParseProcedureDeclaration(table);
                break;
            }
            case Function: {
                ParseFunctionDeclaration(table);
                break;
            }
            default: {
                return;
            }
        }
    }
}

void Parser::ParseProgram() {
    scanner->NextToken();
    ParseDeclaration(tableStack->Top());
    tree = ParseGlobalCompoundStatement();
}

PNodeAssignmentOp Parser::ParseAssignmentOperator() {
    PToken token = scanner->GetToken();
    if (assignmentOperators.find(token->GetState()) == assignmentOperators.end())
        throw Error(ErrorType::IllegalExpression, *token);
    return PNodeAssignmentOp(new NodeAssignmentOp(token));
}

PNode Parser::ParseIfStatement() {
    PToken token = scanner->GetToken();
    CheckTokenState(token, If);
    PNodeIfStatement node(new NodeIfStatement(token));
    PNodeOp expr = ParseExpression();
    CheckNodeType(expr, baseType.at(BaseType::Integer));
    CheckTokenState(token, Then);
    node->SetIfNode(expr);
    scanner->NextToken();
    node->SetThenNode(ParseStatement());
    if (token->GetState() == Else) {
        scanner->NextToken();
        node->SetElseNode(ParseStatement());
    }
    return node;
}

void Parser::ParseSemiColons() {
    PToken token = scanner->GetToken();
    do {
        ParseSemiColon();
    } while (token->GetState() == SemiColon);
}

void Parser::ParseSemiColon() {
    CheckTokenState(scanner->GetToken(), SemiColon);
    scanner->NextToken();
}

PNode Parser::ParseStatement() {
    PToken token = scanner->GetToken();
    switch (token->GetType()) {
        case TK::Identifier: {
            PSymbolComplex ident = ParseExistingIdentifier();
            SymType symType = ident->GetSymType();
            scanner->PrevToken();
            PNodeOp node = ParseExpression();
            if ((symType == SymType::Procedure || symType == SymType::Function) &&
                    token->GetState() == SemiColon) {
                ParseSemiColons();
                return node;
            }
            if (node->GetNodeType() == NodeType::NodePeriod ||
                    node->GetNodeType() == NodeType::NodeBrackets ||
                    node->GetNodeType() == NodeType::NodeValue)
                return ParseAssignmentStatement(node);
            throw Error(ErrorType::InvalidVariable, *token);
        }
        case TK::ReservedWord: {
            return ParseComplexStatement();
        }
        case TK::EOFF: {
            throw Error(ErrorType::UnexpectedEOF, *token);
        }
        default: {
            throw Error(ErrorType::InvalidVariable, *token);
        }
    }
}

PNodeAssignmentOp Parser::ParseAssignmentStatement(PNodeOp left) {
    PNodeAssignmentOp op = ParseAssignmentOperator();
    PNodeOp right = ParseExpression();
    op->SetLeft(left);
    op->SetRight(right);
    op->CalcType();
    ParseSemiColons();
    return op;
}

PNode Parser::ParseProcedureStatement() {
    scanner->PrevToken();
    PNodeOp node = ParseExpression();
    if (node->GetNodeType() != NodeType::NodeParentehsiss)
        throw Error(ErrorType::IllegalExpression, *scanner->GetToken());
    ParseSemiColons();
    return node;
}

PSymbolComplex Parser::ParseExistingIdentifier() {
    PToken token = scanner->GetToken();
    CheckTokenType(token, TK::Identifier);
    PSymbolComplex symbol = tableStack->FindSymbol(token->GetText());
    if (symbol == nullptr)
        throw IdentifierNotFound(*token, token->GetText());
    return symbol;
}

PSymbolType Parser::ParseExistingType() {
    PToken token = scanner->GetToken();
    string type = token->GetValue();
    PSymbolComplex symbol = tableStack->FindSymbol(type);
    if (symbol == nullptr || symbol->GetSymType() != SymType::Type)
        throw Error(ErrorType::IllegalType, *token);
    scanner->NextToken();
    return dynamic_pointer_cast<SymbolType>(symbol);
}

PNode Parser::ParseComplexStatement() {
    PToken token = scanner->GetToken();
    switch (token->GetState()) {
        case Begin: {
            return ParseLocalCompoundStatement();
        }
        case If: {
            return ParseIfStatement();
        }
        case For: {
            return ParseForStatement();
        }
        case Repeat: {
            return ParseRepeatStatement();
        }
        case While: {
            return ParseWhileStatement();
        }
        default: {
            throw Error(ErrorType::IllegalStatement, *token);
        }
    }
}

PNode Parser::ParseForStatement() {
    PToken token = scanner->GetToken();
    CheckTokenState(token, For);
    PNodeForStatement nodeFor(new NodeForStatement(token));
    scanner->NextToken();
    PSymbolComplex ident = ParseExistingIdentifier();
    PNodeValue variable(new NodeValue(token));
    if (ident->GetSymType() != SymType::Var)
        throw Error(ErrorType::IllegalExpression, *token);
    variable->SetType(ident->GetType());
    scanner->NextToken();
    CheckTokenState(token, ColonEqual);
    PNodeAssignmentOp assignmentOp(new NodeAssignmentOp(token));
    PNodeOp initValue = ParseExpression();
    assignmentOp->SetLeft(variable);
    assignmentOp->SetRight(initValue);
    CalcNodeType(assignmentOp);
    if (assignmentOp->GetTypeName() != baseType.at(BaseType::Integer))
        throw Error(ErrorType::InvalidOrdinalType, *token);
    nodeFor->SetControlVar(assignmentOp);
    if (token->GetState() != To && token->GetState() != Downto)
        throw SyntaxError(*token, To, token->GetText());
    nodeFor->SetToType(token);
    PNodeOp finalVar = ParseExpression();
    if (finalVar->GetTypeName() != assignmentOp->GetTypeName())
        throw IncompatibleTypes(*token, finalVar->GetTypeName(), assignmentOp->GetTypeName());
    nodeFor->SetFinalVar(finalVar);
    CheckTokenState(token, Do);
    scanner->NextToken();
    nodeFor->SetDoSt(ParseStatement());
    return nodeFor;
}

PNode Parser::ParseRepeatStatement() {
    PToken token = scanner->GetToken();
    CheckTokenState(token, Repeat);
    PNodeRepeatStatement node(new NodeRepeatStatement(token));
    scanner->NextToken();
    while (token->GetState() != Until)
        node->AddStatement(ParseStatement());
    PNodeOp condition = ParseExpression();
    CheckNodeType(condition, baseType.at(BaseType::Integer));
    node->SetCondition(condition);
    return node;
}

PNode Parser::ParseWhileStatement() {
    PToken token = scanner->GetToken();
    CheckTokenState(token, While);
    PNodeWhileStatement node(new NodeWhileStatement(token));
    PNodeOp condition = ParseExpression();
    CheckNodeType(condition, baseType.at(BaseType::Integer));
    node->SetCondition(condition);
    CheckTokenState(token, Do);
    scanner->NextToken();
    node->AddStatement(ParseStatement());
    return node;
}

PNode Parser::ParseCompoundStatement() {
    PToken token = scanner->GetToken();
    CheckTokenState(token, Begin);
    PNodeCompoundStatement node(new NodeCompoundStatement(token));
    scanner->NextToken();
    while (token->GetState() != End) {
        node->AddStatement(ParseStatement());
    }
    scanner->NextToken();
    return node;
}

PNode Parser::ParseLocalCompoundStatement() {
    PNode node = ParseCompoundStatement();
    ParseSemiColons();
    return node;
}

PNode Parser::ParseGlobalCompoundStatement() {
    PNode node = ParseCompoundStatement();
    CheckTokenState(scanner->GetToken(), Period);
    return node;
}

bool Parser::CheckNodeType(PNodeOp node, std::string type) {
    if (node->GetTypeName() != type)
        throw IncompatibleTypes(*node->GetToken(), node->GetTypeName(), type);
    return true;
}

void Parser::ParseFunctionDeclaration(PSymbolTable table) {
    PToken token = scanner->GetToken();
    CheckTokenState(token, Function);
    scanner->NextToken();
    string ident = ParseIdentifier(table);
    scanner->NextToken();
    PSymbolFuncHeader header(new SymbolFuncHeader());
    PSymbolTable args = header->GetArgs();
    ParseExpParameterList(args);
    CheckTokenState(token, Colon);
    scanner->NextToken();
    PSymbolBase type = ParseType();
    header->SetReturnType(type);
    ParseSemiColon();
    PSymbolTable locals(new SymbolTable());
    tableStack->AddTable(args);
    tableStack->AddTable(locals);
    ParseDeclaration(args);
    ParseLocalCompoundStatement();
    tableStack->Pop();
    tableStack->Pop();
    PSymbolFunction function(new SymbolFunction(ident, header, locals));
    tableStack->Top()->AddSymbol(function);
}

void Parser::ParseProcedureDeclaration(PSymbolTable table) {
    PToken token = scanner->GetToken();
    CheckTokenState(token, Procedure);
    scanner->NextToken();
    string ident = ParseIdentifier(table);
    scanner->NextToken();
    PSymbolProcHeader header(new SymbolProcHeader());
    PSymbolTable args = header->GetArgs();
    ParseExpParameterList(args);
    ParseSemiColon();
    PSymbolTable locals(new SymbolTable());
    tableStack->AddTable(args);
    tableStack->AddTable(locals);
    ParseDeclaration(args);
    ParseLocalCompoundStatement();
    tableStack->Pop();
    tableStack->Pop();
    PSymbolProcedure procedure(new SymbolProcedure(ident, header, locals));
    tableStack->Top()->AddSymbol(procedure);
}

void Parser::ParseExpParameterList(PSymbolTable table) {
    PToken token = scanner->GetToken();
    if (token->GetState() != LeftParenthesis)
        return;
    scanner->NextToken();
    if (token->GetState() == RightParenthesis) {
        scanner->NextToken();
        return;
    }
    ParseParameterList(table);
    while (token->GetState() == SemiColon) {
        scanner->NextToken();
        ParseParameterList(table);
    }
    CheckTokenState(token, RightParenthesis);
    scanner->NextToken();
}

void Parser::ParseParameterList(PSymbolTable table) {
    ParseValueParameters(table);
}

void Parser::ParseValueParameters(PSymbolTable table) {
    set<string> identifiers = ParseIdentifiers(table);
    PToken token = scanner->GetToken();
    CheckTokenState(token, Colon);
    scanner->NextToken();
    if (token->GetState() == Array) {
        scanner->NextToken();
        CheckTokenState(token, Of);
        scanner->NextToken();
        PSymbolType type = ParseExistingType();
        for (const auto &ident: identifiers) {
            PSymbolOpenArray array(new SymbolOpenArray(type->GetType()));
            PSymbolValueParameter param(new SymbolValueParameter(ident, array));
            table->AddSymbol(param);
        }
        return;
    }
    PSymbolType type = ParseExistingType();
    for (const auto &identifier: identifiers) {
        PSymbolValueParameter param(new SymbolValueParameter(identifier, type->GetType()));
        table->AddSymbol(param);
    }
}

any Parser::ParseValue(PSymbolBase type, int size) {
    PToken token = scanner->GetToken();
    any value = type->GetInitValue();
    if (token->GetState() == Equal) {
        if (size > 1)
            throw Error(ErrorType::InvalidDeclaration, *token);
        PNodeOp expr = ParseExpression(ExprType::Const);
        SetCorrectType(expr, type->GetTypeName());
        value = dynamic_pointer_cast<NodeOp>(expr)->CalcValue(tableStack);
    }
    return value;
}

PSymbolSubRange Parser::ParseSubRange() {
    PToken token = scanner->GetToken();
    PSymbolComplex symbol = tableStack->FindSymbol(token->GetText());
    if (symbol != nullptr && symbol->GetType()->GetSymType() == SymType::SubRange) {
        scanner->NextToken();
        PSymbolBase subrange = symbol->GetType();
        return dynamic_pointer_cast<SymbolSubRange>(subrange);
    }
    scanner->PrevToken();
    PNodeOp expr = ParseExpression(ExprType::Const);
    if (expr->GetTypeName() != baseType.at(BaseType::Integer))
        throw Error(ErrorType::IllegalTypeDefinition, *token);
    any value = dynamic_pointer_cast<NodeOp>(expr)->CalcValue(tableStack);
    int left = any_cast<int>(value);
    if (token->GetState() != PeriodPeriod)
        throw Error(ErrorType::IllegalTypeDefinition, *token);
    expr = ParseExpression(ExprType::Const);
    if (expr->GetTypeName() != baseType.at(BaseType::Integer))
        throw Error(ErrorType::IllegalTypeDefinition, *token);
    value = dynamic_pointer_cast<NodeOp>(expr)->CalcValue(tableStack);
    int right = any_cast<int>(value);
    if (left > right)
        throw Error(ErrorType::IllegalRange, *token);
    return PSymbolSubRange(new SymbolSubRange(left, right));
}

vector<PSymbolSubRange> Parser::ParseArrayBrackets() {
    PToken token = scanner->GetToken();
    CheckTokenState(token, LeftBracket);
    vector<PSymbolSubRange> dimensions;
    do {
        scanner->NextToken();
        dimensions.push_back(ParseSubRange());
    } while (token->GetState() == Comma);
    CheckTokenState(token, RightBracket);
    scanner->NextToken();
    return dimensions;
}

PSymbolArray Parser::ParseArray() {
    PToken token = scanner->GetToken();
    CheckTokenState(token, Array);
    scanner->NextToken();
    vector<PSymbolSubRange> dimensions;
    switch (token->GetState()) {
        case Of: {
            return ParseDynamicArray();
        }
        case LeftBracket: {
            dimensions = ParseArrayBrackets();
            CheckTokenState(token, Of);
            return ParseStaticArray(dimensions);
        }
        default: {
            throw SyntaxError(*token, Of, token->GetText());
        }
    }
}

PSymbolDynamicArray Parser::ParseDynamicArray() {
    PSymbolDynamicArray array(new SymbolDynamicArray(nullptr));
    scanner->NextToken();
    array->SetType(ParseType());
    return array;
}

PSymbolStaticArray Parser::ParseStaticArray(vector<PSymbolSubRange> &dimensions) {
    PSymbolStaticArray array(new SymbolStaticArray(nullptr, dimensions[0]));
    PSymbolStaticArray tmp = array;
    for (int i = 1; i < dimensions.size(); i++) {
        PSymbolStaticArray newArray(new SymbolStaticArray(nullptr, dimensions[i]));
        tmp->SetType(newArray);
        tmp = newArray;
    }
    scanner->NextToken();
    tmp->SetType(ParseType());
    return array;
}

PSymbolRecord Parser::ParseRecord() {
    string str;
    PToken token = scanner->GetToken();
    CheckTokenState(token, Record);
    PSymbolRecord record(new SymbolRecord());
    scanner->NextToken();
    while (token->GetState() != End) {
        set<string> identifiers = ParseIdentifiers(record->GetFields());
        CheckTokenState(token, Colon);
        scanner->NextToken();
        PSymbolBase type = ParseType();
        for (const auto &identifier: identifiers) {
            PSymbolRecordField field(new SymbolRecordField(identifier, type));
            record->AddField(field);
        }
        ParseSemiColon();
    }
    CheckTokenState(token, End);
    scanner->NextToken();
    return record;
}

PSymbolBase Parser::ParseType() {
    PToken token = scanner->GetToken();
    if (token->GetType() == TK::Identifier) {
        PSymbolComplex symbol = tableStack->FindSymbol(token->GetText());
        if (symbol != nullptr && symbol->GetSymType() == SymType::Type)
            return ParseSimpleType();
    }
    if (token->GetState() == Array) {
        return ParseArray();
    }
    if (token->GetState() == Record) {
        return ParseRecord();
    }
    return ParseSubRange();
}

PSymbolBase Parser::ParseSimpleType() {
    PToken token = scanner->GetToken();
    PSymbolComplex symbol = ParseExistingIdentifier();
    if (symbol->GetSymType() != SymType::Type)
        throw Error(ErrorType::IllegalTypeDefinition, *token);
    PSymbolBase type = symbol->GetType();
    scanner->NextToken();
    return type;
}

set<string> Parser::ParseIdentifiers(PSymbolTable table) {
    PToken token = scanner->GetToken();
    set<string> identifiers;
    while (true) {
        string ident = ParseIdentifier(table);
        transform(ident.begin(), ident.end(), ident.begin(), ::tolower);
        if (identifiers.find(ident) != identifiers.end())
            throw DuplicateIdentifier(*token, token->GetValue());
        identifiers.insert(ident);
        scanner->NextToken();
        if (token->GetState() != Comma)
            break;
        scanner->NextToken();
    }
    return identifiers;
}

string Parser::ParseIdentifier(PSymbolTable table) {
    PToken token = scanner->GetToken();
    CheckTokenType(token, TK::Identifier);
    if (table->HaveSymbol(token->GetValue()))
        throw DuplicateIdentifier(*token, token->GetValue());
    return token->GetValue();
}

bool Parser::CheckTokenType(PToken token, TK::TokenType type) {
    if (token->GetType() != type)
        throw SyntaxError(*token, type, token->GetText());
    return true;
}

bool Parser::CheckTokenState(PToken token, State state) {
    if (token->GetState() != state)
        throw SyntaxError(*token, state, token->GetText());
    return true;
}

bool Parser::CheckTokenState(PToken token, State state, State stateExpected) {
    if (token->GetState() != state)
        throw SyntaxError(*token, stateExpected, token->GetText());
    return true;
}

void Parser::AddBaseTypesToTable(PSymbolTable table) {
    for (auto iter = basicSymbol.begin(); iter != basicSymbol.end(); iter++) {
        string name = baseType.at(iter->first);
        PSymbolBaseType type = iter->second;
        tableStack->Top()->AddSymbol(PSymbolComplex(new SymbolType(name, type)));
    }
}

void Parser::CreateGlobalTable() {
    tableStack->AddTable(PSymbolTable(new SymbolTable()));
    AddBaseTypesToTable(tableStack->Top());
}

void Parser::SetCorrectType(PNodeOp node, std::string type) {
    if (node->GetTypeName() == baseType.at(BaseType::Integer) &&
        type == baseType.at(BaseType::Double))
        node->SetType(basicSymbol.at(BaseType::Double));
    CheckNodeType(node, type);
}

void Parser::PrintStack() {
    tableStack->Print();
}
