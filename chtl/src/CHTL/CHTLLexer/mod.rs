use pest_derive::Parser;

#[derive(Parser)]
#[grammar = "CHTL/CHTLLexer/chtl.pest"]
pub struct CHTLParser;
