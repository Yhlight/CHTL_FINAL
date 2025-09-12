from .CHTLLexer.token import TokenType
from .CHTLNode.nodes import (
    BaseNode, ExprNode, BinaryOpNode, DimensionNode, IdentifierNode, ConditionalNode,
    AttributeReferenceNode, StyleNode, StylePropertyNode, VarUsageNode, VarDefinitionNode
)
from .element_registry import ElementRegistry
from .template_registry import TemplateRegistry

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
    def __init__(self, style_properties: list, element_registry: ElementRegistry, template_registry: TemplateRegistry, element_context=None):
        self.properties = {p.property_name: p for p in style_properties}
        self.element_registry = element_registry
        self.template_registry = template_registry
        self.element_context = element_context
        self.memo = {}
        self.visiting = set()

    def evaluate(self, property_name: str) -> EvaluatedValue:
        if property_name in self.memo: return self.memo[property_name]
        if property_name in self.visiting: raise Exception(f"Circular dependency: {property_name}")
        if property_name not in self.properties: return EvaluatedString(property_name)
        self.visiting.add(property_name)
        prop = self.properties[property_name]
        result = self._visit(prop.value)
        self.visiting.remove(property_name)
        self.memo[property_name] = result
        if isinstance(result, EvaluatedBool): raise Exception("Property cannot be a boolean.")
        return result

    def _visit(self, node: BaseNode) -> EvaluatedValue:
        visitor = getattr(self, f'_visit_{node.__class__.__name__}', self._generic_visit)
        return visitor(node)
    def _generic_visit(self, node: BaseNode): raise Exception(f'No visit method for {node.__class__.__name__}')
    def _visit_IdentifierNode(self, node: IdentifierNode) -> EvaluatedValue: return self.evaluate(node.name)
    def _visit_DimensionNode(self, node: DimensionNode) -> EvaluatedDimension: return EvaluatedDimension(float(node.value), node.unit)
    def _visit_ConditionalNode(self, node: ConditionalNode) -> EvaluatedValue:
        cond = self._visit(node.condition)
        if not isinstance(cond, EvaluatedBool): raise Exception("Condition must be boolean.")
        return self._visit(node.true_branch) if cond.value else self._visit(node.false_branch)
    def _visit_BinaryOpNode(self, node: BinaryOpNode) -> EvaluatedValue:
        left, right = self._visit(node.left), self._visit(node.right)
        op = node.op.type
        if op in [TokenType.AMPERSAND_AMPERSAND, TokenType.PIPE_PIPE]:
            if not isinstance(left, EvaluatedBool) or not isinstance(right, EvaluatedBool): raise Exception("Logical ops need booleans.")
            return EvaluatedBool(left.value and right.value) if op == TokenType.AMPERSAND_AMPERSAND else EvaluatedBool(left.value or right.value)
        if op in [TokenType.GREATER, TokenType.GREATER_EQUAL, TokenType.LESS, TokenType.LESS_EQUAL, TokenType.EQUAL_EQUAL, TokenType.BANG_EQUAL]:
            if not isinstance(left, EvaluatedDimension) or not isinstance(right, EvaluatedDimension): raise Exception("Comparison only for dimensions.")
            if left.unit != right.unit: raise Exception(f"Cannot compare units {left.unit} and {right.unit}")
            ops = {TokenType.GREATER: '>', TokenType.GREATER_EQUAL: '>=', TokenType.LESS: '<', TokenType.LESS_EQUAL: '<=', TokenType.EQUAL_EQUAL: '==', TokenType.BANG_EQUAL: '!='}
            return EvaluatedBool(eval(f"{left.value} {ops[op]} {right.value}"))
        if op in [TokenType.PLUS, TokenType.MINUS, TokenType.STAR, TokenType.SLASH, TokenType.PERCENT, TokenType.STAR_STAR]:
            if not isinstance(left, EvaluatedDimension) or not isinstance(right, EvaluatedDimension): raise Exception("Arithmetic only for dimensions.")
            unit = left.unit
            if op in [TokenType.PLUS, TokenType.MINUS] and left.unit != right.unit: raise Exception(f"Cannot add/sub units {left.unit} and {right.unit}")
            else: unit = left.unit or right.unit
            ops = {TokenType.PLUS: '+', TokenType.MINUS: '-', TokenType.STAR: '*', TokenType.SLASH: '/', TokenType.PERCENT: '%', TokenType.STAR_STAR: '**'}
            return EvaluatedDimension(eval(f"{left.value} {ops[op]} {right.value}"), unit)
        raise Exception(f"Unsupported binary operator: {op}")
    def _visit_AttributeReferenceNode(self, node: AttributeReferenceNode) -> EvaluatedValue:
        targets = self.element_registry.find(node.selector)
        if not targets: raise Exception(f"No element for selector: {node.selector}")
        target = targets[0]
        if self.element_context == target: return self.evaluate(node.property_name)
        styles = [n for n in target.children if isinstance(n, StyleNode)]
        props = [r for s in styles for r in s.rules if isinstance(r, StylePropertyNode)]
        if not any(p.property_name == node.property_name for p in props): raise Exception(f"Property {node.property_name} not found on {node.selector}")
        return Evaluator(props, self.element_registry, self.template_registry, target).evaluate(node.property_name)
    def _visit_VarUsageNode(self, node: VarUsageNode) -> EvaluatedValue:
        template = self.template_registry.find("Var", node.group_name)
        if not template: raise Exception(f"Var group '{node.group_name}' not found.")
        var_def = next((d for d in template.body if isinstance(d, VarDefinitionNode) and d.name == node.var_name), None)
        if not var_def: raise Exception(f"Var '{node.var_name}' not found in '{node.group_name}'.")
        return self._visit(var_def.value)
