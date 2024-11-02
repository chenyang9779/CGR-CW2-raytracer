#include "json_reader.h"
#include <iostream>
#include <fstream>

json readJsonFile(const std::string& fileName) {
    // Open the JSON file
    std::ifstream inputFile(fileName);
    if (!inputFile.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return nullptr;  // Alternatively, you can return an empty json object.
    }

    // Parse the JSON content
    json jsonData;
    try {
        inputFile >> jsonData;
    } catch (const json::parse_error& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
        return nullptr;  // Alternatively, you can return an empty json object.
    }

    // Close the file
    inputFile.close();

    return jsonData;
}
