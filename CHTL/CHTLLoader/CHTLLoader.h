#ifndef CHTL_LOADER_H
#define CHTL_LOADER_H

#include "../CHTLNode/ImportNode.h"
#include "../CHTLStore/TemplateStore.h"

class CHTLLoader {
public:
    // Constructor takes a reference to the central template store.
    explicit CHTLLoader(TemplateStore& templateStore);

    // Processes an import node, loads the file, and populates the store.
    // Returns true on success, false on failure.
    bool load(const ImportNode* importNode);

private:
    TemplateStore& m_templateStore;
};

#endif //CHTL_LOADER_H
