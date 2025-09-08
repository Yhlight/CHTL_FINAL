#[derive(Debug, PartialEq)]
pub struct Document<'a> {
    pub children: Vec<Node<'a>>,
}

#[derive(Debug, PartialEq)]
pub enum Node<'a> {
    Element(Element<'a>),
    Text(&'a str),
    StyleBlock(&'a str),
    ScriptBlock(&'a str),
}

#[derive(Debug, PartialEq)]
pub struct Element<'a> {
    pub tag_name: &'a str,
    pub children: Vec<Node<'a>>,
    // Will add attributes, style blocks, etc. later
}
