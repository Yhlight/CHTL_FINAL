use clap::Parser;
use notify::{Config, Event, RecommendedWatcher, RecursiveMode, Watcher};
use std::fs;
use std::path::{Path, PathBuf};
use std::sync::mpsc::channel;

// Use the library crate
use chtl_compiler::compile;

/// A compiler for the CHTL hypertext language.
#[derive(Parser, Debug)]
#[command(version, about, long_about = None)]
struct Args {
    /// The path to the input CHTL file.
    input: PathBuf,

    /// The path to the output HTML file.
    /// If not provided, the output is printed to stdout.
    #[arg(short, long)]
    output: Option<PathBuf>,

    /// Watch the input file for changes and recompile automatically.
    #[arg(short, long)]
    watch: bool,
}

/// Reads, compiles, and writes the file, handling all errors.
fn compile_and_write(input_path: &Path, output_path: &Option<PathBuf>) {
    // The dispatcher now handles file reading.
    // Compile the source code by passing the path.
    match compile(input_path) {
        Ok(html) => {
            // Write to output file or stdout
            if let Some(path) = output_path {
                if let Err(e) = fs::write(path, html) {
                    eprintln!("Error writing to output file '{}': {}", path.display(), e);
                } else {
                    println!("Successfully compiled '{}' to '{}'.", input_path.display(), path.display());
                }
            } else {
                println!("{}", html);
            }
        }
        Err(e) => {
            eprintln!("Compilation failed: {}", e);
        }
    }
}

fn main() -> notify::Result<()> {
    let args = Args::parse();

    if args.watch {
        // --- Watch Mode ---
        println!("Initial compilation...");
        compile_and_write(&args.input, &args.output);

        let (tx, rx) = channel();

        // Create a watcher object. The default recommended watcher is used.
        let mut watcher = RecommendedWatcher::new(tx, Config::default())?;

        // Add a path to be watched. All files and directories at that path and
        // below will be monitored for changes.
        watcher.watch(&args.input, RecursiveMode::NonRecursive)?;

        println!("Watching for changes in '{}'...", args.input.display());

        for res in rx {
            match res {
                Ok(Event { kind, .. }) => {
                    if kind.is_modify() || kind.is_create() {
                        println!("\nFile changed. Recompiling...");
                        compile_and_write(&args.input, &args.output);
                    }
                }
                Err(e) => eprintln!("Watch error: {:?}", e),
            }
        }
    } else {
        // --- Single-Run Mode ---
        compile_and_write(&args.input, &args.output);
    }

    Ok(())
}
