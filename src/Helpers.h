#pragma once

namespace toad
{
	struct ProcInfo
	{
		ProcInfo(DWORD pid, std::string_view pname, HWND phwnd)
			: proc_id(pid), proc_name(pname), hwnd(phwnd) {}

		DWORD proc_id{};
		std::string proc_name{};
		HWND hwnd{};
	};

	inline std::random_device rd;
	inline std::mt19937 gen(rd());

	// Returns whether window is focused
	bool window_is_focused(const HWND& window);

	// Converts list of LWJGL keycodes to Windows virtual keycodes and returns them. https://minecraft.fandom.com/el/wiki/Key_codes
	std::vector<int> map_hotkeys(std::vector<std::string>& hotkeys);

	// Returns list of all files in directory with given extension
	std::vector<std::string> get_all_files_ext(const std::filesystem::path& path, const char* ext, const bool include_ext = false);

	int random_float(float min, float max);

	int random_int(int min, int max);

	// https://blat-blatnik.github.io/computerBear/making-accurate-sleep-function/
	void precise_sleep(double seconds);
}
