#include "llvm/Demangle/Demangle.h"

extern "C" char* abidump_demangle(const char* mangled) {
    return llvm::itaniumDemangle(mangled, NULL, NULL, NULL);
}