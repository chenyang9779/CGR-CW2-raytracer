#ifndef JSON_READER_H
#define JSON_READER_H

#include <string>
#include "external/json.hpp"

using json = nlohmann::json;

json readJsonFile(const std::string &fileName);

#endif // JSON_READER_H