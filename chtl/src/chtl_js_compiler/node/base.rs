use super::EnhancedSelectorNode;

#[derive(Debug, PartialEq, Clone)]
pub enum CHTLJSNode {
    EnhancedSelector(EnhancedSelectorNode),
    // Other CHTL JS nodes will be added here, e.g., for listen, animate, etc.
}
