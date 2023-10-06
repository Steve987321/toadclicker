#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include <set>

// make enum work as flags
#define ENUM_FLAG(e) inline constexpr e operator | (e a, e b) { return static_cast<e>(static_cast<int>(a) | static_cast<int>(b)); }

namespace toad
{
	// flags for imgui text placement
	enum center_text_flags
	{
		TEXT_LEFT = 1 << 1,
		TEXT_MIDDLEX = 1 << 2,
		TEXT_MIDDLEY = 1 << 3,
		TEXT_RIGHT = 1 << 4,
		TEXT_TOP = 1 << 5,
		TEXT_BOTTOM = 1 << 6,
		TEXT_MIDDLE = TEXT_MIDDLEX | TEXT_MIDDLEY
	};
	ENUM_FLAG(center_text_flags)

	ImVec2 get_middle_point();

	void center_text_multi(const ImVec4& col = { 1,1,1,1 }, std::string_view txt = "sample text \n second line", center_text_flags flags = TEXT_MIDDLE);

	bool CheckBoxFill(const char* label, bool* v);

	template<typename T>
	std::vector<std::pair<std::string, T>> get_filtered_suggestions(const std::string& input, const std::unordered_map<std::string, T>& map, int max_suggestions = 3)
	{
		if (input.size() <= 1) // only suggest when atleast 2 characters
			return {};

		std::vector<std::pair<std::string, T>> res = {};

		for (const auto& it : map)
		{
			if (it.first.find(input) != std::string::npos)
				res.emplace_back(it.first, it.second);

			if (res.size() > max_suggestions)
			{
				std::sort(res.begin(), res.end(), [](const auto& a, const auto& b) { return a.first < b.first;  });
				return res;
			}
		}

		std::sort(res.begin(), res.end(), [](const auto& a, const auto& b) { return a.first < b.first;  });
		return res;
	}

	template<typename T>
	std::vector<std::pair<T, std::string>> get_filtered_suggestions(const std::string& input, const std::unordered_map<T, std::string>& map, const std::set<std::string> ignore = {}, int max_suggestions = 3)
	{
		if (input.size() <= 1) // only suggest when atleast 2 characters
			return {};
		std::vector<std::pair<T, std::string>> res = {};

		for (const auto& it : map)
		{
			if (it.second.find(input) != std::string::npos && !ignore.contains(it.second))
				res.emplace_back(it.first, it.second);

			if (res.size() >= max_suggestions)
			{
				std::sort(res.begin(), res.end(), [](const auto& a, const auto& b) { return a.second < b.second; });
				return res;
			}
		}

		std::sort(res.begin(), res.end(), [](const auto& a, const auto& b) { return a.second < b.second; });
		return res;
	}

	// simple wrapper for centering a single line of text inside a box on the X axis
	template<typename ...Args>
	void center_textX(const ImVec4& col = { 1,1,1,1 }, const char* txt = "sample text", Args&& ...args)
	{
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x / 2.f - ImGui::CalcTextSize(txt).x / 2.f);
		ImGui::TextColored(col, txt, args...);
	}

	// simple wrapper for centering a single line of text inside a box on the X and Y axis
	template<typename ...Args>
	void center_text(const ImVec4& col = { 1,1,1,1 }, const char* frmt = "sample text", Args&& ...args)
	{
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x / 2.f - ImGui::CalcTextSize(frmt).x / 2.f);
		ImGui::SetCursorPosY(ImGui::GetWindowSize().y / 2.f - ImGui::CalcTextSize(frmt).y / 2.f);
		ImGui::TextColored(col, frmt, args...);
	}
}