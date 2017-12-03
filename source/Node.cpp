#include <iostream>
#include "Evaluate.h"
#include "Node.h"
#include "Token.h"
#include "Error.h"
#include "Symbols.h"

using namespace std;

Node::Node(PToken token) : token(*token) {}

const PToken Node::GetToken() const {
    return PToken(new Token(token));
}

void NodeOp::SetType(PSymbolBase type) {
    this->type = type;
}

const PSymbolBase NodeOp::GetType() const {
    return type;
}

const std::string NodeOp::GetTypeName() const {
    return type->GetTypeName();
}

NodeOp::NodeOp(PToken token) : Node(token) {}

NodeBinOp::NodeBinOp(PToken token) : NodeOp(token) {}

void NodeBinOp::SetLeft(PNodeOp left) {
    this->left = left;
}

void NodeBinOp::SetRight(PNodeOp right) {
    this->right = right;
}

void NodeBinOp::Print(int depth) {
    cout << string(depth * spacesCount, ' ');
    cout << token.GetValue() << endl;
    left->Print(depth + 1);
    right->Print(depth + 1);
}

bool NodeBinOp::CheckTypes(std::string type1, std::string type2) {
    return (left->GetTypeName() == type1 && right->GetTypeName() == type2) ||
           (left->GetTypeName() == type2 && right->GetTypeName() == type1);
}

bool NodeBinOp::CheckTypes(std::string type) {
    return left->GetTypeName() == type && right->GetTypeName() == type;
}

void NodeBinOp::CastTypes() {
    if (GetArgType() == baseType.at(BaseType::Double))
        type = basicSymbol.at(BaseType::Double);
    else if (GetArgType() == baseType.at(BaseType::Integer))
        type = basicSymbol.at(BaseType::Integer);
    else if (GetArgType() == baseType.at(BaseType::Char))
        type = basicSymbol.at(BaseType::Char);
}

void NodeBinOp::CalcType() {
    type = nullptr;
    if (token.GetType() != TK::Operator && token.GetType() != TK::ReservedWord)
        throw Error(ErrorType::IllegalExpression, token);
    CastTypes();
    if (type == nullptr)
        throw IncompatibleTypes(*right->GetToken(), right->GetTypeName(), left->GetTypeName());
    CheckOp();
    State op = token.GetState();
    if (relationalOp.find(op) != relationalOp.end())
        type = basicSymbol.at(BaseType::Integer);
}

const PNodeOp NodeBinOp::GetLeft() const {
    return left;
}

const PNodeOp NodeBinOp::GetRight() const {
    return right;
}

bool NodeBinOp::CheckIntOp() {
    State op = token.GetState();
    return !(baseArithmeticOp.find(op) == baseArithmeticOp.end() &&
             intArithmeticOp.find(op) == intArithmeticOp.end() &&
             logicalOp.find(op) == logicalOp.end() &&
             relationalOp.find(op) == relationalOp.end());
}

bool NodeBinOp::CheckDoubleOp() {
    State op = token.GetState();
    return !(baseArithmeticOp.find(op) == baseArithmeticOp.end() &&
             relationalOp.find(op) == relationalOp.end());
}

bool NodeBinOp::CheckCharOp() {
    return token.GetState() == Plus;
}

bool NodeBinOp::CheckOp() {
    string intType = baseType.at(BaseType::Integer);
    string doubleType = baseType.at(BaseType::Double);
    string charType = baseType.at(BaseType::Char);
    if (GetTypeName() == doubleType && !CheckDoubleOp())
        throw BinOperatorIsNotOverloaded(*left->GetToken(), left->GetTypeName(),
                                      token.GetValue(), right->GetTypeName());
    if (GetTypeName() == intType && !CheckIntOp())
        throw BinOperatorIsNotOverloaded(*left->GetToken(), left->GetTypeName(),
                                      token.GetValue(), right->GetTypeName());
    if (GetTypeName() == charType && !CheckCharOp())
        throw BinOperatorIsNotOverloaded(*left->GetToken(), left->GetTypeName(),
                                      token.GetValue(), right->GetTypeName());
    return true;
}

any NodeBinOp::CalcValue(PSymbolTableStack stack) {
    any leftValue = left->CalcValue(stack);
    any rightValue = right->CalcValue(stack);
    return Evaluate::Calc(token.GetState(), leftValue, rightValue, left->GetTypeName(),
                          right->GetTypeName(), GetTypeName());
}

std::string NodeBinOp::GetArgType() {
    string intType = baseType.at(BaseType::Integer);
    string doubleType = baseType.at(BaseType::Double);
    if (CheckTypes(doubleType), CheckTypes(intType, doubleType))
        return doubleType;
    if (CheckTypes(left->GetTypeName()))
        return left->GetTypeName();
    return "";
}

NodeAssignmentOp::NodeAssignmentOp(PToken token) : NodeBinOp(token) {}

void NodeAssignmentOp::CastTypes() {
    if (NodeBinOp::CheckTypes(left->GetTypeName()))
        type = left->GetType();
    string intType = baseType.at(BaseType::Integer);
    string doubleType = baseType.at(BaseType::Double);
    if (CheckTypes(doubleType, intType))
        type = left->GetType();
}

bool NodeAssignmentOp::CheckAssignmentOp() {
    State op = token.GetState();
    if (assignmetOp.find(op) == assignmetOp.end())
        throw BinOperatorIsNotOverloaded(*left->GetToken(), left->GetTypeName(),
                                      token.GetValue(), right->GetTypeName());
    return true;
}

bool NodeAssignmentOp::CheckOp() {
    string intType = baseType.at(BaseType::Integer);
    string doubleType = baseType.at(BaseType::Double);
    if (GetTypeName() == intType || GetTypeName() == doubleType)
        return CheckAssignmentOp();
    if (token.GetState() != ColonEqual)
        throw BinOperatorIsNotOverloaded(*left->GetToken(), left->GetTypeName(),
                                      token.GetValue(), right->GetTypeName());
    return true;
}

bool NodeAssignmentOp::CheckTypes(std::string type1, std::string type2) {
    return left->GetTypeName() == type1 && right->GetTypeName() == type2;
}

NodePeriod::NodePeriod(PToken token) : NodeOp(token) {}

void NodePeriod::SetName(PNodeOp name) {
    this->name = name;
}

void NodePeriod::SetField(PToken field) {
    this->field = *field;
}

void NodePeriod::Print(int depth) {
    cout << string(depth * spacesCount, ' ');
    cout << token.GetValue() << endl;
    name->Print(depth + 1);
    cout << string((depth + 1) * spacesCount, ' ');
    cout << field.GetValue() << endl;
}

void NodePeriod::CalcType() {
    PSymbolBase type = name->GetType();
    if (type->GetSymType() != SymType::Record)
        throw Error(ErrorType::IllegalExpression, token);
    PSymbolComplex symbol = dynamic_pointer_cast<SymbolRecord>(type)->GetFields()->FindSymbol(field.GetValue());
    if (symbol == nullptr)
        throw Error(ErrorType::IllegalExpression, token);
    this->type = symbol->GetType();
}

NodeUnOp::NodeUnOp(PToken token) : NodeOp(token) {}

void NodeUnOp::SetNode(PNodeOp node) {
    this->node = node;
}

void NodeUnOp::Print(int depth) {
    cout << string(depth * spacesCount, ' ');
    cout << token.GetValue() << endl;
    node->Print(depth + 1);
}

bool NodeUnOp::CheckOp() {
    string intType = baseType.at(BaseType::Integer);
    string doubleType = baseType.at(BaseType::Double);
    if ((GetTypeName() != intType && GetTypeName() != doubleType) ||
            (GetTypeName() == intType && !CheckIntOp()) ||
            (GetTypeName() == doubleType && !CheckDoubleOp()))
        throw UnOperatorIsNotOverloaded(*node->GetToken(), token.GetValue(), node->GetTypeName());
    return true;
}

void NodeUnOp::CalcType() {
    type = node->GetType();
    CheckOp();
}

const PNodeOp NodeUnOp::GetNode() const {
    return node;
}

any NodeUnOp::CalcValue(PSymbolTableStack stack) {
    any value = node->CalcValue(stack);
    return Evaluate::Calc(token.GetState(), value, node->GetTypeName(), GetTypeName());
}

bool NodeUnOp::CheckIntOp() {
    return token.GetState() == Plus || token.GetState() == Minus || token.GetState() == Not;
}

bool NodeUnOp::CheckDoubleOp() {
    return token.GetState() == Plus || token.GetState() == Minus;
}

NodeValue::NodeValue(PToken token, PSymbolBase type) : NodeOp(token) {
    this->type = type;
}

NodeValue::NodeValue(PToken token) : NodeOp(token) {}

void NodeValue::Print(int depth) {
    cout << string(depth * spacesCount, ' ');
    cout << token.GetValue() << endl;
}

any NodeValue::CalcValue(PSymbolTableStack stack) {
    string value = token.GetValue();
    if (token.GetType() == TK::Identifier) {
        PSymbolComplex symbol = stack->FindSymbol(value);
        if (symbol->GetSymType() != SymType::Const)
            throw Error(ErrorType::IllegalExpression, token);
        PSymbolConst symbolConst = dynamic_pointer_cast<SymbolConst>(symbol);
        return Evaluate::Calc(symbolConst->GetValue(), symbolConst->GetTypeName(), GetTypeName());
    }
    return Evaluate::Calc(value, GetTypeName());
}

NodeStructured::NodeStructured(PToken token, PNodeOp name) : NodeOp(token), name(name) {}

void NodeStructured::AddParameter(PNodeOp node) {
    parameters.push_back(node);
}

void NodeStructured::Print(string value, int depth) {
    cout << string(depth * spacesCount, ' ');
    cout << value << endl;
    name->Print(depth + 1);
    for (const auto& param: parameters)
        param->Print(depth + 1);
}

NodeBrackets::NodeBrackets(PToken token, PNodeOp name) : NodeStructured(token, name) {}

void NodeBrackets::Print(int depth) {
    NodeStructured::Print("[]", depth);
}

void NodeBrackets::CalcType() {
    PSymbolBase type = name->GetType();
    for (auto &param: parameters) {
        if (param->GetTypeName() != baseType.at(BaseType::Integer))
            throw Error(ErrorType::IllegalQualifier, *param->GetToken());
        SymType symType = type->GetSymType();
        if (symType != SymType::Array)
            throw Error(ErrorType::IllegalQualifier, *param->GetToken());
        type = dynamic_pointer_cast<SymbolArray>(type)->GetType();
    }
    this->type = type;
}

NodeParenthesiss::NodeParenthesiss(PToken token, PNodeOp name) : NodeStructured(token, name) {}

void NodeParenthesiss::Print(int depth) {
    NodeStructured::Print("()", depth);
}

void NodeParenthesiss::CalcType() {
    SymType symType = name->GetType()->GetSymType();
    if (symType != SymType::ProcHeader && symType != SymType::FuncHeader)
        throw Error(ErrorType::IllegalExpression, token);
    PSymbolProcHeader header = dynamic_pointer_cast<SymbolProcHeader>(name->GetType());
    PToken nameToken = name->GetToken();
    if (header->GetArgs()->Size() != parameters.size())
        throw WrongCountParameters(*nameToken, nameToken->GetValue());
    for (int i = 0; i < parameters.size(); i++) {
        PSymbolBase symb1 = parameters[i]->GetType();
        PSymbolBase symb2 = header->GetArgs()->GetSymbols()[i]->GetType();
        string t1 = symb1->GetTypeName();
        string t2 = symb2->GetTypeName();
        string integerType = baseType.at(BaseType::Integer);
        string doubleType = baseType.at(BaseType::Double);
        if (t1 == integerType && t2 == doubleType)
            continue;
        if (symb1->GetSymType() == SymType::Array && symb2->GetSymType() == SymType::OpenArray)
            if (dynamic_pointer_cast<SymbolArray>(symb1)->GetType()->GetTypeName() ==
                    dynamic_pointer_cast<SymbolArray>(symb2)->GetType()->GetTypeName())
                continue;
        if (t1 != t2)
            throw IncompatibleTypesForArg(*parameters[i]->GetToken(), i + 1, t1, t2);
    }
    type = header->GetReturnType();
}

NodeCompoundStatement::NodeCompoundStatement(PToken token) : Node(token) {}

void NodeCompoundStatement::Print(int depth) {
    cout << string(depth * spacesCount, ' ');
    cout << token.GetValue() << endl;
    for (const auto& statement: statements)
        statement->Print(depth + 1);
    cout << string(depth * spacesCount, ' ');
    cout << "end" << endl;
}

void NodeCompoundStatement::AddStatement(PNode node) {
    statements.push_back(node);
}

NodeIfStatement::NodeIfStatement(PToken token) : Node(token) {}

void NodeIfStatement::Print(int depth) {
    cout << string(depth * spacesCount, ' ');
    cout << token.GetValue() << endl;
    ifNode->Print(depth + 1);
    thenNode->Print(depth + 1);
    if (elseNode != nullptr)
        elseNode->Print(depth + 1);
}

void NodeIfStatement::SetIfNode(PNodeOp node) {
    ifNode = node;
}

void NodeIfStatement::SetThenNode(PNode node) {
    thenNode = node;
}

void NodeIfStatement::SetElseNode(PNode node) {
    elseNode = node;
}

NodeForStatement::NodeForStatement(PToken token) : Node(token) {}

void NodeForStatement::Print(int depth) {
    cout << string(depth * spacesCount, ' ');
    cout << token.GetValue() << endl;
    cout << string(depth * spacesCount, ' ');
    cout << toType.GetValue() << endl;
    controlVar->Print(depth + 1);
    finalVar->Print(depth + 1);
    doSt->Print(depth + 1);
}

void NodeForStatement::SetToType(PToken toType) {
    this->toType = *toType;
}

void NodeForStatement::SetControlVar(PNodeAssignmentOp controlVar) {
    this->controlVar = controlVar;
}

void NodeForStatement::SetFinalVar(PNodeOp finalVar) {
    this->finalVar = finalVar;
}

void NodeForStatement::SetDoSt(PNode doSt) {
    this->doSt = doSt;
}

NodeWhileStatement::NodeWhileStatement(PToken token) : Node(token) {}

void NodeWhileStatement::SetCondition(PNodeOp condition) {
    this->condition = condition;
}

void NodeWhileStatement::AddStatement(PNode statement) {
    statements.push_back(statement);
}

void NodeWhileStatement::Print(int depth) {
    cout << string(depth * spacesCount, ' ');
    cout << token.GetValue() << endl;
    condition->Print(depth + 1);
    for (auto& st: statements)
        st->Print(depth + 1);
}

NodeRepeatStatement::NodeRepeatStatement(PToken token) : NodeWhileStatement(token) {}