#ifndef VALUE_H
#define VALUE_H

#include <ios>
#include <iostream>
#include <string>
#include <variant>

using Value = std::variant<std::monostate, int, double, std::string, bool>;

struct ValuePrinter {
  void operator()(std::monostate) const { std::cout << "nil"; }

  void operator()(int i) const { std::cout << i; }
  void operator()(double d) const { std::cout << d; }
  void operator()(const std::string& s) const { std::cout << "'" << s << "'"; }
  void operator()(bool b) const {
    std::cout << std::boolalpha;
    std::cout << b;
    std::cout << std::noboolalpha;
  }
};

template<class... Ts>
struct overload : Ts... { 
  using Ts::operator()...;
};

template<class... Ts>
overload(Ts...) -> overload<Ts...>;

#endif // !VALUE_H
