#pragma once

#include "Node.h"
#include "TemplateNode.h" // A bit of a hack, but reuses the structure
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// For now, CustomNode can be an alias for TemplateNode since their structure is identical.
// In the future, it might have different methods or properties.
using CustomNode = TemplateNode;

} // namespace CHTL
