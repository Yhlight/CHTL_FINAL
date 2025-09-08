#[derive(Debug, PartialEq, Clone)]
pub struct Document<'a> {
    pub definitions: Vec<TopLevelDefinition<'a>>,
    pub children: Vec<Node<'a>>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct UseStatement<'a> {
    pub path: &'a str,
}

#[derive(Debug, PartialEq, Clone)]
pub enum TopLevelDefinition<'a> {
    Use(UseStatement<'a>),
    Template(TemplateDefinition<'a>),
    Custom(CustomDefinition<'a>),
}

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

#[derive(Debug, PartialEq, Clone)]
pub enum Node<'a> {
    Element(Element<'a>),
    Text(&'a str),
    StyleBlock(Vec<StyleContent<'a>>),
    ScriptBlock(&'a str),
    ElementTemplateUsage {
        name: &'a str,
        specializations: Vec<Specialization<'a>>,
    },
}

#[derive(Debug, PartialEq, Clone)]
pub enum Specialization<'a> {
    Modify(ModifySpec<'a>),
    Insert(InsertSpec<'a>),
    Delete(DeleteSpec<'a>),
}

#[derive(Debug, PartialEq, Clone)]
pub struct ModifySpec<'a> {
    pub selector: ChildSelector<'a>,
    pub modifications: Vec<Node<'a>>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct InsertSpec<'a> {
    pub position: InsertPosition,
    pub target_selector: ChildSelector<'a>,
    pub nodes_to_insert: Vec<Node<'a>>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct DeleteSpec<'a> {
    pub selector: ChildSelector<'a>,
}

#[derive(Debug, PartialEq, Clone)]
pub enum InsertPosition {
    Before,
    After,
    Replace,
    AtTop,
    AtBottom,
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
