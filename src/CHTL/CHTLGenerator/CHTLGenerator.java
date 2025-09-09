package CHTL.CHTLGenerator;

import CHTL.CHTLNode.*;
import java.util.List;
import java.util.Map;

/**
 * Traverses the CHTL AST and generates an HTML string.
 * It implements the Visitor pattern to handle different node types.
 * This generator is stateless; its methods are called to process parts of the AST.
 */
public class CHTLGenerator implements Visitor<String> {

    /**
     * Generates the HTML for a list of AST nodes.
     * @param ast The list of nodes to process.
     * @return The generated HTML as a string.
     */
    public String generate(List<BaseNode> ast) {
        StringBuilder builder = new StringBuilder();
        for (BaseNode node : ast) {
            builder.append(node.accept(this));
        }
        return builder.toString();
    }

    /**
     * Generates the full <style>...</style> block from a list of rules.
     * @param globalStyles The list of rules to include.
     * @return The HTML <style> block, or an empty string if there are no styles.
     */
    public String generateStyleBlock(List<StyleRule> globalStyles) {
        if (globalStyles == null || globalStyles.isEmpty()) {
            return "";
        }
        StringBuilder builder = new StringBuilder();
        builder.append("<style>");
        for (StyleRule rule : globalStyles) {
            // Using the StyleRule's toString() method for convenience.
            builder.append(rule.toString());
        }
        builder.append("</style>");
        return builder.toString();
    }

    @Override
    public String visitElementNode(ElementNode node) {
        StringBuilder builder = new StringBuilder();
        builder.append("<").append(node.getTagName());

        for (Map.Entry<String, String> entry : node.getAttributes().entrySet()) {
            builder.append(" ")
                   .append(entry.getKey())
                   .append("=\"")
                   .append(entry.getValue())
                   .append("\"");
        }

        builder.append(">");

        for (BaseNode child : node.getChildren()) {
            builder.append(child.accept(this));
        }

        builder.append("</").append(node.getTagName()).append(">");
        return builder.toString();
    }

    @Override
    public String visitTextNode(TextNode node) {
        return node.getText();
    }

    @Override
    public String visitCommentNode(CommentNode node) {
        return "<!-- " + node.getComment() + " -->";
    }

    @Override
    public String visitStyleNode(StyleNode node) {
        // This should not be called. The StyleProcessor is expected to remove StyleNodes from the tree.
        // Returning an empty string as a fallback.
        return "";
    }

    @Override
    public String visitStyleTemplateNode(StyleTemplateNode node) {
        // Template definitions do not produce output themselves.
        return "";
    }

    @Override
    public String visitElementTemplateNode(ElementTemplateNode node) {
        // Template definitions do not produce output themselves.
        return "";
    }

    @Override
    public String visitVarTemplateNode(VarTemplateNode node) {
        // Template definitions do not produce output themselves.
        return "";
    }

    @Override
    public String visitStyleTemplateUsageNode(StyleTemplateUsageNode node) {
        // Template usage is handled by the StyleProcessor and does not produce direct output.
        return "";
    }

    @Override
    public String visitElementTemplateUsageNode(ElementTemplateUsageNode node) {
        // Template usage is handled by the ASTProcessor and does not produce direct output.
        return "";
    }

    @Override
    public String visitCustomStyleNode(CustomStyleNode node) {
        // Custom definitions do not produce output themselves.
        return "";
    }

    @Override
    public String visitCustomElementNode(CustomElementNode node) {
        // Custom definitions do not produce output themselves.
        return "";
    }

    @Override
    public String visitCustomVarNode(CustomVarNode node) {
        // Custom definitions do not produce output themselves.
        return "";
    }

    @Override
    public String visitDeleteInstructionNode(DeleteInstructionNode node) {
        // This is a processing instruction and does not produce output.
        return "";
    }

    @Override
    public String visitInsertInstructionNode(InsertInstructionNode node) {
        // This is a processing instruction and does not produce output.
        return "";
    }

    @Override
    public String visitImportNode(ImportNode node) {
        // Import nodes are handled by the parser and do not produce output.
        return "";
    }

    @Override
    public String visitConfigurationNode(ConfigurationNode node) {
        // Configuration nodes do not produce output.
        return "";
    }

    @Override
    public String visitUseNode(UseNode node) {
        // Use nodes are handled by the pre-scanner and do not produce output.
        if (node.getTarget().equalsIgnoreCase("html5")) {
            return "<!DOCTYPE html>";
        }
        return "";
    }

    @Override
    public String visitOriginNode(OriginNode node) {
        // Origin nodes contain raw content to be passed through.
        return node.getContent();
    }
}
