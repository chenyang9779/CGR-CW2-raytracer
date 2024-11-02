#include <iostream>
#include "json_reader.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_json_file>" << std::endl;
        return 1;
    }

    std::string fileName = argv[1];
    json config = readJsonFile(fileName);
    if (config.is_null()) {
        std::cerr << "Failed to read JSON data." << std::endl;
        return 1;
    }

    try {
        std::cout << "nbounces: " << config["nbounces"] << std::endl;
        std::cout << "shapes: " << config["scene"]["shapes"] << std::endl;
    } catch (const json::exception& e) {
        std::cerr << "Error parsing scene configuration: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
