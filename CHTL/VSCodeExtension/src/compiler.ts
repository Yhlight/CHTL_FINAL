import * as vscode from 'vscode';
import { exec } from 'child_process';
import { promisify } from 'util';

const execAsync = promisify(exec);

export class CHTLCompiler {
    private config: vscode.WorkspaceConfiguration;
    private outputChannel: vscode.OutputChannel;

    constructor() {
        this.config = vscode.workspace.getConfiguration('chtl');
        this.outputChannel = vscode.window.createOutputChannel('CHTL Compiler');
    }

    public reloadConfiguration(): void {
        this.config = vscode.workspace.getConfiguration('chtl');
    }

    public async compile(): Promise<void> {
        const editor = vscode.window.activeTextEditor;
        if (!editor || editor.document.languageId !== 'chtl') {
            vscode.window.showWarningMessage('Please open a CHTL file to compile');
            return;
        }

        const document = editor.document;
        await this.compileDocument(document);
    }

    public async compileDocument(document: vscode.TextDocument): Promise<string | null> {
        try {
            const compilerPath = this.config.get('compiler.path', 'chtl');
            const options = this.config.get('compiler.options', {});
            
            // 创建临时文件
            const tempFile = await this.createTempFile(document.getText());
            
            // 构建编译命令
            const command = this.buildCompileCommand(compilerPath, tempFile, options);
            
            // 执行编译
            const { stdout, stderr } = await execAsync(command);
            
            if (stderr) {
                this.outputChannel.appendLine(`Compiler warnings: ${stderr}`);
            }
            
            if (stdout) {
                this.outputChannel.appendLine(`Compilation successful: ${stdout}`);
                return stdout;
            }
            
            return null;
        } catch (error) {
            const errorMessage = `Compilation failed: ${error}`;
            this.outputChannel.appendLine(errorMessage);
            vscode.window.showErrorMessage(errorMessage);
            return null;
        }
    }

    public async watch(): Promise<void> {
        const editor = vscode.window.activeTextEditor;
        if (!editor || editor.document.languageId !== 'chtl') {
            vscode.window.showWarningMessage('Please open a CHTL file to watch');
            return;
        }

        const document = editor.document;
        const compilerPath = this.config.get('compiler.path', 'chtl');
        const options = this.config.get('compiler.options', {});
        
        // 启用监听模式
        options.watch = true;
        
        try {
            const tempFile = await this.createTempFile(document.getText());
            const command = this.buildCompileCommand(compilerPath, tempFile, options);
            
            this.outputChannel.appendLine(`Starting watch mode: ${command}`);
            this.outputChannel.show();
            
            // 执行监听命令
            const process = exec(command);
            
            process.stdout?.on('data', (data) => {
                this.outputChannel.appendLine(data.toString());
            });
            
            process.stderr?.on('data', (data) => {
                this.outputChannel.appendLine(data.toString());
            });
            
            process.on('close', (code) => {
                this.outputChannel.appendLine(`Watch mode ended with code: ${code}`);
            });
            
        } catch (error) {
            const errorMessage = `Watch mode failed: ${error}`;
            this.outputChannel.appendLine(errorMessage);
            vscode.window.showErrorMessage(errorMessage);
        }
    }

    public async build(): Promise<void> {
        try {
            const compilerPath = this.config.get('compiler.path', 'chtl');
            const options = this.config.get('compiler.options', {});
            
            // 构建项目
            const command = `${compilerPath} build`;
            
            this.outputChannel.appendLine(`Building project: ${command}`);
            this.outputChannel.show();
            
            const { stdout, stderr } = await execAsync(command);
            
            if (stderr) {
                this.outputChannel.appendLine(`Build warnings: ${stderr}`);
            }
            
            if (stdout) {
                this.outputChannel.appendLine(`Build successful: ${stdout}`);
                vscode.window.showInformationMessage('Build completed successfully');
            }
            
        } catch (error) {
            const errorMessage = `Build failed: ${error}`;
            this.outputChannel.appendLine(errorMessage);
            vscode.window.showErrorMessage(errorMessage);
        }
    }

    public async clean(): Promise<void> {
        try {
            const compilerPath = this.config.get('compiler.path', 'chtl');
            
            // 清理项目
            const command = `${compilerPath} clean`;
            
            this.outputChannel.appendLine(`Cleaning project: ${command}`);
            this.outputChannel.show();
            
            const { stdout, stderr } = await execAsync(command);
            
            if (stderr) {
                this.outputChannel.appendLine(`Clean warnings: ${stderr}`);
            }
            
            if (stdout) {
                this.outputChannel.appendLine(`Clean successful: ${stdout}`);
                vscode.window.showInformationMessage('Clean completed successfully');
            }
            
        } catch (error) {
            const errorMessage = `Clean failed: ${error}`;
            this.outputChannel.appendLine(errorMessage);
            vscode.window.showErrorMessage(errorMessage);
        }
    }

    private async createTempFile(content: string): Promise<string> {
        const tempDir = require('os').tmpdir();
        const tempFile = require('path').join(tempDir, `chtl_${Date.now()}.chtl`);
        
        await vscode.workspace.fs.writeFile(
            vscode.Uri.file(tempFile),
            Buffer.from(content, 'utf8')
        );
        
        return tempFile;
    }

    private buildCompileCommand(compilerPath: string, inputFile: string, options: any): string {
        let command = `${compilerPath} compile --input "${inputFile}"`;
        
        if (options.output) {
            command += ` --output "${options.output}"`;
        }
        
        if (options.minify) {
            command += ' --minify';
        }
        
        if (options.sourcemap) {
            command += ' --sourcemap';
        }
        
        if (options.watch) {
            command += ' --watch';
        }
        
        return command;
    }
}