import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';

export class CHTLLanguageServer {
    private diagnosticCollection: vscode.DiagnosticCollection;
    private watchers: Map<string, vscode.FileSystemWatcher> = new Map();

    constructor() {
        this.diagnosticCollection = vscode.languages.createDiagnosticCollection('chtl');
    }

    public activate(context: vscode.ExtensionContext) {
        // 注册文档变化事件
        const changeEvent = vscode.workspace.onDidChangeTextDocument((event) => {
            this.validateDocument(event.document);
        });

        // 注册文档打开事件
        const openEvent = vscode.workspace.onDidOpenTextDocument((document) => {
            if (this.isCHTLFile(document)) {
                this.validateDocument(document);
            }
        });

        // 注册文档保存事件
        const saveEvent = vscode.workspace.onDidSaveTextDocument((document) => {
            if (this.isCHTLFile(document)) {
                this.validateDocument(document);
            }
        });

        context.subscriptions.push(changeEvent, openEvent, saveEvent, this.diagnosticCollection);

        // 初始化现有文档
        vscode.workspace.textDocuments.forEach(document => {
            if (this.isCHTLFile(document)) {
                this.validateDocument(document);
            }
        });
    }

    public deactivate() {
        this.diagnosticCollection.clear();
        this.watchers.forEach(watcher => watcher.dispose());
        this.watchers.clear();
    }

    private isCHTLFile(document: vscode.TextDocument): boolean {
        const ext = path.extname(document.fileName).toLowerCase();
        return ext === '.chtl' || ext === '.cmod' || ext === '.cjjs' || ext === '.cjmod';
    }

    private validateDocument(document: vscode.TextDocument) {
        if (!this.isCHTLFile(document)) {
            return;
        }

        const diagnostics: vscode.Diagnostic[] = [];
        const text = document.getText();
        const lines = text.split('\n');

        for (let i = 0; i < lines.length; i++) {
            const line = lines[i];
            const lineNumber = i;
            const lineText = line.trim();

            // 检查语法错误
            this.checkSyntaxErrors(lineText, lineNumber, diagnostics, document);

            // 检查括号匹配
            this.checkBracketMatching(lineText, lineNumber, diagnostics, document);

            // 检查属性语法
            this.checkAttributeSyntax(lineText, lineNumber, diagnostics, document);
        }

        this.diagnosticCollection.set(document.uri, diagnostics);
    }

    private checkSyntaxErrors(line: string, lineNumber: number, diagnostics: vscode.Diagnostic[], document: vscode.TextDocument) {
        // 检查未闭合的注释
        if (line.includes('/*') && !line.includes('*/')) {
            const diagnostic = new vscode.Diagnostic(
                new vscode.Range(lineNumber, line.indexOf('/*'), lineNumber, line.length),
                'Unclosed comment block',
                vscode.DiagnosticSeverity.Warning
            );
            diagnostics.push(diagnostic);
        }

        // 检查未闭合的字符串
        const singleQuoteCount = (line.match(/'/g) || []).length;
        const doubleQuoteCount = (line.match(/"/g) || []).length;
        
        if (singleQuoteCount % 2 !== 0) {
            const diagnostic = new vscode.Diagnostic(
                new vscode.Range(lineNumber, 0, lineNumber, line.length),
                'Unclosed single quote string',
                vscode.DiagnosticSeverity.Error
            );
            diagnostics.push(diagnostic);
        }

        if (doubleQuoteCount % 2 !== 0) {
            const diagnostic = new vscode.Diagnostic(
                new vscode.Range(lineNumber, 0, lineNumber, line.length),
                'Unclosed double quote string',
                vscode.DiagnosticSeverity.Error
            );
            diagnostics.push(diagnostic);
        }
    }

    private checkBracketMatching(line: string, lineNumber: number, diagnostics: vscode.Diagnostic[], document: vscode.TextDocument) {
        const openBrackets = ['{', '[', '('];
        const closeBrackets = ['}', ']', ')'];
        const bracketPairs = [['{', '}'], ['[', ']'], ['(', ')']];

        for (let i = 0; i < bracketPairs.length; i++) {
            const [open, close] = bracketPairs[i];
            const openCount = (line.match(new RegExp('\\' + open, 'g')) || []).length;
            const closeCount = (line.match(new RegExp('\\' + close, 'g')) || []).length;

            if (openCount !== closeCount) {
                const diagnostic = new vscode.Diagnostic(
                    new vscode.Range(lineNumber, 0, lineNumber, line.length),
                    `Mismatched brackets: ${open} and ${close}`,
                    vscode.DiagnosticSeverity.Error
                );
                diagnostics.push(diagnostic);
            }
        }
    }

    private checkAttributeSyntax(line: string, lineNumber: number, diagnostics: vscode.Diagnostic[], document: vscode.TextDocument) {
        // 检查属性语法 : 或 =
        if (line.includes(':') || line.includes('=')) {
            const colonIndex = line.indexOf(':');
            const equalIndex = line.indexOf('=');
            
            if (colonIndex !== -1 && equalIndex !== -1) {
                const diagnostic = new vscode.Diagnostic(
                    new vscode.Range(lineNumber, Math.min(colonIndex, equalIndex), lineNumber, Math.max(colonIndex, equalIndex) + 1),
                    'Use either : or = for attribute assignment, not both',
                    vscode.DiagnosticSeverity.Warning
                );
                diagnostics.push(diagnostic);
            }
        }
    }

    public provideCompletionItems(document: vscode.TextDocument, position: vscode.Position): vscode.CompletionItem[] {
        const line = document.lineAt(position).text;
        const textBeforeCursor = line.substring(0, position.character);

        const completions: vscode.CompletionItem[] = [];

        // HTML标签补全
        const htmlTags = ['html', 'head', 'body', 'div', 'span', 'p', 'h1', 'h2', 'h3', 'h4', 'h5', 'h6', 'a', 'img', 'ul', 'ol', 'li', 'table', 'tr', 'td', 'th', 'form', 'input', 'button', 'section', 'article', 'nav', 'aside', 'header', 'footer', 'main'];
        
        htmlTags.forEach(tag => {
            const completion = new vscode.CompletionItem(tag, vscode.CompletionItemKind.Class);
            completion.insertText = `${tag} {\n    $1\n}`;
            completion.documentation = `HTML ${tag} element`;
            completions.push(completion);
        });

        // CHTL关键字补全
        const chtlKeywords = ['text', 'style', 'script', 'template', 'custom', 'origin', 'import', 'namespace', 'configuration', 'info', 'export', 'use'];
        
        chtlKeywords.forEach(keyword => {
            const completion = new vscode.CompletionItem(keyword, vscode.CompletionItemKind.Keyword);
            completion.insertText = keyword;
            completion.documentation = `CHTL ${keyword} keyword`;
            completions.push(completion);
        });

        // 属性补全
        if (textBeforeCursor.includes(':')) {
            const cssProperties = ['color', 'background-color', 'font-size', 'font-family', 'width', 'height', 'margin', 'padding', 'border', 'display', 'position', 'top', 'left', 'right', 'bottom'];
            
            cssProperties.forEach(prop => {
                const completion = new vscode.CompletionItem(prop, vscode.CompletionItemKind.Property);
                completion.insertText = prop;
                completion.documentation = `CSS ${prop} property`;
                completions.push(completion);
            });
        }

        return completions;
    }

    public provideHover(document: vscode.TextDocument, position: vscode.Position): vscode.Hover | undefined {
        const line = document.lineAt(position).text;
        const word = document.getWordRangeAtPosition(position);
        
        if (!word) {
            return undefined;
        }

        const wordText = document.getText(word);
        
        // HTML标签悬停提示
        const htmlTags: { [key: string]: string } = {
            'html': 'Root element of an HTML document',
            'head': 'Container for metadata about the document',
            'body': 'Main content of the document',
            'div': 'Generic container element',
            'span': 'Inline generic container',
            'p': 'Paragraph element',
            'h1': 'Heading level 1',
            'h2': 'Heading level 2',
            'h3': 'Heading level 3',
            'h4': 'Heading level 4',
            'h5': 'Heading level 5',
            'h6': 'Heading level 6',
            'a': 'Anchor element for links',
            'img': 'Image element',
            'ul': 'Unordered list',
            'ol': 'Ordered list',
            'li': 'List item',
            'table': 'Table element',
            'tr': 'Table row',
            'td': 'Table cell',
            'th': 'Table header cell',
            'form': 'Form element',
            'input': 'Input element',
            'button': 'Button element'
        };

        if (htmlTags[wordText]) {
            return new vscode.Hover(htmlTags[wordText]);
        }

        // CHTL关键字悬停提示
        const chtlKeywords: { [key: string]: string } = {
            'text': 'Text content element',
            'style': 'CSS styling block',
            'script': 'JavaScript code block',
            'template': 'Template declaration',
            'custom': 'Custom element declaration',
            'origin': 'Original content embedding',
            'import': 'Module import statement',
            'namespace': 'Namespace declaration',
            'configuration': 'Configuration block',
            'info': 'Module information block',
            'export': 'Module export block',
            'use': 'Use statement for configuration'
        };

        if (chtlKeywords[wordText]) {
            return new vscode.Hover(chtlKeywords[wordText]);
        }

        return undefined;
    }

    public provideDefinition(document: vscode.TextDocument, position: vscode.Position): vscode.Definition | undefined {
        // 这里可以实现跳转到定义的功能
        // 例如跳转到模块定义、函数定义等
        return undefined;
    }

    public provideReferences(document: vscode.TextDocument, position: vscode.Position): vscode.Location[] {
        // 这里可以实现查找引用的功能
        // 例如查找某个模块或函数的所有引用
        return [];
    }
}