package com.chtholly.chthl.specializer;

import com.chtholly.chthl.ast.ElementNode;
import com.chtholly.chthl.ast.Node;
import com.chtholly.chthl.ast.StyleBlockNode;
import com.chtholly.chthl.ast.StylePropertyNode;
import com.chtholly.chthl.ast.custom.CustomizationBlockNode;
import com.chtholly.chthl.ast.custom.DeleteNode;
import com.chtholly.chthl.ast.custom.InsertNode;
import com.chtholly.chthl.ast.custom.ModificationNode;
import com.chtholly.chthl.ast.template.TemplateUsageNode;
import com.chtholly.chthl.lexer.Token;

import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.stream.Collectors;

public class TemplateSpecializer {

    // Helper class to return search results for insertions
    private static class SearchResult {
        final List<Node> container;
        final int index;
        final Node target;

        SearchResult(List<Node> container, int index, Node target) {
            this.container = container;
            this.index = index;
            this.target = target;
        }
    }

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

            String targetStr = targetTokens.stream().map(Token::getLexeme).collect(Collectors.joining(""));
            Pattern pattern = Pattern.compile("(\\w+)\\[(\\d+)\\]");
            Matcher matcher = pattern.matcher(targetStr);

            if (matcher.matches()) {
                deleteElementByIndex(body, matcher.group(1), Integer.parseInt(matcher.group(2)));
            } else if (targetTokens.get(0).getLexeme().startsWith("@")) {
                if (targetTokens.size() > 1) {
                    deleteTemplateInheritance(body, targetTokens.get(1).getLexeme());
                }
            } else {
                deleteElementByTag(body, targetStr);
                deleteStylePropertyByName(body, targetStr);
            }
        }
    }

    private void deleteElementByIndex(List<Node> body, String tagName, int index) {
        int count = 0;
        int foundAtIndex = -1;
        for (int i = 0; i < body.size(); i++) {
            Node node = body.get(i);
            if (node instanceof ElementNode && ((ElementNode) node).tagName.equals(tagName)) {
                if (count == index) {
                    foundAtIndex = i;
                    break;
                }
                count++;
            }
        }
        if (foundAtIndex != -1) {
            body.remove(foundAtIndex);
        }
    }

    private void deleteTemplateInheritance(List<Node> body, String templateName) {
        body.removeIf(node -> (node instanceof TemplateUsageNode) && ((TemplateUsageNode) node).name.getLexeme().equals(templateName));
    }

    private void deleteElementByTag(List<Node> body, String tagName) {
        body.removeIf(node -> (node instanceof ElementNode) && ((ElementNode) node).tagName.equals(tagName));
    }

    private void deleteStylePropertyByName(List<Node> body, String propertyKey) {
        for (Node node : body) {
            if (node instanceof StyleBlockNode) {
                ((StyleBlockNode) node).directPropertiesAndUsages.removeIf(propOrUsage ->
                    (propOrUsage instanceof StylePropertyNode) && ((StylePropertyNode) propOrUsage).key.equals(propertyKey)
                );
            } else if (node instanceof ElementNode) {
                deleteStylePropertyByName(((ElementNode) node).children, propertyKey);
            }
        }
    }

    private void applyInsert(List<Node> body, InsertNode insertNode) {
        String targetSelector = insertNode.target.stream().map(Token::getLexeme).collect(Collectors.joining(""));
        String position = insertNode.position.getLexeme();

        // Case 1: Inserting into a style block
        if (targetSelector.equals("style") && "into".equals(position)) {
            insertIntoStyleBlocks(body, insertNode.body);
            return;
        }

        // Case 2: Inserting elements relative to another element
        Pattern pattern = Pattern.compile("(\\w+)\\[(\\d+)\\]");
        Matcher matcher = pattern.matcher(targetSelector);

        if (!matcher.matches()) {
            System.err.println("Warning: Insert target selector format not supported yet: " + targetSelector);
            return;
        }
        String targetTag = matcher.group(1);
        int targetIndex = Integer.parseInt(matcher.group(2));

        SearchResult result = findElementTarget(body, targetTag, targetIndex);

        if (result == null) {
            System.err.println("Warning: Could not find insert target: " + targetSelector);
            return;
        }

        List<Node> container = result.container;
        int indexInContainer = result.index;
        Node targetNode = result.target;

        // Clone nodes before inserting to avoid aliasing issues
        List<Node> nodesToInsert = insertNode.body.stream().map(Node::clone).collect(Collectors.toList());

        if ("after".equals(position)) {
            container.addAll(indexInContainer + 1, nodesToInsert);
        } else if ("before".equals(position)) {
            container.addAll(indexInContainer, nodesToInsert);
        } else if ("replace".equals(position)) {
            container.remove(indexInContainer);
            container.addAll(indexInContainer, nodesToInsert);
        } else if ("into".equals(position)) {
            if (targetNode instanceof ElementNode) {
                ((ElementNode) targetNode).children.addAll(nodesToInsert);
            } else {
                 System.err.println("Warning: 'into' is only supported for element nodes. Target: " + targetSelector);
            }
        }
    }

    private void insertIntoStyleBlocks(List<Node> nodes, List<Node> itemsToInsert) {
        for (Node node : nodes) {
            if (node instanceof StyleBlockNode) {
                for (Node item : itemsToInsert) {
                    // Ensure we are inserting compatible nodes (StyleProperty or TemplateUsage)
                    if (item instanceof StylePropertyNode || item instanceof TemplateUsageNode) {
                        ((StyleBlockNode) node).directPropertiesAndUsages.add(item.clone());
                    }
                }
            } else if (node instanceof ElementNode) {
                // Recurse into children
                insertIntoStyleBlocks(((ElementNode) node).children, itemsToInsert);
            }
        }
    }

    private SearchResult findElementTarget(List<Node> body, String targetTag, int targetIndex) {
        return findElementRecursive(body, targetTag, targetIndex, new int[]{0});
    }

    private SearchResult findElementRecursive(List<Node> container, String targetTag, int targetIndex, int[] count) {
        for (int i = 0; i < container.size(); i++) {
            Node node = container.get(i);
            if (node instanceof ElementNode) {
                ElementNode element = (ElementNode) node;
                if (element.tagName.equals(targetTag)) {
                    if (count[0] == targetIndex) {
                        return new SearchResult(container, i, element);
                    }
                    count[0]++;
                }
                // Recurse into children
                SearchResult result = findElementRecursive(element.children, targetTag, targetIndex, count);
                if (result != null) {
                    return result;
                }
            }
        }
        return null;
    }
}
