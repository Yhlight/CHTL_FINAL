package CHTL.Expression;

import CHTL.Node.ElementNode;
import CHTL.Node.BaseNode;
import java.util.List;

/**
 * Holds the necessary context for evaluating a style expression.
 * @param self The element whose style property is being computed.
 * @param documentRoot A reference to the root of the main document AST to resolve references.
 */
public record EvaluationContext(ElementNode self, List<BaseNode> documentRoot) {
}
