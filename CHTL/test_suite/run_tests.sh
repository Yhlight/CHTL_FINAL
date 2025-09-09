#!/bin/bash

# CHTL 测试套件运行脚本
# 测试所有 CHTL 语法特性和功能

echo "=== CHTL 测试套件 ==="
echo "开始运行所有测试..."

# 设置颜色
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 测试计数器
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# 测试函数
run_test() {
    local test_name="$1"
    local input_file="$2"
    local expected_output="$3"
    
    echo -e "\n${YELLOW}运行测试: $test_name${NC}"
    echo "输入文件: $input_file"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    # 运行 CHTL 编译器
    if ../build/bin/chtl "$input_file" > "output_${test_name}.html" 2>&1; then
        echo -e "${GREEN}✓ 编译成功${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
        
        # 显示输出内容
        echo "输出内容:"
        cat "output_${test_name}.html"
        echo ""
    else
        echo -e "${RED}✗ 编译失败${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        
        # 显示错误信息
        echo "错误信息:"
        cat "output_${test_name}.html"
        echo ""
    fi
}

# 运行所有测试
echo "开始运行基础语法测试..."
run_test "basic_syntax" "basic_syntax.chtl" ""

echo "开始运行模板系统测试..."
run_test "template_system" "template_system.chtl" ""

echo "开始运行自定义系统测试..."
run_test "custom_system" "custom_system.chtl" ""

echo "开始运行原始嵌入系统测试..."
run_test "origin_embedding" "origin_embedding.chtl" ""

echo "开始运行导入系统测试..."
run_test "import_system" "import_system.chtl" ""

echo "开始运行约束系统测试..."
run_test "constraint_system" "constraint_system.chtl" ""

echo "开始运行 CHTL JS 扩展语法测试..."
run_test "chtl_js_extended" "chtl_js_extended.chtl" ""

echo "开始运行模块系统测试..."
run_test "module_system" "module_system.chtl" ""

echo "开始运行 SPA 页面测试..."
run_test "spa_page" "spa_page.chtl" ""

# 测试默认结构输出
echo "开始运行默认结构输出测试..."
run_test "spa_page_default_struct" "spa_page.chtl" ""

# 运行默认结构测试
echo -e "\n${YELLOW}运行默认结构测试: spa_page_default_struct${NC}"
echo "输入文件: spa_page.chtl"

TOTAL_TESTS=$((TOTAL_TESTS + 1))

if ../build/bin/chtl --default-struct "spa_page.chtl" > "output_spa_page_default_struct.html" 2>&1; then
    echo -e "${GREEN}✓ 默认结构编译成功${NC}"
    PASSED_TESTS=$((PASSED_TESTS + 1))
    
    # 显示输出内容
    echo "输出内容:"
    cat "output_spa_page_default_struct.html"
    echo ""
else
    echo -e "${RED}✗ 默认结构编译失败${NC}"
    FAILED_TESTS=$((FAILED_TESTS + 1))
    
    # 显示错误信息
    echo "错误信息:"
    cat "output_spa_page_default_struct.html"
    echo ""
fi

# 显示测试结果
echo -e "\n=== 测试结果 ==="
echo "总测试数: $TOTAL_TESTS"
echo -e "通过: ${GREEN}$PASSED_TESTS${NC}"
echo -e "失败: ${RED}$FAILED_TESTS${NC}"

if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "\n${GREEN}🎉 所有测试通过！${NC}"
    exit 0
else
    echo -e "\n${RED}❌ 有 $FAILED_TESTS 个测试失败${NC}"
    exit 1
fi