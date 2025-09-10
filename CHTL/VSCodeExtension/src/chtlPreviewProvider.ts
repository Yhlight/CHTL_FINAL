import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';

export class CHTLPreviewProvider {
    private panel: vscode.WebviewPanel | undefined;
    private compiler: any;

    constructor() {
        // 这里可以注入编译器依赖
    }

    async showPreview(html: string): Promise<void> {
        if (this.panel) {
            this.panel.reveal();
            await this.updatePreview(html);
            return;
        }

        this.panel = vscode.window.createWebviewPanel(
            'chtlPreview',
            'CHTL Preview',
            vscode.ViewColumn.Two,
            {
                enableScripts: true,
                retainContextWhenHidden: true
            }
        );

        this.panel.onDidDispose(() => {
            this.panel = undefined;
        });

        await this.updatePreview(html);
    }

    async updatePreview(html: string): Promise<void> {
        if (!this.panel) {
            return;
        }

        const webview = this.panel.webview;
        const htmlContent = this.generatePreviewHTML(html);
        webview.html = htmlContent;
    }

    private generatePreviewHTML(content: string): string {
        return `<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CHTL Preview</title>
    <style>
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            margin: 0;
            padding: 20px;
            background-color: #f5f5f5;
        }
        .preview-container {
            max-width: 1200px;
            margin: 0 auto;
            background: white;
            border-radius: 8px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
            overflow: hidden;
        }
        .preview-header {
            background: #007acc;
            color: white;
            padding: 15px 20px;
            font-weight: 600;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }
        .preview-content {
            padding: 20px;
            min-height: 400px;
        }
        .preview-actions {
            display: flex;
            gap: 10px;
        }
        .btn {
            background: rgba(255,255,255,0.2);
            border: 1px solid rgba(255,255,255,0.3);
            color: white;
            padding: 5px 10px;
            border-radius: 4px;
            cursor: pointer;
            font-size: 12px;
        }
        .btn:hover {
            background: rgba(255,255,255,0.3);
        }
        .preview-frame {
            width: 100%;
            height: 600px;
            border: none;
            background: white;
        }
        .error-message {
            color: #d73a49;
            background: #ffeef0;
            border: 1px solid #fdbdbd;
            padding: 15px;
            border-radius: 4px;
            margin: 20px 0;
        }
        .success-message {
            color: #28a745;
            background: #f0fff4;
            border: 1px solid #c3e6cb;
            padding: 15px;
            border-radius: 4px;
            margin: 20px 0;
        }
    </style>
</head>
<body>
    <div class="preview-container">
        <div class="preview-header">
            <span>CHTL Preview</span>
            <div class="preview-actions">
                <button class="btn" onclick="refreshPreview()">Refresh</button>
                <button class="btn" onclick="openInBrowser()">Open in Browser</button>
                <button class="btn" onclick="copyHTML()">Copy HTML</button>
            </div>
        </div>
        <div class="preview-content">
            <div id="preview-status" class="success-message">
                Preview loaded successfully
            </div>
            <iframe id="preview-frame" class="preview-frame" srcdoc="${this.escapeHtml(content)}"></iframe>
        </div>
    </div>

    <script>
        const vscode = acquireVsCodeApi();
        
        function refreshPreview() {
            vscode.postMessage({
                command: 'refresh'
            });
        }
        
        function openInBrowser() {
            vscode.postMessage({
                command: 'openInBrowser'
            });
        }
        
        function copyHTML() {
            vscode.postMessage({
                command: 'copyHTML'
            });
        }
        
        // 监听来自扩展的消息
        window.addEventListener('message', event => {
            const message = event.data;
            switch (message.command) {
                case 'updateContent':
                    updatePreviewContent(message.content);
                    break;
                case 'showError':
                    showError(message.error);
                    break;
            }
        });
        
        function updatePreviewContent(content) {
            const frame = document.getElementById('preview-frame');
            frame.srcdoc = content;
            showSuccess('Preview updated successfully');
        }
        
        function showError(error) {
            const status = document.getElementById('preview-status');
            status.className = 'error-message';
            status.textContent = 'Error: ' + error;
        }
        
        function showSuccess(message) {
            const status = document.getElementById('preview-status');
            status.className = 'success-message';
            status.textContent = message;
        }
    </script>
</body>
</html>`;
    }

    private escapeHtml(text: string): string {
        const map: { [key: string]: string } = {
            '&': '&amp;',
            '<': '&lt;',
            '>': '&gt;',
            '"': '&quot;',
            "'": '&#039;'
        };
        return text.replace(/[&<>"']/g, (m) => map[m]);
    }

    async savePreview(html: string, filePath: string): Promise<boolean> {
        try {
            await fs.promises.writeFile(filePath, html, 'utf8');
            return true;
        } catch (error) {
            console.error('Save preview error:', error);
            return false;
        }
    }

    async exportPreview(html: string, format: 'html' | 'pdf' = 'html'): Promise<string | null> {
        try {
            if (format === 'html') {
                return html;
            } else if (format === 'pdf') {
                // 这里可以集成PDF生成库
                throw new Error('PDF export not implemented yet');
            }
            return null;
        } catch (error) {
            console.error('Export preview error:', error);
            return null;
        }
    }

    dispose(): void {
        if (this.panel) {
            this.panel.dispose();
            this.panel = undefined;
        }
    }
}