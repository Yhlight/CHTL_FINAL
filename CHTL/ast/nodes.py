from typing import List, Union, Any, Dict, Optional

class AstNode:
    """Base class for all AST nodes."""
    def accept(self, visitor):
        method_name = f'visit_{self.__class__.__name__.lower()}'
        visitor_method = getattr(visitor, method_name, visitor.visit_default)
        return visitor_method(self)

# --- Core Nodes ---
class AttributeNode(AstNode):
    def __init__(self, name: str, value: Any):
        self.name = name
        self.value = value
    def __repr__(self):
        return f"AttributeNode(name='{self.name}', value='{self.value}')"

class TextNode(AstNode):
    def __init__(self, value: str):
        self.value = value
    def __repr__(self):
        return f"TextNode(value='{self.value}')"

# --- Directives ---
class DirectiveNode(AstNode):
    pass
class ImportNode(DirectiveNode):
    def __init__(self, import_type: str, path: str, alias: Optional[str] = None):
        self.import_type = import_type
        self.path = path
        self.alias = alias
    def __repr__(self):
        if self.alias:
            return f"ImportNode(type='{self.import_type}', path='{self.path}', as='{self.alias}')"
        return f"ImportNode(type='{self.import_type}', path='{self.path}')"
class NamespaceNode(DirectiveNode):
    def __init__(self, name: str, body: List[AstNode]):
        self.name = name
        self.body = body
    def __repr__(self):
        return f"NamespaceNode(name='{self.name}', body_len={len(self.body)})"
class ConfigurationNode(DirectiveNode):
    def __init__(self, name: Optional[str], settings: List[AttributeNode]):
        self.name = name
        self.settings = settings
    def __repr__(self):
        return f"ConfigurationNode(name='{self.name}', settings={len(self.settings)})"

# --- Definitions ---
class DefinitionNode(AstNode):
    """Base class for template and custom definitions."""
    def __init__(self, def_type: str, name: str, body: List['AstNode']):
        self.def_type = def_type
        self.name = name
        self.body = body

class TemplateDefinitionNode(DefinitionNode):
    def __repr__(self):
        return f"TemplateDefinitionNode(type='{self.def_type}', name='{self.name}', body_len={len(self.body)})"

class CustomDefinitionNode(DefinitionNode):
    def __repr__(self):
        return f"CustomDefinitionNode(type='{self.def_type}', name='{self.name}', body_len={len(self.body)})"

# --- Usages ---
class UsageNode(AstNode):
    """Base class for template usage nodes."""
    pass

class ElementUsageNode(UsageNode):
    def __init__(self, name: str, specializations: List['SpecializationNode'], from_namespace: List[str] = None, current_namespace: str = 'global'):
        self.name = name
        self.specializations = specializations
        self.from_namespace = from_namespace or []
        self.current_namespace = current_namespace
    def __repr__(self):
        return f"ElementUsageNode(name='{self.name}', specializations={len(self.specializations)}, from={self.from_namespace}, context='{self.current_namespace}')"

class VarUsageNode(UsageNode):
    def __init__(self, template_name: str, var_name: str, override_value: Any = None):
        self.template_name = template_name
        self.var_name = var_name
        self.override_value = override_value
    def __repr__(self):
        if self.override_value:
            return f"VarUsageNode(template='{self.template_name}', var='{self.var_name}', override='{self.override_value}')"
        return f"VarUsageNode(template='{self.template_name}', var='{self.var_name}')"

class StyleUsageNode(UsageNode):
    def __init__(self, name: str, from_namespace: List[str] = None):
        self.name = name
        self.from_namespace = from_namespace or []
    def __repr__(self):
        return f"StyleUsageNode(name='{self.name}', from={self.from_namespace})"

# --- Specializations ---
class SelectorNode(AstNode):
    def __init__(self, tag_name: str, index: Optional[int] = None):
        self.tag_name = tag_name
        self.index = index
    def __repr__(self):
        if self.index is not None:
            return f"SelectorNode(tag='{self.tag_name}', index={self.index})"
        return f"SelectorNode(tag='{self.tag_name}')"

class SpecializationNode(AstNode):
    pass

class InsertStatementNode(SpecializationNode):
    def __init__(self, elements: List['ElementNode'], position: str, target_selector: Optional[SelectorNode] = None):
        self.elements = elements
        self.position = position # e.g., 'after', 'before', 'at_top', 'at_bottom', 'replace'
        self.target_selector = target_selector
    def __repr__(self):
        return f"InsertStatementNode(pos='{self.position}', target={self.target_selector}, elements_len={len(self.elements)})"

class DeleteStatementNode(SpecializationNode):
    def __init__(self, target_selector: SelectorNode):
        self.target_selector = target_selector
    def __repr__(self):
        return f"DeleteStatementNode(target={self.target_selector})"


# --- Top Level Nodes ---
class DocumentNode(AstNode):
    def __init__(self, children: List[Union['ElementNode', 'DefinitionNode', 'DirectiveNode']]):
        self.children = children
    def __repr__(self):
        return f"DocumentNode(children={len(self.children)})"

class ElementNode(AstNode):
    def __init__(self, tag_name: str, attributes: List['AttributeNode'], children: List['AstNode']):
        self.tag_name = tag_name
        self.attributes = attributes
        self.children = children
    def __repr__(self):
        return f"ElementNode(tag='{self.tag_name}', attributes={self.attributes}, children={self.children})"

class StyleNode(AstNode):
    def __init__(self,
                 inline_styles: Dict[str, 'ExprNode'],
                 global_rules: Dict[str, Dict[str, 'ExprNode']],
                 style_usages: List[StyleUsageNode],
                 deleted_properties: List[str],
                 contextual_selector: Optional[str] = None):
        self.inline_styles = inline_styles
        self.global_rules = global_rules
        self.style_usages = style_usages
        self.deleted_properties = deleted_properties
        self.contextual_selector = contextual_selector
    def __repr__(self):
        return f"StyleNode(inline_count={len(self.inline_styles)}, global_rules_count={len(self.global_rules)}, context='{self.contextual_selector}')"

class ScriptNode(AstNode):
    def __init__(self, script_id: str):
        self.script_id = script_id
    def __repr__(self):
        return f"ScriptNode(id='{self.script_id}')"

class OriginNode(AstNode):
    def __init__(self, origin_type: str, content: str):
        self.origin_type = origin_type
        self.content = content
    def __repr__(self):
        return f"OriginNode(type='{self.origin_type}', content_len={len(self.content)})"

ElementChild = Union[ElementNode, TextNode, StyleNode, ScriptNode, UsageNode, OriginNode]
