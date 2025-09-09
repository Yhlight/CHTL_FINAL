import * as vscode from 'vscode';

export class CHTLSymbols {
    private symbols: Map<string, vscode.SymbolInformation[]> = new Map();

    public addSymbols(uri: vscode.Uri, symbols: vscode.SymbolInformation[]): void {
        this.symbols.set(uri.toString(), symbols);
    }

    public getSymbols(uri: vscode.Uri): vscode.SymbolInformation[] {
        return this.symbols.get(uri.toString()) || [];
    }

    public getAllSymbols(): vscode.SymbolInformation[] {
        const allSymbols: vscode.SymbolInformation[] = [];
        for (const symbols of this.symbols.values()) {
            allSymbols.push(...symbols);
        }
        return allSymbols;
    }

    public clearSymbols(uri: vscode.Uri): void {
        this.symbols.delete(uri.toString());
    }

    public clearAllSymbols(): void {
        this.symbols.clear();
    }
}