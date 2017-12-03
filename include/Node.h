#pragma once
#include <memory>
#include <map>
#include <set>
#include <any>
#include "Token.h"
#include "Symbols.h"

enum class NodeType {
    NodeBinOp,
    NodeUnOp,
    NodeValue,
    NodeBrackets,
    NodeParentehsiss,
    NodeIfStatement,
    NodeRepeatStatement,
    NodeWhileStatement,
    NodeCompoundStatement,
    NodeForStatement,
    NodePeriod,
    NodeAssignmentOp
};

static std::set<State> assignmetOp {
        ColonEqual,
        PlusEqual,
        MinusEqual,
        AsteriskEqual,
        SlashEqual
};

static std::set<State> baseArithmeticOp {
        Plus,
        Minus,
        Asterisk,
        Slash
};


static std::set<State> intArithmeticOp {
        Mod,
        Div
};

static std::set<State> logicalOp {
        Not,
        And,
        Or,
        Xor,
        Shl,
        Shr,
        LessThanLessThan,
        GreaterThanGreaterThan
};

static std::set<State> relationalOp {
        Equal,
        LessThanGreaterThan,
        LessThan,
        GreaterThan,
        LessThanEqual,
        GreaterThanEqual
};

class Node {
public:
    Node(PToken token);
    virtual void Print(int depth = 0) = 0;
    virtual const PToken GetToken() const;
protected:
    static const int spacesCount = 4;
    Token token;
};
typedef std::shared_ptr<Node> PNode;

class NodeOp: public Node {
public:
    NodeOp(PToken token);
    virtual NodeType GetNodeType() = 0;
    virtual void CalcType() = 0;
    virtual void SetType(PSymbolBase);
    virtual const PSymbolBase GetType() const;
    virtual const std::string GetTypeName() const;
    virtual std::any CalcValue(PSymbolTableStack) {};
protected:
    PSymbolBase type;
};
typedef std::shared_ptr<NodeOp> PNodeOp;

class NodeBinOp: public NodeOp {
public:
    NodeBinOp(PToken token);
    NodeType GetNodeType() { return NodeType::NodeBinOp; };
    virtual void CalcType();
    virtual void SetLeft(PNodeOp);
    virtual void SetRight(PNodeOp);
    virtual const PNodeOp GetLeft() const;
    virtual const PNodeOp GetRight() const;
    virtual void Print(int depth = 0);
    std::any CalcValue(PSymbolTableStack);
protected:
    PNodeOp left;
    PNodeOp right;
    virtual bool CheckTypes(std::string, std::string);
    virtual bool CheckTypes(std::string);
    virtual void CastTypes();
    std::string GetArgType();
    bool CheckIntOp();
    bool CheckDoubleOp();
    bool CheckCharOp();
    virtual bool CheckOp();
};
typedef std::shared_ptr<NodeBinOp> PNodeBinOp;

class NodeAssignmentOp: public NodeBinOp {
public:
    NodeAssignmentOp(PToken token);
    NodeType GetNodeType() { return NodeType::NodeAssignmentOp; };
protected:
    void CastTypes();
    bool CheckTypes(std::string, std::string);
    bool CheckAssignmentOp();
    bool CheckOp();
};
typedef std::shared_ptr<NodeAssignmentOp> PNodeAssignmentOp;

class NodePeriod: public NodeOp {
public:
    NodePeriod(PToken token);
    NodeType GetNodeType() { return NodeType::NodePeriod; };
    void CalcType();
    void SetName(PNodeOp);
    void SetField(PToken);
    void Print(int depth = 0);
protected:
    PNodeOp name;
    Token field;
};
typedef std::shared_ptr<NodePeriod> PNodePeriod;

class NodeUnOp: public NodeOp {
public:
    NodeUnOp(PToken token);
    NodeType GetNodeType() { return NodeType::NodeUnOp; };
    void CalcType();
    void SetNode(PNodeOp);
    const PNodeOp GetNode() const;
    void Print(int depth = 0);
    std::any CalcValue(PSymbolTableStack);
protected:
    PNodeOp node;
    bool CheckOp();
    bool CheckIntOp();
    bool CheckDoubleOp();
};
typedef std::shared_ptr<NodeUnOp> PNodeUnOp;

class NodeValue: public NodeOp {
public:
    NodeValue(PToken token, PSymbolBase type);
    NodeValue(PToken token);
    NodeType GetNodeType() { return NodeType::NodeValue; };
    void CalcType() {};
    void Print(int depth = 0);
    std::any CalcValue(PSymbolTableStack);
};
typedef std::shared_ptr<NodeValue> PNodeValue;

class NodeStructured: public NodeOp {
public:
    NodeStructured(PToken token, PNodeOp name);
    virtual void AddParameter(PNodeOp node);
    virtual void CalcType() = 0;
    void Print(std::string value, int depth = 0);
protected:
    std::vector<PNodeOp> parameters;
    PNodeOp name;
};
typedef std::shared_ptr<NodeStructured> PNodeStructed;

class NodeBrackets: public NodeStructured {
public:
    NodeBrackets(PToken token, PNodeOp name);
    NodeType GetNodeType() { return NodeType::NodeBrackets; };
    void Print(int depth = 0);
    void CalcType();
};
typedef std::shared_ptr<NodeBrackets> PNodeBrackets;

class NodeParenthesiss: public NodeStructured {
public:
    NodeParenthesiss(PToken token, PNodeOp name);
    NodeType GetNodeType() { return NodeType::NodeParentehsiss; };
    void Print(int depth = 0);
    void CalcType();
};
typedef std::shared_ptr<NodeParenthesiss> PNodeParenthesiss;

class NodeCompoundStatement: public Node {
public:
    NodeCompoundStatement(PToken token);
    NodeType GetNodeType() { return NodeType::NodeCompoundStatement; };
    void Print(int depth = 0);
    void AddStatement(PNode node);
protected:
    std::vector<PNode> statements;

};
typedef std::shared_ptr<NodeCompoundStatement> PNodeCompoundStatement;

class NodeIfStatement: public Node {
public:
    NodeIfStatement(PToken token);
    NodeType GetNodeType() { return NodeType::NodeIfStatement; };
    void Print(int depth = 0);
    void SetIfNode(PNodeOp node);
    void SetThenNode(PNode node);
    void SetElseNode(PNode node);
protected:
    PNodeOp ifNode;
    PNode thenNode;
    PNode elseNode = nullptr;
};
typedef std::shared_ptr<NodeIfStatement> PNodeIfStatement;

class NodeForStatement: public Node {
public:
    NodeForStatement(PToken token);
    NodeType GetNodeType() { return NodeType::NodeForStatement; };
    void Print(int depth = 0);
    void SetToType(PToken toType);
    void SetControlVar(PNodeAssignmentOp controlVar);
    void SetFinalVar(PNodeOp finalVar);
    void SetDoSt(PNode doSt);
protected:
    Token toType;
    PNodeAssignmentOp controlVar;
    PNodeOp finalVar;
    PNode doSt;
};
typedef std::shared_ptr<NodeForStatement> PNodeForStatement;

class NodeWhileStatement: public Node {
public:
    NodeWhileStatement(PToken token);
    virtual NodeType GetNodeType() { return NodeType::NodeWhileStatement; };
    virtual void Print(int depth = 0);
    virtual void AddStatement(PNode statement);
    virtual void SetCondition(PNodeOp condition);
protected:
    std::vector<PNode> statements;
    PNodeOp condition;
};
typedef std::shared_ptr<NodeWhileStatement> PNodeWhileStatement;

class NodeRepeatStatement: public NodeWhileStatement {
public:
    NodeRepeatStatement(PToken token);
    NodeType GetNodeType() { return NodeType::NodeRepeatStatement; };
};
typedef std::shared_ptr<NodeRepeatStatement> PNodeRepeatStatement;
