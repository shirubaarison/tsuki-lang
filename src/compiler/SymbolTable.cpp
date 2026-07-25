#include "compiler/SymbolTable.h"

SymbolTable::SymbolTable() : m_scopeDepth(0) {}

int SymbolTable::resolveLocal(const std::string& name)
{
  for (int i = m_locals.size() - 1; i >= 0; --i)
  {
    if (m_locals[i].name == name)
      return i;
  }

  return -1;
}

int SymbolTable::resolveGlobal(const std::string& name)
{
  for (size_t i = 0; i < m_globals.size(); ++i)
  {
    if (m_globals[i] == name)
      return i;
  }

  return -1;
}

void SymbolTable::defineLocal(const std::string& name)
{
  Local local = {name, m_scopeDepth};
  m_locals.push_back(local);
}

void SymbolTable::defineGlobal(const std::string& name) { m_globals.push_back(name); }


void SymbolTable::beginScope() { m_scopeDepth++; }

int SymbolTable::endScope()
{
  int toPop{0};
  m_scopeDepth--;

  while (!m_locals.empty() && m_locals.back().depth > m_scopeDepth)
  {
    m_locals.pop_back();
    toPop++;
  }

  return toPop;
}
