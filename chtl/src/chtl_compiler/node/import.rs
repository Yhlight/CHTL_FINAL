#[derive(Debug, PartialEq, Clone)]
pub enum ImportKind {
    Chtl,
    Html,
    Style,
    JavaScript,
    // For future expansion
    // Cjmod,
    // Config,
}

#[derive(Debug, PartialEq, Clone)]
pub struct ImportNode {
    pub kind: ImportKind,
    pub path: String,
    pub alias: Option<String>,
    // More fields will be needed for precise/type/wildcard imports later
}
