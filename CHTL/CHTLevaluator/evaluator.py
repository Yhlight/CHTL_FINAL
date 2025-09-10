import re
from CHTL.CHTLNode.nodes import (
    ExpressionNode, LiteralNode, BinaryOpNode, LogicalOpNode,
    ConditionalExpressionNode, PropertyReferenceNode, ElementNode, DocumentNode, StyleNode, CssPropertyNode, SelectorNode,
    FunctionCallNode, AttributeNode
)
from CHTL.CHTLParser.selector_parser import SelectorParser
from CHTL.CHTLUtils.ast_search import find_nodes_by_selector
from CHTL.CHTLLexer.lexer import TokenType
from CHTL.CHTLContext.context import CompilationContext
from typing import Any, Optional, Tuple

class ExpressionEvaluator:
    """
    Walks an expression AST and computes a final value.
    """
    def __init__(self, document: DocumentNode, context: CompilationContext):
        self.document = document
        self.context = context
        # Caching could be added here later for performance

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
        if node.op.type == TokenType.AND and not left_val:
            return False
        if node.op.type == TokenType.OR and left_val:
            return True
        right_val = self._visit(node.right, context)

        return (left_val and right_val) if node.op.type == TokenType.AND else (left_val or right_val)

    def _visit_ConditionalExpressionNode(self, node: ConditionalExpressionNode, context: ElementNode) -> Any:
        condition_val = self._visit(node.condition, context)
        if condition_val:
            return self._visit(node.true_expr, context)
        elif node.false_expr:
            return self._visit(node.false_expr, context)
        return None

    def _visit_PropertyReferenceNode(self, node: PropertyReferenceNode, context: ElementNode) -> Any:
        target_element = context
        # If the selector is not None, we need to find the target element in the document
        if node.selector:
            # The selector is now a SelectorNode, so we can pass it to the search utility
            matched_nodes = find_nodes_by_selector(self.document, node.selector)
            if not matched_nodes:
                # This should ideally show the raw selector string, not the node
                raise ValueError(f"Could not find element with selector")
            target_element = matched_nodes[0] # Use the first match

        # Now find the property on the target_element
        for child in reversed(target_element.children):
            if isinstance(child, StyleNode):
                for style_child in reversed(child.children):
                    if isinstance(style_child, CssPropertyNode) and style_child.name == node.property_name:
                        if isinstance(style_child.value, ExpressionNode):
                            return self.evaluate(style_child.value, target_element)
                        return style_child.value
        raise ValueError(f"Property '{node.property_name}' not found on target element")

    def _visit_BinaryOpNode(self, node: BinaryOpNode, context: ElementNode) -> Any:
        left = self._visit(node.left, context)
        right = self._visit(node.right, context)

        def parse_value(val_str):
            if isinstance(val_str, (int, float)): return float(val_str), ""
            if not isinstance(val_str, str):
                raise TypeError(f"Cannot parse value of type {type(val_str)}")
            match = re.match(r'(-?\d+\.?\d*)\s*(\w*|%)', val_str.strip())
            if not match: raise ValueError(f"Could not parse numeric value from '{val_str}'")
            return float(match.group(1)), match.group(2)

        l_num, l_unit = parse_value(left)
        r_num, r_unit = parse_value(right)

        if node.op.type in (TokenType.GT, TokenType.GTE, TokenType.LT, TokenType.LTE, TokenType.EQ_EQ, TokenType.NOT_EQ):
            if node.op.type == TokenType.GT: return l_num > r_num
            if node.op.type == TokenType.GTE: return l_num >= r_num
            if node.op.type == TokenType.LT: return l_num < r_num
            if node.op.type == TokenType.LTE: return l_num <= r_num
            if node.op.type == TokenType.EQ_EQ: return l_num == r_num and l_unit == r_unit
            if node.op.type == TokenType.NOT_EQ: return l_num != r_num or l_unit != r_unit

        result_unit = l_unit or r_unit
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

        if result_num == int(result_num): return f"{int(result_num)}{result_unit}"
        return f"{result_num:.2f}{result_unit}".rstrip('0').rstrip('.')

    def _visit_FunctionCallNode(self, node: FunctionCallNode, context: ElementNode) -> Any:
        # This assumes the function call is for a @Var template.
        # A more robust system might check a registry of functions.

        # TODO: Handle namespaces for var templates. For now, use default.
        var_template = self.context.get_var_template(node.callee_name)
        if not var_template:
            raise NameError(f"Variable template '{node.callee_name}' not found.")

        if len(node.arguments) != 1:
            raise ValueError(f"Variable template '{node.callee_name}' expects 1 argument, got {len(node.arguments)}.")

        arg = node.arguments[0]
        if not isinstance(arg, PropertyReferenceNode) or arg.selector:
            raise TypeError("Variable template arguments must be simple names.")

        var_name_to_find = arg.property_name

        for var_def in var_template:
            if isinstance(var_def, AttributeNode) and var_def.name == var_name_to_find:
                # The value in a @Var template is treated as a literal string.
                return var_def.value

        raise NameError(f"Variable '{var_name_to_find}' not found in template '{node.callee_name}'.")
