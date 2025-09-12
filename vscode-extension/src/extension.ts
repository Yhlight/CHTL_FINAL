import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';
import { spawn, ChildProcess } from 'child_process';
import { CHTLLanguageServer } from './languageServer';
import { CHTLModuleView } from './moduleViewProvider';

export function activate(context: vscode.ExtensionContext) {
    console.log('CHTL Language extension is now active!');

    // 初始化语言服务器
    const languageServer = new CHTLLanguageServer();
    languageServer.activate(context);

    // 初始化模块视图
    const moduleView = new CHTLModuleView(context);

    // 注册命令
    const commands = [
        vscode.commands.registerCommand('chtl.compile', compileCHTL),
        vscode.commands.registerCommand('chtl.preview', previewCHTL),
        vscode.commands.registerCommand('chtl.export.html', exportHTML),
        vscode.commands.registerCommand('chtl.export.css', exportCSS),
        vscode.commands.registerCommand('chtl.export.js', exportJS),
        vscode.commands.registerCommand('chtl.watch', watchCHTL),
        vscode.commands.registerCommand('chtl.init', initProject),
        vscode.commands.registerCommand('chtl.modules.refresh', () => moduleView.refresh())
    ];

    commands.forEach(command => context.subscriptions.push(command));

    // 注册语言特性
    const completionProvider = vscode.languages.registerCompletionItemProvider(
        { scheme: 'file', language: 'chtl' },
        {
            provideCompletionItems(document: vscode.TextDocument, position: vscode.Position) {
                return languageServer.provideCompletionItems(document, position);
            }
        }
    );

    const hoverProvider = vscode.languages.registerHoverProvider(
        { scheme: 'file', language: 'chtl' },
        {
            provideHover(document: vscode.TextDocument, position: vscode.Position) {
                return languageServer.provideHover(document, position);
            }
        }
    );

    context.subscriptions.push(completionProvider, hoverProvider);

    // 注册文件保存事件
    const saveEvent = vscode.workspace.onDidSaveTextDocument((document) => {
        if (isCHTLFile(document)) {
            const config = vscode.workspace.getConfiguration('chtl');
            if (config.get('autoCompile', true)) {
                compileDocument(document);
            }
        }
    });

    context.subscriptions.push(saveEvent);

    // 注册文件变化事件
    const changeEvent = vscode.workspace.onDidChangeTextDocument((event) => {
        if (isCHTLFile(event.document)) {
            // 可以在这里实现实时预览功能
        }
    });

    context.subscriptions.push(changeEvent);

    // 初始化模块视图
    initializeModuleView(context);
}

export function deactivate() {
    // 清理资源
}

// 检查是否为CHTL文件
function isCHTLFile(document: vscode.TextDocument): boolean {
    const ext = path.extname(document.fileName).toLowerCase();
    return ext === '.chtl' || ext === '.cmod' || ext === '.cjjs' || ext === '.cjmod';
}

// 编译CHTL文件
async function compileCHTL(uri?: vscode.Uri) {
    const document = await getActiveDocument(uri);
    if (!document) {
        vscode.window.showErrorMessage('No CHTL file to compile');
        return;
    }

    await compileDocument(document);
}

// 编译文档
async function compileDocument(document: vscode.TextDocument) {
    const config = vscode.workspace.getConfiguration('chtl');
    const compilerPath = config.get('compilerPath', 'chtl');
    const outputDir = config.get('outputDirectory', 'dist');
    const debug = config.get('debug', false);

    try {
        // 确保输出目录存在
        const outputPath = path.join(path.dirname(document.fileName), outputDir);
        if (!fs.existsSync(outputPath)) {
            fs.mkdirSync(outputPath, { recursive: true });
        }

        // 构建编译命令
        const args = ['compile', document.fileName, '-o', path.join(outputPath, path.basename(document.fileName, path.extname(document.fileName)) + '.html')];
        if (debug) {
            args.push('--debug');
        }

        // 执行编译
        const result = await executeCommand(compilerPath, args, path.dirname(document.fileName));
        
        if (result.success) {
            vscode.window.showInformationMessage('CHTL compilation successful!');
            // 显示输出
            showOutput(result.output);
        } else {
            vscode.window.showErrorMessage('CHTL compilation failed: ' + result.error);
            // 显示错误
            showError(result.error);
        }
    } catch (error) {
        vscode.window.showErrorMessage('Failed to compile CHTL: ' + error);
    }
}

// 预览CHTL文件
async function previewCHTL(uri?: vscode.Uri) {
    const document = await getActiveDocument(uri);
    if (!document) {
        vscode.window.showErrorMessage('No CHTL file to preview');
        return;
    }

    // 先编译文件
    await compileDocument(document);

    // 打开预览
    const config = vscode.workspace.getConfiguration('chtl');
    const outputDir = config.get('outputDirectory', 'dist');
    const outputFile = path.join(path.dirname(document.fileName), outputDir, path.basename(document.fileName, path.extname(document.fileName)) + '.html');

    if (fs.existsSync(outputFile)) {
        const uri = vscode.Uri.file(outputFile);
        await vscode.commands.executeCommand('vscode.open', uri);
    } else {
        vscode.window.showErrorMessage('Preview file not found. Please compile first.');
    }
}

// 导出HTML
async function exportHTML(uri?: vscode.Uri) {
    const document = await getActiveDocument(uri);
    if (!document) {
        vscode.window.showErrorMessage('No CHTL file to export');
        return;
    }

    await compileDocument(document);
    
    const config = vscode.workspace.getConfiguration('chtl');
    const outputDir = config.get('outputDirectory', 'dist');
    const outputFile = path.join(path.dirname(document.fileName), outputDir, path.basename(document.fileName, path.extname(document.fileName)) + '.html');

    if (fs.existsSync(outputFile)) {
        const saveUri = await vscode.window.showSaveDialog({
            defaultUri: vscode.Uri.file(outputFile),
            filters: {
                'HTML files': ['html']
            }
        });

        if (saveUri) {
            fs.copyFileSync(outputFile, saveUri.fsPath);
            vscode.window.showInformationMessage('HTML exported successfully!');
        }
    }
}

// 导出CSS
async function exportCSS(uri?: vscode.Uri) {
    const document = await getActiveDocument(uri);
    if (!document) {
        vscode.window.showErrorMessage('No CHTL file to export');
        return;
    }

    await compileDocument(document);
    
    const config = vscode.workspace.getConfiguration('chtl');
    const outputDir = config.get('outputDirectory', 'dist');
    const cssFile = path.join(path.dirname(document.fileName), outputDir, path.basename(document.fileName, path.extname(document.fileName)) + '.css');

    if (fs.existsSync(cssFile)) {
        const saveUri = await vscode.window.showSaveDialog({
            defaultUri: vscode.Uri.file(cssFile),
            filters: {
                'CSS files': ['css']
            }
        });

        if (saveUri) {
            fs.copyFileSync(cssFile, saveUri.fsPath);
            vscode.window.showInformationMessage('CSS exported successfully!');
        }
    }
}

// 导出JavaScript
async function exportJS(uri?: vscode.Uri) {
    const document = await getActiveDocument(uri);
    if (!document) {
        vscode.window.showErrorMessage('No CHTL file to export');
        return;
    }

    await compileDocument(document);
    
    const config = vscode.workspace.getConfiguration('chtl');
    const outputDir = config.get('outputDirectory', 'dist');
    const jsFile = path.join(path.dirname(document.fileName), outputDir, path.basename(document.fileName, path.extname(document.fileName)) + '.js');

    if (fs.existsSync(jsFile)) {
        const saveUri = await vscode.window.showSaveDialog({
            defaultUri: vscode.Uri.file(jsFile),
            filters: {
                'JavaScript files': ['js']
            }
        });

        if (saveUri) {
            fs.copyFileSync(jsFile, saveUri.fsPath);
            vscode.window.showInformationMessage('JavaScript exported successfully!');
        }
    }
}

// 监视文件变化
async function watchCHTL(uri?: vscode.Uri) {
    const document = await getActiveDocument(uri);
    if (!document) {
        vscode.window.showErrorMessage('No CHTL file to watch');
        return;
    }

    const config = vscode.workspace.getConfiguration('chtl');
    const compilerPath = config.get('compilerPath', 'chtl');
    const outputDir = config.get('outputDirectory', 'dist');

    const args = ['watch', path.dirname(document.fileName), '-o', outputDir];
    
    try {
        const result = await executeCommand(compilerPath, args, path.dirname(document.fileName));
        if (result.success) {
            vscode.window.showInformationMessage('CHTL watch mode started!');
        } else {
            vscode.window.showErrorMessage('Failed to start watch mode: ' + result.error);
        }
    } catch (error) {
        vscode.window.showErrorMessage('Failed to start watch mode: ' + error);
    }
}

// 初始化项目
async function initProject(uri?: vscode.Uri) {
    const projectName = await vscode.window.showInputBox({
        prompt: 'Enter project name',
        placeHolder: 'chtl-project',
        value: 'chtl-project'
    });

    if (!projectName) {
        return;
    }

    const config = vscode.workspace.getConfiguration('chtl');
    const compilerPath = config.get('compilerPath', 'chtl');

    try {
        const result = await executeCommand(compilerPath, ['init', projectName], vscode.workspace.workspaceFolders?.[0]?.uri.fsPath || '.');
        
        if (result.success) {
            vscode.window.showInformationMessage('CHTL project initialized successfully!');
            // 刷新文件资源管理器
            vscode.commands.executeCommand('workbench.files.action.refreshFilesExplorer');
        } else {
            vscode.window.showErrorMessage('Failed to initialize project: ' + result.error);
        }
    } catch (error) {
        vscode.window.showErrorMessage('Failed to initialize project: ' + error);
    }
}

// 获取活动文档
async function getActiveDocument(uri?: vscode.Uri): Promise<vscode.TextDocument | undefined> {
    if (uri) {
        return await vscode.workspace.openTextDocument(uri);
    }
    
    const editor = vscode.window.activeTextEditor;
    if (editor && isCHTLFile(editor.document)) {
        return editor.document;
    }
    
    return undefined;
}

// 执行命令
function executeCommand(command: string, args: string[], cwd: string): Promise<{ success: boolean; output: string; error: string }> {
    return new Promise((resolve) => {
        const process = spawn(command, args, { cwd });
        
        let output = '';
        let error = '';
        
        process.stdout?.on('data', (data) => {
            output += data.toString();
        });
        
        process.stderr?.on('data', (data) => {
            error += data.toString();
        });
        
        process.on('close', (code) => {
            resolve({
                success: code === 0,
                output: output,
                error: error
            });
        });
        
        process.on('error', (err) => {
            resolve({
                success: false,
                output: output,
                error: err.message
            });
        });
    });
}

// 显示输出
function showOutput(output: string) {
    const channel = vscode.window.createOutputChannel('CHTL Output');
    channel.appendLine('=== CHTL Compilation Output ===');
    channel.appendLine(output);
    channel.show();
}

// 显示错误
function showError(error: string) {
    const channel = vscode.window.createOutputChannel('CHTL Errors');
    channel.appendLine('=== CHTL Compilation Error ===');
    channel.appendLine(error);
    channel.show();
}

// 初始化模块视图
function initializeModuleView(context: vscode.ExtensionContext) {
    // 这里可以实现模块视图的初始化
    // 包括扫描项目中的模块、显示模块信息等
}