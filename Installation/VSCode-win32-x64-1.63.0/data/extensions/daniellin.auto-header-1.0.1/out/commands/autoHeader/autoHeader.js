"use strict";
/**
 * @ Author: Daniel Lin
 * @ Create Time: 2019-04-09 15:02:14
 * @ Modified by: Daniel Lin
 * @ Modified time: 2019-04-10 16:02:49
 * @ Description:
 */
Object.defineProperty(exports, "__esModule", { value: true });
const vscode_1 = require("vscode");
const default_config_1 = require("../../config/default.config");
const errorType_1 = require("../../enums/errorType");
const index_1 = require("../../utils/index");
/**
 * Auto header handler
 */
const autoHeader = () => {
    // Get configuration from user settings
    const autoHeaderConfig = vscode_1.workspace.getConfiguration('autoHeader');
    const format = (autoHeaderConfig && autoHeaderConfig.format) || default_config_1.default.format;
    const header = (autoHeaderConfig && autoHeaderConfig.header) || default_config_1.default.header;
    // Selected activated file the very first time the command is executed
    const activeTextEditor = vscode_1.window.activeTextEditor;
    activeTextEditor.edit((editBuilder) => {
        try {
            const filePath = activeTextEditor.document.fileName;
            const headerTemplate = index_1.generateHeaderTemplate({ format, header }, filePath);
            // Insert Header
            editBuilder.insert(new vscode_1.Position(0, 0), headerTemplate);
        }
        catch (error) {
            throw (new Error(errorType_1.default.InsertFailure));
        }
    });
};
exports.default = autoHeader;
//# sourceMappingURL=autoHeader.js.map