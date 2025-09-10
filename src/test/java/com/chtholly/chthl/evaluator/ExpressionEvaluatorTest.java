package com.chtholly.chthl.evaluator;

import com.chtholly.chthl.ast.expr.Expression;
import com.chtholly.chthl.ast.expr.GlobalVariableExpr;
import com.chtholly.chthl.ast.expr.LiteralExpr;
import com.chtholly.chthl.ast.expr.VariableExpr;
import com.chtholly.chthl.ast.template.TemplateNode;
import com.chtholly.chthl.ast.template.VarTemplateNode;
import com.chtholly.chthl.lexer.Token;
import com.chtholly.chthl.lexer.TokenType;
import org.junit.jupiter.api.Test;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import static org.junit.jupiter.api.Assertions.assertEquals;

class ExpressionEvaluatorTest {

    @Test
    void testGlobalVariableLookup() {
        // 1. Setup the template table with a VarTemplate
        Map<String, Expression> variables = new HashMap<>();
        variables.put("width", new LiteralExpr("100px"));

        VarTemplateNode varTemplate = new VarTemplateNode(
            new Token(TokenType.IDENTIFIER, "Theme", null, 1),
            variables
        );

        Map<String, Map<String, TemplateNode>> templateTable = new HashMap<>();
        Map<String, TemplateNode> globalNamespace = new HashMap<>();
        globalNamespace.put("Theme", varTemplate);
        templateTable.put("__global__", globalNamespace);

        // 2. Create the evaluator
        ExpressionEvaluator evaluator = new ExpressionEvaluator(null, new ArrayList<>(), templateTable, "__global__");

        // 3. Create the expression to evaluate (&width)
        GlobalVariableExpr exprToEvaluate = new GlobalVariableExpr(new Token(TokenType.IDENTIFIER, "width", null, 1));

        // 4. Evaluate and assert
        Object result = evaluator.evaluate(exprToEvaluate);

        // 5. Assert the result
        UnitValue expected = new UnitValue(100, "px");
        assertEquals(expected, result);
    }
}
