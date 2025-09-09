#[derive(Debug, PartialEq, Clone)]
pub struct CommentNode {
    pub content: String,
    /// True if it's a generator-aware comment (`--`), which should be rendered in the output.
    /// False for `//` and `/**/` comments, which should be ignored by the generator.
    pub is_generatable: bool,
}
