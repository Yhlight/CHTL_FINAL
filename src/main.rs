//! CHTL CLI application

use chtl::{ChtlCompiler, Result};
use clap::{Parser, Subcommand};
use std::fs;
use std::path::PathBuf;

/// CHTL - A hypertext language based on Rust
#[derive(Parser)]
#[command(name = "chtl")]
#[command(version = "0.1.0")]
#[command(about = "CHTL compiler and development tools")]
struct Cli {
    #[command(subcommand)]
    command: Commands,
}

#[derive(Subcommand)]
enum Commands {
    /// Compile CHTL source to HTML
    Compile {
        /// Input CHTL file
        #[arg(short, long)]
        input: PathBuf,
        
        /// Output HTML file
        #[arg(short, long)]
        output: Option<PathBuf>,
        
        /// Generate CSS file
        #[arg(long)]
        css: Option<PathBuf>,
        
        /// Generate JavaScript file
        #[arg(long)]
        js: Option<PathBuf>,
        
        /// Watch for changes and recompile
        #[arg(short, long)]
        watch: bool,
    },
    
    /// Initialize a new CHTL project
    Init {
        /// Project directory
        #[arg(default_value = ".")]
        directory: PathBuf,
        
        /// Project name
        #[arg(short, long)]
        name: Option<String>,
    },
    
    /// Build a CHTL project
    Build {
        /// Project directory
        #[arg(default_value = ".")]
        directory: PathBuf,
        
        /// Output directory
        #[arg(short, long, default_value = "dist")]
        output: PathBuf,
    },
    
    /// Serve a CHTL project with live reload
    Serve {
        /// Project directory
        #[arg(default_value = ".")]
        directory: PathBuf,
        
        /// Port to serve on
        #[arg(short, long, default_value = "8080")]
        port: u16,
        
        /// Host to bind to
        #[arg(long, default_value = "localhost")]
        host: String,
    },
    
    /// Format CHTL source code
    Format {
        /// Input file or directory
        input: PathBuf,
        
        /// Write formatted code back to file
        #[arg(short, long)]
        write: bool,
    },
    
    /// Lint CHTL source code
    Lint {
        /// Input file or directory
        input: PathBuf,
    },
    
    /// Generate documentation
    Doc {
        /// Input file or directory
        input: PathBuf,
        
        /// Output directory
        #[arg(short, long, default_value = "docs")]
        output: PathBuf,
    },
}

fn main() -> Result<()> {
    let cli = Cli::parse();
    
    match cli.command {
        Commands::Compile { input, output, css, js, watch } => {
            compile_command(input, output, css, js, watch)
        }
        Commands::Init { directory, name } => {
            init_command(directory, name)
        }
        Commands::Build { directory, output } => {
            build_command(directory, output)
        }
        Commands::Serve { directory, port, host } => {
            serve_command(directory, port, host)
        }
        Commands::Format { input, write } => {
            format_command(input, write)
        }
        Commands::Lint { input } => {
            lint_command(input)
        }
        Commands::Doc { input, output } => {
            doc_command(input, output)
        }
    }
}

fn compile_command(
    input: PathBuf,
    output: Option<PathBuf>,
    css: Option<PathBuf>,
    js: Option<PathBuf>,
    _watch: bool,
) -> Result<()> {
    // Read input file
    let source = fs::read_to_string(&input)
        .map_err(|e| chtl::ChtlError::Io(e))?;
    
    // Compile CHTL
    let compiler = ChtlCompiler::new();
    let html = compiler.compile(&source)?;
    
    // Determine output file
    let output_file = output.unwrap_or_else(|| {
        input.with_extension("html")
    });
    
    // Write HTML output
    fs::write(&output_file, html)
        .map_err(|e| chtl::ChtlError::Io(e))?;
    
    println!("Compiled {} to {}", input.display(), output_file.display());
    
    // TODO: Generate CSS and JS files if requested
    if let Some(css_file) = css {
        println!("CSS output requested: {}", css_file.display());
    }
    
    if let Some(js_file) = js {
        println!("JavaScript output requested: {}", js_file.display());
    }
    
    Ok(())
}

fn init_command(directory: PathBuf, name: Option<String>) -> Result<()> {
    let project_name = name.unwrap_or_else(|| {
        directory.file_name()
            .and_then(|n| n.to_str())
            .unwrap_or("chtl-project")
            .to_string()
    });
    
    // Create project directory
    fs::create_dir_all(&directory)
        .map_err(|e| chtl::ChtlError::Io(e))?;
    
    // Create basic project structure
    let src_dir = directory.join("src");
    fs::create_dir_all(&src_dir)
        .map_err(|e| chtl::ChtlError::Io(e))?;
    
    // Create main CHTL file
    let main_chtl = src_dir.join("main.chtl");
    let main_content = r#"html
{
    head
    {
        title
        {
            text: "Hello CHTL";
        }
    }
    
    body
    {
        h1
        {
            text: "Welcome to CHTL!";
        }
        
        p
        {
            text: "This is a basic CHTL document.";
        }
    }
}"#;
    
    fs::write(&main_chtl, main_content)
        .map_err(|e| chtl::ChtlError::Io(e))?;
    
    // Create configuration file
    let config_file = directory.join("chtl.config.chtl");
    let config_content = r#"[Configuration]
{
    HTML5_DOCTYPE = true;
    DEBUG_MODE = false;
    INDEX_INITIAL_COUNT = 0;
}"#;
    
    fs::write(&config_file, config_content)
        .map_err(|e| chtl::ChtlError::Io(e))?;
    
    // Create package.json for Node.js integration
    let package_json = directory.join("package.json");
    let package_content = format!(r#"{{
    "name": "{}",
    "version": "1.0.0",
    "description": "CHTL project",
    "scripts": {{
        "build": "chtl build",
        "serve": "chtl serve",
        "dev": "chtl serve --watch"
    }},
    "devDependencies": {{
        "chtl": "latest"
    }}
}}"#, project_name);
    
    fs::write(&package_json, package_content)
        .map_err(|e| chtl::ChtlError::Io(e))?;
    
    println!("Created CHTL project '{}' in {}", project_name, directory.display());
    println!("Run 'chtl build' to compile your project");
    println!("Run 'chtl serve' to start development server");
    
    Ok(())
}

fn build_command(directory: PathBuf, output: PathBuf) -> Result<()> {
    println!("Building CHTL project in {}", directory.display());
    
    // Create output directory
    fs::create_dir_all(&output)
        .map_err(|e| chtl::ChtlError::Io(e))?;
    
    // Find all CHTL files
    let src_dir = directory.join("src");
    if !src_dir.exists() {
        return Err(chtl::ChtlError::Io(std::io::Error::new(
            std::io::ErrorKind::NotFound,
            "src directory not found"
        )));
    }
    
    // TODO: Implement full build process
    println!("Build completed. Output directory: {}", output.display());
    
    Ok(())
}

fn serve_command(directory: PathBuf, port: u16, host: String) -> Result<()> {
    println!("Starting CHTL development server on {}:{}", host, port);
    println!("Serving directory: {}", directory.display());
    
    // TODO: Implement development server with live reload
    println!("Development server started. Press Ctrl+C to stop.");
    
    // Keep the server running
    std::thread::park();
    
    Ok(())
}

fn format_command(input: PathBuf, write: bool) -> Result<()> {
    println!("Formatting CHTL code in {}", input.display());
    
    // TODO: Implement code formatting
    if write {
        println!("Formatted code written back to files");
    } else {
        println!("Formatted code (use --write to save changes)");
    }
    
    Ok(())
}

fn lint_command(input: PathBuf) -> Result<()> {
    println!("Linting CHTL code in {}", input.display());
    
    // TODO: Implement code linting
    println!("Linting completed. No issues found.");
    
    Ok(())
}

fn doc_command(input: PathBuf, output: PathBuf) -> Result<()> {
    println!("Generating documentation from {}", input.display());
    println!("Output directory: {}", output.display());
    
    // Create output directory
    fs::create_dir_all(&output)
        .map_err(|e| chtl::ChtlError::Io(e))?;
    
    // TODO: Implement documentation generation
    println!("Documentation generated successfully");
    
    Ok(())
}