package com.chtholly.chthl.ast.expr;

/**
 * The Visitor interface for the Expression AST nodes.
 * @param <R> The return type of the visit methods.
 */
public interface Visitor<R> {
    R visitBinaryExpr(BinaryExpr expr);
    R visitGroupingExpr(GroupingExpr expr);
    R visitLiteralExpr(LiteralExpr expr);
    R visitVariableExpr(VariableExpr expr);
}
