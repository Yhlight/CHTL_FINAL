#ifndef CHTHOLLY_CJMOD_H
#define CHTHOLLY_CJMOD_H

#include <string>
#include <vector>
#include <memory>

namespace CHTL_JS {
namespace CJMOD {

// CJMOD模块基类
class CJMODModule {
public:
    virtual ~CJMODModule() = default;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual bool initialize() = 0;
    virtual void cleanup() = 0;
};

// Chtholly CJMOD模块
class ChthollyCJMOD : public CJMODModule {
public:
    std::string getName() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    bool initialize() override;
    void cleanup() override;
    
    // printMylove功能
    std::string printMylove(const std::string& imagePath, const std::string& options = "");
    
    // util...then表达式
    std::string processUtilThen(const std::string& code);
    
    // iNeverAway功能
    std::string processINeverAway(const std::string& code);
    
private:
    bool isInitialized = false;
};

} // namespace CJMOD
} // namespace CHTL_JS

#endif // CHTHOLLY_CJMOD_H