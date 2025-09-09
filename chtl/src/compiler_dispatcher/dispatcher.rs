use crate::unified_scanner::scanner::SourceFragment;
use crate::chtl_compiler::{
    lexer::Lexer as ChtlLexer,
    parser::Parser as ChtlParser,
    transformer::expander::Expander,
    generator::Generator as ChtlGenerator,
};
// use crate::chtl_js_compiler::{
//     lexer::Lexer as ChtlJsLexer,
//     parser::Parser as ChtlJsParser,
//     generator::Generator as ChtlJsGenerator,
// };

// This will eventually hold instances of our compilers
pub struct CompilerDispatcher;

// This will hold the compiled output from each compiler
#[derive(Default)]
pub struct CompilationOutput {
    pub html: String,
    pub css: String,
    pub js: String,
}

impl CompilerDispatcher {
    pub fn new() -> Self {
        CompilerDispatcher
    }

    pub fn dispatch(&self, fragments: Vec<SourceFragment>) -> CompilationOutput {
        let mut output = CompilationOutput::default();

        for fragment in fragments {
            match fragment {
                SourceFragment::Chtl(source) => {
                    let lexer = ChtlLexer::new(&source);
                    let mut parser = ChtlParser::new(lexer);
                    let program = parser.parse_program();

                    if !parser.errors().is_empty() {
                        // In a real application, we'd handle these errors properly.
                        eprintln!("CHTL Parser errors: {:?}", parser.errors());
                        continue;
                    }

                    let expander = Expander::new(parser.templates());
                    let expanded_program = expander.expand_program(program);

                    let generator = ChtlGenerator::new();
                    output.html.push_str(&generator.generate(expanded_program));
                }
                SourceFragment::ChtlJs(source) => {
                    // let mut lexer = ChtlJsLexer::new(&source);
                    // let mut parser = ChtlJsParser::new(lexer);
                    // let program = parser.parse_program();
                    // let generator = ChtlJsGenerator::new();
                    // output.js.push_str(&generator.generate(program));
                    output.js.push_str(&format!("/* CHTL JS content not compiled: {} */", source));
                }
                // Other fragment types will be handled here later
                _ => {}
            }
        }

        output
    }
}
