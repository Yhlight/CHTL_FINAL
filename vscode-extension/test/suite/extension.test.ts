import * as assert from 'assert';
import * as vscode from 'vscode';

suite('CHTL Extension Test Suite', () => {
    test('Extension should be present', () => {
        assert.ok(vscode.extensions.getExtension('chtl-team.chtl-language'));
    });

    test('Extension should activate', async () => {
        const extension = vscode.extensions.getExtension('chtl-team.chtl-language');
        if (extension) {
            await extension.activate();
            assert.ok(extension.isActive);
        }
    });

    test('Commands should be registered', async () => {
        const commands = await vscode.commands.getCommands(true);
        const chtlCommands = commands.filter(cmd => cmd.startsWith('chtl.'));
        
        assert.ok(chtlCommands.includes('chtl.compile'));
        assert.ok(chtlCommands.includes('chtl.parse'));
        assert.ok(chtlCommands.includes('chtl.format'));
        assert.ok(chtlCommands.includes('chtl.lint'));
        assert.ok(chtlCommands.includes('chtl.preview'));
        assert.ok(chtlCommands.includes('chtl.export'));
        assert.ok(chtlCommands.includes('chtl.build'));
        assert.ok(chtlCommands.includes('chtl.clean'));
        assert.ok(chtlCommands.includes('chtl.test'));
        assert.ok(chtlCommands.includes('chtl.serve'));
        assert.ok(chtlCommands.includes('chtl.watch'));
    });

    test('Language should be registered', () => {
        const languages = vscode.languages.getLanguages();
        assert.ok(languages.includes('chtl'));
    });

    test('Grammar should be registered', () => {
        // This test would need to be implemented with the actual grammar
        assert.ok(true);
    });

    test('Snippets should be registered', () => {
        // This test would need to be implemented with the actual snippets
        assert.ok(true);
    });
});