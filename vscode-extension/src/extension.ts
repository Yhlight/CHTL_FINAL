import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';
import { spawn, ChildProcess } from 'child_process';

export function activate(context: vscode.ExtensionContext) {
    console.log('CHTL extension is now active!');

    // Register commands
    const commands = [
        vscode.commands.registerCommand('chtl.compile', compileCHTL),
        vscode.commands.registerCommand('chtl.parse', parseCHTL),
        vscode.commands.registerCommand('chtl.format', formatCHTL),
        vscode.commands.registerCommand('chtl.lint', lintCHTL),
        vscode.commands.registerCommand('chtl.preview', previewCHTL),
        vscode.commands.registerCommand('chtl.export', exportCHTL),
        vscode.commands.registerCommand('chtl.build', buildCHTL),
        vscode.commands.registerCommand('chtl.clean', cleanCHTL),
        vscode.commands.registerCommand('chtl.test', testCHTL),
        vscode.commands.registerCommand('chtl.serve', serveCHTL),
        vscode.commands.registerCommand('chtl.watch', watchCHTL)
    ];

    // Register providers
    const providers = [
        vscode.languages.registerDocumentFormattingEditProvider('chtl', new CHTLFormatter()),
        vscode.languages.registerHoverProvider('chtl', new CHTLHoverProvider()),
        vscode.languages.registerCompletionItemProvider('chtl', new CHTLCompletionProvider(), '.', ' ', ':', ';', '{', '}'),
        vscode.languages.registerDefinitionProvider('chtl', new CHTLDefinitionProvider()),
        vscode.languages.registerReferenceProvider('chtl', new CHTLReferenceProvider()),
        vscode.languages.registerDocumentSymbolProvider('chtl', new CHTLDocumentSymbolProvider()),
        vscode.languages.registerWorkspaceSymbolProvider(new CHTLWorkspaceSymbolProvider()),
        vscode.languages.registerCodeActionsProvider('chtl', new CHTLCodeActionsProvider()),
        vscode.languages.registerDiagnosticProvider('chtl', new CHTLDiagnosticProvider())
    ];

    // Register views
    const views = [
        vscode.window.createTreeView('chtl.templates', { treeDataProvider: new CHTLTemplatesProvider() }),
        vscode.window.createTreeView('chtl.customs', { treeDataProvider: new CHTLCustomsProvider() }),
        vscode.window.createTreeView('chtl.imports', { treeDataProvider: new CHTLImportsProvider() }),
        vscode.window.createTreeView('chtl.modules', { treeDataProvider: new CHTLModulesProvider() })
    ];

    // Register all
    context.subscriptions.push(...commands, ...providers, ...views);

    // Register configuration change listener
    vscode.workspace.onDidChangeConfiguration(event => {
        if (event.affectsConfiguration('chtl')) {
            updateConfiguration();
        }
    });

    // Register file change listener
    vscode.workspace.onDidSaveTextDocument(document => {
        if (document.languageId === 'chtl') {
            handleDocumentSave(document);
        }
    });

    // Initialize
    updateConfiguration();
}

export function deactivate() {
    // Cleanup
}

// Configuration
let config: vscode.WorkspaceConfiguration;

function updateConfiguration() {
    config = vscode.workspace.getConfiguration('chtl');
}

// Command implementations
async function compileCHTL() {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'chtl') {
        vscode.window.showWarningMessage('Please open a CHTL file to compile');
        return;
    }

    const document = editor.document;
    const fileName = path.basename(document.fileName, '.chtl');
    const outputPath = path.join(path.dirname(document.fileName), `${fileName}.html`);

    try {
        const result = await runCHTLCommand(['compile', document.fileName, '-o', outputPath]);
        if (result.success) {
            vscode.window.showInformationMessage(`Compiled successfully to ${outputPath}`);
            
            // Open preview if enabled
            if (config.get('preview.autoOpen', false)) {
                await previewCHTL();
            }
        } else {
            vscode.window.showErrorMessage(`Compilation failed: ${result.error}`);
        }
    } catch (error) {
        vscode.window.showErrorMessage(`Compilation error: ${error}`);
    }
}

async function parseCHTL() {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'chtl') {
        vscode.window.showWarningMessage('Please open a CHTL file to parse');
        return;
    }

    const document = editor.document;

    try {
        const result = await runCHTLCommand(['parse', document.fileName]);
        if (result.success) {
            vscode.window.showInformationMessage('Parse successful');
        } else {
            vscode.window.showErrorMessage(`Parse failed: ${result.error}`);
        }
    } catch (error) {
        vscode.window.showErrorMessage(`Parse error: ${error}`);
    }
}

async function formatCHTL() {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'chtl') {
        vscode.window.showWarningMessage('Please open a CHTL file to format');
        return;
    }

    const document = editor.document;

    try {
        const result = await runCHTLCommand(['format', document.fileName]);
        if (result.success) {
            vscode.window.showInformationMessage('Format successful');
        } else {
            vscode.window.showErrorMessage(`Format failed: ${result.error}`);
        }
    } catch (error) {
        vscode.window.showErrorMessage(`Format error: ${error}`);
    }
}

async function lintCHTL() {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'chtl') {
        vscode.window.showWarningMessage('Please open a CHTL file to lint');
        return;
    }

    const document = editor.document;

    try {
        const result = await runCHTLCommand(['lint', document.fileName]);
        if (result.success) {
            vscode.window.showInformationMessage('Lint successful');
        } else {
            vscode.window.showErrorMessage(`Lint failed: ${result.error}`);
        }
    } catch (error) {
        vscode.window.showErrorMessage(`Lint error: ${error}`);
    }
}

async function previewCHTL() {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'chtl') {
        vscode.window.showWarningMessage('Please open a CHTL file to preview');
        return;
    }

    const document = editor.document;
    const fileName = path.basename(document.fileName, '.chtl');
    const outputPath = path.join(path.dirname(document.fileName), `${fileName}.html`);

    try {
        // First compile
        const compileResult = await runCHTLCommand(['compile', document.fileName, '-o', outputPath]);
        if (!compileResult.success) {
            vscode.window.showErrorMessage(`Compilation failed: ${compileResult.error}`);
            return;
        }

        // Then open preview
        const port = config.get('preview.port', 3000);
        const result = await runCHTLCommand(['serve', '--port', port.toString()]);
        if (result.success) {
            vscode.window.showInformationMessage(`Preview server started on port ${port}`);
        } else {
            vscode.window.showErrorMessage(`Preview failed: ${result.error}`);
        }
    } catch (error) {
        vscode.window.showErrorMessage(`Preview error: ${error}`);
    }
}

async function exportCHTL() {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'chtl') {
        vscode.window.showWarningMessage('Please open a CHTL file to export');
        return;
    }

    const document = editor.document;
    const fileName = path.basename(document.fileName, '.chtl');
    const outputPath = path.join(path.dirname(document.fileName), `${fileName}.html`);

    try {
        const result = await runCHTLCommand(['compile', document.fileName, '-o', outputPath]);
        if (result.success) {
            vscode.window.showInformationMessage(`Exported successfully to ${outputPath}`);
        } else {
            vscode.window.showErrorMessage(`Export failed: ${result.error}`);
        }
    } catch (error) {
        vscode.window.showErrorMessage(`Export error: ${error}`);
    }
}

async function buildCHTL() {
    try {
        const result = await runCHTLCommand(['build']);
        if (result.success) {
            vscode.window.showInformationMessage('Build successful');
        } else {
            vscode.window.showErrorMessage(`Build failed: ${result.error}`);
        }
    } catch (error) {
        vscode.window.showErrorMessage(`Build error: ${error}`);
    }
}

async function cleanCHTL() {
    try {
        const result = await runCHTLCommand(['clean']);
        if (result.success) {
            vscode.window.showInformationMessage('Clean successful');
        } else {
            vscode.window.showErrorMessage(`Clean failed: ${result.error}`);
        }
    } catch (error) {
        vscode.window.showErrorMessage(`Clean error: ${error}`);
    }
}

async function testCHTL() {
    try {
        const result = await runCHTLCommand(['test']);
        if (result.success) {
            vscode.window.showInformationMessage('Tests passed');
        } else {
            vscode.window.showErrorMessage(`Tests failed: ${result.error}`);
        }
    } catch (error) {
        vscode.window.showErrorMessage(`Test error: ${error}`);
    }
}

async function serveCHTL() {
    try {
        const port = config.get('preview.port', 3000);
        const result = await runCHTLCommand(['serve', '--port', port.toString()]);
        if (result.success) {
            vscode.window.showInformationMessage(`Server started on port ${port}`);
        } else {
            vscode.window.showErrorMessage(`Server failed: ${result.error}`);
        }
    } catch (error) {
        vscode.window.showErrorMessage(`Server error: ${error}`);
    }
}

async function watchCHTL() {
    try {
        const result = await runCHTLCommand(['watch']);
        if (result.success) {
            vscode.window.showInformationMessage('Watching files for changes');
        } else {
            vscode.window.showErrorMessage(`Watch failed: ${result.error}`);
        }
    } catch (error) {
        vscode.window.showErrorMessage(`Watch error: ${error}`);
    }
}

// Helper functions
async function runCHTLCommand(args: string[]): Promise<{ success: boolean; output?: string; error?: string }> {
    return new Promise((resolve) => {
        const compilerPath = config.get('compiler.path', 'chtl');
        const options = config.get('compiler.options', []) as string[];
        const allArgs = [...args, ...options];

        const process = spawn(compilerPath, allArgs);
        let output = '';
        let error = '';

        process.stdout.on('data', (data) => {
            output += data.toString();
        });

        process.stderr.on('data', (data) => {
            error += data.toString();
        });

        process.on('close', (code) => {
            resolve({
                success: code === 0,
                output: output.trim(),
                error: error.trim()
            });
        });

        process.on('error', (err) => {
            resolve({
                success: false,
                error: err.message
            });
        });
    });
}

function handleDocumentSave(document: vscode.TextDocument) {
    // Auto-format if enabled
    if (config.get('format.enable', true)) {
        vscode.commands.executeCommand('editor.action.formatDocument');
    }

    // Auto-lint if enabled
    if (config.get('lint.enable', true)) {
        vscode.commands.executeCommand('chtl.lint');
    }
}

// Provider implementations
class CHTLFormatter implements vscode.DocumentFormattingEditProvider {
    provideDocumentFormattingEdits(document: vscode.TextDocument): vscode.ProviderResult<vscode.TextEdit[]> {
        // Implement CHTL formatting
        return [];
    }
}

class CHTLHoverProvider implements vscode.HoverProvider {
    provideHover(document: vscode.TextDocument, position: vscode.Position): vscode.ProviderResult<vscode.Hover> {
        // Implement CHTL hover information
        return undefined;
    }
}

class CHTLCompletionProvider implements vscode.CompletionItemProvider {
    provideCompletionItems(document: vscode.TextDocument, position: vscode.Position): vscode.ProviderResult<vscode.CompletionItem[]> {
        // Implement CHTL completion items
        return [];
    }
}

class CHTLDefinitionProvider implements vscode.DefinitionProvider {
    provideDefinition(document: vscode.TextDocument, position: vscode.Position): vscode.ProviderResult<vscode.Definition> {
        // Implement CHTL definition provider
        return undefined;
    }
}

class CHTLReferenceProvider implements vscode.ReferenceProvider {
    provideReferences(document: vscode.TextDocument, position: vscode.Position): vscode.ProviderResult<vscode.Location[]> {
        // Implement CHTL reference provider
        return [];
    }
}

class CHTLDocumentSymbolProvider implements vscode.DocumentSymbolProvider {
    provideDocumentSymbols(document: vscode.TextDocument): vscode.ProviderResult<vscode.DocumentSymbol[]> {
        // Implement CHTL document symbols
        return [];
    }
}

class CHTLWorkspaceSymbolProvider implements vscode.WorkspaceSymbolProvider {
    provideWorkspaceSymbols(query: string): vscode.ProviderResult<vscode.SymbolInformation[]> {
        // Implement CHTL workspace symbols
        return [];
    }
}

class CHTLCodeActionsProvider implements vscode.CodeActionProvider {
    provideCodeActions(document: vscode.TextDocument, range: vscode.Range): vscode.ProviderResult<vscode.CodeAction[]> {
        // Implement CHTL code actions
        return [];
    }
}

class CHTLDiagnosticProvider implements vscode.DiagnosticProvider {
    provideDiagnostics(document: vscode.TextDocument): vscode.ProviderResult<vscode.Diagnostic[]> {
        // Implement CHTL diagnostics
        return [];
    }
}

// Tree view providers
class CHTLTemplatesProvider implements vscode.TreeDataProvider<vscode.TreeItem> {
    getTreeItem(element: vscode.TreeItem): vscode.TreeItem {
        return element;
    }

    getChildren(element?: vscode.TreeItem): vscode.ProviderResult<vscode.TreeItem[]> {
        // Implement templates tree view
        return [];
    }
}

class CHTLCustomsProvider implements vscode.TreeDataProvider<vscode.TreeItem> {
    getTreeItem(element: vscode.TreeItem): vscode.TreeItem {
        return element;
    }

    getChildren(element?: vscode.TreeItem): vscode.ProviderResult<vscode.TreeItem[]> {
        // Implement customs tree view
        return [];
    }
}

class CHTLImportsProvider implements vscode.TreeDataProvider<vscode.TreeItem> {
    getTreeItem(element: vscode.TreeItem): vscode.TreeItem {
        return element;
    }

    getChildren(element?: vscode.TreeItem): vscode.ProviderResult<vscode.TreeItem[]> {
        // Implement imports tree view
        return [];
    }
}

class CHTLModulesProvider implements vscode.TreeDataProvider<vscode.TreeItem> {
    getTreeItem(element: vscode.TreeItem): vscode.TreeItem {
        return element;
    }

    getChildren(element?: vscode.TreeItem): vscode.ProviderResult<vscode.TreeItem[]> {
        // Implement modules tree view
        return [];
    }
}