import re
from .nodes import ExprNode, LiteralNode, PropertyRefNode, UnaryOpNode, BinaryOpNode, TernaryOpNode

def _parse_value(val):
    """Parses a string value into a (number, unit) tuple or returns the original value."""
    if not isinstance(val, str):
        return val, None

    match = re.match(r'^(-?\d*\.?\d+)(px|em|rem|%|vw|vh|deg)?$', val)
    if match:
        number = float(match.group(1))
        unit = match.group(2) or ''
        return number, unit

    # Handle color codes and other keywords
    return val, None

class ExpressionEvaluator:
    def __init__(self, context: dict):
        self.context = context

    def evaluate(self, node: ExprNode) -> str:
        result = self._visit(node)
        if isinstance(result, tuple) and len(result) == 2:
            num, unit = result
            if unit is not None:
                # Format integers without decimal points if possible
                if isinstance(num, (int, float)) and num == int(num):
                    return f"{int(num)}{unit}"
                return f"{num}{unit}"
            return str(num)
        return str(result)

    def _visit(self, node: ExprNode):
        if isinstance(node, LiteralNode):
            return self._visit_literal(node)
        elif isinstance(node, PropertyRefNode):
            return self._visit_propertyref(node)
        elif isinstance(node, UnaryOpNode):
            return self._visit_unaryop(node)
        elif isinstance(node, BinaryOpNode):
            return self._visit_binaryop(node)
        elif isinstance(node, TernaryOpNode):
            return self._visit_ternaryop(node)
        else:
            raise TypeError(f"Unknown expression node type: {type(node)}")

    def _visit_literal(self, node: LiteralNode):
        return _parse_value(node.value)

    def _visit_propertyref(self, node: PropertyRefNode):
        # If it's a multi-part path, it must be a real reference
        if len(node.path) > 1:
            # Logic for this will be implemented in a future step
            raise ValueError(f"Multi-part references not yet supported: {'.'.join(node.path)}")

        ref_name = node.path[0]
        if ref_name in self.context:
            # It's a valid reference to another property in the current scope
            return _parse_value(self.context[ref_name])
        else:
            # If it's not in the context, it must be a literal keyword like 'red' or 'auto'
            return ref_name, None

    def _visit_unaryop(self, node: UnaryOpNode):
        operand_val, operand_unit = self._visit(node.operand)
        if node.operator == '-':
            if isinstance(operand_val, (int, float)):
                return -operand_val, operand_unit
        return operand_val, operand_unit # '+' operator does nothing

    def _visit_binaryop(self, node: BinaryOpNode):
        # Note: left and right are tuples of (value, unit)
        left_val, l_unit = self._visit(node.left)
        right_val, r_unit = self._visit(node.right)

        # Logical operators
        if node.operator == '&&':
            return (left_val and right_val), None
        if node.operator == '||':
            return (left_val or right_val), None

        # Ensure we have numbers for the rest of the operators
        if not isinstance(left_val, (int, float)) or not isinstance(right_val, (int, float)):
            # Fallback for non-numeric ops like string concatenation
            if node.operator == '+':
                return str(left_val) + str(right_val), None
            raise TypeError(f"Cannot perform '{node.operator}' on non-numeric values '{left_val}' and '{right_val}'")

        # Relational operators
        if node.operator == '>': return left_val > right_val, None
        if node.operator == '<': return left_val < right_val, None
        if node.operator == '>=': return left_val >= right_val, None
        if node.operator == '<=': return left_val <= right_val, None
        if node.operator == '==': return left_val == right_val, None
        if node.operator == '!=': return left_val != right_val, None

        # Arithmetic operators - simple unit logic
        # Assumes units are compatible if one is empty or they are the same
        unit = l_unit or r_unit
        if l_unit and r_unit and l_unit != r_unit:
            raise TypeError(f"Incompatible units for operator '{node.operator}': '{l_unit}' and '{r_unit}'")

        if node.operator == '+': return left_val + right_val, unit
        if node.operator == '-': return left_val - right_val, unit
        if node.operator == '*': return left_val * right_val, unit
        if node.operator == '/': return left_val / right_val, unit
        if node.operator == '%': return left_val % right_val, unit
        if node.operator == '**': return left_val ** right_val, unit

        raise ValueError(f"Unknown binary operator: {node.operator}")


    def _visit_ternaryop(self, node: TernaryOpNode):
        condition_val, _ = self._visit(node.condition)

        # Explicitly check for "false" values, since bool('false') is True
        is_truthy = True
        if isinstance(condition_val, bool):
            is_truthy = condition_val
        elif isinstance(condition_val, str):
            if condition_val.lower() in ('false', '0', ''):
                is_truthy = False
        elif not condition_val: # Handles numeric 0, None, etc.
            is_truthy = False

        if is_truthy:
            return self._visit(node.true_expr)
        else:
            return self._visit(node.false_expr)
