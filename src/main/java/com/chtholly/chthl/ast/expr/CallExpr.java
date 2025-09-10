package com.chtholly.chthl.ast.expr;

import java.util.ArrayList;
import java.util.List;

/**
 * Represents a function-call-like expression in the AST.
 * This is used for variable template usage, e.g., <code>ThemeColor(tableColor)</code>.
 */
public class CallExpr extends Expression {
    public final Expression callee;
    public final List<Expression> arguments;
    // The closing parenthesis token is stored for error reporting.
    public final com.chtholly.chthl.lexer.Token paren;

    public CallExpr(Expression callee, com.chtholly.chthl.lexer.Token paren, List<Expression> arguments) {
        this.callee = callee;
        this.paren = paren;
        this.arguments = arguments;
    }

    @Override
    public <R> R accept(Visitor<R> visitor) {
        return visitor.visitCallExpr(this);
    }

    @Override
    public Expression clone() {
        List<Expression> clonedArguments = new ArrayList<>();
        for (Expression arg : this.arguments) {
            clonedArguments.add(arg.clone());
        }
        return new CallExpr(this.callee.clone(), this.paren, clonedArguments);
    }
}
