from .CHTLLexer.token import TokenType
from .CHTLNode.nodes import (
    BaseNode, ExprNode, BinaryOpNode, DimensionNode, IdentifierNode, ConditionalNode,
    AttributeReferenceNode, StyleNode, StylePropertyNode
)
from .element_registry import ElementRegistry

class EvaluatedValue:
    def __init__(self, value): self.value = value
    def __repr__(self): return f"{self.__class__.__name__}({self.value})"
class EvaluatedDimension(EvaluatedValue):
    def __init__(self, value: float, unit: str):
        super().__init__(value)
        self.unit = unit
    def __repr__(self): return f"{self.__class__.__name__}({self.value}{self.unit})"
class EvaluatedString(EvaluatedValue): pass
class EvaluatedBool(EvaluatedValue): pass

class Evaluator:
    def __init__(self, style_properties: list, element_registry: ElementRegistry, element_context=None):
        self.properties = {p.property_name: p for p in style_properties}
        self.element_registry = element_registry
        self.element_context = element_context
        self.memo = {}
        self.visiting = set()

    def evaluate(self, property_name: str) -> EvaluatedValue:
        if property_name in self.memo: return self.memo[property_name]
        if property_name in self.visiting: raise Exception(f"Circular dependency detected: {property_name}")
        if property_name not in self.properties: return EvaluatedString(property_name)
        self.visiting.add(property_name)
        property_node = self.properties[property_name]
        result = self._visit(property_node.value)
        self.visiting.remove(property_name)
        self.memo[property_name] = result
        if isinstance(result, EvaluatedBool): raise Exception("Property cannot evaluate to a boolean.")
        return result

    def _visit(self, node: BaseNode) -> EvaluatedValue:
        method_name = f'_visit_{node.__class__.__name__}'
        visitor = getattr(self, method_name, self._generic_visit)
        return visitor(node)
    def _generic_visit(self, node: BaseNode): raise Exception(f'No _visit_{node.__class__.__name__} method')
    def _visit_IdentifierNode(self, node: IdentifierNode) -> EvaluatedValue: return self.evaluate(node.name)
    def _visit_DimensionNode(self, node: DimensionNode) -> EvaluatedDimension: return EvaluatedDimension(float(node.value), node.unit)
    def _visit_ConditionalNode(self, node: ConditionalNode) -> EvaluatedValue:
        condition_result = self._visit(node.condition)
        if not isinstance(condition_result, EvaluatedBool): raise Exception("Condition must be a boolean.")
        return self._visit(node.true_branch) if condition_result.value else self._visit(node.false_branch)
    def _visit_BinaryOpNode(self, node: BinaryOpNode) -> EvaluatedValue:
        left = self._visit(node.left)
        right = self._visit(node.right)
        op_type = node.op.type
        if op_type in [TokenType.AMPERSAND_AMPERSAND, TokenType.PIPE_PIPE]:
            if not isinstance(left, EvaluatedBool) or not isinstance(right, EvaluatedBool): raise Exception("Logical ops require booleans.")
            return EvaluatedBool(left.value and right.value) if op_type == TokenType.AMPERSAND_AMPERSAND else EvaluatedBool(left.value or right.value)
        if op_type in [TokenType.GREATER, TokenType.GREATER_EQUAL, TokenType.LESS, TokenType.LESS_EQUAL, TokenType.EQUAL_EQUAL, TokenType.BANG_EQUAL]:
            if not isinstance(left, EvaluatedDimension) or not isinstance(right, EvaluatedDimension): raise Exception("Comparison only for dimensions.")
            if left.unit != right.unit: raise Exception(f"Cannot compare units: {left.unit} and {right.unit}")
            if op_type == TokenType.GREATER: return EvaluatedBool(left.value > right.value)
            if op_type == TokenType.GREATER_EQUAL: return EvaluatedBool(left.value >= right.value)
            if op_type == TokenType.LESS: return EvaluatedBool(left.value < right.value)
            if op_type == TokenType.LESS_EQUAL: return EvaluatedBool(left.value <= right.value)
            if op_type == TokenType.EQUAL_EQUAL: return EvaluatedBool(left.value == right.value)
            if op_type == TokenType.BANG_EQUAL: return EvaluatedBool(left.value != right.value)
        if op_type in [TokenType.PLUS, TokenType.MINUS, TokenType.STAR, TokenType.SLASH, TokenType.PERCENT, TokenType.STAR_STAR]:
            if not isinstance(left, EvaluatedDimension) or not isinstance(right, EvaluatedDimension): raise Exception("Arithmetic only for dimensions.")
            new_unit = left.unit
            if op_type in [TokenType.PLUS, TokenType.MINUS]:
                if left.unit != right.unit: raise Exception(f"Cannot add/subtract units: {left.unit} and {right.unit}")
            else: new_unit = left.unit or right.unit
            if op_type == TokenType.PLUS: new_value = left.value + right.value
            if op_type == TokenType.MINUS: new_value = left.value - right.value
            if op_type == TokenType.STAR: new_value = left.value * right.value
            if op_type == TokenType.SLASH: new_value = left.value / right.value
            if op_type == TokenType.PERCENT: new_value = left.value % right.value
            if op_type == TokenType.STAR_STAR: new_value = left.value ** right.value
            return EvaluatedDimension(new_value, new_unit)
        raise Exception(f"Unsupported binary operator: {op_type}")
    def _visit_AttributeReferenceNode(self, node: AttributeReferenceNode) -> EvaluatedValue:
        target_nodes = self.element_registry.find(node.selector)
        if not target_nodes: raise Exception(f"No element for selector: {node.selector}")
        target_node = target_nodes[0]
        if self.element_context == target_node: return self.evaluate(node.property_name)
        target_style_nodes = [n for n in target_node.children if isinstance(n, StyleNode)]
        target_properties = []
        for sn in target_style_nodes: target_properties.extend(sn.rules)
        if not any(p.property_name == node.property_name for p in target_properties if isinstance(p, StylePropertyNode)):
            raise Exception(f"Property '{node.property_name}' not found on element with selector '{node.selector}'")
        target_evaluator = Evaluator(target_properties, self.element_registry, element_context=target_node)
        return target_evaluator.evaluate(node.property_name)
