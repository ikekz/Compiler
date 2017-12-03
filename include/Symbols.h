#pragma once
#include <string>
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <any>

enum class BaseType {
    Integer,
    Double,
    Char,
    Untyped
};

static std::map<BaseType, std::string> baseType = {
        { BaseType::Integer, "integer" },
        { BaseType::Double,  "double"  },
        { BaseType::Char,    "char"    },
        { BaseType::Untyped, "untyped" }
};

static std::map<BaseType, std::any> baseTypeInitValue = {
        { BaseType::Integer, std::make_any<int>(0)          },
        { BaseType::Double,  std::make_any<double>(0.0)     },
        { BaseType::Char,    std::make_any<std::string>("") }
};

enum class SymType {
    VarInitialized,
    Var,
    Type,
    TypeAlias,
    Array,
    SubRange,
    Const,
    BaseType,
    RecordField,
    Record,
    ValueParameter,
    ProcHeader,
    Procedure,
    FuncHeader,
    Function,
    OpenArray
};

static std::map<SymType, std::string> symTypeName {
        { SymType::VarInitialized, "var"         },
        { SymType::Var,            "var"         },
        { SymType::Type,           "type"        },
        { SymType::TypeAlias,      "typealias"   },
        { SymType::Array,          "array"       },
        { SymType::SubRange,       "subrange"    },
        { SymType::Const,          "const"       },
        { SymType::BaseType,       "basetype"    },
        { SymType::RecordField,    "recordfield" },
        { SymType::Record,         "record"      },
        { SymType::ValueParameter, "valueparam"  },
        { SymType::ProcHeader,     "procheader"  },
        { SymType::Procedure,      "procedure"   },
        { SymType::FuncHeader,     "funcheader"  },
        { SymType::Function,       "function"    },
        { SymType::OpenArray,      "open array"  }
};

class SymbolTableStack;

class Symbol {
public:
    virtual const SymType GetSymType() const = 0;
    virtual const std::string GetTypeName() const = 0;
    virtual void PrintTypeName(unsigned int depth = 0) = 0;

    static const int nameWidth = 15;
    static const int symTypeWidth = 15;
    static const int typeWidth = 15;
};
typedef std::shared_ptr<Symbol> PSymbol;

class SymbolBase: public Symbol {
public:
    const SymType GetSymType() const override = 0;
    const std::string GetTypeName() const override = 0;
    void PrintTypeName(unsigned int depth = 0) override = 0;
    virtual const std::any GetInitValue() const { return std::any(); };
protected:
};
typedef std::shared_ptr<SymbolBase> PSymbolBase;

class SymbolComplex: public Symbol {
public:
    SymbolComplex(std::string name, PSymbolBase type);
    virtual void Print(unsigned int depth = 0);
    const SymType GetSymType() const override = 0;
    const std::string GetTypeName() const override;
    void PrintTypeName(unsigned int depth = 0) override;
    virtual const PSymbolBase GetType() const;
    virtual void SetName(const std::string name);
    virtual const std::string GetName() const;
protected:
    std::string name;
    PSymbolBase type;
};
typedef std::shared_ptr<SymbolComplex> PSymbolComplex;

class SymbolComplexWithValue: public SymbolComplex {
public:
    SymbolComplexWithValue(std::string name, PSymbolBase type, std::any value);
    void Print(unsigned int depth = 0) override;
    virtual const std::any GetValue() const;
    virtual const std::string GetValueText() const;
    const SymType GetSymType() const override = 0;
protected:
    std::any value;
};
typedef std::shared_ptr<SymbolComplexWithValue> PSymbolComplexWithValue;

class SymbolTable {
public:
    void AddSymbol(PSymbolComplex symbol);
    PSymbolComplex FindSymbol(std::string name);
    bool HaveSymbol(std::string name);
    unsigned int Size();
    void Print(unsigned int depth = 0);
    const std::string GetSymbolTypeNames() const;
    const std::vector<PSymbolComplex> &GetSymbols() const;
private:
    std::vector<PSymbolComplex> symbols;
    std::map<std::string, int> symbolNames;
};

typedef std::shared_ptr<SymbolTable> PSymbolTable;

class SymbolTableStack {
public:
    void AddTable(PSymbolTable table);
    bool HaveSymbol(std::string name);
    PSymbolComplex FindSymbol(std::string name);
    PSymbolTable Top();
    void Pop();
    void Print(unsigned int depth = 0);
private:
    std::vector<PSymbolTable> tables;
};
typedef std::shared_ptr<SymbolTableStack> PSymbolTableStack;

class SymbolConst: public SymbolComplexWithValue {
public:
    SymbolConst(std::string name, PSymbolBase type, std::any value);
    const SymType GetSymType() const { return SymType::Const; }
};
typedef std::shared_ptr<SymbolConst> PSymbolConst;

class SymbolValueParameter: public SymbolComplex {
public:
    SymbolValueParameter(std::string name, PSymbolBase type);
    const SymType GetSymType() const { return SymType::ValueParameter; };
};
typedef std::shared_ptr<SymbolValueParameter> PSymbolValueParameter;

class SymbolVar: public SymbolComplexWithValue {
public:
    SymbolVar(std::string name, PSymbolBase type, std::any value);
    const SymType GetSymType() const { return SymType::Var; };
};
typedef std::shared_ptr<SymbolVar> PSymbolVar;

class SymbolType: public SymbolComplex {
public:
    SymbolType(std::string name, PSymbolBase type);
    const SymType GetSymType() const { return SymType::Type; }
    const std::any GetInitValue() const;
};
typedef std::shared_ptr<SymbolType> PSymbolType;

class SymbolTypeAlias: public SymbolType {
public:
    SymbolTypeAlias(std::string name, PSymbolBase type);
};
typedef std::shared_ptr<SymbolTypeAlias> PSymbolTypeAlias;

class SymbolRecordField: public SymbolComplex {
public:
    SymbolRecordField(std::string name, PSymbolBase type);
    const SymType GetSymType() const { return SymType::RecordField; };
};
typedef std::shared_ptr<SymbolRecordField> PSymbolRecordField;

class SymbolRecord: public SymbolBase {
public:
    SymbolRecord();
    const SymType GetSymType() const { return SymType::Record; };
    void PrintTypeName(unsigned int depth = 0);
    const std::string GetTypeName() const;
    const PSymbolTable GetFields() const;
    void AddField(PSymbolRecordField);
protected:
    PSymbolTable fields;
};
typedef std::shared_ptr<SymbolRecord> PSymbolRecord;

class SymbolProcHeader: public SymbolBase {
public:
    SymbolProcHeader();
    const SymType GetSymType() const { return SymType::ProcHeader; };
    void PrintTypeName(unsigned int depth = 0);
    virtual const PSymbolTable GetArgs() const;
    virtual const std::string GetTypeName() const;
    virtual const PSymbolBase GetReturnType() const;
protected:
    PSymbolBase returnType;
    PSymbolTable args;
};
typedef std::shared_ptr<SymbolProcHeader> PSymbolProcHeader;

class SymbolFuncHeader: public SymbolProcHeader {
public:
    SymbolFuncHeader();
    void SetReturnType(const PSymbolBase returnType);
    const SymType GetSymType() const { return SymType::FuncHeader; };
    void PrintTypeName(unsigned int depth = 0);
    const std::string GetTypeName() const;
};
typedef std::shared_ptr<SymbolFuncHeader> PSymbolFuncHeader;

class SymbolProcedure: public SymbolComplex {
public:
    SymbolProcedure(std::string name, PSymbolProcHeader type, PSymbolTable locals);
    void Print(unsigned int depth = 0);
    const SymType GetSymType() const { return SymType::Procedure; };
protected:
    PSymbolTable locals;
};
typedef std::shared_ptr<SymbolProcedure> PSymbolProcedure;

class SymbolFunction: public SymbolProcedure {
public:
    SymbolFunction(std::string name, PSymbolFuncHeader type, PSymbolTable locals);
    const SymType GetSymType() const { return SymType::Function; };
protected:
    PSymbolTable locals;
};
typedef std::shared_ptr<SymbolFunction> PSymbolFunction;

class SymbolBaseType: public SymbolBase {
public:
    SymbolBaseType(BaseType type);
    const SymType GetSymType() const { return SymType::BaseType; }
    const std::string GetTypeName() const;
    void PrintTypeName(unsigned int depth = 0);
    const std::any GetInitValue() const;
protected:
    BaseType type;
};
typedef std::shared_ptr<SymbolBaseType> PSymbolBaseType;

class SymbolSubRange: public SymbolBase {
public:
    SymbolSubRange(int left, int right);
    const SymType GetSymType() const { return SymType::SubRange; }
    void PrintTypeName(unsigned int depth = 0);
    const std::string GetTypeName() const;
    const std::string GetValue() const;
protected:
    int left, right;
};
typedef std::shared_ptr<SymbolSubRange> PSymbolSubRange;

class SymbolArray: public SymbolBase {
public:
    SymbolArray(PSymbolBase type);
    const SymType GetSymType() const override { return SymType::Array; }
    const std::any GetInitValue() const override;
    const std::string GetTypeName() const override = 0;
    virtual void SetType(const PSymbolBase type);
    const PSymbolBase GetType() const;
protected:
    PSymbolBase type;
};
typedef std::shared_ptr<SymbolArray> PSymbolArray;

class SymbolOpenArray: public SymbolArray {
public:
    SymbolOpenArray(PSymbolBase);
    const SymType GetSymType() const { return SymType::OpenArray; }
    const std::string GetTypeName() const;
    void PrintTypeName(unsigned int depth = 0);
};
typedef std::shared_ptr<SymbolOpenArray> PSymbolOpenArray;

class SymbolStaticArray: public SymbolArray {
public:
    SymbolStaticArray(PSymbolBase type, PSymbolSubRange subRange);
    const std::string GetTypeName() const;
    void PrintTypeName(unsigned int depth = 0);
protected:
    PSymbolSubRange subRange;
};
typedef std::shared_ptr<SymbolStaticArray> PSymbolStaticArray;

class SymbolDynamicArray: public SymbolArray {
public:
    SymbolDynamicArray(PSymbolBase type);
    const std::string GetTypeName() const;
    void PrintTypeName(unsigned int depth = 0);
};
typedef std::shared_ptr<SymbolDynamicArray> PSymbolDynamicArray;

static std::map<BaseType, PSymbolBaseType> basicSymbol = {
        { BaseType::Integer, PSymbolBaseType(new SymbolBaseType(BaseType::Integer)) },
        { BaseType::Double,  PSymbolBaseType(new SymbolBaseType(BaseType::Double))  },
        { BaseType::Char,    PSymbolBaseType(new SymbolBaseType(BaseType::Char))    }
};