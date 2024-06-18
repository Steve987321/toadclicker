#include "pch.h"
#include "Helpers.h"

#include "Logger.h"
#include "Keys.h"

namespace toad 
{
	bool window_is_focused(const HWND& window)
	{
		return GetForegroundWindow() == window;
	}

	bool map_hotkeys(const std::vector<std::string>& hotkeys, std::vector<int>& mapped)
	{
		for (int i = 0, j = 2; i < hotkeys.size(); i++, j++)
		{
			const std::string& hotkey = hotkeys[i];
			int keycode = -1;

			try
			{
				keycode = std::stoi(hotkey);
			}
			catch (std::exception& e)
			{
				LOG_ERRORF("[map_hotkeys] %s", e.what());
				return false;
			}

			auto it = mc_as_vkc.find(keycode == -1 ? j : keycode);
			if (it != mc_as_vkc.end())
				mapped.push_back(it->second);
		}

		return true;
	}


	std::vector<std::string> get_all_files_ext(const std::filesystem::path& path, const char* ext, const bool includeExt)
	{
		std::vector <std::string> vec = {};
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			if (entry.path().extension() == ext)
				includeExt ? vec.push_back(entry.path().stem().string() + ext) : vec.push_back(entry.path().stem().string());
		}
		return vec;
	}

	std::filesystem::path get_exe_path()
	{
		char path[MAX_PATH];
		GetModuleFileNameA(NULL, path, MAX_PATH);
		return path;
	}

	std::string get_date_str(std::string_view format)
	{
		std::ostringstream ss;
		std::string time;

		auto t = std::time(nullptr);
		tm newtime{};

		localtime_s(&newtime, &t);

		ss << std::put_time(&newtime, format.data());
		return ss.str();
	}

	int random_float(float min, float max)
	{
		std::uniform_real_distribution<float> dis(min, max);
		return dis(gen);
	}

	int random_int(int min, int max)
	{
		std::uniform_int_distribution<int> dis(min, max);
		return dis(gen);
	}

	void precise_sleep(double seconds)
	{
		static double estimate = 0.005;
		static double mean = 0.005;
		static double m2 = 0;
		static int64_t count = 1;

		while (seconds > estimate) {
			auto start = std::chrono::high_resolution_clock::now();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			auto end = std::chrono::high_resolution_clock::now();

			double observed = (end - start).count() / 1e9;
			seconds -= observed;

			++count;
			double delta = observed - mean;
			mean += delta / count;
			m2 += delta * (observed - mean);
			double stddev = sqrt(m2 / (count - 1));
			estimate = mean + stddev;
		}

		// spin lock
		auto start = std::chrono::high_resolution_clock::now();
		while ((std::chrono::high_resolution_clock::now() - start).count() / 1e9 < seconds);
	}

}