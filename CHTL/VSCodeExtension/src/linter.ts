import * as vscode from 'vscode';
import { CHTLParser } from './parser';

export class CHTLLinter implements vscode.DiagnosticProvider {
    private parser: CHTLParser;
    private config: vscode.WorkspaceConfiguration;
    private diagnostics: vscode.DiagnosticCollection;

    constructor() {
        this.parser = new CHTLParser();
        this.config = vscode.workspace.getConfiguration('chtl');
        this.diagnostics = vscode.languages.createDiagnosticCollection('chtl');
    }

    public reloadConfiguration(): void {
        this.config = vscode.workspace.getConfiguration('chtl');
    }

    public async provideDiagnostics(
        document: vscode.TextDocument,
        token: vscode.CancellationToken
    ): Promise<vscode.Diagnostic[]> {
        if (!this.config.get('linting.enabled', true)) {
            return [];
        }

        const diagnostics: vscode.Diagnostic[] = [];
        const text = document.getText();
        const lines = text.split('\n');

        for (let i = 0; i < lines.length; i++) {
            const line = lines[i];
            const lineNumber = i + 1;

            // 检查语法错误
            const syntaxErrors = this.checkSyntax(line, lineNumber);
            diagnostics.push(...syntaxErrors);

            // 检查缩进错误
            const indentErrors = this.checkIndentation(line, lineNumber, i);
            diagnostics.push(...indentErrors);

            // 检查未闭合的括号
            const bracketErrors = this.checkBrackets(line, lineNumber);
            diagnostics.push(...bracketErrors);

            // 检查未闭合的字符串
            const stringErrors = this.checkStrings(line, lineNumber);
            diagnostics.push(...stringErrors);
        }

        // 检查整体结构
        const structureErrors = this.checkStructure(text);
        diagnostics.push(...structureErrors);

        return diagnostics;
    }

    public async checkDocument(uri: vscode.Uri): Promise<void> {
        const document = await vscode.workspace.openTextDocument(uri);
        const diagnostics = await this.provideDiagnostics(document, new vscode.CancellationTokenSource().token);
        this.diagnostics.set(uri, diagnostics);
    }

    private checkSyntax(line: string, lineNumber: number): vscode.Diagnostic[] {
        const diagnostics: vscode.Diagnostic[] = [];
        const trimmedLine = line.trim();

        // 检查无效的关键字
        const invalidKeywords = this.findInvalidKeywords(trimmedLine);
        for (const keyword of invalidKeywords) {
            const position = line.indexOf(keyword);
            const range = new vscode.Range(
                lineNumber - 1,
                position,
                lineNumber - 1,
                position + keyword.length
            );
            diagnostics.push({
                range,
                message: `Invalid keyword: ${keyword}`,
                severity: vscode.DiagnosticSeverity.Error,
                source: 'chtl-linter'
            });
        }

        // 检查无效的运算符
        const invalidOperators = this.findInvalidOperators(trimmedLine);
        for (const operator of invalidOperators) {
            const position = line.indexOf(operator);
            const range = new vscode.Range(
                lineNumber - 1,
                position,
                lineNumber - 1,
                position + operator.length
            );
            diagnostics.push({
                range,
                message: `Invalid operator: ${operator}`,
                severity: vscode.DiagnosticSeverity.Error,
                source: 'chtl-linter'
            });
        }

        return diagnostics;
    }

    private checkIndentation(line: string, lineNumber: number, lineIndex: number): vscode.Diagnostic[] {
        const diagnostics: vscode.Diagnostic[] = [];
        const trimmedLine = line.trim();

        if (trimmedLine === '') {
            return diagnostics;
        }

        // 计算期望的缩进
        const expectedIndent = this.calculateExpectedIndent(lineIndex);
        const actualIndent = line.length - line.trimStart().length;

        if (actualIndent !== expectedIndent) {
            const range = new vscode.Range(
                lineNumber - 1,
                0,
                lineNumber - 1,
                actualIndent
            );
            diagnostics.push({
                range,
                message: `Expected indentation of ${expectedIndent} spaces, found ${actualIndent}`,
                severity: vscode.DiagnosticSeverity.Warning,
                source: 'chtl-linter'
            });
        }

        return diagnostics;
    }

    private checkBrackets(line: string, lineNumber: number): vscode.Diagnostic[] {
        const diagnostics: vscode.Diagnostic[] = [];
        const trimmedLine = line.trim();

        // 检查未闭合的括号
        const openBrackets = ['{', '[', '('];
        const closeBrackets = ['}', ']', ')'];

        for (let i = 0; i < trimmedLine.length; i++) {
            const char = trimmedLine[i];
            
            if (openBrackets.includes(char)) {
                // 查找对应的闭合括号
                const bracketIndex = openBrackets.indexOf(char);
                const closeBracket = closeBrackets[bracketIndex];
                
                if (!this.findMatchingBracket(trimmedLine, i, char, closeBracket)) {
                    const range = new vscode.Range(
                        lineNumber - 1,
                        i,
                        lineNumber - 1,
                        i + 1
                    );
                    diagnostics.push({
                        range,
                        message: `Unclosed bracket: ${char}`,
                        severity: vscode.DiagnosticSeverity.Error,
                        source: 'chtl-linter'
                    });
                }
            }
        }

        return diagnostics;
    }

    private checkStrings(line: string, lineNumber: number): vscode.Diagnostic[] {
        const diagnostics: vscode.Diagnostic[] = [];
        const trimmedLine = line.trim();

        // 检查未闭合的字符串
        const stringDelimiters = ['"', "'", '`'];
        let inString = false;
        let stringChar = '';

        for (let i = 0; i < trimmedLine.length; i++) {
            const char = trimmedLine[i];
            
            if (!inString && stringDelimiters.includes(char)) {
                inString = true;
                stringChar = char;
            } else if (inString && char === stringChar) {
                inString = false;
                stringChar = '';
            }
        }

        if (inString) {
            const range = new vscode.Range(
                lineNumber - 1,
                trimmedLine.lastIndexOf(stringChar),
                lineNumber - 1,
                trimmedLine.length
            );
            diagnostics.push({
                range,
                message: `Unclosed string: ${stringChar}`,
                severity: vscode.DiagnosticSeverity.Error,
                source: 'chtl-linter'
            });
        }

        return diagnostics;
    }

    private checkStructure(text: string): vscode.Diagnostic[] {
        const diagnostics: vscode.Diagnostic[] = [];

        // 检查是否有根HTML元素
        if (!text.includes('html')) {
            diagnostics.push({
                range: new vscode.Range(0, 0, 0, 0),
                message: 'CHTL document should have a root html element',
                severity: vscode.DiagnosticSeverity.Warning,
                source: 'chtl-linter'
            });
        }

        // 检查模板定义
        const templateRegex = /\[Template\]\s+@\w+\s+(\w+)/g;
        const templates = new Set<string>();
        let match;
        while ((match = templateRegex.exec(text)) !== null) {
            if (templates.has(match[1])) {
                diagnostics.push({
                    range: new vscode.Range(0, 0, 0, 0),
                    message: `Duplicate template definition: ${match[1]}`,
                    severity: vscode.DiagnosticSeverity.Error,
                    source: 'chtl-linter'
                });
            }
            templates.add(match[1]);
        }

        return diagnostics;
    }

    private findInvalidKeywords(line: string): string[] {
        const validKeywords = [
            'Template', 'Custom', 'Origin', 'Import', 'Namespace', 'Configuration',
            'Info', 'Export', 'text', 'style', 'script', 'use', 'except',
            'delete', 'insert', 'inherit', 'from', 'as', 'after', 'before',
            'replace', 'at', 'top', 'bottom', 'fileloader', 'listen', 'delegate',
            'animate', 'vir', 'router', 'util', 'iNeverAway'
        ];

        const words = line.split(/\s+/);
        const invalidKeywords: string[] = [];

        for (const word of words) {
            if (word.match(/^[A-Z][a-zA-Z]*$/) && !validKeywords.includes(word)) {
                invalidKeywords.push(word);
            }
        }

        return invalidKeywords;
    }

    private findInvalidOperators(line: string): string[] {
        const validOperators = ['->', '++', '--', '+', '-', '*', '/', '%', '==', '!=', '<=', '>=', '<', '>', '&&', '||', '!', '?', ':', '='];
        const invalidOperators: string[] = [];

        // 简化的运算符检查
        const operatorRegex = /[+\-*/%=<>!&|?:]+/g;
        let match;
        while ((match = operatorRegex.exec(line)) !== null) {
            if (!validOperators.includes(match[0])) {
                invalidOperators.push(match[0]);
            }
        }

        return invalidOperators;
    }

    private calculateExpectedIndent(lineIndex: number): number {
        // 简化的缩进计算
        return lineIndex * 2;
    }

    private findMatchingBracket(line: string, startIndex: number, openBracket: string, closeBracket: string): boolean {
        let count = 0;
        for (let i = startIndex; i < line.length; i++) {
            if (line[i] === openBracket) {
                count++;
            } else if (line[i] === closeBracket) {
                count--;
                if (count === 0) {
                    return true;
                }
            }
        }
        return false;
    }
}