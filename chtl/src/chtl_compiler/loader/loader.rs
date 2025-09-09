use std::fs;
use std::io;
use std::path::Path;

pub struct Loader;

impl Loader {
    pub fn new() -> Self {
        Loader
    }

    pub fn load_file(&self, path: &str) -> io::Result<String> {
        fs::read_to_string(Path::new(path))
    }
}
