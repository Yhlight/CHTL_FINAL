#[derive(Debug, PartialEq)]
pub struct Document<'a> {
    pub children: Vec<Node<'a>>,
}

#[derive(Debug, PartialEq)]
pub enum Node<'a> {
    Element(Element<'a>),
    Text(&'a str),
    StyleBlock(Vec<CssProperty<'a>>),
    ScriptBlock(&'a str),
}

#[derive(Debug, PartialEq, Clone)]
pub struct CssProperty<'a> {
    pub key: &'a str,
    pub value: &'a str,
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
