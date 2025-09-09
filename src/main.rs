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
use std::fs;
use std::path::Path;

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
            let html = compiler.compile_file(&input, output.as_deref()).await?;
            
            if let Some(output_path) = output {
                info!("Compiled to: {}", output_path);
            } else {
                println!("{}", html);
            }
            
            if watch {
                info!("Watch mode enabled - monitoring for changes...");
                // TODO: Implement watch mode with file system monitoring
            }
        }
        Commands::Init { name } => {
            info!("Initializing new CHTL project: {}", name);
            init_project(&name)?;
        }
        Commands::Build { source, output } => {
            info!("Building CHTL project from: {} to: {}", source, output);
            build_project(&source, &output)?;
        }
        Commands::Serve { port, source } => {
            info!("Serving CHTL project on port {} from: {}", port, source);
            serve_project(port, &source).await?;
        }
    }
    
    Ok(())
}

/// Initialize a new CHTL project
fn init_project(name: &str) -> Result<()> {
    // Create project directory
    fs::create_dir_all(name)?;
    
    // Create src directory
    let src_dir = Path::new(name).join("src");
    fs::create_dir_all(&src_dir)?;
    
    // Create example CHTL file
    let example_content = r#"html lang="en" {
    head {
        meta charset="UTF-8";
        meta name="viewport" content="width=device-width, initial-scale=1.0";
        title { "Welcome to CHTL" }
    }
    
    body {
        h1 { "Hello, CHTL!" }
        p { "This is your first CHTL project." }
        
        style {
            "body {
                font-family: Arial, sans-serif;
                max-width: 800px;
                margin: 0 auto;
                padding: 20px;
            }
            
            h1 {
                color: #333;
                text-align: center;
            }
            
            p {
                color: #666;
                line-height: 1.6;
            }"
        }
    }
}"#;
    
    let example_file = src_dir.join("index.chtl");
    fs::write(&example_file, example_content)?;
    
    // Create package.json equivalent
    let package_content = format!(r#"{{
    "name": "{}",
    "version": "1.0.0",
    "description": "A CHTL project",
    "main": "src/index.chtl",
    "scripts": {{
        "build": "chtl build",
        "serve": "chtl serve",
        "dev": "chtl serve --watch"
    }},
    "keywords": ["chtl", "html", "css", "javascript"],
    "author": "",
    "license": "MIT"
}}"#, name);
    
    let package_file = Path::new(name).join("package.json");
    fs::write(&package_file, package_content)?;
    
    // Create README
    let readme_content = format!(r#"# {}

A CHTL project

## Getting Started

1. Install CHTL compiler
2. Run `chtl build` to build the project
3. Run `chtl serve` to start the development server

## Project Structure

- `src/` - Source CHTL files
- `dist/` - Compiled output (generated)

## Commands

- `chtl build` - Build the project
- `chtl serve` - Start development server
- `chtl compile src/index.chtl` - Compile a single file
"#, name);
    
    let readme_file = Path::new(name).join("README.md");
    fs::write(&readme_file, readme_content)?;
    
    info!("Project '{}' initialized successfully!", name);
    info!("Run 'cd {} && chtl build' to build the project", name);
    
    Ok(())
}

/// Build a CHTL project
fn build_project(source: &str, output: &str) -> Result<()> {
    let source_path = Path::new(source);
    let output_path = Path::new(output);
    
    // Create output directory
    fs::create_dir_all(output_path)?;
    
    // Find all CHTL files
    let mut chtl_files = Vec::new();
    find_chtl_files(source_path, &mut chtl_files)?;
    
    let compiler = ChtlCompiler::new();
    
    for chtl_file in chtl_files {
        let relative_path = chtl_file.strip_prefix(source_path)?;
        let output_file = output_path.join(relative_path).with_extension("html");
        
        // Create output directory if it doesn't exist
        if let Some(parent) = output_file.parent() {
            fs::create_dir_all(parent)?;
        }
        
        info!("Compiling: {} -> {}", chtl_file.display(), output_file.display());
        
        // Compile the file
        let html = compiler.compile_source(&fs::read_to_string(&chtl_file)?)?;
        fs::write(&output_file, html)?;
    }
    
    info!("Build completed successfully!");
    Ok(())
}

/// Serve a CHTL project with live reload
async fn serve_project(port: u16, source: &str) -> Result<()> {
    info!("Starting development server on port {}", port);
    info!("Source directory: {}", source);
    info!("Open http://localhost:{} in your browser", port);
    
    // TODO: Implement actual HTTP server with live reload
    // For now, just build the project and serve static files
    build_project(source, "dist")?;
    
    // Keep the server running
    loop {
        tokio::time::sleep(tokio::time::Duration::from_secs(1)).await;
    }
}

/// Find all CHTL files in a directory recursively
fn find_chtl_files(dir: &Path, files: &mut Vec<std::path::PathBuf>) -> Result<()> {
    if dir.is_dir() {
        for entry in fs::read_dir(dir)? {
            let entry = entry?;
            let path = entry.path();
            
            if path.is_dir() {
                find_chtl_files(&path, files)?;
            } else if path.extension().and_then(|s| s.to_str()) == Some("chtl") {
                files.push(path);
            }
        }
    }
    Ok(())
}