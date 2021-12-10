"use strict";
/**
 * @ Author: Daniel Lin
 * @ Create Time: 2019-04-09 17:13:08
 * @ Modified by: Daniel Lin
 * @ Modified time: 2019-04-10 16:07:58
 * @ Description:
 */
Object.defineProperty(exports, "__esModule", { value: true });
const fs = require("fs");
const moment = require("moment");
const path = require("path");
const fileType_1 = require("../enums/fileType");
const itemType_1 = require("../enums/itemType");
/**
 * Get Format config
 * @param {string} extname file extname
 * @param {object} format format config
 */
const getFormat = (extname, format) => {
    let result = format;
    switch (extname) {
        case fileType_1.default.Ejs:
        case fileType_1.default.Html:
        case fileType_1.default.Xml:
        case fileType_1.default.Vue:
            result = {
                startWith: '<!--',
                middleWith: '*',
                endWith: '-->',
                headerPrefix: '@',
            };
            break;
        case fileType_1.default.Python:
            result = {
                startWith: `'''`,
                middleWith: '#',
                endWith: `'''`,
                headerPrefix: '@',
            };
            break;
        case fileType_1.default.Javascript:
        case fileType_1.default.Typescript:
            result = {
                startWith: `/**`,
                middleWith: '*',
                endWith: `*/`,
                headerPrefix: '@',
            };
            break;
        default:
            break;
    }
    return result;
};
/**
 * Get config option count
 * @param {object} headerConfig header config
 */
exports.getConfigOptionCount = (headerConfig) => {
    // Cantians top and footer
    let result = 2;
    for (const key in headerConfig) {
        if (headerConfig.hasOwnProperty(key)) {
            result++;
        }
    }
    return result;
};
/**
 * Generate header template
 * @param {object} config config
 * @param {string} filePath file path
 */
exports.generateHeaderTemplate = (config, filePath) => {
    let result = '';
    const fileCreateTime = fs.statSync(filePath).birthtime;
    const extname = path.extname(filePath);
    const format = getFormat(extname, config.format);
    // Add top comment
    result += `${format.startWith}\n`;
    const header = config.header;
    for (const key in header) {
        if (header.hasOwnProperty(key)) {
            const element = header[key];
            if (typeof element === 'string') {
                let value = ` ${element}`;
                if (!element) {
                    value = ``;
                }
                result += ` ${format.middleWith} ${format.headerPrefix} ${key}:${value}\n`;
                continue;
            }
            if (typeof element === 'object') {
                let timeFormat;
                switch (element.type) {
                    case itemType_1.default.CreateTime:
                        timeFormat = element.format || 'YYYY-MM-DD HH:mm:ss';
                        const createTime = moment(fileCreateTime).format(timeFormat);
                        result += ` ${format.middleWith} ${format.headerPrefix} ${key}: ${createTime}\n`;
                        break;
                    case itemType_1.default.ModifyTime:
                        timeFormat = element.format || 'YYYY-MM-DD HH:mm:ss';
                        const modifyTime = moment().format(timeFormat);
                        result += ` ${format.middleWith} ${format.headerPrefix} ${key}: ${modifyTime}\n`;
                        break;
                    case itemType_1.default.Modifier:
                        result += ` ${format.middleWith} ${format.headerPrefix} ${key}: ${element.value}\n`;
                        break;
                    default:
                        break;
                }
            }
        }
    }
    // Add footer comment
    result += ` ${format.endWith}\n\n`;
    return result;
};
/**
 * Get mofidy entity
 * @param {object} config config
 * @param {string} filePath file path
 */
exports.getModify = (config, filePath) => {
    const result = {
        modifyTime: {
            key: '',
            value: '',
        },
        modifier: {
            key: '',
            value: '',
        },
    };
    const extname = path.extname(filePath);
    const format = getFormat(extname, config.format);
    const header = config.header;
    for (const key in header) {
        if (header.hasOwnProperty(key)) {
            const element = header[key];
            if (typeof element === 'string') {
                continue;
            }
            if (typeof element === 'object') {
                switch (element.type) {
                    case itemType_1.default.ModifyTime:
                        const timeFormat = element.format || 'YYYY-MM-DD HH:mm:ss';
                        const modifyTime = moment().format(timeFormat);
                        result.modifyTime = {
                            key,
                            value: ` ${format.middleWith} ${format.headerPrefix} ${key}: ${modifyTime}`,
                        };
                        break;
                    case itemType_1.default.Modifier:
                        result.modifier = {
                            key,
                            value: ` ${format.middleWith} ${format.headerPrefix} ${key}: ${element.value}`,
                        };
                        break;
                    default:
                        break;
                }
            }
        }
    }
    return result;
};
/**
 * Check line starts with
 * @param {object} config config
 */
exports.checkLineStartsWith = (target, match) => {
    if (target.startsWith(match)) {
        return true;
    }
    return false;
};
//# sourceMappingURL=index.js.map