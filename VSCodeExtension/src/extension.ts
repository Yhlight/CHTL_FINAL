import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';
import { spawn } from 'child_process';

export function activate(context: vscode.ExtensionContext) {
    console.log('CHTL Language Support extension is now active!');

    // 注册命令
    const compileCommand = vscode.commands.registerCommand('chtl.compile', () => {
        compileCurrentFile();
    });

    const compileToHtmlCommand = vscode.commands.registerCommand('chtl.compileToHtml', () => {
        compileCurrentFile('html');
    });

    const compileToCssCommand = vscode.commands.registerCommand('chtl.compileToCss', () => {
        compileCurrentFile('css');
    });

    const compileToJsCommand = vscode.commands.registerCommand('chtl.compileToJs', () => {
        compileCurrentFile('js');
    });

    const previewCommand = vscode.commands.registerCommand('chtl.preview', () => {
        previewInBrowser();
    });

    const openDocsCommand = vscode.commands.registerCommand('chtl.openDocs', () => {
        openDocumentation();
    });

    // 注册自动编译
    const autoCompile = vscode.workspace.onDidSaveTextDocument((document) => {
        if (document.languageId === 'chtl') {
            const config = vscode.workspace.getConfiguration('chtl');
            if (config.get('autoCompile')) {
                compileDocument(document);
            }
        }
    });

    // 注册文件监听
    const fileWatcher = vscode.workspace.createFileSystemWatcher('**/*.chtl');
    fileWatcher.onDidChange((uri) => {
        const config = vscode.workspace.getConfiguration('chtl');
        if (config.get('autoCompile')) {
            vscode.workspace.openTextDocument(uri).then((document) => {
                compileDocument(document);
            });
        }
    });

    // 添加到上下文
    context.subscriptions.push(
        compileCommand,
        compileToHtmlCommand,
        compileToCssCommand,
        compileToJsCommand,
        previewCommand,
        openDocsCommand,
        autoCompile,
        fileWatcher
    );
}

export function deactivate() {
    console.log('CHTL Language Support extension is now deactivated');
}

async function compileCurrentFile(outputType?: string) {
    const editor = vscode.window.activeTextEditor;
    if (!editor) {
        vscode.window.showErrorMessage('No active editor');
        return;
    }

    if (editor.document.languageId !== 'chtl') {
        vscode.window.showErrorMessage('Current file is not a CHTL file');
        return;
    }

    await compileDocument(editor.document, outputType);
}

async function compileDocument(document: vscode.TextDocument, outputType?: string) {
    const config = vscode.workspace.getConfiguration('chtl');
    const compilerPath = config.get<string>('compilerPath') || 'chtl';
    const outputDir = config.get<string>('outputDirectory') || 'dist';
    const debugMode = config.get<boolean>('debugMode') || false;

    const inputFile = document.uri.fsPath;
    const outputFile = getOutputPath(inputFile, outputType);

    // 确保输出目录存在
    const outputDirPath = path.dirname(outputFile);
    if (!fs.existsSync(outputDirPath)) {
        fs.mkdirSync(outputDirPath, { recursive: true });
    }

    const args = ['compile', inputFile, '-o', outputFile];
    if (debugMode) {
        args.push('-d');
    }

    return new Promise<void>((resolve, reject) => {
        const process = spawn(compilerPath, args, { cwd: vscode.workspace.workspaceFolders?.[0]?.uri.fsPath });

        let errorOutput = '';

        process.stderr.on('data', (data) => {
            errorOutput += data.toString();
        });

        process.on('close', (code) => {
            if (code === 0) {
                vscode.window.showInformationMessage(`CHTL compiled successfully: ${outputFile}`);
                resolve();
            } else {
                vscode.window.showErrorMessage(`CHTL compilation failed: ${errorOutput}`);
                reject(new Error(errorOutput));
            }
        });

        process.on('error', (error) => {
            vscode.window.showErrorMessage(`Failed to start CHTL compiler: ${error.message}`);
            reject(error);
        });
    });
}

function getOutputPath(inputFile: string, outputType?: string): string {
    const config = vscode.workspace.getConfiguration('chtl');
    const outputDir = config.get<string>('outputDirectory') || 'dist';
    const workspaceRoot = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath || path.dirname(inputFile);
    
    const relativePath = path.relative(workspaceRoot, inputFile);
    const baseName = path.basename(inputFile, '.chtl');
    const dirName = path.dirname(relativePath);
    
    let extension = '.html';
    if (outputType === 'css') {
        extension = '.css';
    } else if (outputType === 'js') {
        extension = '.js';
    }
    
    return path.join(workspaceRoot, outputDir, dirName, baseName + extension);
}

async function previewInBrowser() {
    const editor = vscode.window.activeTextEditor;
    if (!editor) {
        vscode.window.showErrorMessage('No active editor');
        return;
    }

    if (editor.document.languageId !== 'chtl') {
        vscode.window.showErrorMessage('Current file is not a CHTL file');
        return;
    }

    // 先编译文件
    await compileDocument(editor.document);
    
    // 然后打开预览
    const outputFile = getOutputPath(editor.document.uri.fsPath);
    const uri = vscode.Uri.file(outputFile);
    
    vscode.commands.executeCommand('vscode.open', uri);
}

function openDocumentation() {
    const docUrl = 'https://chtl.dev/docs';
    vscode.env.openExternal(vscode.Uri.parse(docUrl));
}