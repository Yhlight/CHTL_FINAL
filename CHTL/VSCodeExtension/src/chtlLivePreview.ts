import * as vscode from 'vscode';
import * as http from 'http';
import * as path from 'path';
import * as fs from 'fs';
import { CHTLCompiler } from './chtlCompiler';

export class CHTLLivePreview {
    private server: http.Server | undefined;
    private port: number;
    private isActiveFlag: boolean;
    private compiler: CHTLCompiler;
    private currentDocument: vscode.Uri | undefined;

    constructor() {
        this.port = 3000;
        this.isActiveFlag = false;
        this.compiler = new CHTLCompiler();
    }

    async startLivePreview(documentUri: vscode.Uri): Promise<void> {
        if (this.isActiveFlag) {
            await this.stopLivePreview();
        }

        this.currentDocument = documentUri;
        this.isActiveFlag = true;

        // 获取配置
        const config = vscode.workspace.getConfiguration('chtl');
        this.port = config.get('preview.port', 3000);

        // 启动服务器
        await this.startServer();

        // 打开浏览器
        const url = `http://localhost:${this.port}`;
        await vscode.env.openExternal(vscode.Uri.parse(url));

        vscode.window.showInformationMessage(`Live preview started at ${url}`);
    }

    async stopLivePreview(): Promise<void> {
        if (this.server) {
            this.server.close();
            this.server = undefined;
        }
        this.isActiveFlag = false;
        this.currentDocument = undefined;
    }

    async updatePreview(html: string): Promise<void> {
        if (!this.isActiveFlag || !this.server) {
            return;
        }

        // 更新服务器内容
        this.updateServerContent(html);
    }

    isActive(): boolean {
        return this.isActiveFlag;
    }

    private async startServer(): Promise<void> {
        return new Promise((resolve, reject) => {
            this.server = http.createServer((req, res) => {
                this.handleRequest(req, res);
            });

            this.server.listen(this.port, 'localhost', () => {
                console.log(`Live preview server started on port ${this.port}`);
                resolve();
            });

            this.server.on('error', (err) => {
                if ((err as any).code === 'EADDRINUSE') {
                    this.port++;
                    this.startServer().then(resolve).catch(reject);
                } else {
                    reject(err);
                }
            });
        });
    }

    private handleRequest(req: http.IncomingMessage, res: http.ServerResponse): void {
        const url = req.url || '/';

        if (url === '/') {
            this.servePreviewPage(res);
        } else if (url === '/api/refresh') {
            this.handleRefreshRequest(req, res);
        } else if (url.startsWith('/api/content')) {
            this.handleContentRequest(req, res);
        } else {
            this.serve404(res);
        }
    }

    private servePreviewPage(res: http.ServerResponse): void {
        const html = this.generateLivePreviewHTML();
        res.writeHead(200, { 'Content-Type': 'text/html' });
        res.end(html);
    }

    private handleRefreshRequest(req: http.IncomingMessage, res: http.ServerResponse): void {
        if (req.method === 'POST' && this.currentDocument) {
            this.refreshContent().then((content) => {
                res.writeHead(200, { 'Content-Type': 'application/json' });
                res.end(JSON.stringify({ success: true, content }));
            }).catch((error) => {
                res.writeHead(500, { 'Content-Type': 'application/json' });
                res.end(JSON.stringify({ success: false, error: error.message }));
            });
        } else {
            res.writeHead(400, { 'Content-Type': 'application/json' });
            res.end(JSON.stringify({ success: false, error: 'Invalid request' }));
        }
    }

    private handleContentRequest(req: http.IncomingMessage, res: http.ServerResponse): void {
        this.refreshContent().then((content) => {
            res.writeHead(200, { 'Content-Type': 'text/html' });
            res.end(content);
        }).catch((error) => {
            res.writeHead(500, { 'Content-Type': 'text/html' });
            res.end(`<html><body><h1>Error</h1><p>${error.message}</p></body></html>`);
        });
    }

    private serve404(res: http.ServerResponse): void {
        res.writeHead(404, { 'Content-Type': 'text/html' });
        res.end('<html><body><h1>404 Not Found</h1></body></html>');
    }

    private async refreshContent(): Promise<string> {
        if (!this.currentDocument) {
            throw new Error('No document loaded');
        }

        try {
            const document = await vscode.workspace.openTextDocument(this.currentDocument);
            const result = await this.compiler.compile(document.getText());
            
            if (result.success) {
                return result.html;
            } else {
                throw new Error(result.error || 'Compilation failed');
            }
        } catch (error) {
            throw new Error(`Failed to refresh content: ${error}`);
        }
    }

    private updateServerContent(html: string): void {
        // 这里可以实现WebSocket或其他实时更新机制
        // 目前使用简单的轮询方式
    }

    private generateLivePreviewHTML(): string {
        return `<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CHTL Live Preview</title>
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
        .status-indicator {
            display: inline-block;
            width: 10px;
            height: 10px;
            border-radius: 50%;
            background: #28a745;
            margin-right: 8px;
            animation: pulse 2s infinite;
        }
        @keyframes pulse {
            0% { opacity: 1; }
            50% { opacity: 0.5; }
            100% { opacity: 1; }
        }
        .preview-frame {
            width: 100%;
            height: 600px;
            border: 1px solid #ddd;
            border-radius: 4px;
        }
        .error-message {
            color: #d73a49;
            background: #ffeef0;
            border: 1px solid #fdbdbd;
            padding: 15px;
            border-radius: 4px;
            margin: 20px 0;
        }
    </style>
</head>
<body>
    <div class="preview-container">
        <div class="preview-header">
            <span>
                <span class="status-indicator"></span>
                CHTL Live Preview
            </span>
            <span>Port: ${this.port}</span>
        </div>
        <div class="preview-content">
            <div id="preview-status"></div>
            <iframe id="preview-frame" class="preview-frame" src="/api/content"></iframe>
        </div>
    </div>

    <script>
        let refreshInterval;
        
        function startAutoRefresh() {
            refreshInterval = setInterval(refreshPreview, 1000);
        }
        
        function stopAutoRefresh() {
            if (refreshInterval) {
                clearInterval(refreshInterval);
            }
        }
        
        async function refreshPreview() {
            try {
                const response = await fetch('/api/refresh', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json'
                    }
                });
                
                const result = await response.json();
                
                if (result.success) {
                    const frame = document.getElementById('preview-frame');
                    frame.srcdoc = result.content;
                    updateStatus('Preview updated', 'success');
                } else {
                    updateStatus('Error: ' + result.error, 'error');
                }
            } catch (error) {
                updateStatus('Connection error: ' + error.message, 'error');
            }
        }
        
        function updateStatus(message, type) {
            const status = document.getElementById('preview-status');
            if (type === 'error') {
                status.className = 'error-message';
                status.textContent = message;
            } else {
                status.className = '';
                status.textContent = message;
            }
        }
        
        // 页面加载完成后开始自动刷新
        document.addEventListener('DOMContentLoaded', () => {
            startAutoRefresh();
        });
        
        // 页面卸载时停止自动刷新
        window.addEventListener('beforeunload', () => {
            stopAutoRefresh();
        });
    </script>
</body>
</html>`;
    }

    dispose(): void {
        this.stopLivePreview();
    }
}