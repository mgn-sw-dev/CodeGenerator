#pragma once

#include <map>
#include <string>

#include <Extern/Valijson/Subschema.hpp>

namespace valijson {

typedef std::map<std::string, const Subschema *> SchemaCache;

}  // namespace valijson
