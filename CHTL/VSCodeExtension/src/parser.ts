import * as vscode from 'vscode';

export class CHTLParser {
    private config: vscode.WorkspaceConfiguration;

    constructor() {
        this.config = vscode.workspace.getConfiguration('chtl');
    }

    public parse(text: string): any {
        // 简化的解析器实现
        // 实际实现应该使用完整的CHTL解析器
        return {
            type: 'CHTLDocument',
            content: text,
            elements: this.extractElements(text),
            templates: this.extractTemplates(text),
            imports: this.extractImports(text)
        };
    }

    private extractElements(text: string): any[] {
        const elements: any[] = [];
        const lines = text.split('\n');

        for (let i = 0; i < lines.length; i++) {
            const line = lines[i].trim();
            if (this.isElementLine(line)) {
                elements.push({
                    type: 'element',
                    name: this.getElementName(line),
                    line: i + 1,
                    content: line
                });
            }
        }

        return elements;
    }

    private extractTemplates(text: string): any[] {
        const templates: any[] = [];
        const templateRegex = /\[Template\]\s+@(\w+)\s+(\w+)/g;
        let match;

        while ((match = templateRegex.exec(text)) !== null) {
            templates.push({
                type: 'template',
                templateType: match[1],
                name: match[2],
                position: match.index
            });
        }

        return templates;
    }

    private extractImports(text: string): any[] {
        const imports: any[] = [];
        const importRegex = /\[Import\]\s+"([^"]+)"/g;
        let match;

        while ((match = importRegex.exec(text)) !== null) {
            imports.push({
                type: 'import',
                path: match[1],
                position: match.index
            });
        }

        return imports;
    }

    private isElementLine(line: string): boolean {
        // 简化的元素行检测
        return /^\s*[a-zA-Z][a-zA-Z0-9]*\s*\{?\s*$/.test(line);
    }

    private getElementName(line: string): string {
        const match = line.match(/^\s*([a-zA-Z][a-zA-Z0-9]*)/);
        return match ? match[1] : '';
    }
}