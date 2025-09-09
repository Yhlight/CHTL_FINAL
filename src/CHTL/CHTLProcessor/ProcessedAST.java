package CHTL.CHTLProcessor;

import CHTL.CHTLNode.BaseNode;
import CHTL.CHTLNode.StyleRule;

import java.util.List;

/**
 * A data class (record) to hold the results of the AST processing step.
 * It contains the modified Abstract Syntax Tree and a list of style rules
 * that have been collected for the global stylesheet.
 */
public record ProcessedAST(List<BaseNode> ast, List<StyleRule> globalStyles) {
}
