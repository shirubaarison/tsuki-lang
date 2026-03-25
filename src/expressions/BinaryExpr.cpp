#include "expressions/BinaryExpr.h"

#include "frontend/token/TokenType.h"
#include "Visitor.h"

BinaryExpr::BinaryExpr(std::unique_ptr<Expr> left, TokenType op,
                       std::unique_ptr<Expr> right)
    : mLeft(std::move(left)), mOperator(op), mRight(std::move(right)) {}

void BinaryExpr::print(std::ostream& builder) const 
{
  builder << "BinaryExpr(";
  mLeft->print(builder);
  builder << " " << tokenToOperator(mOperator) << " ";
  mRight->print(builder);
  builder << ")";
}

void BinaryExpr::accept(Visitor& visitor) const { visitor.visitBinaryExpr(this); }

const Expr* BinaryExpr::getLeft() const { return mLeft.get(); }

const Expr* BinaryExpr::getRight() const { return mRight.get(); }

TokenType BinaryExpr::getOperatorType() const { return mOperator; }
