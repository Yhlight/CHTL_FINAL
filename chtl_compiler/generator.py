from . import ast
from lark import Tree

class HtmlGenerator:
    def generate(self, doc: ast.Document) -> str:
        return "".join(self._generate_node(child, indent_level=0) for child in doc.children)

    def _generate_node(self, node: ast.Node, indent_level: int) -> str:
        indent = "  " * indent_level
        if isinstance(node, ast.ElementNode):
            return self._generate_element(node, indent_level)
        elif isinstance(node, ast.TextNode):
            return f"{indent}{node.content}\n"
        return ""

    def _generate_element(self, node: ast.ElementNode, indent_level: int) -> str:
        indent = "  " * indent_level

        attrs_list = [f'{attr.name}="{attr.value}"' for attr in node.attributes]

        if node.styles:
            style_parts = []
            for style in node.styles:
                style_parts.append(f"{style.prop}:{self._evaluate_and_format_expression(style.value)}")

            attrs_list.append(f'style="{";".join(style_parts)}"')

        attrs_str = (" " + " ".join(attrs_list)) if attrs_list else ""

        children_str = "".join(self._generate_node(child, indent_level + 1) for child in node.children)

        self_closing_tags = {"area", "base", "br", "col", "embed", "hr", "img", "input", "link", "meta", "param", "source", "track", "wbr"}
        if not node.children and node.tag in self_closing_tags:
            return f"{indent}<{node.tag}{attrs_str} />\n"

        return f"{indent}<{node.tag}{attrs_str}>\n{children_str}{indent}</{node.tag}>\n"

    def _evaluate_and_format_expression(self, expr) -> str:
        # The expression from the parser might be wrapped in a Tree if the rule wasn't inlined.
        # We need to handle this.
        if isinstance(expr, Tree):
            # This is a hacky workaround for the nested Tree issue.
            # A better solution is to fix the transformer to not produce these.
            if expr.data == 'css_expression' and len(expr.children) == 1:
                expr = expr.children[0]

        try:
            term = self._evaluate_expression(expr)
            if term.unit:
                val = float(term.value)
                if val == int(val):
                    return f"{int(val)}{term.unit}"
                return f"{val}{term.unit}"
            return term.value
        except (ValueError, TypeError):
            return "evaluation_error"

    def _evaluate_expression(self, expr) -> ast.Term:
        # Unwrap Tree objects if they exist.
        if isinstance(expr, Tree):
            if expr.data == 'css_expression' and len(expr.children) == 1:
                expr = expr.children[0]

        if isinstance(expr, ast.Term):
            return expr

        if isinstance(expr, ast.BinaryOpExpression):
            left = self._evaluate_expression(expr.left)
            right = self._evaluate_expression(expr.right)

            l_num = float(left.value)
            r_num = float(right.value)
            result_unit = left.unit or right.unit or ""

            op = expr.operator
            if op == '+': result_val = l_num + r_num
            elif op == '-': result_val = l_num - r_num
            elif op == '*': result_val = l_num * r_num
            elif op == '/': result_val = l_num / r_num
            elif op == '%': result_val = l_num % r_num
            elif op == '**': result_val = l_num ** r_num
            else: raise ValueError(f"Unknown operator: {op}")

            return ast.Term(str(result_val), result_unit)

        raise TypeError(f"Unknown expression type: {type(expr)}")
