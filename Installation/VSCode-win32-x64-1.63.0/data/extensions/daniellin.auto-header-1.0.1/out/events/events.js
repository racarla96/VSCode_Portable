"use strict";
/**
 * @ Author: Daniel Lin
 * @ Create Time: 2019-04-09 18:20:22
 * @ Modified by: Daniel Lin
 * @ Modified time: 2019-04-10 16:05:37
 * @ Description:
 */
Object.defineProperty(exports, "__esModule", { value: true });
const vscode_1 = require("vscode");
const index_1 = require("./workspace/index");
/**
 * Handle events
 */
const handleEvents = () => {
    // Handle workspace events
    vscode_1.workspace.onDidSaveTextDocument(index_1.onDidSaveTextDocument);
};
exports.default = handleEvents;
//# sourceMappingURL=events.js.map