package CHTL.Expression;

import CHTL.Node.ElementNode;
import CHTL.Node.BaseNode;
import java.util.List;

/**
 * Holds the necessary context for evaluating a style expression.
 * @param self The element whose style property is being computed.
 * @param documentRoot A reference to the root of the main document AST to resolve references.
 */
import java.util.Map;

import CHTL.Manage.DefinitionManager;

/**
 * Holds the necessary context for evaluating a style expression.
 * @param self The element whose style property is being computed.
 * @param documentRoot A reference to the root of the main document AST to resolve references.
 * @param computedValues A map of already computed style values to handle self-references.
 * @param definitionManager A reference to the definition manager for variable lookups.
 */
public record EvaluationContext(ElementNode self, List<BaseNode> documentRoot, Map<String, String> computedValues, DefinitionManager definitionManager) {
}
