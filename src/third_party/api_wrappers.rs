//! API wrappers
//! 
//! This module provides API wrapper functionality for CHTL,
//! including REST API wrappers, GraphQL wrappers, and custom API wrappers.

use super::*;
use std::collections::HashMap;
use std::time::Duration;

/// API wrapper manager
#[derive(Debug, Clone)]
pub struct APIWrapperManager {
    /// Registered wrappers
    wrappers: HashMap<String, APIWrapper>,
    /// Configuration
    config: APIWrapperConfig,
    /// Request client
    client: Option<reqwest::Client>,
}

/// API wrapper configuration
#[derive(Debug, Clone)]
pub struct APIWrapperConfig {
    /// Base URLs
    pub base_urls: HashMap<String, String>,
    /// Timeouts
    pub timeouts: HashMap<String, Duration>,
    /// Retry settings
    pub retry_settings: HashMap<String, RetrySettings>,
    /// Authentication
    pub authentication: HashMap<String, AuthenticationConfig>,
    /// Headers
    pub headers: HashMap<String, String>,
}

/// Retry settings
#[derive(Debug, Clone)]
pub struct RetrySettings {
    /// Maximum retries
    pub max_retries: u32,
    /// Retry delay
    pub retry_delay: Duration,
    /// Backoff multiplier
    pub backoff_multiplier: f64,
}

/// Authentication configuration
#[derive(Debug, Clone)]
pub struct AuthenticationConfig {
    /// Authentication type
    pub auth_type: AuthType,
    /// Credentials
    pub credentials: HashMap<String, String>,
}

/// Authentication type
#[derive(Debug, Clone, PartialEq)]
pub enum AuthType {
    None,
    Basic,
    Bearer,
    APIKey,
    OAuth2,
    Custom,
}

/// API request
#[derive(Debug, Clone)]
pub struct APIRequest {
    /// Request method
    pub method: RequestMethod,
    /// Request URL
    pub url: String,
    /// Request headers
    pub headers: HashMap<String, String>,
    /// Request body
    pub body: Option<String>,
    /// Query parameters
    pub query_params: HashMap<String, String>,
    /// Timeout
    pub timeout: Option<Duration>,
}

/// Request method
#[derive(Debug, Clone, PartialEq)]
pub enum RequestMethod {
    GET,
    POST,
    PUT,
    DELETE,
    PATCH,
    HEAD,
    OPTIONS,
}

/// API response
#[derive(Debug, Clone)]
pub struct APIResponse {
    /// Response status
    pub status: u16,
    /// Response headers
    pub headers: HashMap<String, String>,
    /// Response body
    pub body: String,
    /// Response time
    pub response_time: Duration,
}

/// API wrapper error
#[derive(Debug, Clone)]
pub struct APIWrapperError {
    pub message: String,
    pub wrapper_name: String,
    pub suggestion: Option<String>,
}

impl APIWrapperManager {
    /// Create a new API wrapper manager
    pub fn new() -> Self {
        Self {
            wrappers: HashMap::new(),
            config: APIWrapperConfig::default(),
            client: None,
        }
    }
    
    /// Create a new API wrapper manager with configuration
    pub fn with_config(config: APIWrapperConfig) -> Self {
        Self {
            wrappers: HashMap::new(),
            config,
            client: None,
        }
    }
    
    /// Register an API wrapper
    pub fn register_wrapper(&mut self, wrapper: APIWrapper) -> Result<(), APIWrapperError> {
        let name = wrapper.name.clone();
        
        // Validate wrapper
        self.validate_wrapper(&wrapper)?;
        
        // Register wrapper
        self.wrappers.insert(name.clone(), wrapper);
        
        Ok(())
    }
    
    /// Validate API wrapper
    fn validate_wrapper(&self, wrapper: &APIWrapper) -> Result<(), APIWrapperError> {
        // Check if wrapper name is valid
        if wrapper.name.is_empty() {
            return Err(APIWrapperError {
                message: "Wrapper name cannot be empty".to_string(),
                wrapper_name: wrapper.name.clone(),
                suggestion: Some("Provide a valid wrapper name".to_string()),
            });
        }
        
        // Check if endpoint is valid
        if wrapper.endpoint.is_empty() {
            return Err(APIWrapperError {
                message: "API endpoint cannot be empty".to_string(),
                wrapper_name: wrapper.name.clone(),
                suggestion: Some("Provide a valid API endpoint".to_string()),
            });
        }
        
        // Check if functions are valid
        for function in &wrapper.functions {
            if function.is_empty() {
                return Err(APIWrapperError {
                    message: "Function name cannot be empty".to_string(),
                    wrapper_name: wrapper.name.clone(),
                    suggestion: Some("Provide valid function names".to_string()),
                });
            }
        }
        
        Ok(())
    }
    
    /// Make an API request
    pub async fn make_request(&mut self, wrapper_name: &str, request: APIRequest) -> Result<APIResponse, APIWrapperError> {
        // Get wrapper
        let wrapper = self.wrappers.get(wrapper_name)
            .ok_or_else(|| APIWrapperError {
                message: format!("Wrapper '{}' not found", wrapper_name),
                wrapper_name: wrapper_name.to_string(),
                suggestion: Some("Register the wrapper first".to_string()),
            })?;
        
        // Initialize client if needed
        if self.client.is_none() {
            self.client = Some(reqwest::Client::new());
        }
        
        let client = self.client.as_ref().unwrap();
        
        // Build request URL
        let url = self.build_request_url(wrapper, &request)?;
        
        // Build request
        let mut req_builder = match request.method {
            RequestMethod::GET => client.get(&url),
            RequestMethod::POST => client.post(&url),
            RequestMethod::PUT => client.put(&url),
            RequestMethod::DELETE => client.delete(&url),
            RequestMethod::PATCH => client.patch(&url),
            RequestMethod::HEAD => client.head(&url),
            RequestMethod::OPTIONS => client.options(&url),
        };
        
        // Add headers
        for (key, value) in &request.headers {
            req_builder = req_builder.header(key, value);
        }
        
        // Add query parameters
        for (key, value) in &request.query_params {
            req_builder = req_builder.query(&[(key, value)]);
        }
        
        // Add body
        if let Some(body) = &request.body {
            req_builder = req_builder.body(body.clone());
        }
        
        // Set timeout
        if let Some(timeout) = request.timeout {
            req_builder = req_builder.timeout(timeout);
        }
        
        // Send request
        let start_time = std::time::Instant::now();
        let response = req_builder.send().await
            .map_err(|e| APIWrapperError {
                message: format!("Request failed: {}", e),
                wrapper_name: wrapper_name.to_string(),
                suggestion: Some("Check network connection and API endpoint".to_string()),
            })?;
        
        let response_time = start_time.elapsed();
        
        // Get response status
        let status = response.status().as_u16();
        
        // Get response headers
        let headers: HashMap<String, String> = response.headers()
            .iter()
            .map(|(key, value)| (key.to_string(), value.to_str().unwrap_or("").to_string()))
            .collect();
        
        // Get response body
        let body = response.text().await
            .map_err(|e| APIWrapperError {
                message: format!("Failed to read response body: {}", e),
                wrapper_name: wrapper_name.to_string(),
                suggestion: Some("Check response format".to_string()),
            })?;
        
        Ok(APIResponse {
            status,
            headers,
            body,
            response_time,
        })
    }
    
    /// Build request URL
    fn build_request_url(&self, wrapper: &APIWrapper, request: &APIRequest) -> Result<String, APIWrapperError> {
        let base_url = self.config.base_urls.get(&wrapper.name)
            .unwrap_or(&wrapper.endpoint);
        
        let mut url = base_url.clone();
        
        // Add request URL if it's not empty
        if !request.url.is_empty() {
            if url.ends_with('/') && request.url.starts_with('/') {
                url.pop();
            } else if !url.ends_with('/') && !request.url.starts_with('/') {
                url.push('/');
            }
            url.push_str(&request.url);
        }
        
        Ok(url)
    }
    
    /// Get wrapper
    pub fn get_wrapper(&self, name: &str) -> Option<&APIWrapper> {
        self.wrappers.get(name)
    }
    
    /// Get all wrappers
    pub fn get_wrappers(&self) -> &HashMap<String, APIWrapper> {
        &self.wrappers
    }
    
    /// Check if wrapper is registered
    pub fn is_wrapper_registered(&self, name: &str) -> bool {
        self.wrappers.contains_key(name)
    }
    
    /// Unregister wrapper
    pub fn unregister_wrapper(&mut self, name: &str) -> Result<(), APIWrapperError> {
        if self.wrappers.remove(name).is_some() {
            Ok(())
        } else {
            Err(APIWrapperError {
                message: format!("Wrapper '{}' is not registered", name),
                wrapper_name: name.to_string(),
                suggestion: Some("Check wrapper name".to_string()),
            })
        }
    }
    
    /// Clear all wrappers
    pub fn clear_wrappers(&mut self) {
        self.wrappers.clear();
    }
    
    /// Set configuration
    pub fn set_config(&mut self, config: APIWrapperConfig) {
        self.config = config;
    }
    
    /// Get configuration
    pub fn get_config(&self) -> &APIWrapperConfig {
        &self.config
    }
}

impl Default for APIWrapperManager {
    fn default() -> Self {
        Self::new()
    }
}

impl Default for APIWrapperConfig {
    fn default() -> Self {
        Self {
            base_urls: HashMap::new(),
            timeouts: HashMap::new(),
            retry_settings: HashMap::new(),
            authentication: HashMap::new(),
            headers: HashMap::new(),
        }
    }
}

impl Default for RetrySettings {
    fn default() -> Self {
        Self {
            max_retries: 3,
            retry_delay: Duration::from_secs(1),
            backoff_multiplier: 2.0,
        }
    }
}

impl Default for AuthenticationConfig {
    fn default() -> Self {
        Self {
            auth_type: AuthType::None,
            credentials: HashMap::new(),
        }
    }
}