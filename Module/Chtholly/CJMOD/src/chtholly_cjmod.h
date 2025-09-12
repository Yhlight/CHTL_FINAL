#ifndef CHTHOLLY_CJMOD_H
#define CHTHOLLY_CJMOD_H

#include "CJMODSystem.h"

namespace CHTL {

/**
 * 珂朵莉模块CJMOD头文件
 */

// 导出函数声明
extern "C" {
    /**
     * 注册珂朵莉模块的CJMOD函数
     * @param system CJMOD系统
     */
    void registerChthollyCJMOD(CJMODSystem& system);
}

} // namespace CHTL

#endif // CHTHOLLY_CJMOD_H