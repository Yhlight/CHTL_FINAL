import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';
import { exec } from 'child_process';
import { promisify } from 'util';

const execAsync = promisify(exec);

export function activate(context: vscode.ExtensionContext) {
    console.log('CHTL Language Support extension is now active!');

    // 注册命令
    const commands = [
        vscode.commands.registerCommand('chtl.compile', compileCHTL),
        vscode.commands.registerCommand('chtl.compileJS', compileCHTLJS),
        vscode.commands.registerCommand('chtl.preview', previewCHTL),
        vscode.commands.registerCommand('chtl.build', buildCHTLProject),
        vscode.commands.registerCommand('chtl.watch', watchCHTLFiles),
        vscode.commands.registerCommand('chtl.clean', cleanCHTLBuild),
        vscode.commands.registerCommand('chtl.init', initCHTLProject)
    ];

    // 注册提供者
    const providers = [
        vscode.languages.registerCompletionItemProvider(
            ['chtl', 'chtljs'],
            new CHTLCompletionItemProvider(),
            '[', '@', '$', '{', '.', '#'
        ),
        vscode.languages.registerHoverProvider(
            ['chtl', 'chtljs'],
            new CHTLHoverProvider()
        ),
        vscode.languages.registerDocumentSymbolProvider(
            ['chtl', 'chtljs'],
            new CHTLDocumentSymbolProvider()
        ),
        vscode.languages.registerDefinitionProvider(
            ['chtl', 'chtljs'],
            new CHTLDefinitionProvider()
        ),
        vscode.languages.registerDiagnosticCollection('chtl'),
        vscode.languages.registerCodeActionsProvider(
            ['chtl', 'chtljs'],
            new CHTLCodeActionsProvider(),
            { providedCodeActionKinds: [vscode.CodeActionKind.QuickFix] }
        )
    ];

    // 注册事件监听器
    const eventListeners = [
        vscode.workspace.onDidSaveTextDocument(onDocumentSaved),
        vscode.workspace.onDidChangeTextDocument(onDocumentChanged),
        vscode.workspace.onDidOpenTextDocument(onDocumentOpened)
    ];

    // 注册所有提供者
    context.subscriptions.push(...commands, ...providers, ...eventListeners);

    // 初始化CHTL Explorer
    const chtlExplorer = new CHTLExplorer(context);
    vscode.window.registerTreeDataProvider('chtlExplorer', chtlExplorer);

    // 自动编译设置
    const config = vscode.workspace.getConfiguration('chtl');
    if (config.get('autoCompile')) {
        vscode.workspace.textDocuments.forEach(doc => {
            if (doc.languageId === 'chtl' || doc.languageId === 'chtljs') {
                compileDocument(doc);
            }
        });
    }
}

export function deactivate() {
    console.log('CHTL Language Support extension is now deactivated!');
}

// 编译CHTL文件
async function compileCHTL() {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'chtl') {
        vscode.window.showWarningMessage('Please open a CHTL file to compile.');
        return;
    }

    await compileDocument(editor.document);
}

// 编译CHTL JS文件
async function compileCHTLJS() {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'chtljs') {
        vscode.window.showWarningMessage('Please open a CHTL JS file to compile.');
        return;
    }

    await compileDocument(editor.document);
}

// 预览CHTL文件
async function previewCHTL() {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'chtl') {
        vscode.window.showWarningMessage('Please open a CHTL file to preview.');
        return;
    }

    await compileDocument(editor.document);
    
    const outputPath = getOutputPath(editor.document.uri.fsPath);
    if (fs.existsSync(outputPath)) {
        const doc = await vscode.workspace.openTextDocument(outputPath);
        await vscode.window.showTextDocument(doc, vscode.ViewColumn.Beside);
    }
}

// 构建CHTL项目
async function buildCHTLProject() {
    const workspaceFolder = vscode.workspace.workspaceFolders?.[0];
    if (!workspaceFolder) {
        vscode.window.showWarningMessage('Please open a workspace folder.');
        return;
    }

    try {
        vscode.window.showInformationMessage('Building CHTL project...');
        
        const config = vscode.workspace.getConfiguration('chtl');
        const compilerPath = config.get('compilerPath', './build/working_chtl_compiler');
        
        const { stdout, stderr } = await execAsync(`${compilerPath} --build`, {
            cwd: workspaceFolder.uri.fsPath
        });

        if (stderr) {
            vscode.window.showErrorMessage(`Build failed: ${stderr}`);
        } else {
            vscode.window.showInformationMessage('CHTL project built successfully!');
        }
    } catch (error) {
        vscode.window.showErrorMessage(`Build failed: ${error}`);
    }
}

// 监视CHTL文件
async function watchCHTLFiles() {
    const workspaceFolder = vscode.workspace.workspaceFolders?.[0];
    if (!workspaceFolder) {
        vscode.window.showWarningMessage('Please open a workspace folder.');
        return;
    }

    try {
        vscode.window.showInformationMessage('Starting CHTL watch mode...');
        
        const config = vscode.workspace.getConfiguration('chtl');
        const compilerPath = config.get('compilerPath', './build/working_chtl_compiler');
        
        const { stdout, stderr } = await execAsync(`${compilerPath} --watch`, {
            cwd: workspaceFolder.uri.fsPath
        });

        if (stderr) {
            vscode.window.showErrorMessage(`Watch mode failed: ${stderr}`);
        } else {
            vscode.window.showInformationMessage('CHTL watch mode started!');
        }
    } catch (error) {
        vscode.window.showErrorMessage(`Watch mode failed: ${error}`);
    }
}

// 清理CHTL构建
async function cleanCHTLBuild() {
    const workspaceFolder = vscode.workspace.workspaceFolders?.[0];
    if (!workspaceFolder) {
        vscode.window.showWarningMessage('Please open a workspace folder.');
        return;
    }

    try {
        vscode.window.showInformationMessage('Cleaning CHTL build...');
        
        const config = vscode.workspace.getConfiguration('chtl');
        const compilerPath = config.get('compilerPath', './build/working_chtl_compiler');
        
        const { stdout, stderr } = await execAsync(`${compilerPath} --clean`, {
            cwd: workspaceFolder.uri.fsPath
        });

        if (stderr) {
            vscode.window.showErrorMessage(`Clean failed: ${stderr}`);
        } else {
            vscode.window.showInformationMessage('CHTL build cleaned successfully!');
        }
    } catch (error) {
        vscode.window.showErrorMessage(`Clean failed: ${error}`);
    }
}

// 初始化CHTL项目
async function initCHTLProject() {
    const workspaceFolder = vscode.workspace.workspaceFolders?.[0];
    if (!workspaceFolder) {
        vscode.window.showWarningMessage('Please open a workspace folder.');
        return;
    }

    try {
        vscode.window.showInformationMessage('Initializing CHTL project...');
        
        const config = vscode.workspace.getConfiguration('chtl');
        const compilerPath = config.get('compilerPath', './build/working_chtl_compiler');
        
        const { stdout, stderr } = await execAsync(`${compilerPath} --init`, {
            cwd: workspaceFolder.uri.fsPath
        });

        if (stderr) {
            vscode.window.showErrorMessage(`Init failed: ${stderr}`);
        } else {
            vscode.window.showInformationMessage('CHTL project initialized successfully!');
        }
    } catch (error) {
        vscode.window.showErrorMessage(`Init failed: ${error}`);
    }
}

// 编译文档
async function compileDocument(document: vscode.TextDocument) {
    try {
        const config = vscode.workspace.getConfiguration('chtl');
        const compilerPath = config.get('compilerPath', './build/working_chtl_compiler');
        const jsCompilerPath = config.get('jsCompilerPath', './build/chtl_js_compiler');
        
        const isCHTLJS = document.languageId === 'chtljs';
        const compiler = isCHTLJS ? jsCompilerPath : compilerPath;
        
        const outputPath = getOutputPath(document.uri.fsPath);
        
        const { stdout, stderr } = await execAsync(`${compiler} "${document.uri.fsPath}" "${outputPath}"`);

        if (stderr) {
            vscode.window.showErrorMessage(`Compilation failed: ${stderr}`);
        } else {
            vscode.window.showInformationMessage(`Compiled successfully: ${path.basename(outputPath)}`);
            
            // 自动预览
            if (config.get('autoPreview') && !isCHTLJS) {
                const doc = await vscode.workspace.openTextDocument(outputPath);
                await vscode.window.showTextDocument(doc, vscode.ViewColumn.Beside);
            }
        }
    } catch (error) {
        vscode.window.showErrorMessage(`Compilation failed: ${error}`);
    }
}

// 获取输出路径
function getOutputPath(inputPath: string): string {
    const config = vscode.workspace.getConfiguration('chtl');
    const outputDir = config.get('outputDirectory', './build');
    
    const ext = path.extname(inputPath);
    const basename = path.basename(inputPath, ext);
    const dirname = path.dirname(inputPath);
    
    if (ext === '.chtl') {
        return path.join(dirname, outputDir, `${basename}.html`);
    } else if (ext === '.chtljs') {
        return path.join(dirname, outputDir, `${basename}.js`);
    }
    
    return path.join(dirname, outputDir, `${basename}.out`);
}

// 事件处理器
function onDocumentSaved(document: vscode.TextDocument) {
    if (document.languageId === 'chtl' || document.languageId === 'chtljs') {
        const config = vscode.workspace.getConfiguration('chtl');
        if (config.get('autoCompile')) {
            compileDocument(document);
        }
    }
}

function onDocumentChanged(event: vscode.TextDocumentChangeEvent) {
    if (event.document.languageId === 'chtl' || event.document.languageId === 'chtljs') {
        // 实时语法检查
        validateDocument(event.document);
    }
}

function onDocumentOpened(document: vscode.TextDocument) {
    if (document.languageId === 'chtl' || document.languageId === 'chtljs') {
        validateDocument(document);
    }
}

// 验证文档
function validateDocument(document: vscode.TextDocument) {
    const diagnostics: vscode.Diagnostic[] = [];
    
    // 简化的语法检查
    const text = document.getText();
    const lines = text.split('\n');
    
    lines.forEach((line, lineIndex) => {
        // 检查模板语法
        if (line.includes('[Template]') && !line.includes('@Style') && !line.includes('@Element') && !line.includes('@Var')) {
            const diagnostic = new vscode.Diagnostic(
                new vscode.Range(lineIndex, 0, lineIndex, line.length),
                'Template must specify type: @Style, @Element, or @Var',
                vscode.DiagnosticSeverity.Error
            );
            diagnostics.push(diagnostic);
        }
        
        // 检查自定义语法
        if (line.includes('[Custom]') && !line.includes('Style') && !line.includes('Element') && !line.includes('Var')) {
            const diagnostic = new vscode.Diagnostic(
                new vscode.Range(lineIndex, 0, lineIndex, line.length),
                'Custom must specify type: Style, Element, or Var',
                vscode.DiagnosticSeverity.Error
            );
            diagnostics.push(diagnostic);
        }
    });
    
    // 发布诊断
    const diagnosticCollection = vscode.languages.getDiagnostics();
    diagnosticCollection.set(document.uri, diagnostics);
}

// 代码补全提供者
class CHTLCompletionItemProvider implements vscode.CompletionItemProvider {
    provideCompletionItems(
        document: vscode.TextDocument,
        position: vscode.Position,
        token: vscode.CancellationToken,
        context: vscode.CompletionContext
    ): vscode.ProviderResult<vscode.CompletionItem[] | vscode.CompletionList> {
        const completions: vscode.CompletionItem[] = [];
        
        // 模板补全
        const templateCompletion = new vscode.CompletionItem('[Template]', vscode.CompletionItemKind.Snippet);
        templateCompletion.insertText = new vscode.SnippetString('[Template] @${1|Style,Element,Var|} ${2:name} {\n\t${3:content}\n}');
        templateCompletion.documentation = new vscode.MarkdownString('Create a CHTL template');
        completions.push(templateCompletion);
        
        // 自定义补全
        const customCompletion = new vscode.CompletionItem('[Custom]', vscode.CompletionItemKind.Snippet);
        customCompletion.insertText = new vscode.SnippetString('[Custom] ${1:group} ${2|Style,Element,Var|} ${3:name} {\n\t${4:content}\n}');
        customCompletion.documentation = new vscode.MarkdownString('Create a CHTL custom element');
        completions.push(customCompletion);
        
        // 导入补全
        const importCompletion = new vscode.CompletionItem('[Import]', vscode.CompletionItemKind.Snippet);
        importCompletion.insertText = new vscode.SnippetString('[Import] ${1:path}');
        importCompletion.documentation = new vscode.MarkdownString('Import a CHTL module');
        completions.push(importCompletion);
        
        // 命名空间补全
        const namespaceCompletion = new vscode.CompletionItem('[Namespace]', vscode.CompletionItemKind.Snippet);
        namespaceCompletion.insertText = new vscode.SnippetString('[Namespace] ${1:name}');
        namespaceCompletion.documentation = new vscode.MarkdownString('Define a namespace');
        completions.push(namespaceCompletion);
        
        // 原始内容补全
        const originCompletion = new vscode.CompletionItem('[Origin]', vscode.CompletionItemKind.Snippet);
        originCompletion.insertText = new vscode.SnippetString('[Origin] @${1|Html,Style,JavaScript|} {\n\t${2:content}\n}');
        originCompletion.documentation = new vscode.MarkdownString('Embed raw content');
        completions.push(originCompletion);
        
        // 配置补全
        const configCompletion = new vscode.CompletionItem('[Configuration]', vscode.CompletionItemKind.Snippet);
        configCompletion.insertText = new vscode.SnippetString('[Configuration] ${1:key} = ${2:value}');
        configCompletion.documentation = new vscode.MarkdownString('Set configuration');
        completions.push(configCompletion);
        
        return completions;
    }
}

// 悬停提供者
class CHTLHoverProvider implements vscode.HoverProvider {
    provideHover(
        document: vscode.TextDocument,
        position: vscode.Position,
        token: vscode.CancellationToken
    ): vscode.ProviderResult<vscode.Hover> {
        const line = document.lineAt(position.line);
        const text = line.text;
        
        // 检查模板语法
        if (text.includes('[Template]')) {
            const hoverText = new vscode.MarkdownString();
            hoverText.appendMarkdown('**CHTL Template**\n\n');
            hoverText.appendMarkdown('Templates are reusable components in CHTL.\n\n');
            hoverText.appendMarkdown('**Types:**\n');
            hoverText.appendMarkdown('- `@Style`: CSS style templates\n');
            hoverText.appendMarkdown('- `@Element`: HTML element templates\n');
            hoverText.appendMarkdown('- `@Var`: Variable templates\n\n');
            hoverText.appendMarkdown('**Example:**\n');
            hoverText.appendCodeblock('[Template] @Style MyButton {\n    background-color: #007bff;\n    color: white;\n}', 'chtl');
            
            return new vscode.Hover(hoverText);
        }
        
        // 检查自定义语法
        if (text.includes('[Custom]')) {
            const hoverText = new vscode.MarkdownString();
            hoverText.appendMarkdown('**CHTL Custom**\n\n');
            hoverText.appendMarkdown('Custom elements provide specialized functionality.\n\n');
            hoverText.appendMarkdown('**Types:**\n');
            hoverText.appendMarkdown('- `Style`: Custom style groups\n');
            hoverText.appendMarkdown('- `Element`: Custom element groups\n');
            hoverText.appendMarkdown('- `Var`: Custom variable groups\n\n');
            hoverText.appendMarkdown('**Example:**\n');
            hoverText.appendCodeblock('[Custom] MyApp Style ButtonGroup {\n    display: flex;\n    gap: 10px;\n}', 'chtl');
            
            return new vscode.Hover(hoverText);
        }
        
        return null;
    }
}

// 文档符号提供者
class CHTLDocumentSymbolProvider implements vscode.DocumentSymbolProvider {
    provideDocumentSymbols(
        document: vscode.TextDocument,
        token: vscode.CancellationToken
    ): vscode.ProviderResult<vscode.SymbolInformation[] | vscode.DocumentSymbol[]> {
        const symbols: vscode.DocumentSymbol[] = [];
        const text = document.getText();
        const lines = text.split('\n');
        
        lines.forEach((line, lineIndex) => {
            // 模板符号
            const templateMatch = line.match(/\[Template\]\s+@(\w+)\s+(\w+)/);
            if (templateMatch) {
                const type = templateMatch[1];
                const name = templateMatch[2];
                const range = new vscode.Range(lineIndex, 0, lineIndex, line.length);
                
                const symbol = new vscode.DocumentSymbol(
                    `${type}: ${name}`,
                    `Template ${type}`,
                    vscode.SymbolKind.Class,
                    range,
                    range
                );
                symbols.push(symbol);
            }
            
            // 自定义符号
            const customMatch = line.match(/\[Custom\]\s+(\w+)\s+(\w+)\s+(\w+)/);
            if (customMatch) {
                const group = customMatch[1];
                const type = customMatch[2];
                const name = customMatch[3];
                const range = new vscode.Range(lineIndex, 0, lineIndex, line.length);
                
                const symbol = new vscode.DocumentSymbol(
                    `${group}.${type}: ${name}`,
                    `Custom ${type}`,
                    vscode.SymbolKind.Class,
                    range,
                    range
                );
                symbols.push(symbol);
            }
        });
        
        return symbols;
    }
}

// 定义提供者
class CHTLDefinitionProvider implements vscode.DefinitionProvider {
    provideDefinition(
        document: vscode.TextDocument,
        position: vscode.Position,
        token: vscode.CancellationToken
    ): vscode.ProviderResult<vscode.Definition | vscode.LocationLink[]> {
        // 简化的定义提供者
        return null;
    }
}

// 代码操作提供者
class CHTLCodeActionsProvider implements vscode.CodeActionProvider {
    provideCodeActions(
        document: vscode.TextDocument,
        range: vscode.Range | vscode.Selection,
        context: vscode.CodeActionContext,
        token: vscode.CancellationToken
    ): vscode.ProviderResult<(vscode.Command | vscode.CodeAction)[]> {
        const actions: vscode.CodeAction[] = [];
        
        // 快速修复模板语法
        if (context.diagnostics.length > 0) {
            const diagnostic = context.diagnostics[0];
            if (diagnostic.message.includes('Template must specify type')) {
                const fix = new vscode.CodeAction('Fix template type', vscode.CodeActionKind.QuickFix);
                fix.edit = new vscode.WorkspaceEdit();
                fix.edit.replace(document.uri, range, '[Template] @Style ${1:name} {\n\t${2:content}\n}');
                actions.push(fix);
            }
        }
        
        return actions;
    }
}

// CHTL Explorer
class CHTLExplorer implements vscode.TreeDataProvider<CHTLNode> {
    private _onDidChangeTreeData: vscode.EventEmitter<CHTLNode | undefined | null | void> = new vscode.EventEmitter<CHTLNode | undefined | null | void>();
    readonly onDidChangeTreeData: vscode.Event<CHTLNode | undefined | null | void> = this._onDidChangeTreeData.event;

    constructor(private context: vscode.ExtensionContext) {}

    refresh(): void {
        this._onDidChangeTreeData.fire();
    }

    getTreeItem(element: CHTLNode): vscode.TreeItem {
        return element;
    }

    getChildren(element?: CHTLNode): Thenable<CHTLNode[]> {
        if (!element) {
            return this.getCHTLFiles();
        }
        return Promise.resolve([]);
    }

    private async getCHTLFiles(): Promise<CHTLNode[]> {
        const workspaceFolder = vscode.workspace.workspaceFolders?.[0];
        if (!workspaceFolder) {
            return [];
        }

        const files = await vscode.workspace.findFiles('**/*.chtl', '**/node_modules/**');
        const jsFiles = await vscode.workspace.findFiles('**/*.chtljs', '**/node_modules/**');
        
        const nodes: CHTLNode[] = [];
        
        files.forEach(file => {
            const node = new CHTLNode(
                path.basename(file.fsPath),
                vscode.TreeItemCollapsibleState.None,
                file
            );
            node.command = {
                command: 'vscode.open',
                title: 'Open File',
                arguments: [file]
            };
            nodes.push(node);
        });
        
        jsFiles.forEach(file => {
            const node = new CHTLNode(
                path.basename(file.fsPath),
                vscode.TreeItemCollapsibleState.None,
                file
            );
            node.command = {
                command: 'vscode.open',
                title: 'Open File',
                arguments: [file]
            };
            nodes.push(node);
        });
        
        return nodes;
    }
}

class CHTLNode extends vscode.TreeItem {
    constructor(
        public readonly label: string,
        public readonly collapsibleState: vscode.TreeItemCollapsibleState,
        public readonly resourceUri?: vscode.Uri
    ) {
        super(label, collapsibleState);
        this.tooltip = this.label;
        this.description = this.resourceUri?.fsPath;
    }
}