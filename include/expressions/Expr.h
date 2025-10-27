#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <ostream>

class Expr {
public:
  virtual ~Expr() = default;
  virtual void print(std::ostream &builder) const = 0;
};

#endif // !EXPRESSION_H
