//! CHTL - A C++-based hypertext language for more developer-friendly HTML coding
//! 
//! This is the main entry point for the CHTL compiler and toolchain.

use anyhow::Result;
use clap::{Parser, Subcommand};
use log::info;

mod chtl;
mod chtl_js;
mod css;
mod js;
mod scanner;
mod code_merger;
mod compiler_dispatcher;
mod util;
mod test;

use chtl::ChtlCompiler;
use compiler_dispatcher::CompilerDispatcher;

/// CHTL - A C++-based hypertext language
#[derive(Parser)]
#[command(name = "chtl")]
#[command(about = "CHTL compiler and toolchain")]
#[command(version)]
struct Cli {
    #[command(subcommand)]
    command: Commands,
}

#[derive(Subcommand)]
enum Commands {
    /// Compile CHTL files to HTML
    Compile {
        /// Input CHTL file
        input: String,
        /// Output HTML file
        #[arg(short, long)]
        output: Option<String>,
        /// Watch mode - recompile on file changes
        #[arg(short, long)]
        watch: bool,
    },
    /// Initialize a new CHTL project
    Init {
        /// Project name
        name: String,
    },
    /// Build all CHTL files in the project
    Build {
        /// Source directory
        #[arg(short, long, default_value = ".")]
        source: String,
        /// Output directory
        #[arg(short, long, default_value = "dist")]
        output: String,
    },
    /// Serve the project with live reload
    Serve {
        /// Port to serve on
        #[arg(short, long, default_value = "8080")]
        port: u16,
        /// Source directory
        #[arg(short, long, default_value = ".")]
        source: String,
    },
}

#[tokio::main]
async fn main() -> Result<()> {
    env_logger::init();
    
    let cli = Cli::parse();
    
    match cli.command {
        Commands::Compile { input, output, watch } => {
            info!("Compiling CHTL file: {}", input);
            let compiler = ChtlCompiler::new();
            compiler.compile_file(&input, output.as_deref()).await?;
            
            if watch {
                info!("Watch mode enabled - monitoring for changes...");
                // TODO: Implement watch mode
            }
        }
        Commands::Init { name } => {
            info!("Initializing new CHTL project: {}", name);
            // TODO: Implement project initialization
        }
        Commands::Build { source, output } => {
            info!("Building CHTL project from: {} to: {}", source, output);
            // TODO: Implement project build
        }
        Commands::Serve { port, source } => {
            info!("Serving CHTL project on port {} from: {}", port, source);
            // TODO: Implement development server
        }
    }
    
    Ok(())
}