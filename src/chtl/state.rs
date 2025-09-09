//! CHTL compilation state management
//! 
//! This module provides state management for CHTL compilation,
//! including tracking compilation progress and managing state transitions.

use std::collections::HashMap;
use crate::chtl::context::*;

/// CHTL compilation state manager
#[derive(Debug, Clone)]
pub struct CHTLState {
    /// Current compilation state
    current_state: CompilationState,
    /// State history
    state_history: Vec<CompilationState>,
    /// State transitions
    transitions: HashMap<CompilationState, Vec<CompilationState>>,
    /// State data
    state_data: HashMap<String, StateData>,
}

/// State data container
#[derive(Debug, Clone)]
pub enum StateData {
    String(String),
    Number(f64),
    Boolean(bool),
    Array(Vec<StateData>),
    Object(HashMap<String, StateData>),
}

/// State transition error
#[derive(Debug, Clone)]
pub struct StateTransitionError {
    pub message: String,
    pub current_state: CompilationState,
    pub target_state: CompilationState,
    pub suggestion: Option<String>,
}

impl CHTLState {
    /// Create a new CHTL state manager
    pub fn new() -> Self {
        let mut state = Self {
            current_state: CompilationState::Initial,
            state_history: Vec::new(),
            transitions: HashMap::new(),
            state_data: HashMap::new(),
        };
        
        // Initialize valid state transitions
        state.initialize_transitions();
        
        state
    }
    
    /// Initialize valid state transitions
    fn initialize_transitions(&mut self) {
        self.transitions.insert(CompilationState::Initial, vec![
            CompilationState::Lexing,
            CompilationState::Failed,
        ]);
        
        self.transitions.insert(CompilationState::Lexing, vec![
            CompilationState::Parsing,
            CompilationState::Failed,
        ]);
        
        self.transitions.insert(CompilationState::Parsing, vec![
            CompilationState::SemanticAnalysis,
            CompilationState::Failed,
        ]);
        
        self.transitions.insert(CompilationState::SemanticAnalysis, vec![
            CompilationState::Generating,
            CompilationState::Failed,
        ]);
        
        self.transitions.insert(CompilationState::Generating, vec![
            CompilationState::Completed,
            CompilationState::Failed,
        ]);
        
        self.transitions.insert(CompilationState::Completed, vec![]);
        self.transitions.insert(CompilationState::Failed, vec![]);
    }
    
    /// Get current state
    pub fn current_state(&self) -> &CompilationState {
        &self.current_state
    }
    
    /// Get state history
    pub fn state_history(&self) -> &[CompilationState] {
        &self.state_history
    }
    
    /// Transition to a new state
    pub fn transition_to(&mut self, target_state: CompilationState) -> Result<(), StateTransitionError> {
        // Check if transition is valid
        if let Some(valid_transitions) = self.transitions.get(&self.current_state) {
            if !valid_transitions.contains(&target_state) {
                return Err(StateTransitionError {
                    message: format!("Invalid state transition from {:?} to {:?}", self.current_state, target_state),
                    current_state: self.current_state.clone(),
                    target_state: target_state.clone(),
                    suggestion: Some(format!("Valid transitions from {:?}: {:?}", self.current_state, valid_transitions)),
                });
            }
        }
        
        // Record state history
        self.state_history.push(self.current_state.clone());
        
        // Transition to new state
        self.current_state = target_state;
        
        Ok(())
    }
    
    /// Force transition to a new state (for error recovery)
    pub fn force_transition_to(&mut self, target_state: CompilationState) {
        self.state_history.push(self.current_state.clone());
        self.current_state = target_state;
    }
    
    /// Check if in a specific state
    pub fn is_in_state(&self, state: &CompilationState) -> bool {
        self.current_state == *state
    }
    
    /// Check if compilation is complete
    pub fn is_complete(&self) -> bool {
        self.current_state == CompilationState::Completed
    }
    
    /// Check if compilation failed
    pub fn has_failed(&self) -> bool {
        self.current_state == CompilationState::Failed
    }
    
    /// Check if compilation is in progress
    pub fn is_in_progress(&self) -> bool {
        matches!(self.current_state, 
            CompilationState::Lexing | 
            CompilationState::Parsing | 
            CompilationState::SemanticAnalysis | 
            CompilationState::Generating
        )
    }
    
    /// Set state data
    pub fn set_data(&mut self, key: String, data: StateData) {
        self.state_data.insert(key, data);
    }
    
    /// Get state data
    pub fn get_data(&self, key: &str) -> Option<&StateData> {
        self.state_data.get(key)
    }
    
    /// Get mutable state data
    pub fn get_data_mut(&mut self, key: &str) -> Option<&mut StateData> {
        self.state_data.get_mut(key)
    }
    
    /// Remove state data
    pub fn remove_data(&mut self, key: &str) -> Option<StateData> {
        self.state_data.remove(key)
    }
    
    /// Clear all state data
    pub fn clear_data(&mut self) {
        self.state_data.clear();
    }
    
    /// Reset state to initial
    pub fn reset(&mut self) {
        self.current_state = CompilationState::Initial;
        self.state_history.clear();
        self.state_data.clear();
    }
    
    /// Get state summary
    pub fn get_summary(&self) -> StateSummary {
        StateSummary {
            current_state: self.current_state.clone(),
            state_count: self.state_history.len(),
            data_count: self.state_data.len(),
            is_complete: self.is_complete(),
            has_failed: self.has_failed(),
            is_in_progress: self.is_in_progress(),
        }
    }
}

/// State summary
#[derive(Debug, Clone)]
pub struct StateSummary {
    pub current_state: CompilationState,
    pub state_count: usize,
    pub data_count: usize,
    pub is_complete: bool,
    pub has_failed: bool,
    pub is_in_progress: bool,
}

impl Default for CHTLState {
    fn default() -> Self {
        Self::new()
    }
}

/// State data accessor macros
#[macro_export]
macro_rules! get_state_string {
    ($state:expr, $key:expr) => {
        $state.get_data($key).and_then(|data| match data {
            StateData::String(s) => Some(s),
            _ => None,
        })
    };
}

#[macro_export]
macro_rules! get_state_number {
    ($state:expr, $key:expr) => {
        $state.get_data($key).and_then(|data| match data {
            StateData::Number(n) => Some(*n),
            _ => None,
        })
    };
}

#[macro_export]
macro_rules! get_state_boolean {
    ($state:expr, $key:expr) => {
        $state.get_data($key).and_then(|data| match data {
            StateData::Boolean(b) => Some(*b),
            _ => None,
        })
    };
}

#[macro_export]
macro_rules! get_state_array {
    ($state:expr, $key:expr) => {
        $state.get_data($key).and_then(|data| match data {
            StateData::Array(a) => Some(a),
            _ => None,
        })
    };
}

#[macro_export]
macro_rules! get_state_object {
    ($state:expr, $key:expr) => {
        $state.get_data($key).and_then(|data| match data {
            StateData::Object(o) => Some(o),
            _ => None,
        })
    };
}