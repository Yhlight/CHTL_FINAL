import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';

export class CHTLDefinitionProvider implements vscode.DefinitionProvider {
    async provideDefinition(
        document: vscode.TextDocument,
        position: vscode.Position,
        token: vscode.CancellationToken
    ): Promise<vscode.Definition | vscode.LocationLink[]> {
        const word = document.getWordRangeAtPosition(position);
        if (!word) {
            return [];
        }

        const text = document.getText(word);
        const line = document.lineAt(position).text;
        
        // 检查是否是模板引用
        if (line.includes('@') && line.includes(text)) {
            return this.findTemplateDefinition(document, text);
        }
        
        // 检查是否是导入引用
        if (line.includes('from') && line.includes(text)) {
            return this.findImportDefinition(document, text);
        }
        
        // 检查是否是命名空间引用
        if (line.includes('::') && line.includes(text)) {
            return this.findNamespaceDefinition(document, text);
        }
        
        // 检查是否是配置引用
        if (line.includes('=') && line.includes(text)) {
            return this.findConfigurationDefinition(document, text);
        }

        return [];
    }

    private async findTemplateDefinition(
        document: vscode.TextDocument,
        templateName: string
    ): Promise<vscode.Definition> {
        const text = document.getText();
        const lines = text.split('\n');
        
        for (let i = 0; i < lines.length; i++) {
            const line = lines[i];
            const match = line.match(/\[Template\]\s+(\w+)/);
            if (match && match[1] === templateName) {
                const position = new vscode.Position(i, line.indexOf(templateName));
                return new vscode.Location(document.uri, position);
            }
        }
        
        return [];
    }

    private async findImportDefinition(
        document: vscode.TextDocument,
        importName: string
    ): Promise<vscode.Definition> {
        const text = document.getText();
        const lines = text.split('\n');
        
        for (let i = 0; i < lines.length; i++) {
            const line = lines[i];
            const match = line.match(/\[Import\]\s+(\w+)\s+from\s+['"]([^'"]+)['"]/);
            if (match && match[1] === importName) {
                const importPath = match[2];
                const workspaceFolder = vscode.workspace.getWorkspaceFolder(document.uri);
                
                if (workspaceFolder) {
                    const fullPath = path.join(workspaceFolder.uri.fsPath, importPath);
                    try {
                        const stat = await fs.promises.stat(fullPath);
                        if (stat.isFile()) {
                            const uri = vscode.Uri.file(fullPath);
                            return new vscode.Location(uri, new vscode.Position(0, 0));
                        }
                    } catch (error) {
                        // 文件不存在，返回空定义
                    }
                }
            }
        }
        
        return [];
    }

    private async findNamespaceDefinition(
        document: vscode.TextDocument,
        namespaceName: string
    ): Promise<vscode.Definition> {
        const text = document.getText();
        const lines = text.split('\n');
        
        for (let i = 0; i < lines.length; i++) {
            const line = lines[i];
            const match = line.match(/\[Namespace\]\s+(\w+)/);
            if (match && match[1] === namespaceName) {
                const position = new vscode.Position(i, line.indexOf(namespaceName));
                return new vscode.Location(document.uri, position);
            }
        }
        
        return [];
    }

    private async findConfigurationDefinition(
        document: vscode.TextDocument,
        configName: string
    ): Promise<vscode.Definition> {
        const text = document.getText();
        const lines = text.split('\n');
        
        for (let i = 0; i < lines.length; i++) {
            const line = lines[i];
            const match = line.match(/\[Configuration\]\s+(\w+)/);
            if (match && match[1] === configName) {
                const position = new vscode.Position(i, line.indexOf(configName));
                return new vscode.Location(document.uri, position);
            }
        }
        
        return [];
    }

    async findReferences(
        document: vscode.TextDocument,
        position: vscode.Position
    ): Promise<vscode.Location[]> {
        const word = document.getWordRangeAtPosition(position);
        if (!word) {
            return [];
        }

        const text = document.getText(word);
        const references: vscode.Location[] = [];
        const lines = document.getText().split('\n');
        
        for (let i = 0; i < lines.length; i++) {
            const line = lines[i];
            let index = 0;
            
            while ((index = line.indexOf(text, index)) !== -1) {
                const start = new vscode.Position(i, index);
                const end = new vscode.Position(i, index + text.length);
                const range = new vscode.Range(start, end);
                references.push(new vscode.Location(document.uri, range));
                index += text.length;
            }
        }
        
        return references;
    }

    async findImplementations(
        document: vscode.TextDocument,
        position: vscode.Position
    ): Promise<vscode.Definition> {
        const word = document.getWordRangeAtPosition(position);
        if (!word) {
            return [];
        }

        const text = document.getText(word);
        const line = document.lineAt(position).text;
        
        // 检查是否是模板引用
        if (line.includes('@') && line.includes(text)) {
            return this.findTemplateDefinition(document, text);
        }
        
        // 检查是否是自定义元素引用
        if (line.includes('[') && line.includes(']') && line.includes(text)) {
            return this.findCustomDefinition(document, text);
        }
        
        return [];
    }

    private async findCustomDefinition(
        document: vscode.TextDocument,
        customName: string
    ): Promise<vscode.Definition> {
        const text = document.getText();
        const lines = text.split('\n');
        
        for (let i = 0; i < lines.length; i++) {
            const line = lines[i];
            const match = line.match(/\[Custom\]\s+(\w+)/);
            if (match && match[1] === customName) {
                const position = new vscode.Position(i, line.indexOf(customName));
                return new vscode.Location(document.uri, position);
            }
        }
        
        return [];
    }
}