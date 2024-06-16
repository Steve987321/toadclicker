#pragma once

struct ProcInfo
{
	ProcInfo(DWORD pid, std::string_view pname, HWND phwnd)
		: proc_id(pid), proc_name(pname), hwnd(phwnd) {}

	DWORD proc_id{};
	std::string proc_name{};
	HWND hwnd{};

};

namespace toad::misc
{
	inline std::vector<ProcInfo> proc_list = {};

	BOOL get_window_list();
	void window_scanner();
	void show(HWND window);
	void hide(HWND window);
}