import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';

export class CHTLModuleViewProvider implements vscode.TreeDataProvider<CHTLModuleItem> {
    private _onDidChangeTreeData: vscode.EventEmitter<CHTLModuleItem | undefined | null | void> = new vscode.EventEmitter<CHTLModuleItem | undefined | null | void>();
    readonly onDidChangeTreeData: vscode.Event<CHTLModuleItem | undefined | null | void> = this._onDidChangeTreeData.event;

    constructor(private workspaceRoot: string | undefined) {}

    refresh(): void {
        this._onDidChangeTreeData.fire();
    }

    getTreeItem(element: CHTLModuleItem): vscode.TreeItem {
        return element;
    }

    getChildren(element?: CHTLModuleItem): Thenable<CHTLModuleItem[]> {
        if (!this.workspaceRoot) {
            vscode.window.showInformationMessage('No folder opened');
            return Promise.resolve([]);
        }

        if (!element) {
            return this.getModulesInWorkspace();
        } else {
            return this.getModuleChildren(element);
        }
    }

    private async getModulesInWorkspace(): Promise<CHTLModuleItem[]> {
        const modules: CHTLModuleItem[] = [];
        
        if (!this.workspaceRoot) {
            return modules;
        }

        try {
            // 扫描modules目录
            const modulesDir = path.join(this.workspaceRoot, 'modules');
            if (fs.existsSync(modulesDir)) {
                const moduleItems = await this.scanModuleDirectory(modulesDir);
                modules.push(...moduleItems);
            }

            // 扫描当前目录的.chtl和.cmod文件
            const chtlFiles = await this.scanCHTLFiles(this.workspaceRoot);
            chtlFiles.forEach(file => {
                const moduleItem = new CHTLModuleItem(
                    path.basename(file),
                    vscode.TreeItemCollapsibleState.None,
                    file,
                    'file'
                );
                modules.push(moduleItem);
            });

        } catch (error) {
            console.error('Error scanning modules:', error);
        }

        return modules;
    }

    private async getModuleChildren(element: CHTLModuleItem): Promise<CHTLModuleItem[]> {
        if (element.type === 'directory') {
            try {
                const children = await this.scanModuleDirectory(element.resourceUri!.fsPath);
                return children;
            } catch (error) {
                console.error('Error scanning module directory:', error);
                return [];
            }
        }
        return [];
    }

    private async scanModuleDirectory(dirPath: string): Promise<CHTLModuleItem[]> {
        const items: CHTLModuleItem[] = [];
        
        try {
            const entries = await fs.promises.readdir(dirPath, { withFileTypes: true });
            
            for (const entry of entries) {
                const fullPath = path.join(dirPath, entry.name);
                
                if (entry.isDirectory()) {
                    const moduleItem = new CHTLModuleItem(
                        entry.name,
                        vscode.TreeItemCollapsibleState.Collapsed,
                        fullPath,
                        'directory'
                    );
                    items.push(moduleItem);
                } else if (entry.isFile() && (entry.name.endsWith('.chtl') || entry.name.endsWith('.cmod'))) {
                    const moduleItem = new CHTLModuleItem(
                        entry.name,
                        vscode.TreeItemCollapsibleState.None,
                        fullPath,
                        'file'
                    );
                    items.push(moduleItem);
                }
            }
        } catch (error) {
            console.error('Error reading directory:', error);
        }

        return items;
    }

    private async scanCHTLFiles(dirPath: string): Promise<string[]> {
        const files: string[] = [];
        
        try {
            const entries = await fs.promises.readdir(dirPath, { withFileTypes: true });
            
            for (const entry of entries) {
                const fullPath = path.join(dirPath, entry.name);
                
                if (entry.isDirectory()) {
                    const subFiles = await this.scanCHTLFiles(fullPath);
                    files.push(...subFiles);
                } else if (entry.isFile() && (entry.name.endsWith('.chtl') || entry.name.endsWith('.cmod'))) {
                    files.push(fullPath);
                }
            }
        } catch (error) {
            console.error('Error scanning files:', error);
        }

        return files;
    }
}

export class CHTLModuleItem extends vscode.TreeItem {
    constructor(
        public readonly label: string,
        public readonly collapsibleState: vscode.TreeItemCollapsibleState,
        public readonly resourceUri?: vscode.Uri,
        public readonly type?: string
    ) {
        super(label, collapsibleState);
        
        this.tooltip = `${this.label}`;
        this.description = this.type === 'directory' ? 'Directory' : 'File';
        
        if (resourceUri) {
            this.resourceUri = resourceUri;
        }

        if (type === 'file') {
            this.command = {
                command: 'vscode.open',
                title: 'Open File',
                arguments: [resourceUri]
            };
        }

        // 设置图标
        if (type === 'directory') {
            this.iconPath = new vscode.ThemeIcon('folder');
        } else if (label.endsWith('.chtl')) {
            this.iconPath = new vscode.ThemeIcon('file-code');
        } else if (label.endsWith('.cmod')) {
            this.iconPath = new vscode.ThemeIcon('package');
        } else {
            this.iconPath = new vscode.ThemeIcon('file');
        }
    }
}

export class CHTLModuleView {
    private moduleViewProvider: CHTLModuleViewProvider;
    private treeView: vscode.TreeView<CHTLModuleItem>;

    constructor(context: vscode.ExtensionContext) {
        this.moduleViewProvider = new CHTLModuleViewProvider(
            vscode.workspace.workspaceFolders?.[0]?.uri.fsPath
        );

        this.treeView = vscode.window.createTreeView('chtlModules', {
            treeDataProvider: this.moduleViewProvider
        });

        // 注册刷新命令
        const refreshCommand = vscode.commands.registerCommand('chtl.modules.refresh', () => {
            this.moduleViewProvider.refresh();
        });

        context.subscriptions.push(refreshCommand, this.treeView);

        // 监听工作区变化
        const workspaceChangeEvent = vscode.workspace.onDidChangeWorkspaceFolders(() => {
            this.moduleViewProvider = new CHTLModuleViewProvider(
                vscode.workspace.workspaceFolders?.[0]?.uri.fsPath
            );
            this.treeView = vscode.window.createTreeView('chtlModules', {
                treeDataProvider: this.moduleViewProvider
            });
        });

        context.subscriptions.push(workspaceChangeEvent);
    }

    public refresh(): void {
        this.moduleViewProvider.refresh();
    }
}