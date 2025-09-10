import * as vscode from 'vscode';

export class CHTLCompletionProvider implements vscode.CompletionItemProvider {
    private keywords: vscode.CompletionItem[];
    private elements: vscode.CompletionItem[];
    private attributes: vscode.CompletionItem[];
    private templates: vscode.CompletionItem[];

    constructor() {
        this.initializeKeywords();
        this.initializeElements();
        this.initializeAttributes();
        this.initializeTemplates();
    }

    provideCompletionItems(
        document: vscode.TextDocument,
        position: vscode.Position,
        token: vscode.CancellationToken,
        context: vscode.CompletionContext
    ): vscode.ProviderResult<vscode.CompletionItem[] | vscode.CompletionList> {
        const line = document.lineAt(position).text;
        const linePrefix = line.substring(0, position.character);

        // 检测上下文
        if (linePrefix.endsWith('[')) {
            return this.templates;
        } else if (linePrefix.includes(':')) {
            return this.attributes;
        } else if (linePrefix.trim() === '' || linePrefix.endsWith(' ')) {
            return this.elements;
        } else if (linePrefix.startsWith('use ')) {
            return this.getUseCompletions();
        }

        return [...this.keywords, ...this.elements, ...this.attributes, ...this.templates];
    }

    private initializeKeywords(): void {
        this.keywords = [
            this.createCompletionItem('text', 'Text keyword', 'CHTL text element'),
            this.createCompletionItem('style', 'Style keyword', 'CHTL style element'),
            this.createCompletionItem('script', 'Script keyword', 'CHTL script element'),
            this.createCompletionItem('use', 'Use keyword', 'CHTL use statement'),
            this.createCompletionItem('inherit', 'Inherit keyword', 'CHTL inherit keyword'),
            this.createCompletionItem('delete', 'Delete keyword', 'CHTL delete keyword'),
            this.createCompletionItem('insert', 'Insert keyword', 'CHTL insert keyword'),
            this.createCompletionItem('except', 'Except keyword', 'CHTL except keyword'),
            this.createCompletionItem('from', 'From keyword', 'CHTL from keyword'),
            this.createCompletionItem('as', 'As keyword', 'CHTL as keyword'),
            this.createCompletionItem('after', 'After keyword', 'CHTL after keyword'),
            this.createCompletionItem('before', 'Before keyword', 'CHTL before keyword'),
            this.createCompletionItem('replace', 'Replace keyword', 'CHTL replace keyword'),
            this.createCompletionItem('html5', 'HTML5 keyword', 'CHTL HTML5 type')
        ];
    }

    private initializeElements(): void {
        this.elements = [
            this.createCompletionItem('div', 'Div element', 'HTML div element'),
            this.createCompletionItem('span', 'Span element', 'HTML span element'),
            this.createCompletionItem('p', 'Paragraph element', 'HTML paragraph element'),
            this.createCompletionItem('h1', 'Heading 1', 'HTML heading 1 element'),
            this.createCompletionItem('h2', 'Heading 2', 'HTML heading 2 element'),
            this.createCompletionItem('h3', 'Heading 3', 'HTML heading 3 element'),
            this.createCompletionItem('h4', 'Heading 4', 'HTML heading 4 element'),
            this.createCompletionItem('h5', 'Heading 5', 'HTML heading 5 element'),
            this.createCompletionItem('h6', 'Heading 6', 'HTML heading 6 element'),
            this.createCompletionItem('a', 'Link element', 'HTML anchor element'),
            this.createCompletionItem('img', 'Image element', 'HTML image element'),
            this.createCompletionItem('ul', 'Unordered list', 'HTML unordered list element'),
            this.createCompletionItem('ol', 'Ordered list', 'HTML ordered list element'),
            this.createCompletionItem('li', 'List item', 'HTML list item element'),
            this.createCompletionItem('table', 'Table element', 'HTML table element'),
            this.createCompletionItem('tr', 'Table row', 'HTML table row element'),
            this.createCompletionItem('td', 'Table cell', 'HTML table cell element'),
            this.createCompletionItem('th', 'Table header', 'HTML table header element'),
            this.createCompletionItem('form', 'Form element', 'HTML form element'),
            this.createCompletionItem('input', 'Input element', 'HTML input element'),
            this.createCompletionItem('button', 'Button element', 'HTML button element'),
            this.createCompletionItem('textarea', 'Textarea element', 'HTML textarea element'),
            this.createCompletionItem('select', 'Select element', 'HTML select element'),
            this.createCompletionItem('option', 'Option element', 'HTML option element'),
            this.createCompletionItem('label', 'Label element', 'HTML label element'),
            this.createCompletionItem('section', 'Section element', 'HTML section element'),
            this.createCompletionItem('article', 'Article element', 'HTML article element'),
            this.createCompletionItem('header', 'Header element', 'HTML header element'),
            this.createCompletionItem('footer', 'Footer element', 'HTML footer element'),
            this.createCompletionItem('nav', 'Navigation element', 'HTML navigation element'),
            this.createCompletionItem('main', 'Main element', 'HTML main element'),
            this.createCompletionItem('aside', 'Aside element', 'HTML aside element')
        ];
    }

    private initializeAttributes(): void {
        this.attributes = [
            this.createCompletionItem('class', 'Class attribute', 'CSS class attribute'),
            this.createCompletionItem('id', 'ID attribute', 'CSS ID attribute'),
            this.createCompletionItem('style', 'Style attribute', 'Inline CSS style attribute'),
            this.createCompletionItem('src', 'Source attribute', 'Source URL attribute'),
            this.createCompletionItem('href', 'Hyperlink attribute', 'Hyperlink reference attribute'),
            this.createCompletionItem('alt', 'Alt text attribute', 'Alternative text attribute'),
            this.createCompletionItem('title', 'Title attribute', 'Title attribute'),
            this.createCompletionItem('type', 'Type attribute', 'Input type attribute'),
            this.createCompletionItem('value', 'Value attribute', 'Input value attribute'),
            this.createCompletionItem('placeholder', 'Placeholder attribute', 'Input placeholder attribute'),
            this.createCompletionItem('required', 'Required attribute', 'Required input attribute'),
            this.createCompletionItem('disabled', 'Disabled attribute', 'Disabled input attribute'),
            this.createCompletionItem('readonly', 'Readonly attribute', 'Readonly input attribute'),
            this.createCompletionItem('checked', 'Checked attribute', 'Checked input attribute'),
            this.createCompletionItem('selected', 'Selected attribute', 'Selected option attribute'),
            this.createCompletionItem('multiple', 'Multiple attribute', 'Multiple selection attribute'),
            this.createCompletionItem('rows', 'Rows attribute', 'Textarea rows attribute'),
            this.createCompletionItem('cols', 'Columns attribute', 'Textarea columns attribute'),
            this.createCompletionItem('maxlength', 'Max length attribute', 'Maximum length attribute'),
            this.createCompletionItem('minlength', 'Min length attribute', 'Minimum length attribute'),
            this.createCompletionItem('pattern', 'Pattern attribute', 'Input pattern attribute'),
            this.createCompletionItem('autocomplete', 'Autocomplete attribute', 'Autocomplete attribute'),
            this.createCompletionItem('autofocus', 'Autofocus attribute', 'Autofocus attribute'),
            this.createCompletionItem('tabindex', 'Tab index attribute', 'Tab index attribute'),
            this.createCompletionItem('role', 'Role attribute', 'ARIA role attribute'),
            this.createCompletionItem('aria-label', 'ARIA label', 'ARIA label attribute'),
            this.createCompletionItem('aria-describedby', 'ARIA describedby', 'ARIA describedby attribute'),
            this.createCompletionItem('data-', 'Data attribute', 'Custom data attribute'),
            this.createCompletionItem('onclick', 'Click event', 'Click event handler'),
            this.createCompletionItem('onchange', 'Change event', 'Change event handler'),
            this.createCompletionItem('onsubmit', 'Submit event', 'Submit event handler'),
            this.createCompletionItem('onload', 'Load event', 'Load event handler')
        ];
    }

    private initializeTemplates(): void {
        this.templates = [
            this.createCompletionItem('[Template]', 'Template declaration', 'CHTL template declaration'),
            this.createCompletionItem('[Custom]', 'Custom declaration', 'CHTL custom declaration'),
            this.createCompletionItem('[Origin]', 'Origin declaration', 'CHTL origin declaration'),
            this.createCompletionItem('[Import]', 'Import declaration', 'CHTL import declaration'),
            this.createCompletionItem('[Namespace]', 'Namespace declaration', 'CHTL namespace declaration'),
            this.createCompletionItem('[Configuration]', 'Configuration declaration', 'CHTL configuration declaration'),
            this.createCompletionItem('[Constraint]', 'Constraint declaration', 'CHTL constraint declaration')
        ];
    }

    private getUseCompletions(): vscode.CompletionItem[] {
        return [
            this.createCompletionItem('html5', 'HTML5 type', 'Use HTML5 document type'),
            this.createCompletionItem('xhtml', 'XHTML type', 'Use XHTML document type'),
            this.createCompletionItem('html4', 'HTML4 type', 'Use HTML4 document type')
        ];
    }

    private createCompletionItem(
        label: string,
        detail: string,
        documentation: string
    ): vscode.CompletionItem {
        const item = new vscode.CompletionItem(label, vscode.CompletionItemKind.Keyword);
        item.detail = detail;
        item.documentation = new vscode.MarkdownString(documentation);
        return item;
    }
}