#!/bin/bash
# CHTL Test Suite Runner (Linux Version)
# Tests all CHTL syntax features and functionality

echo "=== CHTL Test Suite ==="
echo "Starting all tests..."

# Set colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Test function
run_test() {
    local test_name="$1"
    local input_file="$2"
    local expected_output="$3"
    
    echo -e "\n${YELLOW}Running test: $test_name${NC}"
    echo "Input file: $input_file"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    # Run CHTL compiler
    if ../build/bin/chtl "$input_file" > "output_${test_name}.html" 2>&1; then
        echo -e "${GREEN}✓ Compilation successful${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
        
        # Show output content
        echo "Output content:"
        cat "output_${test_name}.html"
        echo ""
    else
        echo -e "${RED}✗ Compilation failed${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        
        # Show error information
        echo "Error information:"
        cat "output_${test_name}.html"
        echo ""
    fi
}

# Run all tests
echo "Starting basic syntax tests..."
run_test "basic_syntax" "basic_syntax.chtl" ""

echo "Starting template system tests..."
run_test "template_system" "template_system.chtl" ""

echo "Starting custom system tests..."
run_test "custom_system" "custom_system.chtl" ""

echo "Starting origin embedding tests..."
run_test "origin_embedding" "origin_embedding.chtl" ""

echo "Starting import system tests..."
run_test "import_system" "import_system.chtl" ""

echo "Starting constraint system tests..."
run_test "constraint_system" "constraint_system.chtl" ""

echo "Starting configuration system tests..."
run_test "config_system" "config_system.chtl" ""

echo "Starting namespace system tests..."
run_test "namespace_system" "namespace_system.chtl" ""

echo "Starting CHTL JS extended syntax tests..."
run_test "chtl_js_extended" "chtl_js_extended.chtl" ""

echo "Starting module system tests..."
run_test "module_system" "module_system.chtl" ""

echo "Starting SPA page tests..."
run_test "spa_page" "spa_page.chtl" ""

echo "Starting complete features tests..."
run_test "complete_features" "complete_features.chtl" ""

echo "Starting property arithmetic tests..."
run_test "property_arithmetic" "property_arithmetic.chtl" ""

echo "Starting property reference tests..."
run_test "property_reference" "property_reference.chtl" ""

echo "Starting property conditionals tests..."
run_test "property_conditionals" "property_conditionals.chtl" ""

echo "Starting dynamic property conditionals tests..."
run_test "dynamic_property_conditionals" "dynamic_property_conditionals.chtl" ""

echo "Starting comprehensive property expressions tests..."
run_test "comprehensive_property_expressions" "comprehensive_property_expressions.chtl" ""

echo "Starting global style restrictions tests..."
run_test "global_style_restrictions" "global_style_restrictions.chtl" ""

echo "Starting template system tests..."
run_test "template_system" "template_system_test.chtl" ""

echo "Starting custom system tests..."
run_test "custom_system" "custom_system_test.chtl" ""

echo "Starting origin embedding tests..."
run_test "origin_embedding" "origin_embedding_test.chtl" ""

# Test default structure output
echo -e "\n${YELLOW}Running default structure test: spa_page_default_struct${NC}"
echo "Input file: spa_page.chtl"

TOTAL_TESTS=$((TOTAL_TESTS + 1))

if ../build/bin/chtl --default-struct "spa_page.chtl" > "output_spa_page_default_struct.html" 2>&1; then
    echo -e "${GREEN}✓ Default structure compilation successful${NC}"
    PASSED_TESTS=$((PASSED_TESTS + 1))
    
    # Show output content
    echo "Output content:"
    cat "output_spa_page_default_struct.html"
    echo ""
else
    echo -e "${RED}✗ Default structure compilation failed${NC}"
    FAILED_TESTS=$((FAILED_TESTS + 1))
    
    # Show error information
    echo "Error information:"
    cat "output_spa_page_default_struct.html"
    echo ""
fi

# Show test results
echo -e "\n=== Test Results ==="
echo "Total tests: $TOTAL_TESTS"
echo -e "Passed: ${GREEN}$PASSED_TESTS${NC}"
echo -e "Failed: ${RED}$FAILED_TESTS${NC}"

if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "\n${GREEN}🎉 All tests passed!${NC}"
    exit 0
else
    echo -e "\n${RED}❌ $FAILED_TESTS tests failed${NC}"
    exit 1
fi