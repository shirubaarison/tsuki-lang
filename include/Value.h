#ifndef VALUE_H
#define VALUE_H

#include <iostream>
#include <string>
#include <variant>

using Value = std::variant<std::monostate, int, double, std::string>;

struct ValuePrinter {
  void operator()(std::monostate) const { std::cout << "nil"; }

  void operator()(int i) const { std::cout << i; }
  void operator()(double d) const { std::cout << d; }
  void operator()(const std::string &s) const { std::cout << "'" << s << "'"; }
};

#endif // !VALUE_H
