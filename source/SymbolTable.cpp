#include "Symbols.h"
#include "Error.h"
#include <string>
#include <algorithm>

using namespace std;

void SymbolTableStack::AddTable(PSymbolTable table) {
    tables.push_back(table);
}

PSymbolTable SymbolTableStack::Top() {
    return tables.back();
}

void SymbolTableStack::Pop() {
    tables.pop_back();
}

bool SymbolTableStack::HaveSymbol(std::string name) {
    return FindSymbol(name) != nullptr;
}

PSymbolComplex SymbolTableStack::FindSymbol(std::string name) {
    transform(name.begin(), name.end(), name.begin(), ::tolower);
    for (int i = tables.size() - 1; i >= 0; i--) {
        PSymbolComplex symb = tables[i]->FindSymbol(name);
        if (symb != nullptr)
            return symb;
    }
    return nullptr;
}

void SymbolTableStack::Print(unsigned int depth) {
    for (int i = tables.size() - 1; i >= 0; i--)
        tables[i]->Print(depth);
}

void SymbolTable::AddSymbol(PSymbolComplex symbol) {
    symbols.push_back(symbol);
    string name = symbol->GetName();
    transform(name.begin(), name.end(), name.begin(), ::tolower);
    symbolNames.insert({ name, symbols.size() - 1 });
}

PSymbolComplex SymbolTable::FindSymbol(std::string name) {
    auto it = symbolNames.find(name);
    if (it == symbolNames.end())
        return nullptr;
    return symbols[it->second];
}

void SymbolTable::Print(unsigned int depth) {
    for (auto &symbol: symbols)
        symbol->Print(depth);
}

const std::string SymbolTable::GetSymbolTypeNames() const {
    string str;
    for (auto &symbol: symbols)
        str += "<" + symbol->GetName() + ": " + symbol->GetTypeName() + "> ";
    return str;
}

bool SymbolTable::HaveSymbol(std::string name) {
    return FindSymbol(name) != nullptr;
}

unsigned int SymbolTable::Size() {
    return symbols.size();
}

const std::vector<PSymbolComplex> &SymbolTable::GetSymbols() const {
    return symbols;
}

