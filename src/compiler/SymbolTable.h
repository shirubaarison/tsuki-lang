#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <vector>

struct Local {
  std::string name;
  int depth;
};

class SymbolTable {
public:
  SymbolTable();
  int resolveLocal(const std::string& name);
  int resolveGlobal(const std::string& name);

  void defineLocal(const std::string& name);
  void defineGlobal(const std::string& name);

  void beginScope();
  int endScope();

  int getScopeDepth() { return m_scopeDepth; }

private:
  int m_scopeDepth;

  std::vector<Local> m_locals;
  std::vector<std::string> m_globals;
};

#endif // !SYMBOL_TABLE_H
