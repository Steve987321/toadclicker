#pragma once

namespace toad::config
{
    void loadConfig(const std::string path);

    void saveConfig(std::string name); // create and load configs

    inline std::vector<std::string> ConfigList = {};
    inline int selectedConfig = 0;

    std::vector <std::string> GetAllToadConfigs(std::filesystem::path path);

}