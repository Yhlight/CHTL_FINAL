import * as vscode from 'vscode';
import { spawn } from 'child_process';
import * as path from 'path';

export interface CompileResult {
    success: boolean;
    html: string;
    css: string;
    javascript: string;
    error?: string;
}

export class CHTLCompiler {
    private compilerPath: string;

    constructor() {
        // 假设CHTL编译器在系统PATH中或特定位置
        this.compilerPath = 'chtl_cli';
    }

    async compile(source: string): Promise<CompileResult> {
        return new Promise((resolve) => {
            try {
                // 创建临时文件
                const tempFile = path.join(__dirname, 'temp.chtl');
                require('fs').writeFileSync(tempFile, source);

                // 运行CHTL编译器
                const process = spawn(this.compilerPath, [tempFile, '-o', 'temp.html'], {
                    cwd: __dirname
                });

                let output = '';
                let error = '';

                process.stdout.on('data', (data) => {
                    output += data.toString();
                });

                process.stderr.on('data', (data) => {
                    error += data.toString();
                });

                process.on('close', (code) => {
                    // 清理临时文件
                    try {
                        require('fs').unlinkSync(tempFile);
                    } catch (e) {
                        // 忽略清理错误
                    }

                    if (code === 0) {
                        // 读取输出文件
                        try {
                            const html = require('fs').readFileSync(path.join(__dirname, 'temp.html'), 'utf8');
                            // 清理输出文件
                            try {
                                require('fs').unlinkSync(path.join(__dirname, 'temp.html'));
                            } catch (e) {
                                // 忽略清理错误
                            }

                            resolve({
                                success: true,
                                html: html,
                                css: '',
                                javascript: ''
                            });
                        } catch (e) {
                            resolve({
                                success: false,
                                html: '',
                                css: '',
                                javascript: '',
                                error: 'Failed to read output file'
                            });
                        }
                    } else {
                        resolve({
                            success: false,
                            html: '',
                            css: '',
                            javascript: '',
                            error: error || 'Compilation failed'
                        });
                    }
                });

                process.on('error', (err) => {
                    resolve({
                        success: false,
                        html: '',
                        css: '',
                        javascript: '',
                        error: `Failed to start compiler: ${err.message}`
                    });
                });

            } catch (error) {
                resolve({
                    success: false,
                    html: '',
                    css: '',
                    javascript: '',
                    error: `Compilation error: ${error}`
                });
            }
        });
    }

    async compileToFile(source: string, outputPath: string): Promise<boolean> {
        try {
            const result = await this.compile(source);
            if (result.success) {
                require('fs').writeFileSync(outputPath, result.html);
                return true;
            }
            return false;
        } catch (error) {
            console.error('Compile to file error:', error);
            return false;
        }
    }

    async getCompileInfo(): Promise<any> {
        return {
            version: '1.0.0',
            features: [
                'CHTL Core',
                'CHTL JS',
                'CSS Compilation',
                'JavaScript Compilation',
                'Module System',
                'Template System'
            ],
            supportedFormats: ['.chtl', '.chtljs']
        };
    }
}