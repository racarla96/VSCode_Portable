"use strict";
/**
 * @ Author: Daniel Lin
 * @ Create Time: 2019-04-08 17:21:36
 * @ Modified by: Daniel Lin
 * @ Modified time: 2019-04-10 16:08:06
 * @ Description:
 */
Object.defineProperty(exports, "__esModule", { value: true });
const vscode_1 = require("vscode");
const commands_1 = require("./commands/commands");
const events_1 = require("./events/events");
// this method is called when your extension is activated
// your extension is activated the very first time the command is executed
function activate(context) {
    // Register commands
    for (const [key, value] of commands_1.default) {
        const command = vscode_1.commands.registerCommand(key, value.handler);
        context.subscriptions.push(command);
    }
    // Handle events
    events_1.default();
}
exports.activate = activate;
// this method is called when your extension is deactivated
function deactivate() {
    // tslint:disable-next-line:no-empty
}
exports.deactivate = deactivate;
//# sourceMappingURL=extension.js.map