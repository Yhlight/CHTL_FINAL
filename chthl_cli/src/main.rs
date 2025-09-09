use std::env;
use std::fs;
use std::process;

use chthl::generator::generate_program;
use chthl::lexer::Lexer;
use chthl::parser::Parser;

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 2 {
        eprintln!("Usage: {} <filename.chtl>", args[0]);
        process::exit(1);
    }

    let filename = &args[1];
    let file_content = match fs::read_to_string(filename) {
        Ok(content) => content,
        Err(e) => {
            eprintln!("Error reading file '{}': {}", filename, e);
            process::exit(1);
        }
    };

    let lexer = Lexer::new(&file_content);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    if !parser.errors().is_empty() {
        eprintln!("Found {} parsing errors:", parser.errors().len());
        for error in parser.errors() {
            eprintln!("\t{}", error);
        }
        process::exit(1);
    }

    let html = generate_program(&program);
    println!("{}", html);
}
