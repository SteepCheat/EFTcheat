#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_impl_dx9.h"
#include "imgui-docking/imgui_impl_win32.h"
#include <algorithm>
#include <map>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui-docking/imgui_internal.h"
#include <WinUser.h>

namespace Retake
{
	struct Animation {
		float size;
		float hovered;
	};

	const char* const KeyNames[] = {
	"Unknown",
	"VK_LBUTTON",
	"VK_RBUTTON",
	"VK_CANCEL",
	"VK_MBUTTON",
	"VK_XBUTTON1",
	"VK_XBUTTON2",
	"Unknown",
	"VK_BACK",
	"VK_TAB",
	"Unknown",
	"Unknown",
	"VK_CLEAR",
	"VK_RETURN",
	"Unknown",
	"Unknown",
	"VK_SHIFT",
	"VK_CONTROL",
	"VK_MENU",
	"VK_PAUSE",
	"VK_CAPITAL",
	"VK_KANA",
	"Unknown",
	"VK_JUNJA",
	"VK_FINAL",
	"VK_KANJI",
	"Unknown",
	"VK_ESCAPE",
	"VK_CONVERT",
	"VK_NONCONVERT",
	"VK_ACCEPT",
	"VK_MODECHANGE",
	"VK_SPACE",
	"VK_PRIOR",
	"VK_NEXT",
	"VK_END",
	"VK_HOME",
	"VK_LEFT",
	"VK_UP",
	"VK_RIGHT",
	"VK_DOWN",
	"VK_SELECT",
	"VK_PRINT",
	"VK_EXECUTE",
	"VK_SNAPSHOT",
	"VK_INSERT",
	"VK_DELETE",
	"VK_HELP",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"VK_LWIN",
	"VK_RWIN",
	"VK_APPS",
	"Unknown",
	"VK_SLEEP",
	"VK_NUMPAD0",
	"VK_NUMPAD1",
	"VK_NUMPAD2",
	"VK_NUMPAD3",
	"VK_NUMPAD4",
	"VK_NUMPAD5",
	"VK_NUMPAD6",
	"VK_NUMPAD7",
	"VK_NUMPAD8",
	"VK_NUMPAD9",
	"VK_MULTIPLY",
	"VK_ADD",
	"VK_SEPARATOR",
	"VK_SUBTRACT",
	"VK_DECIMAL",
	"VK_DIVIDE",
	"VK_F1",
	"VK_F2",
	"VK_F3",
	"VK_F4",
	"VK_F5",
	"VK_F6",
	"VK_F7",
	"VK_F8",
	"VK_F9",
	"VK_F10",
	"VK_F11",
	"VK_F12",
	"VK_F13",
	"VK_F14",
	"VK_F15",
	"VK_F16",
	"VK_F17",
	"VK_F18",
	"VK_F19",
	"VK_F20",
	"VK_F21",
	"VK_F22",
	"VK_F23",
	"VK_F24",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"VK_NUMLOCK",
	"VK_SCROLL",
	"VK_OEM_NEC_EQUAL",
	"VK_OEM_FJ_MASSHOU",
	"VK_OEM_FJ_TOUROKU",
	"VK_OEM_FJ_LOYA",
	"VK_OEM_FJ_ROYA",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"VK_LSHIFT",
	"VK_RSHIFT",
	"VK_LCONTROL",
	"VK_RCONTROL",
	"VK_LMENU",
	"VK_RMENU"
	};

	inline bool Hotkey(const char* label, int* k)
	{
		using namespace ImGui;

		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		ImGuiIO& io = g.IO;
		const ImGuiStyle& style = g.Style;

		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(150, 22));
		const ImRect total_bb(window->DC.CursorPos, frame_bb.Max);

		ImGui::ItemSize(total_bb, style.FramePadding.y);
		if (!ImGui::ItemAdd(total_bb, id, NULL))
			return false;


		const bool focus_requested = GetItemStatusFlags() != 0;
		bool hover, held;
		const bool clicked = ImGui::ButtonBehavior(frame_bb, id, &hover, &held);

		if (hover) {
			ImGui::SetHoveredID(id);
			g.MouseCursor = ImGuiMouseCursor_TextInput;
		}


		if (focus_requested || clicked) {
			if (g.ActiveId != id) {
				// Start edition
				memset(io.MouseDown, 0, sizeof(io.MouseDown));
				memset(io.KeysDown, 0, sizeof(io.KeysDown));
				*k = 0;
			}
			ImGui::SetActiveID(id, window);
			ImGui::FocusWindow(window);
		}
		else if (clicked) {
			// Release focus when we click outside
			if (g.ActiveId == id)
				ImGui::ClearActiveID();
		}

		bool value_changed = false;
		int key = *k;

		if (g.ActiveId == id) {
			for (auto i = 0; i < 5; i++) {
				if (io.MouseDown[i]) {
					switch (i) {
					case 0:
						key = VK_LBUTTON;
						break;
					case 1:
						key = VK_RBUTTON;
						break;
					case 2:
						key = VK_MBUTTON;
						break;
					case 3:
						key = VK_XBUTTON1;
						break;
					case 4:
						key = VK_XBUTTON2;
						break;
					}
					value_changed = true;
					ImGui::ClearActiveID();
				}
			}
			if (!value_changed) {
				for (auto i = VK_BACK; i <= VK_RMENU; i++) {
					if (io.KeysDown[i]) {
						key = i;
						value_changed = true;
						ImGui::ClearActiveID();
					}
				}
			}

			if (IsKeyPressedMap(ImGuiKey_Escape)) {
				*k = 0;
				ImGui::ClearActiveID();
			}
			else {
				*k = key;
			}
		}

		// Render
		// Select which buffer we are going to display. When ImGuiInputTextFlags_NoLiveEdit is Set 'buf' might still be the old value. We Set buf to NULL to prevent accidental usage from now on.

		char buf_display[64] = "NONE";

		static std::map<ImGuiID, Animation> AnimationMap;
		auto AnimationHandle = AnimationMap.find(ImGui::GetItemID());

		if (AnimationHandle == AnimationMap.end())
		{
			AnimationMap.insert({ ImGui::GetItemID(), {0.0f} });
			AnimationHandle = AnimationMap.find(ImGui::GetItemID());
		}

		if (hover)
		{
			if (AnimationHandle->second.hovered < 1.0f)
				AnimationHandle->second.hovered += 0.1f * (60 / ImGui::GetIO().Framerate);
		}
		else if (!hover)
		{
			if (AnimationHandle->second.hovered > 0.0f)
				AnimationHandle->second.hovered -= 0.1f * (60 / ImGui::GetIO().Framerate);
		}

		window->DrawList->AddRectFilled(frame_bb.Min, frame_bb.Max, ImColor(46, 46, 46, 186 + int(AnimationHandle->second.hovered * 50)), 4);

		ImGui::RenderFrame(frame_bb.Min, frame_bb.Min + ImVec2(22, 22), ImColor(56, 56, 56, 186 + int(AnimationHandle->second.hovered * 50)), false, 4);

		if (*k != 0 && g.ActiveId != id) {
			strcpy_s(buf_display, KeyNames[*k]);
		}
		else if (g.ActiveId == id) {
			strcpy_s(buf_display, "<Press a key>");
		}

		//window->DrawList->AddText(frame_bb.Min + ImVec2(4, 2), ImColor(220, 220, 220, 186 + int(AnimationHandle->second.hovered * 50)), ICON_FA_KEYBOARD);
		window->DrawList->AddText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), ImColor(225, 225, 225, 190 + int(AnimationHandle->second.hovered * 60)), label);
		window->DrawList->AddText(frame_bb.Min + ImVec2(30, style.FramePadding.y), ImColor(220, 220, 220, 186 + int(AnimationHandle->second.hovered * 50)), buf_display);

		return value_changed;
	}

	inline void Tab(const char* label, const char* icon, int* tab, int index)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = ImGui::CalcItemSize({ 130.f, 30.f }, (label_size.x + style.FramePadding.x * 2.0f), (label_size.y + style.FramePadding.y * 2.0f));

		const ImRect bb(pos, pos + size);
		ImGui::ItemSize(size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, NULL);

		if (hovered || held)
			ImGui::SetMouseCursor(7);

		static std::map<ImGuiID, Animation> AnimationMap;
		auto AnimationHandle = AnimationMap.find(ImGui::GetItemID());

		if (AnimationHandle == AnimationMap.end())
		{
			AnimationMap.insert({ ImGui::GetItemID(), {0.0f} });
			AnimationHandle = AnimationMap.find(ImGui::GetItemID());
		}

		if (*tab == index)
		{
			if (AnimationHandle->second.size < 1.0f)
				AnimationHandle->second.size += 0.1f * (60 / ImGui::GetIO().Framerate);
		}
		else if (*tab != index)
		{
			if (AnimationHandle->second.size > 0.0f)
				AnimationHandle->second.size -= 0.1f * (60 / ImGui::GetIO().Framerate);
		}

		window->DrawList->AddRectFilled(bb.Min, bb.Max, ImColor(36, 36, 36, 136 + int(AnimationHandle->second.size * 50)), 4);
		window->DrawList->AddText(bb.Min + ImVec2(35, 6), ImColor(225, 225, 225, 190 + int(AnimationHandle->second.size * 60)), label);

		ImColor IconColor = ImLerp(ImVec4(225 / 255.f, 225 / 255.f, 225 / 255.f, 255 / 255.f), ImVec4(0, 1, 0, 255 / 255.f), AnimationHandle->second.size);
		window->DrawList->AddText(bb.Min + ImVec2(10, 6), IconColor, icon);

		if (pressed)
			*tab = index;
	}
	static int tabs;
	/*__forceinline void _RenderTab()
	{
		const char* tabs_name[] = { ("Combat"), ("Visuals") };
		const char* tabs_icon[] = { ICON_FA_CROSSHAIRS, ICON_FA_USER };

		ImGui::SetCursorPos({ 10, 40 });
		ImGui::BeginGroup();
		{
			for (auto i = 0; i < 2; i++)
			{
				Tab(tabs_name[i], tabs_icon[i], &tabs, i);
			}
		}
		ImGui::EndGroup();
	}*/

	inline void Subtab(const char* label, ImVec2 size_arg, int* tab, int index)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = ImGui::CalcItemSize(size_arg, (label_size.x + style.FramePadding.x * 2.0f), (label_size.y + style.FramePadding.y * 2.0f));

		const ImRect bb(pos, pos + size);
		ImGui::ItemSize(size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, NULL);

		if (hovered || held)
			ImGui::SetMouseCursor(7);

		static std::map<ImGuiID, Animation> AnimationMap;
		auto AnimationHandle = AnimationMap.find(ImGui::GetItemID());

		if (AnimationHandle == AnimationMap.end())
		{
			AnimationMap.insert({ ImGui::GetItemID(), {0.0f} });
			AnimationHandle = AnimationMap.find(ImGui::GetItemID());
		}

		if (*tab == index)
		{
			if (AnimationHandle->second.size < 1.0f)
				AnimationHandle->second.size += 0.1f * (60 / ImGui::GetIO().Framerate);
		}
		else if (*tab != index)
		{
			if (AnimationHandle->second.size > 0.0f)
				AnimationHandle->second.size -= 0.1f * (60 / ImGui::GetIO().Framerate);
		}

		window->DrawList->AddRectFilled(bb.Min, bb.Max, ImColor(36, 36, 36, 106 + int(AnimationHandle->second.size * 80)), 4);
		window->DrawList->AddText(bb.Min + ImVec2(
			size_arg.x / 2 - ImGui::CalcTextSize(label).x / 2,
			size_arg.y / 2 - ImGui::CalcTextSize(label).y / 2),
			ImColor(225, 225, 225, 190 + int(AnimationHandle->second.size * 60)), label);

		if (pressed)
			*tab = index;
	}

	inline bool Button(const char* label, ImVec2 size_arg)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = ImGui::CalcItemSize(size_arg, (label_size.x + style.FramePadding.x * 2.0f), (label_size.y + style.FramePadding.y * 2.0f));

		const ImRect bb(pos, pos + size);
		ImGui::ItemSize(size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, NULL);

		if (hovered || held)
			ImGui::SetMouseCursor(7);

		static std::map<ImGuiID, Animation> AnimationMap;
		auto AnimationHandle = AnimationMap.find(ImGui::GetItemID());

		if (AnimationHandle == AnimationMap.end())
		{
			AnimationMap.insert({ ImGui::GetItemID(), {0.0f} });
			AnimationHandle = AnimationMap.find(ImGui::GetItemID());
		}

		if (hovered)
		{
			if (AnimationHandle->second.size < 1.0f)
				AnimationHandle->second.size += 0.1f * (60 / ImGui::GetIO().Framerate);
		}
		else if (!hovered)
		{
			if (AnimationHandle->second.size > 0.0f)
				AnimationHandle->second.size -= 0.1f * (60 / ImGui::GetIO().Framerate);
		}

		window->DrawList->AddRectFilled(bb.Min, bb.Max, ImColor(149, 137, 255 + int(AnimationHandle->second.hovered * 50)), 4);
		window->DrawList->AddText(bb.Min + ImVec2(
			size_arg.x / 2 - ImGui::CalcTextSize(label).x / 2,
			size_arg.y / 2 - ImGui::CalcTextSize(label).y / 2),
			ImColor(149, 137, 255 + int(AnimationHandle->second.size * 60)), label);

		return pressed;
	}

	inline bool Checkbox(const char* label, bool& v)
	{
		using namespace ImGui;

		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = CalcTextSize(label, NULL, true);

		const float square_sz = GetFrameHeight() - 5;
		const ImVec2 pos = window->DC.CursorPos;
		const ImRect total_bb(pos, pos + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f));
		const float pad = ImMax(1.0f, IM_FLOOR(square_sz / 6.0f));

		ItemSize(total_bb, style.FramePadding.y);
		if (!ItemAdd(total_bb, id))
			return false;

		bool hovered, held;
		bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);

		if (hovered || held)
			ImGui::SetMouseCursor(7);

		static std::map<ImGuiID, Animation> AnimationMap;
		auto AnimationHandle = AnimationMap.find(ImGui::GetItemID());

		if (AnimationHandle == AnimationMap.end())
		{
			AnimationMap.insert({ ImGui::GetItemID(), {0.0f} });
			AnimationHandle = AnimationMap.find(ImGui::GetItemID());
		}

		if (hovered)
		{
			if (AnimationHandle->second.hovered < 1.0f)
				AnimationHandle->second.hovered += 0.1f * (60 / ImGui::GetIO().Framerate);
		}
		else if (!hovered)
		{
			if (AnimationHandle->second.hovered > 0.0f)
				AnimationHandle->second.hovered -= 0.1f * (60 / ImGui::GetIO().Framerate);
		}

		if (v)
		{
			if (AnimationHandle->second.size < 1.0f)
				AnimationHandle->second.size += 0.1f * (60 / ImGui::GetIO().Framerate);
		}
		else if (!v)
		{
			if (AnimationHandle->second.size > 0.0f)
				AnimationHandle->second.size -= 0.1f * (60 / ImGui::GetIO().Framerate);
		}



		if (pressed)
			v = !(v);

		const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));
		window->DrawList->AddRectFilled(check_bb.Min, check_bb.Max, ImColor(149, 137, 255 + int(AnimationHandle->second.hovered * 50)), 4);
		RenderCheckMark(window->DrawList, check_bb.Min + ImVec2(pad + 1, pad + 1), ImColor(1.F, 191.F, 248.F, 0.2f + AnimationHandle->second.size), square_sz - pad * 3.0f);

		ImVec2 label_pos = ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + style.FramePadding.y - 3);

		window->DrawList->AddText(label_pos, ImColor(225, 225, 225, 190 + int(AnimationHandle->second.size * 60)), label);

		return pressed;
	}

	inline static const char* _PatchFormatStringFloatToInt(const char* fmt)
	{
		if (fmt[0] == '%' && fmt[1] == '.' && fmt[2] == '0' && fmt[3] == 'f' && fmt[4] == 0) // Fast legacy path for "%.0f" which is expected to be the most common case.
			return "%d";
		const char* fmt_start = ImParseFormatFindStart(fmt);    // Find % (if any, and ignore %%)
		const char* fmt_end = ImParseFormatFindEnd(fmt_start);  // Find end of format specifier, which itself is an exercise of confidence/recklessness (because snprintf is dependent on libc or user).
		if (fmt_end > fmt_start && fmt_end[-1] == 'f')
		{
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
			if (fmt_start == fmt && fmt_end[0] == 0)
				return "%d";
			ImGuiContext& g = *GImGui;
			ImFormatString(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), "%.*s%%d%s", (int)(fmt_start - fmt), fmt, fmt_end); // Honor leading and trailing decorations, but lose alignment/precision.
			return g.TempBuffer;
#else
			IM_ASSERT(0 && "DragInt(): Invalid format string!"); // Old versions used a default parameter of "%.0f", please replace with e.g. "%d"
#endif
		}
		return fmt;
	}

	inline bool _SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format = NULL, ImGuiSliderFlags flags = NULL)
	{
		using namespace ImGui;

		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const float w = 150;

		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

		const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
		ItemSize(total_bb, style.FramePadding.y);
		if (!ItemAdd(total_bb, id, &frame_bb))
			return false;

		if (format == NULL)
			format = DataTypeGetInfo(data_type)->PrintFmt;
		else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0) // (FIXME-LEGACY: Patch old "%.0f" format string to use "%d", read function more details.)
			format = _PatchFormatStringFloatToInt(format);

		const bool hovered = ItemHoverable(frame_bb, id);
		bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);
		if (!temp_input_is_active)
		{
			const bool focus_requested = temp_input_allowed && (window->DC.LastItemStatusFlags) != 0;
			const bool clicked = (hovered && g.IO.MouseClicked[0]);
			if (focus_requested || clicked || g.NavActivateId == id || g.NavInputId == id)
			{
				SetActiveID(id, window);
				SetFocusID(id, window);
				FocusWindow(window);
				g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
				if (temp_input_allowed && (focus_requested || (clicked && g.IO.KeyCtrl) || g.NavInputId == id))
					temp_input_is_active = true;
			}
		}

		static std::map<ImGuiID, Animation> AnimationMap;
		auto AnimationHandle = AnimationMap.find(ImGui::GetItemID());

		if (AnimationHandle == AnimationMap.end())
		{
			AnimationMap.insert({ ImGui::GetItemID(), {0.0f} });
			AnimationHandle = AnimationMap.find(ImGui::GetItemID());
		}

		if (hovered)
		{
			if (AnimationHandle->second.hovered < 1.0f)
				AnimationHandle->second.hovered += 0.1f * (60 / ImGui::GetIO().Framerate);
		}
		else if (!hovered)
		{
			if (AnimationHandle->second.hovered > 0.0f)
				AnimationHandle->second.hovered -= 0.1f * (60 / ImGui::GetIO().Framerate);
		}

		window->DrawList->AddRectFilled(frame_bb.Min, frame_bb.Max, ImColor(149, 137, 255 + int(AnimationHandle->second.hovered * 50)), 4);

		ImRect grab_bb;
		const bool value_changed = SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
		if (value_changed)
			MarkItemEdited(id);

		if (grab_bb.Max.x > grab_bb.Min.x)
			window->DrawList->AddRectFilled(frame_bb.Min + ImVec2(2, 2), grab_bb.Max, ImColor(133, 98, 202), 4);

		char value_buf[64];
		const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

		RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

		window->DrawList->AddText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), ImColor(225, 225, 225, 190 + int(AnimationHandle->second.hovered * 60)), label);


		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.LastItemStatusFlags);
		return value_changed;
	}

	inline bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = NULL, ImGuiSliderFlags flags = NULL)
	{
		return _SliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
	}

	inline bool SliderInt(const char* label, int* v, int v_min, int v_max, const char* format = NULL, ImGuiSliderFlags flags = NULL)
	{
		return _SliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
	}


}