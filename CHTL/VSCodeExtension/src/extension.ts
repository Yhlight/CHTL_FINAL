import * as vscode from 'vscode';
import { CHTLLanguageClient } from './languageClient';
import { CHTLProvider } from './provider';
import { CHTLFormatter } from './formatter';
import { CHTLLinter } from './linter';
import { CHTLPreview } from './preview';
import { CHTLCompiler } from './compiler';

export function activate(context: vscode.ExtensionContext) {
    console.log('CHTL Language extension is now active!');

    // 初始化语言客户端
    const languageClient = new CHTLLanguageClient(context);
    
    // 初始化提供者
    const provider = new CHTLProvider();
    
    // 初始化格式化器
    const formatter = new CHTLFormatter();
    
    // 初始化语法检查器
    const linter = new CHTLLinter();
    
    // 初始化预览器
    const preview = new CHTLPreview();
    
    // 初始化编译器
    const compiler = new CHTLCompiler();

    // 注册命令
    const commands = [
        vscode.commands.registerCommand('chtl.compile', () => compiler.compile()),
        vscode.commands.registerCommand('chtl.preview', () => preview.show()),
        vscode.commands.registerCommand('chtl.format', () => formatter.format()),
        vscode.commands.registerCommand('chtl.watch', () => compiler.watch()),
        vscode.commands.registerCommand('chtl.build', () => compiler.build()),
        vscode.commands.registerCommand('chtl.clean', () => compiler.clean())
    ];

    // 注册文档格式化提供者
    const documentFormattingProvider = vscode.languages.registerDocumentFormattingEditProvider(
        'chtl',
        formatter
    );

    // 注册代码完成提供者
    const completionProvider = vscode.languages.registerCompletionItemProvider(
        'chtl',
        provider,
        '.', ':', '=', ' ', '\t', '\n'
    );

    // 注册悬停提供者
    const hoverProvider = vscode.languages.registerHoverProvider(
        'chtl',
        provider
    );

    // 注册定义提供者
    const definitionProvider = vscode.languages.registerDefinitionProvider(
        'chtl',
        provider
    );

    // 注册引用提供者
    const referenceProvider = vscode.languages.registerReferenceProvider(
        'chtl',
        provider
    );

    // 注册重命名提供者
    const renameProvider = vscode.languages.registerRenameProvider(
        'chtl',
        provider
    );

    // 注册诊断提供者
    const diagnosticProvider = vscode.languages.registerDiagnosticProvider(
        'chtl',
        linter
    );

    // 注册文件监听器
    const fileWatcher = vscode.workspace.createFileSystemWatcher('**/*.chtl');
    fileWatcher.onDidChange(uri => {
        linter.checkDocument(uri);
    });

    // 注册配置更改监听器
    const configWatcher = vscode.workspace.onDidChangeConfiguration(event => {
        if (event.affectsConfiguration('chtl')) {
            // 重新加载配置
            provider.reloadConfiguration();
            formatter.reloadConfiguration();
            linter.reloadConfiguration();
            compiler.reloadConfiguration();
        }
    });

    // 注册文档保存监听器
    const saveWatcher = vscode.workspace.onDidSaveTextDocument(document => {
        if (document.languageId === 'chtl') {
            // 自动格式化（如果启用）
            const config = vscode.workspace.getConfiguration('chtl');
            if (config.get('formatting.autoFormatOnSave', false)) {
                formatter.formatDocument(document);
            }
            
            // 自动编译（如果启用）
            if (config.get('compiler.autoCompileOnSave', false)) {
                compiler.compileDocument(document);
            }
        }
    });

    // 注册所有提供者到上下文
    context.subscriptions.push(
        ...commands,
        documentFormattingProvider,
        completionProvider,
        hoverProvider,
        definitionProvider,
        referenceProvider,
        renameProvider,
        diagnosticProvider,
        fileWatcher,
        configWatcher,
        saveWatcher,
        languageClient
    );

    // 启动语言客户端
    languageClient.start();

    // 显示欢迎消息
    vscode.window.showInformationMessage('CHTL Language extension activated!');
}

export function deactivate() {
    console.log('CHTL Language extension is now deactivated');
}