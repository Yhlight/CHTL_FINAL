use std::env;
use std::fs;
use std::process;

use chtl::CHTL::CHTLParser::parse_chtl;
use chtl::CHTL::CHTLGenerator::generate_html;

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 2 {
        eprintln!("Usage: {} <file_path>", args[0]);
        process::exit(1);
    }

    let file_path = &args[1];
    println!("Compiling file: {}", file_path);

    let source = match fs::read_to_string(file_path) {
        Ok(s) => s,
        Err(e) => {
            eprintln!("Error reading file '{}': {}", file_path, e);
            process::exit(1);
        }
    };

    match parse_chtl(&source) {
        Ok(ast) => {
            let html = generate_html(&ast);
            println!("\n--- Generated HTML ---");
            println!("{}", html);
        }
        Err(e) => {
            eprintln!("\n--- Parsing Error ---");
            eprintln!("{}", e);
            process::exit(1);
        }
    }
}
