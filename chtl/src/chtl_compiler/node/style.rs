use super::TemplateUsageNode;

#[derive(Debug, PartialEq, Clone)]
pub enum StyleContent {
    Property(StyleProperty),
    TemplateUsage(TemplateUsageNode),
}

#[derive(Debug, PartialEq, Clone)]
pub struct StyleNode {
    pub content: Vec<StyleContent>,
}

#[derive(Debug, PartialEq, Clone)]
pub struct StyleProperty {
    pub name: String,
    pub value: PropertyValue,
}

#[derive(Debug, PartialEq, Clone)]
pub enum PropertyValue {
    Literal(String),
    Conditional(ConditionalExpression),
}

#[derive(Debug, PartialEq, Clone)]
pub struct ConditionalExpression {
    // This is a simplified representation for now.
    // A full implementation would have a nested expression tree for the condition.
    pub condition: String,
    pub true_value: Box<PropertyValue>,
    pub false_value: Option<Box<PropertyValue>>,
}
