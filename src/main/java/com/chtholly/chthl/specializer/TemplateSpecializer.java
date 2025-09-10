package com.chtholly.chthl.specializer;

import com.chtholly.chthl.ast.ElementNode;
import com.chtholly.chthl.ast.Node;
import com.chtholly.chthl.ast.StyleBlockNode;
import com.chtholly.chthl.ast.StylePropertyNode;
import com.chtholly.chthl.ast.custom.CustomizationBlockNode;
import com.chtholly.chthl.ast.custom.DeleteNode;
import com.chtholly.chthl.ast.custom.InsertNode;
import com.chtholly.chthl.ast.custom.ModificationNode;
import com.chtholly.chthl.lexer.Token;

import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.stream.Collectors;

public class TemplateSpecializer {

    public List<Node> specialize(List<Node> originalBody, CustomizationBlockNode customization) {
        List<Node> clonedBody = new ArrayList<>();
        for (Node node : originalBody) {
            clonedBody.add(node.clone());
        }

        if (customization != null) {
            for (ModificationNode mod : customization.modifications) {
                if (mod instanceof DeleteNode) {
                    applyDelete(clonedBody, (DeleteNode) mod);
                } else if (mod instanceof InsertNode) {
                    applyInsert(clonedBody, (InsertNode) mod);
                }
            }
        }

        return clonedBody;
    }

    private void applyDelete(List<Node> body, DeleteNode deleteNode) {
        for (List<Token> targetTokens : deleteNode.targets) {
            if (targetTokens.isEmpty()) continue;
            if (targetTokens.size() == 1) {
                String targetName = targetTokens.get(0).getLexeme();
                body.removeIf(node -> (node instanceof ElementNode) && ((ElementNode) node).tagName.equals(targetName));
                for (Node node : body) {
                    deleteStylePropertiesRecursive(node, targetName);
                }
            }
        }
    }

    private void deleteStylePropertiesRecursive(Node node, String propertyKey) {
        if (!(node instanceof ElementNode)) return;
        ElementNode element = (ElementNode) node;
        for (Node child : element.children) {
            if (child instanceof StyleBlockNode) {
                ((StyleBlockNode) child).directPropertiesAndUsages.removeIf(propOrUsage ->
                    (propOrUsage instanceof StylePropertyNode) && ((StylePropertyNode) propOrUsage).key.equals(propertyKey)
                );
            }
            deleteStylePropertiesRecursive(child, propertyKey);
        }
    }

    private void applyInsert(List<Node> body, InsertNode insertNode) {
        String targetSelector = insertNode.target.stream().map(Token::getLexeme).collect(Collectors.joining(""));
        Pattern pattern = Pattern.compile("(\\w+)\\[(\\d+)\\]");
        Matcher matcher = pattern.matcher(targetSelector);

        if (!matcher.matches()) {
            System.err.println("Warning: Insert target selector format not supported yet: " + targetSelector);
            return;
        }
        String targetTag = matcher.group(1);
        int targetIndex = Integer.parseInt(matcher.group(2));

        int foundAtIndex = -1;
        int count = 0;
        for (int i = 0; i < body.size(); i++) {
            Node node = body.get(i);
            if (node instanceof ElementNode && ((ElementNode) node).tagName.equals(targetTag)) {
                if (count == targetIndex) {
                    foundAtIndex = i;
                    break;
                }
                count++;
            }
        }

        if (foundAtIndex == -1) {
            System.err.println("Warning: Could not find insert target: " + targetSelector);
            return;
        }

        String position = insertNode.position.getLexeme();
        if ("after".equals(position)) {
            body.addAll(foundAtIndex + 1, insertNode.body);
        } else if ("before".equals(position)) {
            body.addAll(foundAtIndex, insertNode.body);
        } else if ("replace".equals(position)) {
            body.remove(foundAtIndex);
            body.addAll(foundAtIndex, insertNode.body);
        }
    }
}
