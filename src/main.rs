//! CHTL CLI application

use chtl::cli::CliApp;
use std::env;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args: Vec<String> = env::args().collect();
    
    let mut app = CliApp::new();
    app.run(args)?;
    
    Ok(())
}
