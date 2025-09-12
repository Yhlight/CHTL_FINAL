#include "chtholly_cjmod.h"
#include "CJMODSystem.h"
#include <iostream>
#include <sstream>

namespace CHTL {

/**
 * 珂朵莉模块CJMOD实现
 */
class ChthollyCJMOD {
public:
    static void registerFunctions(CJMODSystem& system) {
        // 注册printMylove函数
        registerPrintMylove(system);
        
        // 注册iNeverAway函数
        registerINeverAway(system);
        
        // 注册util...then表达式
        registerUtilThen(system);
    }

private:
    static void registerPrintMylove(CJMODSystem& system) {
        auto module = std::make_unique<CJMODModule>("Chtholly");
        
        // 创建printMylove函数
        CHTLJSFunction printMylove = CHTLJSFunction::createCHTLJSFunction(
            "printMylove {url: $!_, mode: $?_, width: $?_, height: $?_, scale: $?_}"
        );
        
        printMylove.name = "printMylove";
        printMylove.parameters = {"url", "mode", "width", "height", "scale"};
        printMylove.body = R"(
    // 图片转字符像素块
    const canvas = document.createElement('canvas');
    const ctx = canvas.getContext('2d');
    const img = new Image();
    
    img.onload = function() {
        canvas.width = width || 80;
        canvas.height = height || 40;
        
        ctx.drawImage(img, 0, 0, canvas.width, canvas.height);
        const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
        const data = imageData.data;
        
        let result = '';
        const chars = mode === 'ASCII' ? 
            '@%#*+=-:. ' : 
            '█▓▒░ ';
        
        for (let i = 0; i < data.length; i += 4) {
            const r = data[i];
            const g = data[i + 1];
            const b = data[i + 2];
            const brightness = (r + g + b) / 3;
            
            const charIndex = Math.floor((brightness / 255) * (chars.length - 1));
            result += chars[charIndex];
            
            if ((i / 4 + 1) % canvas.width === 0) {
                result += '\n';
            }
        }
        
        console.log(result);
    };
    
    img.src = url;
)";
        
        module->addFunction(printMylove);
        system.registerModule(std::move(module));
    }
    
    static void registerINeverAway(CJMODSystem& system) {
        auto module = std::make_unique<CJMODModule>("Chtholly");
        
        // 创建iNeverAway函数
        CHTLJSFunction iNeverAway = CHTLJSFunction::createCHTLJSFunction(
            "iNeverAway {Void<A>: function($!_), Void<B>: function($!_), Void: $?_, Ax: $_}"
        );
        
        iNeverAway.name = "iNeverAway";
        iNeverAway.body = R"(
    // 创建标记函数组
    const functionGroup = {};
    
    // 处理状态函数
    for (const [key, value] of Object.entries(parameters)) {
        if (key.includes('<')) {
            const [funcName, state] = key.split('<');
            const cleanState = state.replace('>', '');
            
            if (!functionGroup[funcName]) {
                functionGroup[funcName] = {};
            }
            
            functionGroup[funcName][cleanState] = value;
        } else {
            functionGroup[key] = value;
        }
    }
    
    // 创建虚对象
    const virtualObject = new Proxy({}, {
        get(target, prop) {
            if (functionGroup[prop]) {
                if (typeof functionGroup[prop] === 'object') {
                    return new Proxy({}, {
                        get(target, state) {
                            return functionGroup[prop][state] || function() {};
                        }
                    });
                }
                return functionGroup[prop];
            }
            return function() {};
        }
    });
    
    return virtualObject;
)";
        
        module->addFunction(iNeverAway);
        system.registerModule(std::move(module));
    }
    
    static void registerUtilThen(CJMODSystem& system) {
        auto module = std::make_unique<CJMODModule>("Chtholly");
        
        // 创建util...then表达式函数
        CHTLJSFunction utilThen = CHTLJSFunction::createCHTLJSFunction(
            "util $! -> change $? -> then $!"
        );
        
        utilThen.name = "utilThen";
        utilThen.body = R"(
    // util...then表达式实现
    let condition = false;
    let changeAction = null;
    let thenAction = null;
    
    // 解析参数
    const parts = expression.split('->');
    if (parts.length >= 3) {
        condition = eval(parts[0].trim());
        changeAction = parts[1].trim().replace('change', '').trim();
        thenAction = parts[2].trim().replace('then', '').trim();
    }
    
    if (condition) {
        if (changeAction) {
            eval(changeAction);
        }
    } else {
        if (thenAction) {
            eval(thenAction);
        }
    }
)";
        
        module->addFunction(utilThen);
        system.registerModule(std::move(module));
    }
};

// 导出函数
extern "C" {
    void registerChthollyCJMOD(CJMODSystem& system) {
        ChthollyCJMOD::registerFunctions(system);
    }
}

} // namespace CHTL