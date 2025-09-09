import re
from CHTL.CHTLNode.nodes import (
    ExpressionNode, LiteralNode, BinaryOpNode, LogicalOpNode,
    ConditionalExpressionNode, PropertyReferenceNode, ElementNode, DocumentNode, StyleNode, CssPropertyNode
)
from CHTL.CHTLLexer.lexer import TokenType
from typing import Any, Optional, Tuple

class ExpressionEvaluator:
    """
    Walks an expression AST and computes a final value.
    """
    def __init__(self, document: DocumentNode):
        self.document = document
        self.node_cache = {} # Cache for resolved nodes to speed up selector searches

    def evaluate(self, node: ExpressionNode, context_element: ElementNode) -> Any:
        """
        Evaluates an expression node within the context of a specific element.
        """
        return self._visit(node, context_element)

    def _visit(self, node: ExpressionNode, context: ElementNode) -> Any:
        method_name = f'_visit_{type(node).__name__}'
        visitor = getattr(self, method_name, self._generic_visit)
        return visitor(node, context)

    def _generic_visit(self, node: ExpressionNode, context: ElementNode):
        raise Exception(f'No _visit_{type(node).__name__} method')

    def _visit_LiteralNode(self, node: LiteralNode, context: ElementNode) -> Any:
        return node.value

    def _visit_LogicalOpNode(self, node: LogicalOpNode, context: ElementNode) -> bool:
        left_val = self._visit(node.left, context)
        # Short-circuit evaluation
        if node.op.type == TokenType.AND and not left_val:
            return False
        if node.op.type == TokenType.OR and left_val:
            return True
        right_val = self._visit(node.right, context)

        if node.op.type == TokenType.AND:
            return left_val and right_val
        if node.op.type == TokenType.OR:
            return left_val or right_val
        return False

    def _visit_ConditionalExpressionNode(self, node: ConditionalExpressionNode, context: ElementNode) -> Any:
        condition_val = self._visit(node.condition, context)
        if condition_val:
            return self._visit(node.true_expr, context)
        elif node.false_expr:
            return self._visit(node.false_expr, context)
        return None # If condition is false and there's no false_expr

    def _visit_PropertyReferenceNode(self, node: PropertyReferenceNode, context: ElementNode) -> Any:
        target_element = context
        if node.selector:
            # This is a simplification. A real implementation needs a robust CSS selector engine.
            # For now, we'll handle simple tag, .class, and #id selectors.
            found_node = self._find_node_by_selector(node.selector)
            if not found_node:
                raise ValueError(f"Could not find element with selector '{node.selector}'")
            target_element = found_node

        # Now find the property on the target_element
        for child in reversed(target_element.children): # Search backwards to find latest definition
            if isinstance(child, StyleNode):
                for style_child in reversed(child.children):
                    if isinstance(style_child, CssPropertyNode) and style_child.name == node.property_name:
                        # The property's value could itself be an expression, so we must evaluate it.
                        if isinstance(style_child.value, ExpressionNode):
                             # Pass the target_element as context for the nested evaluation
                            return self.evaluate(style_child.value, target_element)
                        return style_child.value
        raise ValueError(f"Property '{node.property_name}' not found on element '{node.selector or 'local'}'")

    def _visit_BinaryOpNode(self, node: BinaryOpNode, context: ElementNode) -> Any:
        left = self._visit(node.left, context)
        right = self._visit(node.right, context)

        # Helper to extract numeric value and unit
        def parse_value(val_str):
            if not isinstance(val_str, str): return float(val_str), ""
            match = re.match(r'(-?\d+\.?\d*)\s*(\w*|%)', val_str.strip())
            if not match: raise ValueError(f"Could not parse numeric value from '{val_str}'")
            return float(match.group(1)), match.group(2)

        l_num, l_unit = parse_value(left)
        r_num, r_unit = parse_value(right)

        # For comparisons, units should ideally match or be convertible, but we'll compare numbers directly for now.
        if node.op.type == TokenType.GT: return l_num > r_num
        if node.op.type == TokenType.GTE: return l_num >= r_num
        if node.op.type == TokenType.LT: return l_num < r_num
        if node.op.type == TokenType.LTE: return l_num <= r_num
        if node.op.type == TokenType.EQ_EQ: return l_num == r_num and l_unit == r_unit
        if node.op.type == TokenType.NOT_EQ: return l_num != r_num or l_unit != r_unit

        # For arithmetic, units should be compatible
        result_unit = l_unit or r_unit # Default to the unit that exists
        if l_unit and r_unit and l_unit != r_unit:
            raise TypeError(f"Cannot perform arithmetic on incompatible units: {l_unit} and {r_unit}")

        result_num = 0
        if node.op.type == TokenType.PLUS: result_num = l_num + r_num
        elif node.op.type == TokenType.MINUS: result_num = l_num - r_num
        elif node.op.type == TokenType.STAR: result_num = l_num * r_num
        elif node.op.type == TokenType.SLASH:
            if r_num == 0: raise ZeroDivisionError("Division by zero in expression.")
            result_num = l_num / r_num
        else:
            raise TypeError(f"Unsupported binary operator: {node.op.value}")

        # Format the result back into a string like "150px"
        if result_num == int(result_num): return f"{int(result_num)}{result_unit}"
        return f"{result_num:.2f}{result_unit}".rstrip('0').rstrip('.')

    def _find_node_by_selector(self, selector: str) -> Optional[ElementNode]:
        """
        A simplified CSS selector engine. Caches results.
        Supports: #id, .class, tag
        """
        if selector in self.node_cache:
            return self.node_cache[selector]

        # Walk the entire AST to find the first match.
        # This is inefficient but will work for now.

        nodes_to_visit = [self.document]
        while nodes_to_visit:
            current_node = nodes_to_visit.pop(0)
            if isinstance(current_node, ElementNode):
                # Check for match
                if selector.startswith('#') and any(attr.name == 'id' and attr.value == selector[1:] for attr in current_node.attributes):
                    self.node_cache[selector] = current_node
                    return current_node
                if selector.startswith('.') and any(attr.name == 'class' and selector[1:] in attr.value.split() for attr in current_node.attributes):
                    self.node_cache[selector] = current_node
                    return current_node
                if current_node.tag_name == selector:
                    self.node_cache[selector] = current_node
                    return current_node

            if hasattr(current_node, 'children'):
                nodes_to_visit.extend(current_node.children)

        return None
