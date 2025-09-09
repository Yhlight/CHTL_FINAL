from CHTL.CHTLNode.nodes import Node, ElementNode, SelectorNode, SelectorPartNode, SelectorType
from typing import List

def find_nodes_by_selector(start_node: Node, parsed_selector: SelectorNode) -> List[ElementNode]:
    """
    Finds all ElementNodes that match a parsed selector AST, starting from a given node.

    Args:
        start_node: The node to begin the search from (e.g., the DocumentNode).
        parsed_selector: A SelectorNode representing the parsed selector.

    Returns:
        A list of matching ElementNodes.
    """
    if not parsed_selector.parts:
        return []

    # Start with the first part of the selector, searching from the start_node
    current_matches = _find_descendants(start_node, parsed_selector.parts[0])

    # For the rest of the parts (descendant selectors), search within the previous matches
    for i in range(1, len(parsed_selector.parts)):
        next_matches = []
        selector_part = parsed_selector.parts[i]
        for match in current_matches:
            # Find descendants of the current match that fit the next part
            next_matches.extend(_find_descendants(match, selector_part))
        current_matches = next_matches
        if not current_matches:
            break # No need to continue if a part of the chain matches nothing

    return current_matches

def _find_descendants(start_node: Node, selector_part: SelectorPartNode) -> List[ElementNode]:
    """
    Recursively finds all descendant nodes of a start_node that match a single selector part.
    """
    found_nodes = []

    # Use a list as a queue for a breadth-first search
    nodes_to_visit = []
    if hasattr(start_node, 'children'):
        nodes_to_visit.extend(start_node.children)

    while nodes_to_visit:
        current_node = nodes_to_visit.pop(0)
        if not isinstance(current_node, ElementNode):
            if hasattr(current_node, 'children'):
                 nodes_to_visit.extend(current_node.children)
            continue

        # Check if the current element matches the selector part
        if _element_matches_part(current_node, selector_part):
            found_nodes.append(current_node)

        if hasattr(current_node, 'children'):
            nodes_to_visit.extend(current_node.children)

    # If an index is specified, filter the results
    if selector_part.index is not None:
        try:
            return [found_nodes[selector_part.index]]
        except IndexError:
            return [] # Index out of bounds means no match

    return found_nodes

def _element_matches_part(element: ElementNode, selector_part: SelectorPartNode) -> bool:
    """Checks if a single element matches a single selector part."""
    if selector_part.type == SelectorType.TAG:
        return element.tag_name == selector_part.value

    if selector_part.type == SelectorType.ID:
        for attr in element.attributes:
            if attr.name == 'id' and attr.value == selector_part.value:
                return True

    if selector_part.type == SelectorType.CLASS:
        for attr in element.attributes:
            if attr.name == 'class' and selector_part.value in attr.value.split():
                return True

    return False
