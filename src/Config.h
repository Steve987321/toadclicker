#pragma once

namespace toad::config
{
	void load_config(const std::string& path);
	void save_config(std::string name);

	std::vector <std::string> get_all_toad_configs(const std::filesystem::path& path);
}