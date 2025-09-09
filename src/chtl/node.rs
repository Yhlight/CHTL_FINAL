//! CHTL AST nodes

use std::collections::HashMap;
use crate::chtl::lexer::SourceLocation;

/// Root AST node
#[derive(Debug, Clone, PartialEq)]
pub struct Document {
    pub children: Vec<Node>,
    pub location: SourceLocation,
}

/// Main AST node enumeration
#[derive(Debug, Clone, PartialEq)]
pub enum Node {
    Element(ElementNode),
    Text(TextNode),
    Style(StyleNode),
    Template(TemplateNode),
    Custom(CustomNode),
    Import(ImportNode),
    Namespace(NamespaceNode),
    Configuration(ConfigurationNode),
    Origin(OriginNode),
    Comment(CommentNode),
    Script(ScriptNode),
}

/// HTML element node
#[derive(Debug, Clone, PartialEq)]
pub struct ElementNode {
    pub tag: String,
    pub attributes: HashMap<String, AttributeValue>,
    pub children: Vec<Node>,
    pub self_closing: bool,
    pub location: SourceLocation,
}

/// Text content node
#[derive(Debug, Clone, PartialEq)]
pub struct TextNode {
    pub content: String,
    pub text_type: TextType,
    pub location: SourceLocation,
}

/// Text type
#[derive(Debug, Clone, PartialEq)]
pub enum TextType {
    Literal,
    DoubleQuoted,
    SingleQuoted,
}

/// Style block node
#[derive(Debug, Clone, PartialEq)]
pub struct StyleNode {
    pub selector: StyleSelector,
    pub properties: Vec<StyleProperty>,
    pub nested: Vec<StyleNode>,
    pub location: SourceLocation,
}

/// Style selector
#[derive(Debug, Clone, PartialEq)]
pub enum StyleSelector {
    Class(String),
    Id(String),
    Element(String),
    Context,
    Pseudo(String),
    AttributeConditional(AttributeConditional),
    Complex(Vec<StyleSelector>),
}

/// Attribute conditional
#[derive(Debug, Clone, PartialEq)]
pub struct AttributeConditional {
    pub attribute: String,
    pub operator: ConditionalOperator,
    pub value: String,
    pub chain: Option<Box<AttributeConditional>>,
    pub chain_operator: Option<ChainOperator>,
}

/// Conditional operator
#[derive(Debug, Clone, PartialEq)]
pub enum ConditionalOperator {
    Equal,
    NotEqual,
    GreaterThan,
    LessThan,
    GreaterEqual,
    LessEqual,
    Contains,
    StartsWith,
    EndsWith,
}

/// Chain operator
#[derive(Debug, Clone, PartialEq)]
pub enum ChainOperator {
    And,
    Or,
}

/// Style property
#[derive(Debug, Clone, PartialEq)]
pub struct StyleProperty {
    pub name: String,
    pub value: StyleValue,
    pub location: SourceLocation,
}

/// Style value
#[derive(Debug, Clone, PartialEq)]
pub enum StyleValue {
    Literal(String),
    Conditional(ConditionalExpression),
    CrossElement(CrossElementReference),
}

/// Conditional expression
#[derive(Debug, Clone, PartialEq)]
pub struct ConditionalExpression {
    pub condition: String,
    pub true_value: String,
    pub false_value: String,
}

impl std::fmt::Display for ConditionalExpression {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{} ? {} : {}", self.condition, self.true_value, self.false_value)
    }
}

/// Cross-element reference
#[derive(Debug, Clone, PartialEq)]
pub struct CrossElementReference {
    pub selector: String,
    pub attribute: String,
}

/// Template definition node
#[derive(Debug, Clone, PartialEq)]
pub struct TemplateNode {
    pub template_type: TemplateType,
    pub name: String,
    pub parameters: Vec<String>,
    pub content: Vec<Node>,
    pub location: SourceLocation,
}

/// Template type
#[derive(Debug, Clone, PartialEq)]
pub enum TemplateType {
    Style,
    Element,
    Variable,
}

/// Custom definition node
#[derive(Debug, Clone, PartialEq)]
pub struct CustomNode {
    pub custom_type: CustomType,
    pub name: String,
    pub base_template: Option<String>,
    pub modifications: Vec<CustomModification>,
    pub location: SourceLocation,
}

/// Custom type
#[derive(Debug, Clone, PartialEq)]
pub enum CustomType {
    Style,
    Element,
}

/// Custom modification
#[derive(Debug, Clone, PartialEq)]
pub enum CustomModification {
    AddProperty(String, String),
    DeleteProperty(String),
    DeleteInheritance,
    AddStyle(StyleNode),
    AddElement(ElementNode),
    DeleteElement(String),
}

/// Import statement node
#[derive(Debug, Clone, PartialEq)]
pub struct ImportNode {
    pub import_type: ImportType,
    pub path: String,
    pub items: Option<Vec<ImportItem>>,
    pub location: SourceLocation,
}

/// Import type
#[derive(Debug, Clone, PartialEq)]
pub enum ImportType {
    CHTL,
    HTML,
    CSS,
    JavaScript,
}

/// Import item
#[derive(Debug, Clone, PartialEq)]
pub struct ImportItem {
    pub name: String,
    pub alias: Option<String>,
}

/// Namespace declaration node
#[derive(Debug, Clone, PartialEq)]
pub struct NamespaceNode {
    pub name: String,
    pub content: Vec<Node>,
    pub location: SourceLocation,
}

/// Configuration node
#[derive(Debug, Clone, PartialEq)]
pub struct ConfigurationNode {
    pub name: Option<String>,
    pub properties: HashMap<String, String>,
    pub location: SourceLocation,
}

/// Origin embedding node
#[derive(Debug, Clone, PartialEq)]
pub struct OriginNode {
    pub origin_type: OriginType,
    pub name: Option<String>,
    pub content: String,
    pub location: SourceLocation,
}

/// Origin type
#[derive(Debug, Clone, PartialEq)]
pub enum OriginType {
    HTML,
    CSS,
    JavaScript,
}

/// Comment node
#[derive(Debug, Clone, PartialEq)]
pub struct CommentNode {
    pub content: String,
    pub comment_type: CommentType,
    pub location: SourceLocation,
}

/// Comment type
#[derive(Debug, Clone, PartialEq)]
pub enum CommentType {
    SingleLine,
    MultiLine,
    Generator,
}

/// Script block node
#[derive(Debug, Clone, PartialEq)]
pub struct ScriptNode {
    pub content: String,
    pub script_type: ScriptType,
    pub location: SourceLocation,
}

/// Script type
#[derive(Debug, Clone, PartialEq)]
pub enum ScriptType {
    Local,
    Global,
    CHTLJS,
}

/// Attribute value
#[derive(Debug, Clone, PartialEq)]
pub enum AttributeValue {
    String(String),
    Number(f64),
    Boolean(bool),
    Conditional(ConditionalExpression),
}

impl std::fmt::Display for AttributeValue {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            AttributeValue::String(s) => write!(f, "{}", s),
            AttributeValue::Number(n) => write!(f, "{}", n),
            AttributeValue::Boolean(b) => write!(f, "{}", b),
            AttributeValue::Conditional(expr) => write!(f, "{}", expr),
        }
    }
}

impl Document {
    pub fn new() -> Self {
        Self {
            children: Vec::new(),
            location: SourceLocation {
                file: String::new(),
                line: 0,
                column: 0,
                position: 0,
            },
        }
    }
    
    pub fn add_child(&mut self, child: Node) {
        self.children.push(child);
    }
}

impl ElementNode {
    pub fn new(tag: String) -> Self {
        Self {
            tag,
            attributes: HashMap::new(),
            children: Vec::new(),
            self_closing: false,
            location: SourceLocation {
                file: String::new(),
                line: 0,
                column: 0,
                position: 0,
            },
        }
    }
    
    pub fn set_attribute(&mut self, name: String, value: AttributeValue) {
        self.attributes.insert(name, value);
    }
    
    pub fn add_child(&mut self, child: Node) {
        self.children.push(child);
    }
}

impl TextNode {
    pub fn new(content: String, text_type: TextType) -> Self {
        Self {
            content,
            text_type,
            location: SourceLocation {
                file: String::new(),
                line: 0,
                column: 0,
                position: 0,
            },
        }
    }
}

impl StyleNode {
    pub fn new(selector: StyleSelector) -> Self {
        Self {
            selector,
            properties: Vec::new(),
            nested: Vec::new(),
            location: SourceLocation {
                file: String::new(),
                line: 0,
                column: 0,
                position: 0,
            },
        }
    }
    
    pub fn add_property(&mut self, name: String, value: StyleValue) {
        self.properties.push(StyleProperty {
            name,
            value,
            location: SourceLocation {
                file: String::new(),
                line: 0,
                column: 0,
                position: 0,
            },
        });
    }
}

impl TemplateNode {
    pub fn new(template_type: TemplateType, name: String) -> Self {
        Self {
            template_type,
            name,
            parameters: Vec::new(),
            content: Vec::new(),
            location: SourceLocation {
                file: String::new(),
                line: 0,
                column: 0,
                position: 0,
            },
        }
    }
    
    pub fn add_content(&mut self, node: Node) {
        self.content.push(node);
    }
}

impl CustomNode {
    pub fn new(custom_type: CustomType, name: String) -> Self {
        Self {
            custom_type,
            name,
            base_template: None,
            modifications: Vec::new(),
            location: SourceLocation {
                file: String::new(),
                line: 0,
                column: 0,
                position: 0,
            },
        }
    }
    
    pub fn set_base_template(&mut self, base_template: String) {
        self.base_template = Some(base_template);
    }
    
    pub fn add_modification(&mut self, modification: CustomModification) {
        self.modifications.push(modification);
    }
}

impl ImportNode {
    pub fn new(import_type: ImportType, path: String) -> Self {
        Self {
            import_type,
            path,
            items: None,
            location: SourceLocation {
                file: String::new(),
                line: 0,
                column: 0,
                position: 0,
            },
        }
    }
    
    pub fn set_items(&mut self, items: Vec<ImportItem>) {
        self.items = Some(items);
    }
}

impl NamespaceNode {
    pub fn new(name: String) -> Self {
        Self {
            name,
            content: Vec::new(),
            location: SourceLocation {
                file: String::new(),
                line: 0,
                column: 0,
                position: 0,
            },
        }
    }
    
    pub fn add_content(&mut self, node: Node) {
        self.content.push(node);
    }
}

impl ConfigurationNode {
    pub fn new(name: Option<String>) -> Self {
        Self {
            name,
            properties: HashMap::new(),
            location: SourceLocation {
                file: String::new(),
                line: 0,
                column: 0,
                position: 0,
            },
        }
    }
    
    pub fn set_property(&mut self, key: String, value: String) {
        self.properties.insert(key, value);
    }
}

impl OriginNode {
    pub fn new(origin_type: OriginType, content: String) -> Self {
        Self {
            origin_type,
            name: None,
            content,
            location: SourceLocation {
                file: String::new(),
                line: 0,
                column: 0,
                position: 0,
            },
        }
    }
    
    pub fn set_name(&mut self, name: String) {
        self.name = Some(name);
    }
}

impl CommentNode {
    pub fn new(content: String, comment_type: CommentType) -> Self {
        Self {
            content,
            comment_type,
            location: SourceLocation {
                file: String::new(),
                line: 0,
                column: 0,
                position: 0,
            },
        }
    }
}

impl ScriptNode {
    pub fn new(content: String, script_type: ScriptType) -> Self {
        Self {
            content,
            script_type,
            location: SourceLocation {
                file: String::new(),
                line: 0,
                column: 0,
                position: 0,
            },
        }
    }
}

impl Default for Document {
    fn default() -> Self {
        Self::new()
    }
}