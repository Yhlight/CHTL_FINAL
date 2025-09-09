use clap::Parser;
use std::path::PathBuf;

pub mod chtl;
pub mod chtl_js;
pub mod scanner;
pub mod cmod;
pub mod cjmod_api;
pub mod util;

/// A CHTL compiler implemented in Rust
#[derive(Parser, Debug)]
#[command(version, about, long_about = None)]
struct Cli {
    #[command(subcommand)]
    command: Commands,
}

#[derive(Parser, Debug)]
enum Commands {
    /// Compiles a CHTL file into HTML
    Compile {
        /// The path to the CHTL file to compile
        #[arg(short, long)]
        input: PathBuf,
    },
}

fn main() {
    let cli = Cli::parse();

    match &cli.command {
        Commands::Compile { input } => {
            println!("Compiling file: {:?}", input);
            // In a real implementation, we would call the compiler pipeline here.
            // For example:
            // 1. Read the file content.
            // 2. Run the UnifiedScanner.
            // 3. Dispatch fragments to compilers.
            // 4. Merge results.
            // 5. Write to an output file.
        }
    }
}
