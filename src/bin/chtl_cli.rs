//! CHTL command-line interface
//! 
//! This is the main CLI entry point for CHTL,
//! providing a command-line interface for compiling CHTL files.

use clap::{Arg, Command};
use chtl::{CHTLCompiler, CompilationError};
use std::fs;
use std::path::Path;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let matches = Command::new("chtl")
        .version("0.1.0")
        .about("CHTL - A hypertext language for more developer-friendly HTML coding")
        .arg(
            Arg::new("input")
                .help("The input CHTL file to compile")
                .required(true)
                .index(1),
        )
        .arg(
            Arg::new("output")
                .short('o')
                .long("output")
                .value_name("FILE")
                .help("Sets the output file")
                .num_args(1),
        )
        .arg(
            Arg::new("verbose")
                .short('v')
                .long("verbose")
                .action(clap::ArgAction::SetTrue)
                .help("Enable verbose output"),
        )
        .get_matches();

    let input_file = matches.get_one::<String>("input").unwrap();
    let output_file = matches.get_one::<String>("output");
    let verbose = matches.get_flag("verbose");

    if verbose {
        println!("Compiling CHTL file: {}", input_file);
    }

    // Read input file
    let source = fs::read_to_string(input_file)?;

    // Create compiler
    let compiler = CHTLCompiler::new();

    // Compile
    match compiler.compile(&source) {
        Ok(result) => {
            if verbose {
                println!("Compilation successful!");
                println!("HTML: {} chars", result.html.len());
                println!("CSS: {} chars", result.css.len());
                println!("JavaScript: {} chars", result.javascript.len());
            }

            // Output results
            if let Some(output) = output_file {
                let output_path = Path::new(output);
                let base_name = output_path.file_stem().unwrap().to_str().unwrap();
                let dir = output_path.parent().unwrap_or(Path::new("."));

                fs::write(dir.join(format!("{}.html", base_name)), &result.html)?;
                fs::write(dir.join(format!("{}.css", base_name)), &result.css)?;
                fs::write(dir.join(format!("{}.js", base_name)), &result.javascript)?;

                if verbose {
                    println!("Output written to {}.{{html,css,js}}", base_name);
                }
            } else {
                println!("{}", result.html);
            }
        }
        Err(CompilationError::Lexical(msg)) => {
            eprintln!("Lexical error: {}", msg);
            std::process::exit(1);
        }
        Err(CompilationError::Parse(msg)) => {
            eprintln!("Parse error: {}", msg);
            std::process::exit(1);
        }
        Err(CompilationError::Semantic(msg)) => {
            eprintln!("Semantic error: {}", msg);
            std::process::exit(1);
        }
        Err(CompilationError::Generation(msg)) => {
            eprintln!("Generation error: {}", msg);
            std::process::exit(1);
        }
        Err(CompilationError::Io(err)) => {
            eprintln!("IO error: {}", err);
            std::process::exit(1);
        }
    }

    Ok(())
}