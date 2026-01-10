#ifndef STMT_H
#define STMT_H

#include "Visitor.h"
#include <ostream>

class Stmt {
public:
  virtual ~Stmt() = default;
  virtual void print(std::ostream& builder) const = 0;
  virtual void accept(Visitor& visitor) const = 0;
};

#endif // !STMT_H
