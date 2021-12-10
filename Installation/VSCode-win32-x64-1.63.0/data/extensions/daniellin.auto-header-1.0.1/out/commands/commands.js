"use strict";
/**
 * @ Author: Daniel Lin
 * @ Create Time: 2019-04-09 14:48:24
 * @ Modified by: Daniel Lin
 * @ Modified time: 2019-04-10 16:04:25
 * @ Description:
 */
Object.defineProperty(exports, "__esModule", { value: true });
const commandType_1 = require("../enums/commandType");
const autoHeader_1 = require("./autoHeader/autoHeader");
/**
 * Commands Map
 */
const commandList = new Map([
    [commandType_1.default.AutoHeader, { handler: autoHeader_1.default }],
]);
exports.default = commandList;
//# sourceMappingURL=commands.js.map