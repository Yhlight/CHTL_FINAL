#[derive(Debug, PartialEq, Clone)]
pub struct Document<'a> {
    pub definitions: Vec<TopLevelDefinition<'a>>,
    pub children: Vec<Node<'a>>,
}

#[derive(Debug, PartialEq, Clone)]
pub enum TopLevelDefinition<'a> {
    Template(TemplateDefinition<'a>),
    Custom(CustomDefinition<'a>),
}

// --- Template & Custom Definitions ---

#[derive(Debug, PartialEq, Clone)]
pub enum TemplateDefinition<'a> {
    Style(TemplateStyleGroup<'a>),
    Element(TemplateElementGroup<'a>),
    Var(TemplateVarGroup<'a>),
}

#[derive(Debug, PartialEq, Clone)]
pub enum CustomDefinition<'a> {
    Style(CustomStyleGroup<'a>),
    Element(CustomElementGroup<'a>),
    Var(CustomVarGroup<'a>),
}

#[derive(Debug, PartialEq, Clone)]
pub struct TemplateStyleGroup<'a> {
    pub name: &'a str,
    pub content: Vec<StyleContent<'a>>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct CustomStyleGroup<'a> {
    pub name: &'a str,
    pub content: Vec<StyleContent<'a>>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct TemplateElementGroup<'a> {
    pub name: &'a str,
    pub children: Vec<Node<'a>>,
}

// Trait for any group of elements (Template or Custom)
pub trait ElementGroup<'a> {
    fn children(&self) -> &Vec<Node<'a>>;
}

impl<'a> ElementGroup<'a> for TemplateElementGroup<'a> {
    fn children(&self) -> &Vec<Node<'a>> {
        &self.children
    }
}

impl<'a> ElementGroup<'a> for CustomElementGroup<'a> {
    fn children(&self) -> &Vec<Node<'a>> {
        &self.children
    }
}

#[derive(Debug, PartialEq, Clone)]
pub struct CustomElementGroup<'a> {
    pub name: &'a str,
    pub children: Vec<Node<'a>>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct TemplateVarGroup<'a> {
    pub name: &'a str,
    pub variables: Vec<TemplateVariable<'a>>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct CustomVarGroup<'a> {
    pub name: &'a str,
    pub variables: Vec<TemplateVariable<'a>>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct TemplateVariable<'a> {
    pub key: &'a str,
    pub value: &'a str,
}

// --- Renderable Nodes & Content ---

#[derive(Debug, PartialEq, Clone)]
pub enum Node<'a> {
    Element(Element<'a>),
    Text(&'a str),
    StyleBlock(Vec<StyleContent<'a>>),
    ScriptBlock(&'a str),
    ElementTemplateUsage {
        name: &'a str,
        specializations: Vec<ElementSpecialization<'a>>,
    },
}

#[derive(Debug, PartialEq, Clone)]
pub struct ElementSpecialization<'a> {
    pub selector: ChildSelector<'a>,
    pub modifications: Vec<Node<'a>>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct ChildSelector<'a> {
    pub tag_name: &'a str,
    pub index: Option<usize>,
}

#[derive(Debug, PartialEq, Clone)]
pub enum StyleContent<'a> {
    Property(CssProperty<'a>),
    Ruleset(CssRuleset<'a>),
    StyleTemplateUsage { name: &'a str },
    InheritStyleTemplate { name: &'a str },
    CustomStyleUsage { name: &'a str, properties: Vec<CssProperty<'a>> },
    Delete(Vec<&'a str>),
}

#[derive(Debug, PartialEq, Clone)]
pub struct CssProperty<'a> {
    pub key: &'a str,
    pub value: Option<CssValue<'a>>,
}

#[derive(Debug, PartialEq, Clone)]
pub enum CssValue<'a> {
    Literal(&'a str),
    Variable(VarUsage<'a>),
}

#[derive(Debug, PartialEq, Clone)]
pub struct VarUsage<'a> {
    pub group_name: &'a str,
    pub var_name: &'a str,
}

#[derive(Debug, PartialEq, Clone)]
pub struct CssRuleset<'a> {
    pub selector: &'a str,
    pub properties: Vec<CssProperty<'a>>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct Attribute<'a> {
    pub key: &'a str,
    pub value: &'a str,
}

#[derive(Debug, PartialEq, Clone)]
pub struct Element<'a> {
    pub tag_name: &'a str,
    pub attributes: Vec<Attribute<'a>>,
    pub children: Vec<Node<'a>>,
}
