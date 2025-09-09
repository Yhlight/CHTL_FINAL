package CHTL.Processor;

import CHTL.Node.ElementNode;

/**
 * A record to uniquely identify a style property within the document.
 * This is used as a node in the dependency graph.
 * Using the ElementNode instance and property name ensures uniqueness.
 * A null parent means it's a property in a global style block that was hoisted.
 */
public record StyleProperty(ElementNode parent, String propertyName) {
}
