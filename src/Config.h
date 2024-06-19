#pragma once

#include <json.hpp>
#include "Logger.h"

namespace toad::config
{
	void load_config(const std::string& path);
	void save_config(std::string name);

	std::vector <std::string> get_all_toad_configs(const std::filesystem::path& path);

	template<typename T>
	void get_json_element(T& val, const nlohmann::json& data, std::string_view key) noexcept
	{
		if (data.contains(key))
		{
			try
			{
				val = data[key];
			}
			catch (const nlohmann::json::exception& e)
			{
				LOG_ERRORF("[config] Failed to load property %s, %s", key.data(), e.what());
			}
		}
		else
			LOG_ERRORF("[config] Failed to load property: %s, doesn't exist", key.data());
	}
}