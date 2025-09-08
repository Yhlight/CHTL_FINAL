#[derive(Debug, PartialEq)]
pub struct Document<'a> {
    pub definitions: Vec<TopLevelDefinition<'a>>,
    pub children: Vec<Node<'a>>,
}

#[derive(Debug, PartialEq)]
pub enum TopLevelDefinition<'a> {
    Template(TemplateDefinition<'a>),
    // Future: Custom, Namespace, etc.
}

#[derive(Debug, PartialEq)]
pub enum TemplateDefinition<'a> {
    Style(TemplateStyleGroup<'a>),
    Element(TemplateElementGroup<'a>),
    Var(TemplateVarGroup<'a>),
}

#[derive(Debug, PartialEq)]
pub struct TemplateStyleGroup<'a> {
    pub name: &'a str,
    pub content: Vec<StyleContent<'a>>,
}

#[derive(Debug, PartialEq)]
pub struct TemplateElementGroup<'a> {
    pub name: &'a str,
    pub children: Vec<Node<'a>>,
}

#[derive(Debug, PartialEq)]
pub struct TemplateVarGroup<'a> {
    pub name: &'a str,
    pub variables: Vec<TemplateVariable<'a>>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct TemplateVariable<'a> {
    pub key: &'a str,
    pub value: &'a str,
}


// --- Renderable Nodes ---

#[derive(Debug, PartialEq)]
pub enum Node<'a> {
    Element(Element<'a>),
    Text(&'a str),
    StyleBlock(Vec<StyleContent<'a>>),
    ScriptBlock(&'a str),
    ElementTemplateUsage { name: &'a str },
}

#[derive(Debug, PartialEq, Clone)]
pub enum StyleContent<'a> {
    Property(CssProperty<'a>),
    Ruleset(CssRuleset<'a>),
    StyleTemplateUsage { name: &'a str },
    InheritStyleTemplate { name: &'a str },
}

#[derive(Debug, PartialEq, Clone)]
pub struct CssProperty<'a> {
    pub key: &'a str,
    pub value: &'a str,
}

#[derive(Debug, PartialEq, Clone)]
pub struct CssRuleset<'a> {
    pub selector: &'a str,
    pub properties: Vec<CssProperty<'a>>,
}

#[derive(Debug, PartialEq)]
pub struct Attribute<'a> {
    pub key: &'a str,
    pub value: &'a str,
}

#[derive(Debug, PartialEq)]
pub struct Element<'a> {
    pub tag_name: &'a str,
    pub attributes: Vec<Attribute<'a>>,
    pub children: Vec<Node<'a>>,
}
