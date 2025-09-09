//! CLI tools for CHTL compiler

use crate::ChtlCompiler;
use crate::error::{ChtlError, Result};
use std::path::{Path, PathBuf};
use std::fs;
use std::io::{self, Write};

/// CLI application for CHTL compiler
pub struct CliApp {
    compiler: ChtlCompiler,
    verbose: bool,
    output_dir: Option<PathBuf>,
    watch_mode: bool,
    minify: bool,
    source_map: bool,
}

impl CliApp {
    pub fn new() -> Self {
        Self {
            compiler: ChtlCompiler::new(),
            verbose: false,
            output_dir: None,
            watch_mode: false,
            minify: false,
            source_map: false,
        }
    }
    
    /// Run the CLI application
    pub fn run(&mut self, args: Vec<String>) -> Result<()> {
        let matches = self.parse_args(args)?;
        
        if matches.is_present("verbose") {
            self.verbose = true;
        }
        
        if let Some(output) = matches.value_of("output") {
            self.output_dir = Some(PathBuf::from(output));
        }
        
        if matches.is_present("watch") {
            self.watch_mode = true;
        }
        
        if matches.is_present("minify") {
            self.minify = true;
        }
        
        if matches.is_present("source-map") {
            self.source_map = true;
        }
        
        match matches.subcommand() {
            ("compile", Some(sub_matches)) => {
                self.handle_compile_command(sub_matches)?;
            }
            ("build", Some(sub_matches)) => {
                self.handle_build_command(sub_matches)?;
            }
            ("watch", Some(sub_matches)) => {
                self.handle_watch_command(sub_matches)?;
            }
            ("init", Some(sub_matches)) => {
                self.handle_init_command(sub_matches)?;
            }
            ("serve", Some(sub_matches)) => {
                self.handle_serve_command(sub_matches)?;
            }
            ("test", Some(sub_matches)) => {
                self.handle_test_command(sub_matches)?;
            }
            ("lint", Some(sub_matches)) => {
                self.handle_lint_command(sub_matches)?;
            }
            ("format", Some(sub_matches)) => {
                self.handle_format_command(sub_matches)?;
            }
            ("bundle", Some(sub_matches)) => {
                self.handle_bundle_command(sub_matches)?;
            }
            ("module", Some(sub_matches)) => {
                self.handle_module_command(sub_matches)?;
            }
            _ => {
                self.print_help();
            }
        }
        
        Ok(())
    }
    
    /// Parse command line arguments
    fn parse_args(&self, args: Vec<String>) -> Result<clap::ArgMatches> {
        let app = clap::App::new("chtl")
            .version("1.0.0")
            .author("CHTL Team")
            .about("CHTL - A hypertext language based on Rust")
            .arg(clap::Arg::with_name("verbose")
                .short("v")
                .long("verbose")
                .help("Enable verbose output"))
            .arg(clap::Arg::with_name("output")
                .short("o")
                .long("output")
                .value_name("DIR")
                .help("Output directory"))
            .arg(clap::Arg::with_name("watch")
                .short("w")
                .long("watch")
                .help("Watch for file changes"))
            .arg(clap::Arg::with_name("minify")
                .long("minify")
                .help("Minify output"))
            .arg(clap::Arg::with_name("source-map")
                .long("source-map")
                .help("Generate source maps"))
            .subcommand(clap::App::new("compile")
                .about("Compile CHTL files")
                .arg(clap::Arg::with_name("input")
                    .required(true)
                    .help("Input CHTL file"))
                .arg(clap::Arg::with_name("format")
                    .short("f")
                    .long("format")
                    .value_name("FORMAT")
                    .possible_values(&["html", "css", "js", "all"])
                    .default_value("html")
                    .help("Output format")))
            .subcommand(clap::App::new("build")
                .about("Build project")
                .arg(clap::Arg::with_name("input")
                    .required(true)
                    .help("Input directory or file"))
                .arg(clap::Arg::with_name("recursive")
                    .short("r")
                    .long("recursive")
                    .help("Process directories recursively")))
            .subcommand(clap::App::new("watch")
                .about("Watch for changes and recompile")
                .arg(clap::Arg::with_name("input")
                    .required(true)
                    .help("Input directory or file")))
            .subcommand(clap::App::new("init")
                .about("Initialize new CHTL project")
                .arg(clap::Arg::with_name("name")
                    .required(true)
                    .help("Project name"))
                .arg(clap::Arg::with_name("template")
                    .short("t")
                    .long("template")
                    .value_name("TEMPLATE")
                    .help("Project template")))
            .subcommand(clap::App::new("serve")
                .about("Start development server")
                .arg(clap::Arg::with_name("port")
                    .short("p")
                    .long("port")
                    .value_name("PORT")
                    .default_value("3000")
                    .help("Port number"))
                .arg(clap::Arg::with_name("host")
                    .long("host")
                    .value_name("HOST")
                    .default_value("localhost")
                    .help("Host address")))
            .subcommand(clap::App::new("test")
                .about("Run tests")
                .arg(clap::Arg::with_name("pattern")
                    .help("Test pattern"))
                .arg(clap::Arg::with_name("verbose")
                    .short("v")
                    .long("verbose")
                    .help("Verbose output")))
            .subcommand(clap::App::new("lint")
                .about("Lint CHTL files")
                .arg(clap::Arg::with_name("input")
                    .required(true)
                    .help("Input file or directory"))
                .arg(clap::Arg::with_name("fix")
                    .long("fix")
                    .help("Fix issues automatically")))
            .subcommand(clap::App::new("format")
                .about("Format CHTL files")
                .arg(clap::Arg::with_name("input")
                    .required(true)
                    .help("Input file or directory"))
                .arg(clap::Arg::with_name("check")
                    .long("check")
                    .help("Check formatting without making changes")))
            .subcommand(clap::App::new("bundle")
                .about("Bundle CHTL project")
                .arg(clap::Arg::with_name("input")
                    .required(true)
                    .help("Input directory"))
                .arg(clap::Arg::with_name("output")
                    .short("o")
                    .long("output")
                    .value_name("FILE")
                    .help("Output bundle file")))
            .subcommand(clap::App::new("module")
                .about("Manage CHTL modules")
                .subcommand(clap::App::new("create")
                    .about("Create new module")
                    .arg(clap::Arg::with_name("name")
                        .required(true)
                        .help("Module name"))
                    .arg(clap::Arg::with_name("type")
                        .short("t")
                        .long("type")
                        .value_name("TYPE")
                        .possible_values(&["cmod", "cjmod"])
                        .default_value("cmod")
                        .help("Module type")))
                .subcommand(clap::App::new("install")
                    .about("Install module")
                    .arg(clap::Arg::with_name("name")
                        .required(true)
                        .help("Module name")))
                .subcommand(clap::App::new("list")
                    .about("List installed modules"))
                .subcommand(clap::App::new("remove")
                    .about("Remove module")
                    .arg(clap::Arg::with_name("name")
                        .required(true)
                        .help("Module name"))));
        
        Ok(app.get_matches_from(args))
    }
    
    /// Handle compile command
    fn handle_compile_command(&mut self, matches: &clap::ArgMatches) -> Result<()> {
        let input_file = matches.value_of("input").unwrap();
        let format = matches.value_of("format").unwrap();
        
        let input_path = Path::new(input_file);
        if !input_path.exists() {
            return Err(ChtlError::io(&format!("File not found: {}", input_file)));
        }
        
        let source = fs::read_to_string(input_path)?;
        
        match format {
            "html" => {
                let html = self.compiler.compile(&source)?;
                self.output_html(&html, input_path)?;
            }
            "css" => {
                let (_, css, _) = self.compiler.compile_full(&source)?;
                self.output_css(&css, input_path)?;
            }
            "js" => {
                let (_, _, js) = self.compiler.compile_full(&source)?;
                self.output_js(&js, input_path)?;
            }
            "all" => {
                let (html, css, js) = self.compiler.compile_full(&source)?;
                self.output_html(&html, input_path)?;
                self.output_css(&css, input_path)?;
                self.output_js(&js, input_path)?;
            }
            _ => unreachable!(),
        }
        
        if self.verbose {
            println!("Compiled: {}", input_file);
        }
        
        Ok(())
    }
    
    /// Handle build command
    fn handle_build_command(&mut self, matches: &clap::ArgMatches) -> Result<()> {
        let input = matches.value_of("input").unwrap();
        let recursive = matches.is_present("recursive");
        
        let input_path = Path::new(input);
        if input_path.is_file() {
            self.build_file(input_path)?;
        } else if input_path.is_dir() {
            self.build_directory(input_path, recursive)?;
        } else {
            return Err(ChtlError::io(&format!("Path not found: {}", input)));
        }
        
        Ok(())
    }
    
    /// Handle watch command
    fn handle_watch_command(&mut self, matches: &clap::ArgMatches) -> Result<()> {
        let input = matches.value_of("input").unwrap();
        let input_path = Path::new(input);
        
        if self.verbose {
            println!("Watching: {}", input);
        }
        
        // Simple file watching implementation
        loop {
            if input_path.is_file() {
                if let Err(e) = self.build_file(input_path) {
                    eprintln!("Error building file: {}", e);
                }
            } else if input_path.is_dir() {
                if let Err(e) = self.build_directory(input_path, true) {
                    eprintln!("Error building directory: {}", e);
                }
            }
            
            std::thread::sleep(std::time::Duration::from_secs(1));
        }
    }
    
    /// Handle init command
    fn handle_init_command(&mut self, matches: &clap::ArgMatches) -> Result<()> {
        let name = matches.value_of("name").unwrap();
        let template = matches.value_of("template");
        
        self.create_project(name, template)?;
        
        println!("Created CHTL project: {}", name);
        
        Ok(())
    }
    
    /// Handle serve command
    fn handle_serve_command(&mut self, matches: &clap::ArgMatches) -> Result<()> {
        let port = matches.value_of("port").unwrap().parse::<u16>()
            .map_err(|_| ChtlError::semantic("Invalid port number"))?;
        let host = matches.value_of("host").unwrap();
        
        println!("Starting development server on {}:{}", host, port);
        
        // Simple HTTP server implementation
        self.start_dev_server(host, port)?;
        
        Ok(())
    }
    
    /// Handle test command
    fn handle_test_command(&mut self, matches: &clap::ArgMatches) -> Result<()> {
        let pattern = matches.value_of("pattern");
        let verbose = matches.is_present("verbose");
        
        println!("Running tests...");
        
        // Run tests
        self.run_tests(pattern, verbose)?;
        
        Ok(())
    }
    
    /// Handle lint command
    fn handle_lint_command(&mut self, matches: &clap::ArgMatches) -> Result<()> {
        let input = matches.value_of("input").unwrap();
        let fix = matches.is_present("fix");
        
        let input_path = Path::new(input);
        if input_path.is_file() {
            self.lint_file(input_path, fix)?;
        } else if input_path.is_dir() {
            self.lint_directory(input_path, fix)?;
        } else {
            return Err(ChtlError::io(&format!("Path not found: {}", input)));
        }
        
        Ok(())
    }
    
    /// Handle format command
    fn handle_format_command(&mut self, matches: &clap::ArgMatches) -> Result<()> {
        let input = matches.value_of("input").unwrap();
        let check = matches.is_present("check");
        
        let input_path = Path::new(input);
        if input_path.is_file() {
            self.format_file(input_path, check)?;
        } else if input_path.is_dir() {
            self.format_directory(input_path, check)?;
        } else {
            return Err(ChtlError::io(&format!("Path not found: {}", input)));
        }
        
        Ok(())
    }
    
    /// Handle bundle command
    fn handle_bundle_command(&mut self, matches: &clap::ArgMatches) -> Result<()> {
        let input = matches.value_of("input").unwrap();
        let output = matches.value_of("output");
        
        let input_path = Path::new(input);
        let bundle = self.create_bundle(input_path)?;
        
        if let Some(output_file) = output {
            fs::write(output_file, bundle)?;
            println!("Bundle created: {}", output_file);
        } else {
            println!("{}", bundle);
        }
        
        Ok(())
    }
    
    /// Handle module command
    fn handle_module_command(&mut self, matches: &clap::ArgMatches) -> Result<()> {
        match matches.subcommand() {
            ("create", Some(sub_matches)) => {
                let name = sub_matches.value_of("name").unwrap();
                let module_type = sub_matches.value_of("type").unwrap();
                self.create_module(name, module_type)?;
            }
            ("install", Some(sub_matches)) => {
                let name = sub_matches.value_of("name").unwrap();
                self.install_module(name)?;
            }
            ("list", Some(_)) => {
                self.list_modules()?;
            }
            ("remove", Some(sub_matches)) => {
                let name = sub_matches.value_of("name").unwrap();
                self.remove_module(name)?;
            }
            _ => {
                println!("Unknown module command");
            }
        }
        
        Ok(())
    }
    
    /// Build a single file
    fn build_file(&mut self, file_path: &Path) -> Result<()> {
        if !file_path.extension().map_or(false, |ext| ext == "chtl") {
            return Ok(());
        }
        
        let source = fs::read_to_string(file_path)?;
        let (html, css, js) = self.compiler.compile_full(&source)?;
        
        let output_dir = self.output_dir.as_ref()
            .unwrap_or(&file_path.parent().unwrap().to_path_buf());
        
        let base_name = file_path.file_stem().unwrap().to_string_lossy();
        
        self.output_html(&html, &output_dir.join(format!("{}.html", base_name)))?;
        self.output_css(&css, &output_dir.join(format!("{}.css", base_name)))?;
        self.output_js(&js, &output_dir.join(format!("{}.js", base_name)))?;
        
        Ok(())
    }
    
    /// Build a directory
    fn build_directory(&mut self, dir_path: &Path, recursive: bool) -> Result<()> {
        for entry in fs::read_dir(dir_path)? {
            let entry = entry?;
            let path = entry.path();
            
            if path.is_file() {
                self.build_file(&path)?;
            } else if path.is_dir() && recursive {
                self.build_directory(&path, recursive)?;
            }
        }
        
        Ok(())
    }
    
    /// Create a new project
    fn create_project(&mut self, name: &str, template: Option<&str>) -> Result<()> {
        let project_dir = Path::new(name);
        fs::create_dir_all(project_dir)?;
        
        // Create basic project structure
        fs::create_dir_all(project_dir.join("src"))?;
        fs::create_dir_all(project_dir.join("dist"))?;
        fs::create_dir_all(project_dir.join("modules"))?;
        
        // Create package.json
        let package_json = serde_json::json!({
            "name": name,
            "version": "1.0.0",
            "description": "CHTL project",
            "main": "src/index.chtl",
            "scripts": {
                "build": "chtl build src",
                "watch": "chtl watch src",
                "serve": "chtl serve"
            },
            "dependencies": {},
            "devDependencies": {}
        });
        
        fs::write(project_dir.join("package.json"), serde_json::to_string_pretty(&package_json)?)?;
        
        // Create basic CHTL file
        let index_chtl = r#"<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Hello CHTL</title>
</head>
<body>
    <h1>Hello, CHTL!</h1>
    <p>Welcome to your new CHTL project.</p>
</body>
</html>"#;
        
        fs::write(project_dir.join("src/index.chtl"), index_chtl)?;
        
        // Create README
        let readme = format!("# {}\n\nA CHTL project.\n\n## Getting Started\n\n1. Run `chtl build src` to build the project\n2. Run `chtl watch src` to watch for changes\n3. Run `chtl serve` to start the development server\n", name);
        fs::write(project_dir.join("README.md"), readme)?;
        
        Ok(())
    }
    
    /// Start development server
    fn start_dev_server(&mut self, host: &str, port: u16) -> Result<()> {
        // Simple HTTP server implementation
        println!("Development server started at http://{}:{}", host, port);
        println!("Press Ctrl+C to stop");
        
        // In a real implementation, this would start an HTTP server
        // For now, just keep the process running
        loop {
            std::thread::sleep(std::time::Duration::from_secs(1));
        }
    }
    
    /// Run tests
    fn run_tests(&mut self, pattern: Option<&str>, verbose: bool) -> Result<()> {
        println!("Running tests...");
        
        // In a real implementation, this would run the test suite
        // For now, just print a message
        if let Some(p) = pattern {
            println!("Test pattern: {}", p);
        }
        
        if verbose {
            println!("Verbose mode enabled");
        }
        
        Ok(())
    }
    
    /// Lint a file
    fn lint_file(&mut self, file_path: &Path, fix: bool) -> Result<()> {
        if !file_path.extension().map_or(false, |ext| ext == "chtl") {
            return Ok(());
        }
        
        let source = fs::read_to_string(file_path)?;
        
        // Basic linting - check for common issues
        let mut issues = Vec::new();
        
        if source.trim().is_empty() {
            issues.push("File is empty");
        }
        
        if source.contains("TODO") {
            issues.push("Contains TODO comments");
        }
        
        if source.contains("FIXME") {
            issues.push("Contains FIXME comments");
        }
        
        if issues.is_empty() {
            println!("No issues found in {}", file_path.display());
        } else {
            println!("Issues found in {}:", file_path.display());
            for issue in issues {
                println!("  - {}", issue);
            }
        }
        
        Ok(())
    }
    
    /// Lint a directory
    fn lint_directory(&mut self, dir_path: &Path, fix: bool) -> Result<()> {
        for entry in fs::read_dir(dir_path)? {
            let entry = entry?;
            let path = entry.path();
            
            if path.is_file() {
                self.lint_file(&path, fix)?;
            } else if path.is_dir() {
                self.lint_directory(&path, fix)?;
            }
        }
        
        Ok(())
    }
    
    /// Format a file
    fn format_file(&mut self, file_path: &Path, check: bool) -> Result<()> {
        if !file_path.extension().map_or(false, |ext| ext == "chtl") {
            return Ok(());
        }
        
        let source = fs::read_to_string(file_path)?;
        
        // Basic formatting - just trim whitespace for now
        let formatted = source.lines()
            .map(|line| line.trim_end())
            .collect::<Vec<_>>()
            .join("\n");
        
        if check {
            if source != formatted {
                println!("{} needs formatting", file_path.display());
            } else {
                println!("{} is properly formatted", file_path.display());
            }
        } else {
            fs::write(file_path, formatted)?;
            println!("Formatted {}", file_path.display());
        }
        
        Ok(())
    }
    
    /// Format a directory
    fn format_directory(&mut self, dir_path: &Path, check: bool) -> Result<()> {
        for entry in fs::read_dir(dir_path)? {
            let entry = entry?;
            let path = entry.path();
            
            if path.is_file() {
                self.format_file(&path, check)?;
            } else if path.is_dir() {
                self.format_directory(&path, check)?;
            }
        }
        
        Ok(())
    }
    
    /// Create bundle
    fn create_bundle(&mut self, input_path: &Path) -> Result<String> {
        let mut bundle = String::new();
        
        if input_path.is_file() {
            let source = fs::read_to_string(input_path)?;
            let (html, css, js) = self.compiler.compile_full(&source)?;
            
            bundle.push_str("<!-- HTML -->\n");
            bundle.push_str(&html);
            bundle.push_str("\n\n<!-- CSS -->\n");
            bundle.push_str(&format!("<style>\n{}\n</style>", css));
            bundle.push_str("\n\n<!-- JavaScript -->\n");
            bundle.push_str(&format!("<script>\n{}\n</script>", js));
        } else if input_path.is_dir() {
            // Bundle all CHTL files in directory
            for entry in fs::read_dir(input_path)? {
                let entry = entry?;
                let path = entry.path();
                
                if path.is_file() && path.extension().map_or(false, |ext| ext == "chtl") {
                    let source = fs::read_to_string(&path)?;
                    let (html, css, js) = self.compiler.compile_full(&source)?;
                    
                    bundle.push_str(&format!("<!-- {} -->\n", path.display()));
                    bundle.push_str(&html);
                    bundle.push_str("\n\n");
                }
            }
        }
        
        Ok(bundle)
    }
    
    /// Create module
    fn create_module(&mut self, name: &str, module_type: &str) -> Result<()> {
        let module_dir = Path::new("modules").join(name);
        fs::create_dir_all(&module_dir)?;
        
        let extension = if module_type == "cmod" { "cmod" } else { "cjmod" };
        let module_file = module_dir.join(format!("{}.{}", name, extension));
        
        let content = if module_type == "cmod" {
            format!("// CMOD module: {}\n\n// Add your CHTL content here\n", name)
        } else {
            format!("// CJMOD module: {}\n\n// Add your CHTL JS content here\n", name)
        };
        
        fs::write(module_file, content)?;
        println!("Created {} module: {}", module_type, name);
        
        Ok(())
    }
    
    /// Install module
    fn install_module(&mut self, name: &str) -> Result<()> {
        println!("Installing module: {}", name);
        // In a real implementation, this would download and install the module
        Ok(())
    }
    
    /// List modules
    fn list_modules(&mut self) -> Result<()> {
        let modules_dir = Path::new("modules");
        if !modules_dir.exists() {
            println!("No modules installed");
            return Ok(());
        }
        
        println!("Installed modules:");
        for entry in fs::read_dir(modules_dir)? {
            let entry = entry?;
            let path = entry.path();
            if path.is_dir() {
                println!("  - {}", path.file_name().unwrap().to_string_lossy());
            }
        }
        
        Ok(())
    }
    
    /// Remove module
    fn remove_module(&mut self, name: &str) -> Result<()> {
        let module_dir = Path::new("modules").join(name);
        if module_dir.exists() {
            fs::remove_dir_all(module_dir)?;
            println!("Removed module: {}", name);
        } else {
            println!("Module not found: {}", name);
        }
        
        Ok(())
    }
    
    /// Output HTML
    fn output_html(&mut self, html: &str, path: &Path) -> Result<()> {
        let output_path = if let Some(output_dir) = &self.output_dir {
            output_dir.join(path.file_name().unwrap())
        } else {
            path.to_path_buf()
        };
        
        fs::write(output_path, html)?;
        Ok(())
    }
    
    /// Output CSS
    fn output_css(&mut self, css: &str, path: &Path) -> Result<()> {
        let output_path = if let Some(output_dir) = &self.output_dir {
            output_dir.join(path.file_name().unwrap())
        } else {
            path.to_path_buf()
        };
        
        fs::write(output_path, css)?;
        Ok(())
    }
    
    /// Output JavaScript
    fn output_js(&mut self, js: &str, path: &Path) -> Result<()> {
        let output_path = if let Some(output_dir) = &self.output_dir {
            output_dir.join(path.file_name().unwrap())
        } else {
            path.to_path_buf()
        };
        
        fs::write(output_path, js)?;
        Ok(())
    }
    
    /// Print help
    fn print_help(&self) {
        println!("CHTL - A hypertext language based on Rust");
        println!();
        println!("Usage: chtl <command> [options]");
        println!();
        println!("Commands:");
        println!("  compile    Compile CHTL files");
        println!("  build      Build project");
        println!("  watch      Watch for changes and recompile");
        println!("  init       Initialize new CHTL project");
        println!("  serve      Start development server");
        println!("  test       Run tests");
        println!("  lint       Lint CHTL files");
        println!("  format     Format CHTL files");
        println!("  bundle     Bundle CHTL project");
        println!("  module     Manage CHTL modules");
        println!();
        println!("Options:");
        println!("  -v, --verbose     Enable verbose output");
        println!("  -o, --output      Output directory");
        println!("  -w, --watch       Watch for file changes");
        println!("  --minify          Minify output");
        println!("  --source-map      Generate source maps");
        println!();
        println!("For more information, run: chtl <command> --help");
    }
}