package Scanner

import (
	"chtl/CHTL/context"
	"fmt"
	"regexp"
	"strings"
)

// CodeSegments 代码片段
type CodeSegments struct {
	CHTL string
	CSS  string
	JS   string
}

// UnifiedScanner 统一扫描器
type UnifiedScanner struct {
	ctx *context.Context
}

// NewUnifiedScanner 创建统一扫描器
func NewUnifiedScanner(ctx *context.Context) *UnifiedScanner {
	return &UnifiedScanner{
		ctx: ctx,
	}
}

// Scan 扫描代码并分离
func (s *UnifiedScanner) Scan(input string) (*CodeSegments, error) {
	segments := &CodeSegments{
		CHTL: "",
		CSS:  "",
		JS:   "",
	}

	// 使用占位符机制分离代码
	processed := s.processWithPlaceholders(input)
	
	// 分离不同类型的代码
	chtlCode, cssCode, jsCode := s.separateCode(processed)
	
	segments.CHTL = chtlCode
	segments.CSS = cssCode
	segments.JS = jsCode

	return segments, nil
}

// processWithPlaceholders 使用占位符处理代码
func (s *UnifiedScanner) processWithPlaceholders(input string) string {
	// 处理 CHTL JS 代码
	processed := s.processCHTLJSCode(input)
	
	// 处理普通 JS 代码
	processed = s.processJSCode(processed)
	
	return processed
}

// processCHTLJSCode 处理 CHTL JS 代码
func (s *UnifiedScanner) processCHTLJSCode(input string) string {
	// 匹配 CHTL JS 函数
	chtlJSRegex := regexp.MustCompile(`(listen|delegate|animate|router|util|vir|fileloader|iNeverAway|printMylove)\s*\{[^{}]*(?:\{[^{}]*\}[^{}]*)*\}`)
	
	return chtlJSRegex.ReplaceAllStringFunc(input, func(match string) string {
		// 将 CHTL JS 函数标记为特殊类型
		return "CHTL_JS_FUNCTION_PLACEHOLDER_" + s.generatePlaceholderID() + "_" + match
	})
}

// processJSCode 处理普通 JS 代码
func (s *UnifiedScanner) processJSCode(input string) string {
	// 匹配 JS 函数、对象、数组等
	jsRegex := regexp.MustCompile(`(function\s+\w+\s*\([^)]*\)\s*\{[^{}]*(?:\{[^{}]*\}[^{}]*)*\}|const\s+\w+\s*=\s*\{[^{}]*(?:\{[^{}]*\}[^{}]*)*\}|const\s+\w+\s*=\s*\[[^\[\]]*(?:\[[^\[\]]*\][^\[\]]*)*\])`)
	
	return jsRegex.ReplaceAllStringFunc(input, func(match string) string {
		// 将 JS 代码替换为占位符
		return "JS_CODE_PLACEHOLDER_" + s.generatePlaceholderID()
	})
}

// separateCode 分离代码
func (s *UnifiedScanner) separateCode(input string) (string, string, string) {
	var chtlCode, cssCode, jsCode strings.Builder
	
	lines := strings.Split(input, "\n")
	inStyleBlock := false
	inScriptBlock := false
	inCHTLJSBlock := false
	
	for _, line := range lines {
		trimmed := strings.TrimSpace(line)
		
		// 检查样式块
		if strings.Contains(trimmed, "<style>") {
			inStyleBlock = true
			continue
		}
		if inStyleBlock && strings.Contains(trimmed, "</style>") {
			inStyleBlock = false
			continue
		}
		
		// 检查脚本块
		if strings.Contains(trimmed, "<script>") {
			inScriptBlock = true
			continue
		}
		if inScriptBlock && strings.Contains(trimmed, "</script>") {
			inScriptBlock = false
			continue
		}
		
		// 检查 CHTL JS 块
		if strings.Contains(trimmed, "CHTL_JS_FUNCTION_PLACEHOLDER_") {
			inCHTLJSBlock = true
		}
		
		// 根据上下文分离代码
		if inStyleBlock {
			cssCode.WriteString(line)
			cssCode.WriteString("\n")
		} else if inScriptBlock || inCHTLJSBlock {
			jsCode.WriteString(line)
			jsCode.WriteString("\n")
		} else {
			chtlCode.WriteString(line)
			chtlCode.WriteString("\n")
		}
	}
	
	return chtlCode.String(), cssCode.String(), jsCode.String()
}

// generatePlaceholderID 生成占位符ID
func (s *UnifiedScanner) generatePlaceholderID() string {
	// 简单的ID生成，实际实现应该使用更复杂的算法
	return "ID123456"
}

// restorePlaceholders 恢复占位符
func (s *UnifiedScanner) restorePlaceholders(input string, placeholders map[string]string) string {
	result := input
	for placeholder, original := range placeholders {
		result = strings.ReplaceAll(result, placeholder, original)
	}
	return result
}

// extractCHTLJSSyntax 提取 CHTL JS 语法
func (s *UnifiedScanner) extractCHTLJSSyntax(input string) []string {
	var syntax []string
	
	// 匹配增强选择器
	selectorRegex := regexp.MustCompile(`\{\{[^}]+\}\}`)
	selectors := selectorRegex.FindAllString(input, -1)
	syntax = append(syntax, selectors...)
	
	// 匹配 CHTL JS 函数
	functionRegex := regexp.MustCompile(`(listen|delegate|animate|router|util|vir|fileloader|iNeverAway|printMylove)\s*\{`)
	functions := functionRegex.FindAllString(input, -1)
	syntax = append(syntax, functions...)
	
	return syntax
}

// validateSyntax 验证语法
func (s *UnifiedScanner) validateSyntax(input string) error {
	// 检查括号匹配
	if !s.checkBracketMatching(input) {
		return fmt.Errorf("括号不匹配")
	}
	
	// 检查引号匹配
	if !s.checkQuoteMatching(input) {
		return fmt.Errorf("引号不匹配")
	}
	
	return nil
}

// checkBracketMatching 检查括号匹配
func (s *UnifiedScanner) checkBracketMatching(input string) bool {
	stack := make([]rune, 0)
	brackets := map[rune]rune{
		'(': ')',
		'[': ']',
		'{': '}',
	}
	
	for _, char := range input {
		if char == '(' || char == '[' || char == '{' {
			stack = append(stack, char)
		} else if char == ')' || char == ']' || char == '}' {
			if len(stack) == 0 {
				return false
			}
			last := stack[len(stack)-1]
			stack = stack[:len(stack)-1]
			if brackets[last] != char {
				return false
			}
		}
	}
	
	return len(stack) == 0
}

// checkQuoteMatching 检查引号匹配
func (s *UnifiedScanner) checkQuoteMatching(input string) bool {
	singleQuotes := 0
	doubleQuotes := 0
	inSingleQuote := false
	inDoubleQuote := false
	
	for _, char := range input {
		if char == '\'' && !inDoubleQuote {
			if inSingleQuote {
				singleQuotes--
				inSingleQuote = false
			} else {
				singleQuotes++
				inSingleQuote = true
			}
		} else if char == '"' && !inSingleQuote {
			if inDoubleQuote {
				doubleQuotes--
				inDoubleQuote = false
			} else {
				doubleQuotes++
				inDoubleQuote = true
			}
		}
	}
	
	return singleQuotes == 0 && doubleQuotes == 0
}