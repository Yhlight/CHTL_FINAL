mod chtl_compiler;
mod chtl_js_compiler;
mod unified_scanner;
mod compiler_dispatcher;
mod code_merger;

use clap::Parser as ClapParser;
use std::fs;
use std::io::{self, Read};

use unified_scanner::scanner::UnifiedScanner;
use compiler_dispatcher::dispatcher::CompilerDispatcher;
use code_merger::merger::CodeMerger;

/// CHTL Compiler: A new way to write HTML
#[derive(ClapParser, Debug)]
#[command(version, about, long_about = None)]
struct Args {
    /// Path to the input CHTL file. Reads from stdin if not provided.
    input_path: Option<String>,

    /// Path to the output HTML file. Writes to stdout if not provided.
    #[arg(short, long)]
    output_path: Option<String>,
}

fn main() {
    let args = Args::parse();

    // 1. Read Input
    let input = match args.input_path {
        Some(path) => match fs::read_to_string(path) {
            Ok(content) => content,
            Err(e) => {
                eprintln!("Error reading input file: {}", e);
                return;
            }
        },
        None => {
            let mut buffer = String::new();
            if io::stdin().read_to_string(&mut buffer).is_err() {
                eprintln!("Error reading from stdin");
                return;
            }
            buffer
        }
    };

    if input.trim().is_empty() {
        println!("// No input provided.");
        return;
    }

    // 2. Scan the source into fragments
    let scanner = UnifiedScanner::new(&input);
    let fragments = scanner.scan();

    // 3. Dispatch fragments to appropriate compilers
    let dispatcher = CompilerDispatcher::new();
    let compiled_output = dispatcher.dispatch(fragments);

    // 4. Merge the compiled outputs into a final string
    let merger = CodeMerger::new();
    let final_html = merger.merge(compiled_output);

    // 5. Write Output
    match args.output_path {
        Some(path) => {
            if fs::write(path, final_html).is_err() {
                eprintln!("Error writing to output file");
            }
        }
        None => {
            println!("{}", final_html);
        }
    }
}
