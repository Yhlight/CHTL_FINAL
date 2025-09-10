import * as vscode from 'vscode';
import { CHTLCompiler } from './chtlCompiler';
import { CHTLFormatter } from './chtlFormatter';
import { CHTLPreviewProvider } from './chtlPreviewProvider';
import { CHTLLivePreview } from './chtlLivePreview';
import { CHTLModuleUnpacker } from './chtlModuleUnpacker';
import { CHTLCompletionProvider } from './chtlCompletionProvider';
import { CHTLHoverProvider } from './chtlHoverProvider';
import { CHTLDefinitionProvider } from './chtlDefinitionProvider';

export function activate(context: vscode.ExtensionContext) {
    console.log('CHTL Language Support extension is now active!');

    // 初始化组件
    const compiler = new CHTLCompiler();
    const formatter = new CHTLFormatter();
    const previewProvider = new CHTLPreviewProvider();
    const livePreview = new CHTLLivePreview();
    const moduleUnpacker = new CHTLModuleUnpacker();
    const completionProvider = new CHTLCompletionProvider();
    const hoverProvider = new CHTLHoverProvider();
    const definitionProvider = new CHTLDefinitionProvider();

    // 注册命令
    const compileCommand = vscode.commands.registerCommand('chtl.compile', async () => {
        const editor = vscode.window.activeTextEditor;
        if (!editor || editor.document.languageId !== 'chtl') {
            vscode.window.showWarningMessage('Please open a CHTL file to compile.');
            return;
        }

        try {
            const result = await compiler.compile(editor.document.getText());
            if (result.success) {
                vscode.window.showInformationMessage('CHTL compiled successfully!');
                
                // 创建输出文档
                const doc = await vscode.workspace.openTextDocument({
                    content: result.html,
                    language: 'html'
                });
                await vscode.window.showTextDocument(doc);
            } else {
                vscode.window.showErrorMessage(`Compilation failed: ${result.error}`);
            }
        } catch (error) {
            vscode.window.showErrorMessage(`Compilation error: ${error}`);
        }
    });

    const previewCommand = vscode.commands.registerCommand('chtl.preview', async () => {
        const editor = vscode.window.activeTextEditor;
        if (!editor || editor.document.languageId !== 'chtl') {
            vscode.window.showWarningMessage('Please open a CHTL file to preview.');
            return;
        }

        try {
            const result = await compiler.compile(editor.document.getText());
            if (result.success) {
                await previewProvider.showPreview(result.html);
            } else {
                vscode.window.showErrorMessage(`Preview failed: ${result.error}`);
            }
        } catch (error) {
            vscode.window.showErrorMessage(`Preview error: ${error}`);
        }
    });

    const formatCommand = vscode.commands.registerCommand('chtl.format', async () => {
        const editor = vscode.window.activeTextEditor;
        if (!editor || editor.document.languageId !== 'chtl') {
            vscode.window.showWarningMessage('Please open a CHTL file to format.');
            return;
        }

        try {
            const formatted = await formatter.format(editor.document.getText());
            const edit = new vscode.WorkspaceEdit();
            edit.replace(editor.document.uri, new vscode.Range(0, 0, editor.document.lineCount, 0), formatted);
            await vscode.workspace.applyEdit(edit);
            vscode.window.showInformationMessage('CHTL formatted successfully!');
        } catch (error) {
            vscode.window.showErrorMessage(`Formatting error: ${error}`);
        }
    });

    const livePreviewCommand = vscode.commands.registerCommand('chtl.livePreview', async () => {
        const editor = vscode.window.activeTextEditor;
        if (!editor || editor.document.languageId !== 'chtl') {
            vscode.window.showWarningMessage('Please open a CHTL file for live preview.');
            return;
        }

        try {
            await livePreview.startLivePreview(editor.document.uri);
            vscode.window.showInformationMessage('Live preview started!');
        } catch (error) {
            vscode.window.showErrorMessage(`Live preview error: ${error}`);
        }
    });

    const unpackModuleCommand = vscode.commands.registerCommand('chtl.unpackModule', async () => {
        const editor = vscode.window.activeTextEditor;
        if (!editor || editor.document.languageId !== 'chtl') {
            vscode.window.showWarningMessage('Please open a CHTL file to unpack modules.');
            return;
        }

        try {
            const modules = await moduleUnpacker.unpackModules(editor.document.getText());
            if (modules.length > 0) {
                vscode.window.showInformationMessage(`Found ${modules.length} modules to unpack.`);
                // 显示模块列表
                const moduleNames = modules.map(m => m.name);
                const selected = await vscode.window.showQuickPick(moduleNames, {
                    placeHolder: 'Select modules to unpack'
                });
                if (selected) {
                    await moduleUnpacker.unpackSelectedModules(modules.filter(m => m.name === selected));
                }
            } else {
                vscode.window.showInformationMessage('No modules found to unpack.');
            }
        } catch (error) {
            vscode.window.showErrorMessage(`Module unpacking error: ${error}`);
        }
    });

    // 注册语言特性提供者
    const completionDisposable = vscode.languages.registerCompletionItemProvider(
        'chtl',
        completionProvider,
        '.', ':', '=', '[', '{'
    );

    const hoverDisposable = vscode.languages.registerHoverProvider(
        'chtl',
        hoverProvider
    );

    const definitionDisposable = vscode.languages.registerDefinitionProvider(
        'chtl',
        definitionProvider
    );

    // 注册格式化提供者
    const formatDisposable = vscode.languages.registerDocumentFormattingEditProvider(
        'chtl',
        {
            provideDocumentFormattingEdits: async (document: vscode.TextDocument) => {
                try {
                    const formatted = await formatter.format(document.getText());
                    return [vscode.TextEdit.replace(
                        new vscode.Range(0, 0, document.lineCount, 0),
                        formatted
                    )];
                } catch (error) {
                    console.error('Formatting error:', error);
                    return [];
                }
            }
        }
    );

    // 注册文档变化监听器（用于实时预览）
    const documentChangeDisposable = vscode.workspace.onDidChangeTextDocument(async (event) => {
        if (event.document.languageId === 'chtl' && livePreview.isActive()) {
            try {
                const result = await compiler.compile(event.document.getText());
                if (result.success) {
                    await livePreview.updatePreview(result.html);
                }
            } catch (error) {
                console.error('Live preview update error:', error);
            }
        }
    });

    // 注册文件保存监听器（用于自动编译）
    const saveDisposable = vscode.workspace.onDidSaveTextDocument(async (document) => {
        if (document.languageId === 'chtl') {
            const config = vscode.workspace.getConfiguration('chtl');
            if (config.get('compiler.autoCompile', false)) {
                try {
                    const result = await compiler.compile(document.getText());
                    if (result.success) {
                        // 保存编译结果
                        const outputPath = document.uri.fsPath.replace('.chtl', '.html');
                        await vscode.workspace.fs.writeFile(
                            vscode.Uri.file(outputPath),
                            Buffer.from(result.html)
                        );
                    }
                } catch (error) {
                    console.error('Auto compilation error:', error);
                }
            }
        }
    });

    // 注册所有disposables
    context.subscriptions.push(
        compileCommand,
        previewCommand,
        formatCommand,
        livePreviewCommand,
        unpackModuleCommand,
        completionDisposable,
        hoverDisposable,
        definitionDisposable,
        formatDisposable,
        documentChangeDisposable,
        saveDisposable
    );
}

export function deactivate() {
    console.log('CHTL Language Support extension is now deactivated!');
}