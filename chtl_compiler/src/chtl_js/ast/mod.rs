#[derive(Debug, PartialEq)]
pub struct Document<'a> {
    pub nodes: Vec<Node<'a>>,
}

#[derive(Debug, PartialEq)]
pub enum Node<'a> {
    EnhancedSelector(EnhancedSelector<'a>),
    Code(&'a str),
}

#[derive(Debug, PartialEq)]
pub struct EnhancedSelector<'a> {
    pub selector: &'a str,
}
