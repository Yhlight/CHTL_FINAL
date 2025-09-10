import * as vscode from 'vscode';

export class CHTLHoverProvider implements vscode.HoverProvider {
    private hoverInfo: Map<string, vscode.Hover>;

    constructor() {
        this.initializeHoverInfo();
    }

    provideHover(
        document: vscode.TextDocument,
        position: vscode.Position,
        token: vscode.CancellationToken
    ): vscode.ProviderResult<vscode.Hover> {
        const word = document.getWordRangeAtPosition(position);
        if (!word) {
            return null;
        }

        const text = document.getText(word);
        const hover = this.hoverInfo.get(text.toLowerCase());
        
        if (hover) {
            return hover;
        }

        // 检查是否是CHTL特有语法
        const line = document.lineAt(position).text;
        const linePrefix = line.substring(0, position.character);
        
        if (linePrefix.includes('[') && linePrefix.includes(']')) {
            return this.getTemplateHover(text);
        }
        
        if (linePrefix.includes(':')) {
            return this.getAttributeHover(text);
        }

        return null;
    }

    private initializeHoverInfo(): void {
        this.hoverInfo = new Map();

        // HTML元素悬停信息
        this.hoverInfo.set('div', new vscode.Hover(
            new vscode.MarkdownString(`
# div

**HTML Element**

A generic container element that can be used to group other elements for styling or layout purposes.

**Usage:**
\`\`\`chtl
div: Content
div.class: Styled content
div#id: Identified content
\`\`\`
            `)
        ));

        this.hoverInfo.set('span', new vscode.Hover(
            new vscode.MarkdownString(`
# span

**HTML Element**

An inline container element used to group inline elements for styling purposes.

**Usage:**
\`\`\`chtl
span: Inline content
span.highlight: Highlighted text
\`\`\`
            `)
        ));

        this.hoverInfo.set('p', new vscode.Hover(
            new vscode.MarkdownString(`
# p

**HTML Element**

A paragraph element used to contain text content.

**Usage:**
\`\`\`chtl
p: This is a paragraph
p.intro: Introduction paragraph
\`\`\`
            `)
        ));

        // CHTL关键字悬停信息
        this.hoverInfo.set('text', new vscode.Hover(
            new vscode.MarkdownString(`
# text

**CHTL Keyword**

Declares a text element in CHTL.

**Usage:**
\`\`\`chtl
text: Plain text content
\`\`\`
            `)
        ));

        this.hoverInfo.set('style', new vscode.Hover(
            new vscode.MarkdownString(`
# style

**CHTL Keyword**

Declares a style element for CSS content.

**Usage:**
\`\`\`chtl
style: body { color: red; }
\`\`\`
            `)
        ));

        this.hoverInfo.set('script', new vscode.Hover(
            new vscode.MarkdownString(`
# script

**CHTL Keyword**

Declares a script element for JavaScript content.

**Usage:**
\`\`\`chtl
script: console.log('Hello World');
\`\`\`
            `)
        ));

        this.hoverInfo.set('use', new vscode.Hover(
            new vscode.MarkdownString(`
# use

**CHTL Keyword**

Specifies the document type or configuration to use.

**Usage:**
\`\`\`chtl
use html5
use xhtml
use MyConfig
\`\`\`
            `)
        ));

        // 属性悬停信息
        this.hoverInfo.set('class', new vscode.Hover(
            new vscode.MarkdownString(`
# class

**HTML Attribute**

Specifies one or more CSS class names for an element.

**Usage:**
\`\`\`chtl
div {
    class: "container main"
    class: "highlighted"
}
\`\`\`
            `)
        ));

        this.hoverInfo.set('id', new vscode.Hover(
            new vscode.MarkdownString(`
# id

**HTML Attribute**

Specifies a unique identifier for an element.

**Usage:**
\`\`\`chtl
div {
    id: "main-content"
    id: "header"
}
\`\`\`
            `)
        ));

        this.hoverInfo.set('style', new vscode.Hover(
            new vscode.MarkdownString(`
# style

**HTML Attribute**

Specifies inline CSS styles for an element.

**Usage:**
\`\`\`chtl
div {
    style: "color: red; font-size: 16px;"
}
\`\`\`
            `)
        ));
    }

    private getTemplateHover(text: string): vscode.Hover {
        const templateInfo: { [key: string]: string } = {
            'template': `
# [Template]

**CHTL Template Declaration**

Creates a reusable template that can be referenced by name.

**Usage:**
\`\`\`chtl
[Template] MyTemplate {
    div: Template content
    p: Template paragraph
}
\`\`\`
            `,
            'custom': `
# [Custom]

**CHTL Custom Declaration**

Creates a custom element or component.

**Usage:**
\`\`\`chtl
[Custom] MyCustom {
    div: Custom content
    style: .my-custom { color: blue; }
}
\`\`\`
            `,
            'origin': `
# [Origin]

**CHTL Origin Declaration**

Embeds raw HTML, CSS, or JavaScript content.

**Usage:**
\`\`\`chtl
[Origin] @Html <div>Raw HTML</div>
[Origin] @Style body { margin: 0; }
[Origin] @JavaScript console.log('Hello');
\`\`\`
            `,
            'import': `
# [Import]

**CHTL Import Declaration**

Imports external files or modules.

**Usage:**
\`\`\`chtl
[Import] MyModule from 'path/to/module'
[Import] * from 'path/to/all'
\`\`\`
            `,
            'namespace': `
# [Namespace]

**CHTL Namespace Declaration**

Creates a namespace for organizing elements and styles.

**Usage:**
\`\`\`chtl
[Namespace] MyNamespace {
    div: Namespaced content
}
\`\`\`
            `,
            'configuration': `
# [Configuration]

**CHTL Configuration Declaration**

Defines configuration settings for the CHTL compiler.

**Usage:**
\`\`\`chtl
[Configuration] MyConfig {
    INDEX_INITIAL_COUNT: 10
    DEBUG_MODE: true
}
\`\`\`
            `
        };

        const info = templateInfo[text.toLowerCase()];
        if (info) {
            return new vscode.Hover(new vscode.MarkdownString(info));
        }

        return null;
    }

    private getAttributeHover(text: string): vscode.Hover {
        const attributeInfo: { [key: string]: string } = {
            'src': `
# src

**HTML Attribute**

Specifies the source URL for media elements like img, audio, video, etc.

**Usage:**
\`\`\`chtl
img {
    src: "image.jpg"
    alt: "Description"
}
\`\`\`
            `,
            'href': `
# href

**HTML Attribute**

Specifies the URL for links and other elements.

**Usage:**
\`\`\`chtl
a {
    href: "https://example.com"
    text: "Link text"
}
\`\`\`
            `,
            'alt': `
# alt

**HTML Attribute**

Specifies alternative text for images and other media.

**Usage:**
\`\`\`chtl
img {
    src: "image.jpg"
    alt: "Alternative text"
}
\`\`\`
            `,
            'title': `
# title

**HTML Attribute**

Specifies a title for an element (shown on hover).

**Usage:**
\`\`\`chtl
div {
    title: "Tooltip text"
    text: "Hover me"
}
\`\`\`
            `
        };

        const info = attributeInfo[text.toLowerCase()];
        if (info) {
            return new vscode.Hover(new vscode.MarkdownString(info));
        }

        return null;
    }
}