#pragma once

#include "Value.h"

#include <string>
#include <memory>

namespace ccd {
namespace Json {

Value objectFromString(const std::string& jsonString);
Value arrayFromString(const std::string& jsonString);
std::string stringFromObject (const Object& jsonObject );
std::string stringFromArray (const Array& jsonArray );

}
}
