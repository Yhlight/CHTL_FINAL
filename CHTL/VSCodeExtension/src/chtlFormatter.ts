import * as vscode from 'vscode';

export class CHTLFormatter {
    private indentSize: number;

    constructor() {
        this.indentSize = 2;
    }

    async format(source: string): Promise<string> {
        const config = vscode.workspace.getConfiguration('chtl');
        this.indentSize = config.get('formatting.indentSize', 2);

        const lines = source.split('\n');
        const formattedLines: string[] = [];
        let indentLevel = 0;
        let inBlock = false;
        let inString = false;
        let stringChar = '';

        for (let i = 0; i < lines.length; i++) {
            let line = lines[i].trim();
            
            if (line === '') {
                formattedLines.push('');
                continue;
            }

            // 跳过注释
            if (line.startsWith('//') || line.startsWith('/*') || line.startsWith('--')) {
                formattedLines.push(this.indentLine(line, indentLevel));
                continue;
            }

            // 处理字符串
            for (let j = 0; j < line.length; j++) {
                const char = line[j];
                if (!inString && (char === '"' || char === "'")) {
                    inString = true;
                    stringChar = char;
                } else if (inString && char === stringChar && line[j - 1] !== '\\') {
                    inString = false;
                    stringChar = '';
                }
            }

            // 检查是否在块内
            if (line.includes('{') && !inString) {
                inBlock = true;
            }
            if (line.includes('}') && !inString) {
                inBlock = false;
                indentLevel = Math.max(0, indentLevel - 1);
            }

            // 格式化行
            const formattedLine = this.formatLine(line, indentLevel, inBlock);
            formattedLines.push(formattedLine);

            // 增加缩进
            if (line.includes('{') && !inString) {
                indentLevel++;
            }
        }

        return formattedLines.join('\n');
    }

    private indentLine(line: string, level: number): string {
        const indent = ' '.repeat(level * this.indentSize);
        return indent + line;
    }

    private formatLine(line: string, indentLevel: number, inBlock: boolean): string {
        const indent = ' '.repeat(indentLevel * this.indentSize);
        
        // 处理元素定义
        if (line.includes(':') && !line.includes('{') && !line.includes('}')) {
            return this.formatElementLine(line, indent);
        }
        
        // 处理块定义
        if (line.includes('{')) {
            return this.formatBlockLine(line, indent);
        }
        
        // 处理属性
        if (inBlock && line.includes(':') && !line.includes('{')) {
            return this.formatAttributeLine(line, indent);
        }
        
        // 处理模板关键字
        if (line.startsWith('[') && line.includes(']')) {
            return this.formatTemplateLine(line, indent);
        }
        
        // 处理use语句
        if (line.startsWith('use ')) {
            return this.formatUseLine(line, indent);
        }
        
        return indent + line;
    }

    private formatElementLine(line: string, indent: string): string {
        // 元素: 文本内容
        const parts = line.split(':');
        if (parts.length === 2) {
            const element = parts[0].trim();
            const content = parts[1].trim();
            return `${indent}${element}: ${content}`;
        }
        return indent + line;
    }

    private formatBlockLine(line: string, indent: string): string {
        // 元素 { 或 元素: 文本 {
        if (line.includes('{')) {
            const beforeBrace = line.substring(0, line.indexOf('{')).trim();
            const afterBrace = line.substring(line.indexOf('{')).trim();
            
            if (afterBrace === '{') {
                return `${indent}${beforeBrace} {`;
            }
        }
        return indent + line;
    }

    private formatAttributeLine(line: string, indent: string): string {
        // 属性: 值
        const parts = line.split(':');
        if (parts.length === 2) {
            const attribute = parts[0].trim();
            const value = parts[1].trim();
            return `${indent}    ${attribute}: ${value}`;
        }
        return indent + line;
    }

    private formatTemplateLine(line: string, indent: string): string {
        // [Template] 名称
        const match = line.match(/\[(\w+)\]\s*(.*)/);
        if (match) {
            const keyword = match[1];
            const name = match[2].trim();
            return `${indent}[${keyword}] ${name}`;
        }
        return indent + line;
    }

    private formatUseLine(line: string, indent: string): string {
        // use 类型
        const parts = line.split(' ');
        if (parts.length >= 2) {
            const type = parts[1].trim();
            return `${indent}use ${type}`;
        }
        return indent + line;
    }

    async validateFormatting(source: string): Promise<boolean> {
        try {
            const formatted = await this.format(source);
            return formatted === source;
        } catch (error) {
            return false;
        }
    }

    async getFormattingInfo(): Promise<any> {
        return {
            supportedFeatures: [
                'Element formatting',
                'Block formatting',
                'Attribute formatting',
                'Template formatting',
                'Use statement formatting',
                'Comment preservation'
            ],
            indentSize: this.indentSize,
            version: '1.0.0'
        };
    }
}