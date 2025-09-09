import * as vscode from 'vscode';
import { CHTLParser } from './parser';

export class CHTLFormatter implements vscode.DocumentFormattingEditProvider {
    private parser: CHTLParser;
    private config: vscode.WorkspaceConfiguration;

    constructor() {
        this.parser = new CHTLParser();
        this.config = vscode.workspace.getConfiguration('chtl');
    }

    public reloadConfiguration(): void {
        this.config = vscode.workspace.getConfiguration('chtl');
    }

    public async provideDocumentFormattingEdits(
        document: vscode.TextDocument,
        options: vscode.FormattingOptions,
        token: vscode.CancellationToken
    ): Promise<vscode.TextEdit[]> {
        if (!this.config.get('formatting.enabled', true)) {
            return [];
        }

        const text = document.getText();
        const formattedText = this.formatText(text, options);
        
        if (formattedText === text) {
            return [];
        }

        const fullRange = new vscode.Range(
            document.positionAt(0),
            document.positionAt(text.length)
        );

        return [vscode.TextEdit.replace(fullRange, formattedText)];
    }

    public async formatDocument(document: vscode.TextDocument): Promise<void> {
        const edits = await this.provideDocumentFormattingEdits(
            document,
            { insertSpaces: true, tabSize: 2 },
            new vscode.CancellationTokenSource().token
        );

        if (edits.length > 0) {
            const edit = new vscode.WorkspaceEdit();
            edit.set(document.uri, edits);
            await vscode.workspace.applyEdit(edit);
        }
    }

    private formatText(text: string, options: vscode.FormattingOptions): string {
        const lines = text.split('\n');
        const formattedLines: string[] = [];
        let indentLevel = 0;
        const indentSize = this.config.get('formatting.indentSize', 2);
        const useTabs = this.config.get('formatting.useTabs', false);
        const indentChar = useTabs ? '\t' : ' '.repeat(indentSize);

        for (let i = 0; i < lines.length; i++) {
            let line = lines[i];
            const trimmedLine = line.trim();

            // 跳过空行
            if (trimmedLine === '') {
                formattedLines.push('');
                continue;
            }

            // 处理注释
            if (trimmedLine.startsWith('//') || trimmedLine.startsWith('/*') || trimmedLine.startsWith('--')) {
                formattedLines.push(indentChar.repeat(indentLevel) + trimmedLine);
                continue;
            }

            // 处理结束括号
            if (trimmedLine === '}' || trimmedLine === ']' || trimmedLine === ')') {
                indentLevel = Math.max(0, indentLevel - 1);
                formattedLines.push(indentChar.repeat(indentLevel) + trimmedLine);
                continue;
            }

            // 处理开始括号
            if (trimmedLine.endsWith('{') || trimmedLine.endsWith('[') || trimmedLine.endsWith('(')) {
                formattedLines.push(indentChar.repeat(indentLevel) + trimmedLine);
                indentLevel++;
                continue;
            }

            // 处理分号结尾的语句
            if (trimmedLine.endsWith(';')) {
                formattedLines.push(indentChar.repeat(indentLevel) + trimmedLine);
                continue;
            }

            // 处理其他行
            formattedLines.push(indentChar.repeat(indentLevel) + trimmedLine);
        }

        return formattedLines.join('\n');
    }
}