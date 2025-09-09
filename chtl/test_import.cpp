#include <iostream>
#include "import/import_manager.h"

using namespace chtl::import;

int main() {
    std::cout << "Testing ImportManager..." << std::endl;
    
    try {
        ImportManager manager("/workspace/chtl");
        
        // 添加搜索路径
        manager.add_search_path("/workspace/chtl/examples");
        
        // 手动添加一些导入
        manager.add_import("test_component", "test.chtl", ImportType::CHTL);
        manager.add_import("styles", "styles.css", ImportType::CSS);
        manager.add_import("utils", "utils.js", ImportType::JAVASCRIPT);
        
        // 测试获取导入
        auto test_import = manager.get_import("test_component");
        if (test_import) {
            std::cout << "Found import: " << test_import->name 
                      << " (" << static_cast<int>(test_import->type) << ")" << std::endl;
        }
        
        // 测试按类型获取导入
        auto css_imports = manager.get_imports_by_type(ImportType::CSS);
        std::cout << "Found " << css_imports.size() << " CSS imports" << std::endl;
        
        // 测试命名空间
        auto global_ns = manager.get_namespace("global");
        if (global_ns) {
            std::cout << "Global namespace has " << global_ns->get_items().size() << " items" << std::endl;
        }
        
        // 打印调试信息
        manager.print_imports();
        manager.print_namespaces();
        
        std::cout << "ImportManager test completed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}