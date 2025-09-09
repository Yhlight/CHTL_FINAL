use super::base::{Node, Attribute};

#[derive(Debug, PartialEq, Clone)]
pub struct ElementNode {
    pub tag_name: String,
    pub attributes: Vec<Attribute>,
    pub children: Vec<Node>,
}
