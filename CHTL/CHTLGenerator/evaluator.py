from CHTL.CHTLNode import nodes
from CHTL.CHTLLexer.token_type import TokenType

class EvaluationResult:
    """A wrapper for the result of an evaluation, containing value and unit."""
    def __init__(self, value, unit=None):
        self.value = value
        self.unit = unit
    def __str__(self):
        unit_str = self.unit or ""
        # Format to int if it's a whole number
        if isinstance(self.value, float) and self.value == int(self.value):
            return f"{int(self.value)}{unit_str}"
        return f"{self.value}{unit_str}"

class ExpressionEvaluator:
    def __init__(self, document_root: nodes.ProgramNode):
        self.document_root = document_root

    def _find_node(self, selector_tokens: list) -> nodes.ElementNode:
        if not selector_tokens or len(selector_tokens) != 2: return None
        selector_type, selector_name = selector_tokens[0].type, selector_tokens[1].lexeme
        def traverse(node):
            if isinstance(node, nodes.ElementNode):
                if selector_type == TokenType.HASH:
                    id_attr = next((a for a in node.attributes if a.name == 'id'), None)
                    if id_attr and id_attr.value == selector_name: return node
                elif selector_type == TokenType.DOT:
                    class_attr = next((a for a in node.attributes if a.name == 'class'), None)
                    if class_attr and selector_name in class_attr.value.split(): return node
            if hasattr(node, 'children'):
                for child in node.children:
                    if child and (found := traverse(child)): return found
            return None
        return traverse(self.document_root)

    def evaluate(self, expression_node: nodes.ExpressionNode, current_element: nodes.ElementNode) -> EvaluationResult:
        return self._evaluate_node(expression_node, current_element)

    def _evaluate_node(self, node: nodes.ExpressionNode, current_element: nodes.ElementNode) -> EvaluationResult:
        method_name = f'_eval_{type(node).__name__.lower()}'
        return getattr(self, method_name)(node, current_element)

    def _eval_literalnode(self, node: nodes.LiteralNode, current_element: nodes.ElementNode):
        return EvaluationResult(node.value)

    def _eval_valuewithunitnode(self, node: nodes.ValueWithUnitNode, current_element: nodes.ElementNode):
        return EvaluationResult(node.value, node.unit)

    def _eval_conditionalnode(self, node: nodes.ConditionalNode, current_element: nodes.ElementNode):
        condition_res = self._evaluate_node(node.condition, current_element)
        condition_val = condition_res.value
        if isinstance(condition_val, (int, float)): condition_val = condition_val != 0

        return self._evaluate_node(node.true_branch if condition_val else node.false_branch, current_element)

    def _eval_binaryopnode(self, node: nodes.BinaryOpNode, current_element: nodes.ElementNode):
        left_res = self._evaluate_node(node.left, current_element)
        right_res = self._evaluate_node(node.right, current_element)

        left_val, right_val = left_res.value, right_res.value
        # The unit of the left side takes precedence
        result_unit = left_res.unit or right_res.unit

        op = node.op.type
        if op == TokenType.PLUS: return EvaluationResult(left_val + right_val, result_unit)
        if op == TokenType.MINUS: return EvaluationResult(left_val - right_val, result_unit)
        if op == TokenType.STAR: return EvaluationResult(left_val * right_val, result_unit)
        if op == TokenType.SLASH: return EvaluationResult(left_val / right_val if right_val != 0 else 0, result_unit)
        if op == TokenType.STAR_STAR: return EvaluationResult(left_val ** right_val, result_unit)
        if op == TokenType.PERCENT: return EvaluationResult(left_val % right_val if right_val != 0 else 0, result_unit)
        if op == TokenType.GREATER: return EvaluationResult(left_val > right_val)
        if op == TokenType.GREATER_EQUAL: return EvaluationResult(left_val >= right_val)
        if op == TokenType.LESS: return EvaluationResult(left_val < right_val)
        if op == TokenType.LESS_EQUAL: return EvaluationResult(left_val <= right_val)
        if op == TokenType.EQUAL_EQUAL: return EvaluationResult(left_val == right_val)
        if op == TokenType.BANG_EQUAL: return EvaluationResult(left_val != right_val)
        if op == TokenType.AMPERSAND_AMPERSAND: return EvaluationResult(left_val and right_val)
        if op == TokenType.PIPE_PIPE: return EvaluationResult(left_val or right_val)
        return EvaluationResult(None)

    def _eval_referencenode(self, node: nodes.ReferenceNode, current_element: nodes.ElementNode):
        target_element = self._find_node(node.selector_tokens) if node.selector_tokens else current_element
        if not target_element:
            return EvaluationResult(node.property_name)

        style_node = next((c for c in target_element.children if isinstance(c, nodes.StyleNode)), None)
        if not style_node: return EvaluationResult(0)

        prop_node = next((p for p in style_node.children if isinstance(p, nodes.StylePropertyNode) and p.name == node.property_name), None)
        if not prop_node:
            # If the property is not found in the style block, treat the reference name as a literal string value.
            return EvaluationResult(node.property_name)

        return self._evaluate_node(prop_node.value_expression, target_element)
