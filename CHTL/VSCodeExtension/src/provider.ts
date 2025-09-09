import * as vscode from 'vscode';
import { CHTLParser } from './parser';
import { CHTLSymbols } from './symbols';

export class CHTLProvider implements 
    vscode.CompletionItemProvider,
    vscode.HoverProvider,
    vscode.DefinitionProvider,
    vscode.ReferenceProvider,
    vscode.RenameProvider {

    private parser: CHTLParser;
    private symbols: CHTLSymbols;
    private config: vscode.WorkspaceConfiguration;

    constructor() {
        this.parser = new CHTLParser();
        this.symbols = new CHTLSymbols();
        this.config = vscode.workspace.getConfiguration('chtl');
    }

    public reloadConfiguration(): void {
        this.config = vscode.workspace.getConfiguration('chtl');
    }

    // 代码完成
    public async provideCompletionItems(
        document: vscode.TextDocument,
        position: vscode.Position,
        token: vscode.CancellationToken,
        context: vscode.CompletionContext
    ): Promise<vscode.CompletionItem[] | vscode.CompletionList | undefined> {
        const line = document.lineAt(position);
        const text = line.text.substring(0, position.character);
        
        const completions: vscode.CompletionItem[] = [];

        // HTML元素完成
        if (this.isInElementContext(text)) {
            const htmlElements = this.getHTMLElements();
            htmlElements.forEach(element => {
                const item = new vscode.CompletionItem(element, vscode.CompletionItemKind.Class);
                item.detail = 'HTML Element';
                item.documentation = `HTML <${element}> element`;
                completions.push(item);
            });
        }

        // CHTL关键字完成
        if (this.isInKeywordContext(text)) {
            const keywords = this.getCHTLKeywords();
            keywords.forEach(keyword => {
                const item = new vscode.CompletionItem(keyword, vscode.CompletionItemKind.Keyword);
                item.detail = 'CHTL Keyword';
                item.documentation = this.getKeywordDocumentation(keyword);
                completions.push(item);
            });
        }

        // 属性完成
        if (this.isInAttributeContext(text)) {
            const attributes = this.getHTMLAttributes();
            attributes.forEach(attr => {
                const item = new vscode.CompletionItem(attr, vscode.CompletionItemKind.Property);
                item.detail = 'HTML Attribute';
                item.documentation = this.getAttributeDocumentation(attr);
                completions.push(item);
            });
        }

        // CSS属性完成
        if (this.isInStyleContext(text)) {
            const cssProperties = this.getCSSProperties();
            cssProperties.forEach(prop => {
                const item = new vscode.CompletionItem(prop, vscode.CompletionItemKind.Property);
                item.detail = 'CSS Property';
                item.documentation = this.getCSSPropertyDocumentation(prop);
                completions.push(item);
            });
        }

        // 模板完成
        if (this.isInTemplateContext(text)) {
            const templates = this.getTemplates(document);
            templates.forEach(template => {
                const item = new vscode.CompletionItem(template, vscode.CompletionItemKind.Snippet);
                item.detail = 'CHTL Template';
                item.documentation = `Template: ${template}`;
                completions.push(item);
            });
        }

        return completions;
    }

    // 悬停信息
    public async provideHover(
        document: vscode.TextDocument,
        position: vscode.Position,
        token: vscode.CancellationToken
    ): Promise<vscode.Hover | undefined> {
        const word = document.getWordRangeAtPosition(position);
        if (!word) {
            return undefined;
        }

        const text = document.getText(word);
        const line = document.lineAt(position);
        const lineText = line.text;

        // 检查是否是HTML元素
        if (this.isHTMLElement(text)) {
            const hoverText = new vscode.MarkdownString();
            hoverText.appendMarkdown(`**HTML Element: \`<${text}>\`**\n\n`);
            hoverText.appendMarkdown(this.getElementDocumentation(text));
            return new vscode.Hover(hoverText);
        }

        // 检查是否是CHTL关键字
        if (this.isCHTLKeyword(text)) {
            const hoverText = new vscode.MarkdownString();
            hoverText.appendMarkdown(`**CHTL Keyword: \`${text}\`**\n\n`);
            hoverText.appendMarkdown(this.getKeywordDocumentation(text));
            return new vscode.Hover(hoverText);
        }

        // 检查是否是模板
        const templates = this.getTemplates(document);
        if (templates.includes(text)) {
            const hoverText = new vscode.MarkdownString();
            hoverText.appendMarkdown(`**CHTL Template: \`${text}\`**\n\n`);
            hoverText.appendMarkdown(`This template is defined in the current document.`);
            return new vscode.Hover(hoverText);
        }

        return undefined;
    }

    // 定义跳转
    public async provideDefinition(
        document: vscode.TextDocument,
        position: vscode.Position,
        token: vscode.CancellationToken
    ): Promise<vscode.Definition | vscode.LocationLink[] | undefined> {
        const word = document.getWordRangeAtPosition(position);
        if (!word) {
            return undefined;
        }

        const text = document.getText(word);
        
        // 查找模板定义
        const templateDef = this.findTemplateDefinition(document, text);
        if (templateDef) {
            return new vscode.Location(document.uri, templateDef.range);
        }

        // 查找自定义定义
        const customDef = this.findCustomDefinition(document, text);
        if (customDef) {
            return new vscode.Location(document.uri, customDef.range);
        }

        return undefined;
    }

    // 引用查找
    public async provideReferences(
        document: vscode.TextDocument,
        position: vscode.Position,
        context: vscode.ReferenceContext,
        token: vscode.CancellationToken
    ): Promise<vscode.Location[] | undefined> {
        const word = document.getWordRangeAtPosition(position);
        if (!word) {
            return undefined;
        }

        const text = document.getText(word);
        const references: vscode.Location[] = [];

        // 查找所有引用
        const allDocuments = await vscode.workspace.findFiles('**/*.chtl');
        for (const doc of allDocuments) {
            const document = await vscode.workspace.openTextDocument(doc);
            const matches = document.getText().match(new RegExp(`\\b${text}\\b`, 'g'));
            if (matches) {
                for (let i = 0; i < matches.length; i++) {
                    const match = document.getText().indexOf(matches[i], i * matches[i].length);
                    const position = document.positionAt(match);
                    references.push(new vscode.Location(doc, position));
                }
            }
        }

        return references;
    }

    // 重命名
    public async provideRenameEdits(
        document: vscode.TextDocument,
        position: vscode.Position,
        newName: string,
        token: vscode.CancellationToken
    ): Promise<vscode.WorkspaceEdit | undefined> {
        const word = document.getWordRangeAtPosition(position);
        if (!word) {
            return undefined;
        }

        const text = document.getText(word);
        const edit = new vscode.WorkspaceEdit();

        // 查找所有引用并重命名
        const allDocuments = await vscode.workspace.findFiles('**/*.chtl');
        for (const doc of allDocuments) {
            const document = await vscode.workspace.openTextDocument(doc);
            const matches = document.getText().match(new RegExp(`\\b${text}\\b`, 'g'));
            if (matches) {
                for (let i = 0; i < matches.length; i++) {
                    const match = document.getText().indexOf(matches[i], i * matches[i].length);
                    const position = document.positionAt(match);
                    const range = new vscode.Range(position, position.translate(0, text.length));
                    edit.replace(doc, range, newName);
                }
            }
        }

        return edit;
    }

    // 私有辅助方法
    private isInElementContext(text: string): boolean {
        return /^\s*[a-zA-Z_][a-zA-Z0-9_]*\s*\{?\s*$/.test(text);
    }

    private isInKeywordContext(text: string): boolean {
        return /^\s*\[?[A-Z][a-zA-Z]*\s*$/.test(text);
    }

    private isInAttributeContext(text: string): boolean {
        return /^\s*[a-zA-Z-]+\s*[:=]\s*$/.test(text);
    }

    private isInStyleContext(text: string): boolean {
        return /^\s*[a-zA-Z-]+\s*:\s*$/.test(text);
    }

    private isInTemplateContext(text: string): boolean {
        return /^\s*@[A-Z][a-zA-Z]*\s*$/.test(text);
    }

    private isHTMLElement(text: string): boolean {
        const htmlElements = this.getHTMLElements();
        return htmlElements.includes(text.toLowerCase());
    }

    private isCHTLKeyword(text: string): boolean {
        const keywords = this.getCHTLKeywords();
        return keywords.includes(text);
    }

    private getHTMLElements(): string[] {
        return [
            'html', 'head', 'body', 'title', 'meta', 'link', 'script', 'style',
            'div', 'span', 'p', 'h1', 'h2', 'h3', 'h4', 'h5', 'h6',
            'ul', 'ol', 'li', 'a', 'img', 'button', 'input', 'form', 'label',
            'select', 'option', 'table', 'tr', 'td', 'th', 'thead', 'tbody', 'tfoot',
            'section', 'article', 'header', 'footer', 'nav', 'aside', 'main',
            'figure', 'figcaption', 'details', 'summary', 'dialog', 'canvas',
            'svg', 'video', 'audio', 'source', 'track', 'embed', 'object',
            'param', 'iframe', 'noscript', 'template', 'slot'
        ];
    }

    private getCHTLKeywords(): string[] {
        return [
            'Template', 'Custom', 'Origin', 'Import', 'Namespace', 'Configuration',
            'Info', 'Export', 'text', 'style', 'script', 'use', 'except',
            'delete', 'insert', 'inherit', 'from', 'as', 'after', 'before',
            'replace', 'at', 'top', 'bottom', 'fileloader', 'listen', 'delegate',
            'animate', 'vir', 'router', 'util', 'iNeverAway'
        ];
    }

    private getHTMLAttributes(): string[] {
        return [
            'id', 'class', 'style', 'title', 'lang', 'dir', 'hidden', 'tabindex',
            'accesskey', 'contenteditable', 'draggable', 'spellcheck', 'translate',
            'onclick', 'onload', 'onchange', 'onsubmit', 'onfocus', 'onblur',
            'href', 'src', 'alt', 'width', 'height', 'type', 'value', 'name',
            'placeholder', 'required', 'disabled', 'readonly', 'checked', 'selected'
        ];
    }

    private getCSSProperties(): string[] {
        return [
            'color', 'background-color', 'background-image', 'background-size',
            'background-position', 'background-repeat', 'font-family', 'font-size',
            'font-weight', 'font-style', 'text-align', 'text-decoration',
            'text-transform', 'line-height', 'letter-spacing', 'word-spacing',
            'margin', 'margin-top', 'margin-right', 'margin-bottom', 'margin-left',
            'padding', 'padding-top', 'padding-right', 'padding-bottom', 'padding-left',
            'border', 'border-width', 'border-style', 'border-color',
            'border-radius', 'width', 'height', 'max-width', 'max-height',
            'min-width', 'min-height', 'display', 'position', 'top', 'right',
            'bottom', 'left', 'z-index', 'float', 'clear', 'overflow',
            'visibility', 'opacity', 'transform', 'transition', 'animation'
        ];
    }

    private getTemplates(document: vscode.TextDocument): string[] {
        const templates: string[] = [];
        const text = document.getText();
        
        // 查找模板定义
        const templateRegex = /\[Template\]\s+@\w+\s+(\w+)/g;
        let match;
        while ((match = templateRegex.exec(text)) !== null) {
            templates.push(match[1]);
        }
        
        // 查找自定义定义
        const customRegex = /\[Custom\]\s+@\w+\s+(\w+)/g;
        while ((match = customRegex.exec(text)) !== null) {
            templates.push(match[1]);
        }
        
        return templates;
    }

    private getElementDocumentation(element: string): string {
        const docs: { [key: string]: string } = {
            'html': 'The root element of an HTML document.',
            'head': 'Contains metadata about the document.',
            'body': 'Contains the visible content of the document.',
            'div': 'A generic container element.',
            'span': 'An inline generic container element.',
            'p': 'Represents a paragraph.',
            'h1': 'Represents the highest level heading.',
            'h2': 'Represents a second-level heading.',
            'h3': 'Represents a third-level heading.',
            'h4': 'Represents a fourth-level heading.',
            'h5': 'Represents a fifth-level heading.',
            'h6': 'Represents a sixth-level heading.'
        };
        
        return docs[element.toLowerCase()] || `HTML <${element}> element.`;
    }

    private getKeywordDocumentation(keyword: string): string {
        const docs: { [key: string]: string } = {
            'Template': 'Defines a reusable template in CHTL.',
            'Custom': 'Defines a custom element or style in CHTL.',
            'Import': 'Imports external files or modules.',
            'Namespace': 'Defines a namespace for organizing code.',
            'Configuration': 'Defines configuration options.',
            'text': 'Creates a text node.',
            'style': 'Defines CSS styles.',
            'script': 'Defines JavaScript code.',
            'fileloader': 'Loads external JavaScript files.',
            'listen': 'Sets up event listeners.',
            'delegate': 'Sets up event delegation.',
            'animate': 'Defines animations.',
            'vir': 'Defines virtual objects.',
            'router': 'Defines routing logic.'
        };
        
        return docs[keyword] || `CHTL keyword: ${keyword}`;
    }

    private getAttributeDocumentation(attr: string): string {
        const docs: { [key: string]: string } = {
            'id': 'Specifies a unique identifier for the element.',
            'class': 'Specifies one or more class names for the element.',
            'style': 'Specifies inline CSS styles for the element.',
            'title': 'Specifies extra information about the element.',
            'href': 'Specifies the URL of the linked resource.',
            'src': 'Specifies the URL of the media resource.',
            'alt': 'Specifies alternative text for images.',
            'width': 'Specifies the width of the element.',
            'height': 'Specifies the height of the element.'
        };
        
        return docs[attr] || `HTML attribute: ${attr}`;
    }

    private getCSSPropertyDocumentation(prop: string): string {
        const docs: { [key: string]: string } = {
            'color': 'Sets the color of the text.',
            'background-color': 'Sets the background color of the element.',
            'font-size': 'Sets the size of the font.',
            'margin': 'Sets the margin around the element.',
            'padding': 'Sets the padding inside the element.',
            'border': 'Sets the border around the element.',
            'width': 'Sets the width of the element.',
            'height': 'Sets the height of the element.',
            'display': 'Sets how the element is displayed.',
            'position': 'Sets the positioning method of the element.'
        };
        
        return docs[prop] || `CSS property: ${prop}`;
    }

    private findTemplateDefinition(document: vscode.TextDocument, name: string): { range: vscode.Range } | undefined {
        const text = document.getText();
        const regex = new RegExp(`\\[Template\\]\\s+@\\w+\\s+${name}\\b`);
        const match = text.match(regex);
        if (match) {
            const index = text.indexOf(match[0]);
            const position = document.positionAt(index);
            return { range: new vscode.Range(position, position.translate(0, match[0].length)) };
        }
        return undefined;
    }

    private findCustomDefinition(document: vscode.TextDocument, name: string): { range: vscode.Range } | undefined {
        const text = document.getText();
        const regex = new RegExp(`\\[Custom\\]\\s+@\\w+\\s+${name}\\b`);
        const match = text.match(regex);
        if (match) {
            const index = text.indexOf(match[0]);
            const position = document.positionAt(index);
            return { range: new vscode.Range(position, position.translate(0, match[0].length)) };
        }
        return undefined;
    }
}