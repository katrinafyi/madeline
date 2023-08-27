
#include "z3.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <z3++.h>
#include <z3_api.h>

/**
 *
   Demonstration of how Z3 can be used to prove validity of
   De Morgan's Duality Law: {e not(x and y) <-> (not x) or ( not y) }
   https://github.com/Z3Prover/z3/blob/master/examples/c%2B%2B/example.cpp
*/
void demorgan() {
  using namespace z3;

  std::cout << "de-Morgan example\n";

  context c;

  expr x = c.bool_const("x");
  expr y = c.bool_const("y");
  expr conjecture = (!(x && y)) == (!x || !y);

  solver s(c);
  // adding the negation of the conjecture as a constraint.
  s.add(!conjecture);
  std::cout << s << "\n";
  std::cout << s.to_smt2() << "\n";
  switch (s.check()) {
  case unsat:
    std::cout << "de-Morgan is valid\n";
    break;
  case sat:
    std::cout << "de-Morgan is not valid\n";
    break;
  case unknown:
    std::cout << "unknown\n";
    break;
  }
}
