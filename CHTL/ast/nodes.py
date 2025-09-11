from typing import List, Union

class AstNode:
    """Base class for all AST nodes."""
    def accept(self, visitor):
        """Accept a visitor."""
        method_name = f'visit_{self.__class__.__name__.lower()}'
        visitor_method = getattr(visitor, method_name, visitor.visit_default)
        return visitor_method(self)

class DefinitionNode(AstNode):
    """Base class for template and custom definitions."""
    def __init__(self, def_type: str, name: str, body: List['AstNode']):
        self.def_type = def_type # e.g., 'Style', 'Element', 'Var'
        self.name = name
        self.body = body

class TemplateDefinitionNode(DefinitionNode):
    """Represents a [Template] definition."""
    def __repr__(self):
        return f"TemplateDefinitionNode(type='{self.def_type}', name='{self.name}', body_len={len(self.body)})"

class CustomDefinitionNode(DefinitionNode):
    """Represents a [Custom] definition."""
    def __repr__(self):
        return f"CustomDefinitionNode(type='{self.def_type}', name='{self.name}', body_len={len(self.body)})"

class UsageNode(AstNode):
    """Base class for template usage nodes."""
    pass

class ElementUsageNode(UsageNode):
    """Represents a @Element usage."""
    def __init__(self, name: str):
        self.name = name

    def __repr__(self):
        return f"ElementUsageNode(name='{self.name}')"

class VarUsageNode(UsageNode):
    """Represents a variable usage like VarName(varName)."""
    def __init__(self, template_name: str, var_name: str):
        self.template_name = template_name
        self.var_name = var_name

    def __repr__(self):
        return f"VarUsageNode(template='{self.template_name}', var='{self.var_name}')"


class DocumentNode(AstNode):
    """Represents the root of a CHTL document."""
    def __init__(self, children: List[Union['ElementNode', 'DefinitionNode']]):
        self.children = children
        self.global_rules: List[str] = []

    def __repr__(self):
        return f"DocumentNode(children={len(self.children)}, global_rules={len(self.global_rules)})"

class ElementNode(AstNode):
    """Represents an HTML element."""
    def __init__(self, tag_name: str, attributes: List['AttributeNode'], children: List['AstNode']):
        self.tag_name = tag_name
        self.attributes = attributes
        self.children = children

    def __repr__(self):
        return f"ElementNode(tag='{self.tag_name}', attributes={self.attributes}, children={self.children})"

class AttributeNode(AstNode):
    """Represents an attribute of an HTML element."""
    def __init__(self, name: str, value: str):
        self.name = name
        self.value = value

    def __repr__(self):
        return f"AttributeNode(name='{self.name}', value='{self.value}')"

class TextNode(AstNode):
    """Represents a text node."""
    def __init__(self, value: str):
        self.value = value

    def __repr__(self):
        return f"TextNode(value='{self.value}')"

class StyleUsageNode(UsageNode):
    """Represents a @Style usage."""
    def __init__(self, name: str):
        self.name = name

    def __repr__(self):
        return f"StyleUsageNode(name='{self.name}')"

class StyleNode(AstNode):
    """Represents a style block."""
    def __init__(self, raw_content: str, inline_styles: str, global_rules: List[str], style_usages: List[StyleUsageNode]):
        self.raw_content = raw_content
        self.inline_styles = inline_styles
        self.global_rules = global_rules
        self.style_usages = style_usages

    def __repr__(self):
        return f"StyleNode(inline='{self.inline_styles}', global_rules={len(self.global_rules)}, usages={len(self.style_usages)})"

# A type hint for any node that can be a child of an element
ElementChild = Union[ElementNode, TextNode, StyleNode]
