import * as vscode from 'vscode';
import { CHTLCompiler } from './compiler';

export class CHTLPreview {
    private compiler: CHTLCompiler;
    private previewPanel: vscode.WebviewPanel | undefined;
    private config: vscode.WorkspaceConfiguration;

    constructor() {
        this.compiler = new CHTLCompiler();
        this.config = vscode.workspace.getConfiguration('chtl');
    }

    public async show(): Promise<void> {
        const editor = vscode.window.activeTextEditor;
        if (!editor || editor.document.languageId !== 'chtl') {
            vscode.window.showWarningMessage('Please open a CHTL file to preview');
            return;
        }

        const document = editor.document;
        const compiledContent = await this.compiler.compileDocument(document);
        
        if (!compiledContent) {
            vscode.window.showErrorMessage('Failed to compile CHTL document');
            return;
        }

        // 创建或更新预览面板
        if (this.previewPanel) {
            this.previewPanel.webview.html = this.getWebviewContent(compiledContent);
            this.previewPanel.reveal();
        } else {
            this.previewPanel = vscode.window.createWebviewPanel(
                'chtlPreview',
                'CHTL Preview',
                vscode.ViewColumn.Two,
                {
                    enableScripts: true,
                    retainContextWhenHidden: true
                }
            );

            this.previewPanel.webview.html = this.getWebviewContent(compiledContent);

            this.previewPanel.onDidDispose(() => {
                this.previewPanel = undefined;
            });
        }

        // 监听文档变化
        const changeListener = vscode.workspace.onDidChangeTextDocument(event => {
            if (event.document === document) {
                this.updatePreview(document);
            }
        });

        this.previewPanel.onDidDispose(() => {
            changeListener.dispose();
        });
    }

    private async updatePreview(document: vscode.TextDocument): Promise<void> {
        if (!this.previewPanel) {
            return;
        }

        const compiledContent = await this.compiler.compileDocument(document);
        if (compiledContent) {
            this.previewPanel.webview.html = this.getWebviewContent(compiledContent);
        }
    }

    private getWebviewContent(htmlContent: string): string {
        return `
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CHTL Preview</title>
    <style>
        body {
            margin: 0;
            padding: 20px;
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            line-height: 1.6;
        }
        .preview-container {
            max-width: 1200px;
            margin: 0 auto;
        }
        .preview-header {
            background: #f5f5f5;
            padding: 10px 20px;
            border-radius: 4px;
            margin-bottom: 20px;
            font-size: 14px;
            color: #666;
        }
        .preview-content {
            border: 1px solid #ddd;
            border-radius: 4px;
            overflow: hidden;
        }
        iframe {
            width: 100%;
            height: 600px;
            border: none;
        }
    </style>
</head>
<body>
    <div class="preview-container">
        <div class="preview-header">
            CHTL Preview - ${new Date().toLocaleTimeString()}
        </div>
        <div class="preview-content">
            <iframe srcdoc="${this.escapeHtml(htmlContent)}"></iframe>
        </div>
    </div>
</body>
</html>`;
    }

    private escapeHtml(html: string): string {
        return html
            .replace(/&/g, '&amp;')
            .replace(/</g, '&lt;')
            .replace(/>/g, '&gt;')
            .replace(/"/g, '&quot;')
            .replace(/'/g, '&#39;');
    }
}