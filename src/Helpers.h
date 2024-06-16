#pragma once

namespace toad
{
	inline std::random_device rd;
	inline std::mt19937 gen(rd());

	// Returns whether window is focused
	bool window_is_focused(const HWND& window);

	// Converts list of LWJGL keycodes to Windows virtual keycodes. https://minecraft.fandom.com/el/wiki/Key_codes\
	// Returns whether it successfully mapped the given hotkeys.
	// Note: Make sure mapped is empty as the results will be pushed there directly
	bool map_hotkeys(const std::vector<std::string>& hotkeys, std::vector<int>& mapped);

	// Returns list of all files in directory with given extension
	std::vector<std::string> get_all_files_ext(const std::filesystem::path& path, const char* ext, const bool include_ext = false);

	// Returns executable path (including exe file name)
	std::filesystem::path get_exe_path();

	int random_float(float min, float max);

	int random_int(int min, int max);

	// https://blat-blatnik.github.io/computerBear/making-accurate-sleep-function/
	void precise_sleep(double seconds);
}
