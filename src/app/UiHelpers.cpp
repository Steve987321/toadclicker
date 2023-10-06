#include "pch.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "UiHelpers.h"

namespace toad
{
	ImVec2 get_middle_point()
	{
        return { ImGui::GetWindowSize().x / 2, ImGui::GetWindowSize().y / 2 };
	}

	void center_text_multi(const ImVec4& col, std::string_view txt, center_text_flags flags)
	{
        std::string str;
        std::vector <std::pair<std::string, float>> lines = {};
        float txtHeight = 0;

        for (int i = 0; i < txt.length(); i++)
        {
            const char c = txt[i];
            str += c;
            if (c == '\n' || i == txt.length() - 1)
            {
                auto height = ImGui::CalcTextSize(str.c_str()).y;
                txtHeight += height;
                lines.emplace_back(str.c_str(), height);
                str.clear();
            }
        }

        // render our multi line text
        for (int i = 0; i < lines.size(); i++)
        {
            const auto& [lable, height] = lines[i];

            assert("TEXT_MIDDLEX conflicts with TEXT_LEFT and TEXT_RIGHT " && !(flags & TEXT_MIDDLEX && (flags & TEXT_LEFT || flags & TEXT_RIGHT)));
            assert("TEXT_MIDDLEY conflicts with TEXT_TOP and TEXT_BOTTOM " && !(flags & TEXT_MIDDLEY && (flags & TEXT_TOP || flags & TEXT_BOTTOM)));

            if (flags & TEXT_MIDDLEY)
            {
                auto yDist = ImGui::GetStyle().ItemSpacing.y;
                auto yPosNew = ImGui::GetWindowSize().y / 2.f - height / 2.f;
                auto yPos = ImGui::GetCursorPosY();

                if (i == 0)
                    ImGui::SetCursorPosY(yPosNew - txtHeight);

                if (yPos == yPosNew)
                    ImGui::SetCursorPosY(yPosNew + yDist);
            }
            if (flags & TEXT_MIDDLEX)
                ImGui::SetCursorPosX(ImGui::GetWindowSize().x / 2.f - ImGui::CalcTextSize(lable.c_str()).x / 2.f);

            ImGui::TextColored(col, lable.c_str());
        }
	}

	bool CheckBoxFill(const char* label, bool* v)
	{
        using namespace ImGui;

        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = CalcTextSize(label, NULL, true);

        const float square_sz = GetFrameHeight();
        const ImVec2 pos = window->DC.CursorPos;
        const ImRect total_bb(pos, pos + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f));
        ItemSize(total_bb, style.FramePadding.y);
        if (!ItemAdd(total_bb, id))
        {
            IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
            return false;
        }

        bool hovered, held;
        bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
        if (pressed)
        {
            *v = !(*v);
            MarkItemEdited(id);
        }

        const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));
        const ImRect box_bb(ImVec2(pos.x + 6, pos.y + 6), pos + ImVec2(square_sz - 5, square_sz - 5));

        RenderNavHighlight(total_bb, id);
        RenderFrame(ImVec2(box_bb.Min.x - 1, box_bb.Min.y - 1), ImVec2(box_bb.Max.x + 1, box_bb.Max.y + 1), IM_COL32(8, 8, 8, 255), true, style.FrameRounding);
        RenderFrame(box_bb.Min, box_bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

        ImU32 check_col = GetColorU32(ImGuiCol_CheckMark);
        bool mixed_value = (g.LastItemData.InFlags & ImGuiItemFlags_MixedValue) != 0;
        if (mixed_value)
        {
            ImVec2 pad(ImMax(1.0f, IM_FLOOR(square_sz / 3.6f)), ImMax(1.0f, IM_FLOOR(square_sz / 3.6f)));
            window->DrawList->AddRectFilled(check_bb.Min + pad, check_bb.Max - pad, check_col, style.FrameRounding);
        }
        else if (*v)
        {
            const float pad = ImMax(1.0f, IM_FLOOR(square_sz / 6.0f));
            window->DrawList->AddRectFilledMultiColor(box_bb.Min, box_bb.Max, check_col, check_col, GetColorU32(ImGuiCol_ButtonHovered), GetColorU32(ImGuiCol_ButtonHovered));
        }

        ImVec2 label_pos = ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + style.FramePadding.y);
        if (g.LogEnabled)
            LogRenderedText(&label_pos, mixed_value ? "[~]" : *v ? "[x]" : "[ ]");
        if (label_size.x > 0.0f)
            RenderText(label_pos, label);

        IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
        return pressed;
	}
}