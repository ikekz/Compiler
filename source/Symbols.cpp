#include "Symbols.h"
#include <iostream>
#include <iomanip>

using namespace std;

const std::string Symbol::GetTypeName() const {
    return symTypeName.at(GetSymType());
}

SymbolComplex::SymbolComplex(string name, PSymbolBase type) : name(name), type(type) {}

const string SymbolComplex::GetName() const {
    return name;
}

void SymbolComplex::SetName(const string name) {
    this->name = name;
}

const PSymbolBase SymbolComplex::GetType() const {
    return type;
}

SymbolComplexWithValue::SymbolComplexWithValue(std::string name, PSymbolBase type, std::any value) :
        SymbolComplex(name, type), value(value) {}

void SymbolComplexWithValue::Print(unsigned int depth) {
    cout << string(depth, ' ') << left << setw(nameWidth) << name << setw(symTypeWidth)
         << symTypeName.at(GetSymType());
    if (type->GetSymType() == SymType::BaseType)
        cout << setw(typeWidth);
    PrintTypeName(depth + nameWidth + symTypeWidth);
    if (type->GetSymType() == SymType::BaseType)
        cout << GetValueText();
    cout << endl;
}

const std::any SymbolComplexWithValue::GetValue() const {
    return value;
}

const std::string SymbolComplexWithValue::GetValueText() const {
    if (type->GetTypeName() == baseType.at(BaseType::Integer))
        return to_string(any_cast<int>(value));
    if (type->GetTypeName() == baseType.at(BaseType::Double))
        return to_string(any_cast<double>(value));
    if (type->GetTypeName() == baseType.at(BaseType::Char))
        return any_cast<string>(value);
    return "";
}

const string SymbolComplex::GetTypeName() const {
    return type->GetTypeName();
}

void SymbolComplex::PrintTypeName(unsigned int depth) {
    type->PrintTypeName(depth);
}

void SymbolComplex::Print(unsigned int depth) {
    cout << string(depth, ' ') << left << setw(nameWidth) << name
         << setw(symTypeWidth) << symTypeName.at(GetSymType());
    PrintTypeName(depth + nameWidth + symTypeWidth);
    cout << endl;
}

SymbolType::SymbolType(string name, PSymbolBase type) : SymbolComplex(name, type) {}

const any SymbolType::GetInitValue() const {
    return type->GetInitValue();
}

SymbolTypeAlias::SymbolTypeAlias(std::string name, PSymbolBase type) :
        SymbolType(name, type) {}

SymbolSubRange::SymbolSubRange(int left, int right) : left(left), right(right) {}

void SymbolSubRange::PrintTypeName(unsigned int depth) {
    cout << "subrange " + GetValue();
}

const std::string SymbolSubRange::GetValue() const {
    return to_string(left) + ".." + to_string(right);
}

const std::string SymbolSubRange::GetTypeName() const {
    return "subrange " + GetValue();
}

SymbolArray::SymbolArray(PSymbolBase type) : type(type) {}

void SymbolArray::SetType(const PSymbolBase type) {
    this->type = type;
}

const any SymbolArray::GetInitValue() const {
    return type->GetInitValue();
}

const PSymbolBase SymbolArray::GetType() const {
    return type;
}

SymbolStaticArray::SymbolStaticArray(PSymbolBase type, PSymbolSubRange subRange) :
        SymbolArray(type), subRange(subRange) {}

void SymbolStaticArray::PrintTypeName(unsigned int depth) {
    cout << "array [" + subRange->GetValue() + "] of ";
    type->PrintTypeName(depth);
}

const std::string SymbolStaticArray::GetTypeName() const {
    return "array [" + subRange->GetValue() + "] of " + type->GetTypeName();
}

SymbolDynamicArray::SymbolDynamicArray(PSymbolBase type) : SymbolArray(type) {}

void SymbolDynamicArray::PrintTypeName(unsigned int depth) {
    cout << "array of ";
    type->PrintTypeName(depth);
}

const std::string SymbolDynamicArray::GetTypeName() const {
    return "array of " + type->GetTypeName();
}

SymbolConst::SymbolConst(std::string name, PSymbolBase type, any value) :
        SymbolComplexWithValue(name, type, value) {}

SymbolVar::SymbolVar(std::string name, PSymbolBase type, any value) :
        SymbolComplexWithValue(name, type, value) {}

SymbolBaseType::SymbolBaseType(BaseType type) : type(type) {}

const std::string SymbolBaseType::GetTypeName() const {
    return baseType.at(type);
}

void SymbolBaseType::PrintTypeName(unsigned int depth) {
    cout << baseType.at(type);
}

const any SymbolBaseType::GetInitValue() const {
    return baseTypeInitValue.at(type);
}

SymbolRecord::SymbolRecord() : fields(new SymbolTable) {}

void SymbolRecord::AddField(PSymbolRecordField field) {
    fields->AddSymbol(field);
}

const std::string SymbolRecord::GetTypeName() const {
    string str = "record ";
    str += fields->GetSymbolTypeNames();
    return str;
}

void SymbolRecord::PrintTypeName(unsigned int depth) {
    cout << "record:" << endl;
    fields->Print(depth);
    cout << string(depth, ' ');
    cout << "end";
}

const PSymbolTable SymbolRecord::GetFields() const {
    return fields;
}

SymbolRecordField::SymbolRecordField(std::string name, PSymbolBase type) : SymbolComplex(name, type) {}

SymbolValueParameter::SymbolValueParameter(std::string name, PSymbolBase type) :
        SymbolComplex(name, type) {};

SymbolOpenArray::SymbolOpenArray(PSymbolBase type) : SymbolArray(type) {}

void SymbolOpenArray::PrintTypeName(unsigned int depth) {
    cout << "{open} array of ";
    type->PrintTypeName(depth);
}

const std::string SymbolOpenArray::GetTypeName() const {
    return "open array of " + type->GetTypeName();
}

SymbolProcHeader::SymbolProcHeader() : args(new SymbolTable()), returnType(new SymbolBaseType(BaseType::Untyped)) {}

const std::string SymbolProcHeader::GetTypeName() const {
    return "args " + args->GetSymbolTypeNames();
}

void SymbolProcHeader::PrintTypeName(unsigned int depth) {
    cout << "args:" << endl;
    if (args->Size() < 1) {
        cout << string(depth, ' ') << "none" << endl;
        return;
    }
    args->Print(depth);
}

const PSymbolTable SymbolProcHeader::GetArgs() const {
    return args;
}

const PSymbolBase SymbolProcHeader::GetReturnType() const {
    return returnType;
}

SymbolFuncHeader::SymbolFuncHeader() : SymbolProcHeader() {}

void SymbolFuncHeader::SetReturnType(const PSymbolBase returnType) {
    this->returnType = returnType;
}

const std::string SymbolFuncHeader::GetTypeName() const {
    return "args " + args->GetSymbolTypeNames() + " " + returnType->GetTypeName();
}

void SymbolFuncHeader::PrintTypeName(unsigned int depth) {
    cout << "return type: " << endl;
    cout << string(depth, ' ');
    returnType->PrintTypeName(depth);
    cout << endl;
    cout << string(depth, ' ') << "args:" << endl;
    if (args->Size() < 1) {
        cout << string(depth, ' ') << "none" << endl;
        return;
    }
    args->Print(depth);
}

SymbolProcedure::SymbolProcedure(std::string name, PSymbolProcHeader type, PSymbolTable locals) :
        SymbolComplex(name, type), locals(locals) {}

void SymbolProcedure::Print(unsigned int depth) {
    cout << string(depth, ' ') << left << setw(nameWidth) << name << setw(symTypeWidth)
         << symTypeName.at(GetSymType());
    depth += nameWidth + symTypeWidth;
    PrintTypeName(depth);
    cout << string(depth, ' ') << "locals:" << endl;
    if (locals->Size() < 1) {
        cout << string(depth, ' ') << "none" << endl;
        return;
    }
    locals->Print(depth);
}

SymbolFunction::SymbolFunction(std::string name, PSymbolFuncHeader type, PSymbolTable locals) :
        SymbolProcedure(name, type, locals) {}
