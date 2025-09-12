import * as assert from 'assert';
import * as vscode from 'vscode';

suite('Extension Test Suite', () => {
    vscode.window.showInformationMessage('Start all tests.');

    test('Extension should be present', () => {
        assert.ok(vscode.extensions.getExtension('chtl-team.chtl-language'));
    });

    test('Should activate', async () => {
        const extension = vscode.extensions.getExtension('chtl-team.chtl-language');
        if (extension) {
            await extension.activate();
            assert.ok(extension.isActive);
        }
    });

    test('Should register commands', async () => {
        const commands = await vscode.commands.getCommands(true);
        const chtlCommands = commands.filter(cmd => cmd.startsWith('chtl.'));
        
        assert.ok(chtlCommands.length > 0, 'CHTL commands should be registered');
        assert.ok(chtlCommands.includes('chtl.compile'), 'chtl.compile command should be registered');
        assert.ok(chtlCommands.includes('chtl.preview'), 'chtl.preview command should be registered');
    });
});