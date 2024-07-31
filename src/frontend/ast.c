#include "ast.h"

ConcreteVisitor visitor;

void acceptBinary(Expression* expression, Visitor* visitor) {
    visitor->visitBinary(visitor, expression);
}

Binary init_binary(Expression* left, TokenType operator, Expression* right) {
    Binary binary = {.base.accept = acceptBinary, .left = left, .right = right, .operator = operator};
    return binary;
}

ConcreteVisitor* init_visitor() {
    visitor.base.visitBinary = acceptBinary;
    return &visitor;
};

void accept_literal(Expression* expression, Visitor* visitor) {
    visitor->visit_literal(visitor, (Literal*)expression);
}

Literal init_literal(int value) {
    Literal literal = {.base.accept = accept_literal, .value = value};
    return literal;
}