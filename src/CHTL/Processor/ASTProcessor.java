package CHTL.Processor;

import CHTL.Expression.*;
import CHTL.Manage.DefinitionManager;
import CHTL.Node.BaseNode;
import CHTL.Node.ElementNode;
import CHTL.Node.StyleNode;

import java.util.List;
import java.util.Map;

public class ASTProcessor {

    private final DefinitionManager definitionManager;
    private final List<BaseNode> documentRoot;

    public ASTProcessor(List<BaseNode> documentRoot, DefinitionManager definitionManager) {
        this.documentRoot = documentRoot;
        this.definitionManager = definitionManager;
    }

    public void process() {
        // In a full implementation, we would also expand element templates here.
        processStyles(documentRoot, true); // Process global styles first
        processStyles(documentRoot, false); // Process local styles
    }

    private void processStyles(List<BaseNode> nodes, boolean isGlobal) {
        for (BaseNode node : nodes) {
            if (node instanceof ElementNode element) {
                for (BaseNode child : element.getChildren()) {
                    if (child instanceof StyleNode styleNode) {
                        if (isGlobal) continue; // Skip local styles in global pass
                        evaluateStyleNode(styleNode, element);
                    }
                }
                processStyles(element.getChildren(), false); // Children are always in local context
            } else if (node instanceof StyleNode styleNode) {
                if (!isGlobal) continue; // Skip global styles in local pass
                evaluateStyleNode(styleNode, null);
            }
        }
    }

    private void evaluateStyleNode(StyleNode styleNode, ElementNode parent) {
        // This is a simplified evaluation loop. A real implementation needs a dependency graph.
        for (Map.Entry<String, String> property : styleNode.getProperties().entrySet()) {
            String propertyName = property.getKey();
            String propertyValue = property.getValue();

            try {
                ExpressionLexer lexer = new ExpressionLexer(propertyValue);
                List<Token> tokens = lexer.tokenize();

                // If it's just a simple literal, no need to parse/eval
                if (tokens.size() == 2 && (tokens.get(0).type() == ExpressionTokenType.STRING || tokens.get(0).type() == ExpressionTokenType.NUMBER)) {
                    continue;
                }

                ExpressionParser parser = new ExpressionParser(tokens);
                ExpressionNode expression = parser.parse();

                EvaluationContext context = new EvaluationContext(parent, this.documentRoot);
                ExpressionEvaluator evaluator = new ExpressionEvaluator();
                ExpressionNode result = evaluator.evaluate(expression, context);

                // Update the property in the style node with the computed value
                // This part is also simplified. The result node needs to be converted to a string.
                if (result instanceof UnitNode) {
                    UnitNode unitResult = (UnitNode) result;
                    styleNode.getProperties().put(propertyName, unitResult.getValue() + unitResult.getUnit());
                } else if (result instanceof LiteralNode) {
                     styleNode.getProperties().put(propertyName, ((LiteralNode) result).getValue().toString());
                }

            } catch (Exception e) {
                System.err.println("Failed to evaluate expression for property '" + propertyName + "': " + propertyValue);
                e.printStackTrace();
            }
        }
    }
}
