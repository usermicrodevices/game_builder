#ifndef MAP_READER_HPP
#define MAP_READER_HPP

#include <string>
#include <map>
#include <vector>
#include <regex>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <memory>
#include <iostream>


inline std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

inline std::string remove_trailing_comma(const std::string& str) {
    std::string result = trim(str);
    if (!result.empty() && result.back() == ',') {
        result.pop_back();
    }
    return result;
}

inline std::string extract_quoted_string(const std::string& str) {
    std::regex string_regex(R"("([^"]*)")");
    std::smatch match;
    if (std::regex_search(str, match, string_regex)) {
        return match[1].str();
    }
    return "";
}

inline int extract_int_value(const std::string& line, const std::string& key) {
    std::regex regex_pattern(key + R"(\s*:\s*(\d+))");
    std::smatch match;
    if (std::regex_search(line, match, regex_pattern)) {
        return std::stoi(match[1].str());
    }
    return -1;
}

inline std::pair<std::string, std::string> extract_key_value(const std::string& line) {
    std::regex kv_regex(R"("?([^":]+)"?\s*:\s*("?[^",]+"?))");
    std::smatch match;
    if (std::regex_search(line, match, kv_regex)) {
        std::string key = trim(match[1].str());
        std::string value = trim(match[2].str());
        if (!value.empty() && value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.length() - 2);
        }
        return {key, value};
    }
    return {"", ""};
}

struct CellData {
    int x = 0;
    int y = 0;
    int floor = -1;
    int wall = -1;
    int roof = -1;
    int type = 0;
    std::string script;
};

struct LevelData {
    int count_x = 0;
    int count_y = 0;
    int default_side_size = 0;
    std::map<int, std::string> textures;
    std::map<std::string, CellData> cells;
    
    bool has_data() const {
        return count_x > 0 && count_y > 0 && default_side_size > 0;
    }
};

class JsonParser {
private:
    std::map<std::string, std::map<std::string, std::string>> m_levels;
    std::map<std::string, std::map<int, std::string>> m_textures;
    std::map<std::string, std::map<std::string, std::map<std::string, std::string>>> m_cells;
    
public:
    bool parse(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return false;
        }
        
        std::string line;
        std::string current_level;
        std::string current_cell;
        std::string context = "root";
        bool in_levels = false;
        bool in_textures = false;
        bool in_cells = false;
        
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line == "{") continue;
            
            line = remove_trailing_comma(line);
            
            if (line == "}") {
                if (context == "cells") {
                    context = "levels";
                    in_cells = false;
                    current_cell.clear();
                } else if (context == "textures") {
                    context = "levels";
                    in_textures = false;
                } else if (context == "levels") {
                    context = "root";
                    in_levels = false;
                    current_level.clear();
                }
                continue;
            }
            
            // Check for "levels" object
            if (line.find("\"levels\"") != std::string::npos && line.find(':') != std::string::npos) {
                context = "levels";
                in_levels = true;
                continue;
            }
            
            // Parse level number
            if (in_levels && context == "levels") {
                std::regex level_regex(R"("(\d+)"\s*:)");
                std::smatch match;
                if (std::regex_search(line, match, level_regex)) {
                    current_level = match[1].str();
                    continue;
                }
            }
            
            // Check for "textures" object
            if (in_levels && line.find("\"textures\"") != std::string::npos) {
                context = "textures";
                in_textures = true;
                continue;
            }
            
            // Parse texture IDs and paths
            if (in_textures && context == "textures") {
                std::regex texture_regex(R"("(\d+)"\s*:)");
                std::smatch match;
                if (std::regex_search(line, match, texture_regex)) {
                    int tex_id = std::stoi(match[1].str());
                    if (std::getline(file, line)) {
                        line = trim(line);
                        line = remove_trailing_comma(line);
                        if (line.find("\"path\"") != std::string::npos) {
                            std::string path = extract_quoted_string(line);
                            m_textures[current_level][tex_id] = path;
                        }
                    }
                }
                continue;
            }
            
            // Check for "cells" object
            if (in_levels && line.find("\"cells\"") != std::string::npos) {
                context = "cells";
                in_cells = true;
                continue;
            }
            
            // Parse cell coordinates
            if (in_cells && context == "cells") {
                std::regex cell_regex(R"("(\d+)-(\d+)"\s*:)");
                std::smatch match;
                if (std::regex_search(line, match, cell_regex)) {
                    current_cell = match[1].str() + "-" + match[2].str();
                    continue;
                }
            }
            
            // Parse cell properties
            if (in_cells && context == "cells" && !current_cell.empty()) {
                auto [key, value] = extract_key_value(line);
                if (!key.empty()) {
                    m_cells[current_level][current_cell][key] = value;
                }
                continue;
            }
            
            // Parse level properties
            if (in_levels && !current_level.empty()) {
                if (line.find("count_x") != std::string::npos) {
                    m_levels[current_level]["count_x"] = std::to_string(extract_int_value(line, "count_x"));
                } else if (line.find("count_y") != std::string::npos) {
                    m_levels[current_level]["count_y"] = std::to_string(extract_int_value(line, "count_y"));
                } else if (line.find("default_side_size") != std::string::npos) {
                    m_levels[current_level]["default_side_size"] = std::to_string(extract_int_value(line, "default_side_size"));
                }
            }
        }
        
        return !m_levels.empty();
    }
    
    std::map<std::string, std::string> getLevelProperties(int level_id) const {
        std::string level_key = std::to_string(level_id);
        auto it = m_levels.find(level_key);
        if (it != m_levels.end()) {
            return it->second;
        }
        return {};
    }
    
    std::map<int, std::string> getTextures(int level_id) const {
        std::string level_key = std::to_string(level_id);
        auto it = m_textures.find(level_key);
        if (it != m_textures.end()) {
            return it->second;
        }
        return {};
    }
    
    std::map<std::string, std::map<std::string, std::string>> getCells(int level_id) const {
        std::string level_key = std::to_string(level_id);
        auto it = m_cells.find(level_key);
        if (it != m_cells.end()) {
            return it->second;
        }
        return {};
    }
    
    std::vector<int> getLevelIds() const {
        std::vector<int> ids;
        for (const auto& [key, _] : m_levels) {
            ids.push_back(std::stoi(key));
        }
        std::sort(ids.begin(), ids.end());
        return ids;
    }
};

inline LevelData parseLevelFromJson(const std::string& filename, int level_id) {
    LevelData result;
    JsonParser parser;
    
    if (!parser.parse(filename)) {
        return result;
    }
    
    auto props = parser.getLevelProperties(level_id);
    auto it = props.find("count_x");
    if (it != props.end()) result.count_x = std::stoi(it->second);
    it = props.find("count_y");
    if (it != props.end()) result.count_y = std::stoi(it->second);
    it = props.find("default_side_size");
    if (it != props.end()) result.default_side_size = std::stoi(it->second);
    
    result.textures = parser.getTextures(level_id);
    
    auto cells = parser.getCells(level_id);
    for (const auto& [cell_key, properties] : cells) {
        size_t dash_pos = cell_key.find('-');
        if (dash_pos != std::string::npos) {
            CellData cell;
            cell.x = std::stoi(cell_key.substr(0, dash_pos));
            cell.y = std::stoi(cell_key.substr(dash_pos + 1));
            
            for (const auto& [prop_key, prop_value] : properties) {
                if (prop_key == "floor") {
                    cell.floor = std::stoi(prop_value);
                } else if (prop_key == "wall") {
                    cell.wall = std::stoi(prop_value);
                } else if (prop_key == "roof") {
                    cell.roof = std::stoi(prop_value);
                } else if (prop_key == "type") {
                    cell.type = std::stoi(prop_value);
                } else if (prop_key == "script") {
                    cell.script = prop_value;
                }
            }
            
            result.cells[cell_key] = cell;
        }
    }
    
    return result;
}

inline std::map<int, LevelData> parseAllLevelsFromJson(const std::string& filename) {
    std::map<int, LevelData> result;
    JsonParser parser;
    
    if (!parser.parse(filename)) {
        return result;
    }
    
    for (int level_id : parser.getLevelIds()) {
        result[level_id] = parseLevelFromJson(filename, level_id);
    }
    
    return result;
}

#endif // MAP_READER_HPP
