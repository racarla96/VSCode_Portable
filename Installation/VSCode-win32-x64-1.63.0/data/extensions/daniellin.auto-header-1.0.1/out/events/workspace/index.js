"use strict";
/**
 * @ Author: Daniel Lin
 * @ Create Time: 2019-04-10 09:35:10
 * @ Modified by: Daniel Lin
 * @ Modified time: 2019-04-10 16:05:31
 * @ Description:
 */
Object.defineProperty(exports, "__esModule", { value: true });
const vscode_1 = require("vscode");
const default_config_1 = require("../../config/default.config");
const index_1 = require("../../utils/index");
// Last save time
let lastSaveTime = 0;
// Last save file name
let lastFileName = '';
/**
 * File did save event handler
 */
exports.onDidSaveTextDocument = (document) => {
    if (document.fileName === lastFileName) {
        const currentTime = new Date().getTime();
        const timeInterval = currentTime - lastSaveTime;
        if (timeInterval <= 5 * 1000) {
            return;
        }
    }
    lastSaveTime = new Date().getTime();
    lastFileName = document.fileName;
    // Get configuration from user settings
    const autoHeaderConfig = vscode_1.workspace.getConfiguration('autoHeader');
    const format = (autoHeaderConfig && autoHeaderConfig.format) || default_config_1.default.format;
    const header = (autoHeaderConfig && autoHeaderConfig.header) || default_config_1.default.header;
    // Selected activated file the very first time the command is executed
    const activeTextEditor = vscode_1.window.activeTextEditor;
    // Get modify entity
    const modifyEntity = index_1.getModify({ format, header }, document.fileName);
    const modifyTime = modifyEntity.modifyTime;
    const modifier = modifyEntity.modifier;
    const length = index_1.getConfigOptionCount(header);
    let mofidyTimeRange = new vscode_1.Range(new vscode_1.Position(0, 0), new vscode_1.Position(0, 0));
    let modifierRange = new vscode_1.Range(new vscode_1.Position(0, 0), new vscode_1.Position(0, 0));
    const modifyTimeStartsWith = ` ${format.middleWith} ${format.headerPrefix} ${modifyTime.key}:`;
    const modifierStartsWith = ` ${format.middleWith} ${format.headerPrefix} ${modifier.key}:`;
    for (let index = 0; index < length; index++) {
        // Get line text
        const linetAt = document.lineAt(index);
        const line = linetAt.text;
        // tslint:disable-next-line:max-line-length
        const isMofidyTimeLine = index_1.checkLineStartsWith(line, modifyTimeStartsWith);
        if (isMofidyTimeLine) {
            mofidyTimeRange = linetAt.range;
            continue;
        }
        const isModifierLine = index_1.checkLineStartsWith(line, modifierStartsWith);
        if (isModifierLine) {
            modifierRange = linetAt.range;
        }
    }
    const isUpdateModifyTime = !mofidyTimeRange.isEmpty && modifyTime.key && modifyTime.value;
    const isUpdateModifier = !modifierRange.isEmpty && modifier.key && modifier.value;
    if (!isUpdateModifyTime && !isUpdateModifier) {
        return;
    }
    // Update header
    activeTextEditor.edit((editBuilder) => {
        if (isUpdateModifyTime) {
            editBuilder.replace(mofidyTimeRange, modifyTime.value);
        }
        if (isUpdateModifier) {
            editBuilder.replace(modifierRange, modifier.value);
        }
    });
    setTimeout(() => {
        document.save();
    }, 200);
};
//# sourceMappingURL=index.js.map