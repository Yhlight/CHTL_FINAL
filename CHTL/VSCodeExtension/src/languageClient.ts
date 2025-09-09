import * as vscode from 'vscode';
import { LanguageClient, LanguageClientOptions, ServerOptions, TransportKind } from 'vscode-languageclient/node';

export class CHTLLanguageClient {
    private client: LanguageClient;
    private context: vscode.ExtensionContext;

    constructor(context: vscode.ExtensionContext) {
        this.context = context;
        
        // 服务器选项
        const serverOptions: ServerOptions = {
            run: {
                module: context.asAbsolutePath('out/server/server.js'),
                transport: TransportKind.ipc
            },
            debug: {
                module: context.asAbsolutePath('out/server/server.js'),
                transport: TransportKind.ipc,
                options: {
                    execArgv: ['--nolazy', '--inspect=6009']
                }
            }
        };

        // 客户端选项
        const clientOptions: LanguageClientOptions = {
            documentSelector: [{ scheme: 'file', language: 'chtl' }],
            synchronize: {
                fileEvents: vscode.workspace.createFileSystemWatcher('**/*.chtl')
            }
        };

        this.client = new LanguageClient(
            'chtlLanguageServer',
            'CHTL Language Server',
            serverOptions,
            clientOptions
        );
    }

    public start(): void {
        this.client.start();
    }

    public stop(): Promise<void> {
        return this.client.stop();
    }

    public getClient(): LanguageClient {
        return this.client;
    }
}