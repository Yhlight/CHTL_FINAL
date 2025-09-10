import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';

export interface CHTLModule {
    name: string;
    type: 'Template' | 'Custom' | 'Origin' | 'Import' | 'Namespace' | 'Configuration';
    content: string;
    startLine: number;
    endLine: number;
}

export class CHTLModuleUnpacker {
    async unpackModules(source: string): Promise<CHTLModule[]> {
        const modules: CHTLModule[] = [];
        const lines = source.split('\n');
        
        let currentModule: CHTLModule | null = null;
        let braceCount = 0;
        let moduleStartLine = 0;
        
        for (let i = 0; i < lines.length; i++) {
            const line = lines[i].trim();
            
            // 检测模块开始
            if (line.startsWith('[') && line.includes(']')) {
                // 保存之前的模块
                if (currentModule) {
                    currentModule.endLine = i - 1;
                    modules.push(currentModule);
                }
                
                // 解析新模块
                const match = line.match(/\[(\w+)\]\s*(.*)/);
                if (match) {
                    const type = match[1] as CHTLModule['type'];
                    const name = match[2].trim();
                    
                    currentModule = {
                        name,
                        type,
                        content: '',
                        startLine: i,
                        endLine: i
                    };
                    braceCount = 0;
                    moduleStartLine = i;
                }
            }
            
            // 处理模块内容
            if (currentModule) {
                if (line.includes('{')) {
                    braceCount++;
                }
                if (line.includes('}')) {
                    braceCount--;
                }
                
                currentModule.content += line + '\n';
                
                // 模块结束
                if (braceCount === 0 && currentModule.content.includes('{')) {
                    currentModule.endLine = i;
                    modules.push(currentModule);
                    currentModule = null;
                }
            }
        }
        
        // 处理最后一个模块
        if (currentModule) {
            currentModule.endLine = lines.length - 1;
            modules.push(currentModule);
        }
        
        return modules;
    }

    async unpackSelectedModules(modules: CHTLModule[]): Promise<void> {
        for (const module of modules) {
            await this.unpackModule(module);
        }
    }

    private async unpackModule(module: CHTLModule): Promise<void> {
        try {
            const workspaceFolder = vscode.workspace.workspaceFolders?.[0];
            if (!workspaceFolder) {
                vscode.window.showErrorMessage('No workspace folder found');
                return;
            }

            const moduleDir = path.join(workspaceFolder.uri.fsPath, 'chtl_modules', module.name);
            await fs.promises.mkdir(moduleDir, { recursive: true });

            // 创建模块文件
            const moduleFile = path.join(moduleDir, `${module.name}.chtl`);
            await fs.promises.writeFile(moduleFile, module.content, 'utf8');

            // 创建模块信息文件
            const infoFile = path.join(moduleDir, 'module.json');
            const moduleInfo = {
                name: module.name,
                type: module.type,
                version: '1.0.0',
                description: `CHTL ${module.type} module: ${module.name}`,
                created: new Date().toISOString(),
                lines: {
                    start: module.startLine,
                    end: module.endLine
                }
            };
            await fs.promises.writeFile(infoFile, JSON.stringify(moduleInfo, null, 2), 'utf8');

            // 创建README文件
            const readmeFile = path.join(moduleDir, 'README.md');
            const readmeContent = this.generateModuleReadme(module);
            await fs.promises.writeFile(readmeFile, readmeContent, 'utf8');

            vscode.window.showInformationMessage(`Module ${module.name} unpacked successfully`);
        } catch (error) {
            vscode.window.showErrorMessage(`Failed to unpack module ${module.name}: ${error}`);
        }
    }

    private generateModuleReadme(module: CHTLModule): string {
        return `# ${module.name}

**Type:** ${module.type}  
**Version:** 1.0.0  
**Created:** ${new Date().toLocaleDateString()}

## Description

This is a CHTL ${module.type.toLowerCase()} module extracted from the main CHTL file.

## Usage

\`\`\`chtl
${module.content}
\`\`\`

## Module Information

- **Start Line:** ${module.startLine + 1}
- **End Line:** ${module.endLine + 1}
- **Content Length:** ${module.content.length} characters

## Files

- \`${module.name}.chtl\` - Main module file
- \`module.json\` - Module metadata
- \`README.md\` - This file

## Integration

To use this module in your CHTL project, you can:

1. Import it using the Import system:
   \`\`\`chtl
   [Import] ${module.name} from './chtl_modules/${module.name}'
   \`\`\`

2. Reference it directly in your CHTL code

3. Use it as a template or custom component

## Notes

This module was automatically extracted and may require manual adjustments for optimal integration.
`;
    }

    async createModulePackage(modules: CHTLModule[], packageName: string): Promise<void> {
        try {
            const workspaceFolder = vscode.workspace.workspaceFolders?.[0];
            if (!workspaceFolder) {
                vscode.window.showErrorMessage('No workspace folder found');
                return;
            }

            const packageDir = path.join(workspaceFolder.uri.fsPath, 'chtl_packages', packageName);
            await fs.promises.mkdir(packageDir, { recursive: true });

            // 创建包信息文件
            const packageInfo = {
                name: packageName,
                version: '1.0.0',
                description: `CHTL module package containing ${modules.length} modules`,
                created: new Date().toISOString(),
                modules: modules.map(m => ({
                    name: m.name,
                    type: m.type
                }))
            };

            const packageFile = path.join(packageDir, 'package.json');
            await fs.promises.writeFile(packageFile, JSON.stringify(packageInfo, null, 2), 'utf8');

            // 创建模块文件
            for (const module of modules) {
                const moduleFile = path.join(packageDir, `${module.name}.chtl`);
                await fs.promises.writeFile(moduleFile, module.content, 'utf8');
            }

            // 创建索引文件
            const indexFile = path.join(packageDir, 'index.chtl');
            const indexContent = this.generatePackageIndex(modules);
            await fs.promises.writeFile(indexFile, indexContent, 'utf8');

            vscode.window.showInformationMessage(`Package ${packageName} created successfully`);
        } catch (error) {
            vscode.window.showErrorMessage(`Failed to create package ${packageName}: ${error}`);
        }
    }

    private generatePackageIndex(modules: CHTLModule[]): string {
        let content = `# CHTL Module Package Index\n\n`;
        content += `This package contains ${modules.length} CHTL modules:\n\n`;

        for (const module of modules) {
            content += `## ${module.name}\n`;
            content += `**Type:** ${module.type}\n\n`;
            content += `\`\`\`chtl\n${module.content}\n\`\`\`\n\n`;
        }

        return content;
    }

    async analyzeModules(source: string): Promise<any> {
        const modules = await this.unpackModules(source);
        
        const analysis = {
            totalModules: modules.length,
            moduleTypes: {} as { [key: string]: number },
            totalLines: 0,
            totalCharacters: 0,
            modules: modules.map(m => ({
                name: m.name,
                type: m.type,
                lines: m.endLine - m.startLine + 1,
                characters: m.content.length
            }))
        };

        for (const module of modules) {
            analysis.moduleTypes[module.type] = (analysis.moduleTypes[module.type] || 0) + 1;
            analysis.totalLines += module.endLine - module.startLine + 1;
            analysis.totalCharacters += module.content.length;
        }

        return analysis;
    }
}