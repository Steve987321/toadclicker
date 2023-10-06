#pragma once

#include <GLFW/glfw3.h>

namespace toad
{

class ImGuiWindow final
{
public:
	ImGuiWindow(std::string window_title, int win_height, int win_width);
	~ImGuiWindow();

public:
	void StartWindow();
	void DestroyWindow();

public:
	GLFWwindow* GetHandle() const;
	bool IsActive() const;
	bool IsFontUpdated() const;

public:
	void SetUI(const std::function<void(ImGuiIO* io)>& ui_func);
	void AddFontTTF(std::string_view pathTTF);

public:
	/// Returns an active window with given argument as id if found.
	///
	///	Returns null if not found
	static ImGuiWindow* GetWindowInstance(std::string_view window_name);
	static ImGuiWindow* GetWindowInstance(GLFWwindow* hwnd);

private:
	bool CreateImGuiWindow(const std::string& window_title, int win_height, int win_width);

	/// The ui that displays if no UI has been set using SetUI
	static void DefaultUIWindow(ImGuiIO* io);
private:
	void UpdateMenu();

private:
	std::string m_window_name;
	std::string m_window_class_name;

	int m_window_width, m_window_height;

	std::mutex m_destroy_window_mutex;

	std::atomic_bool m_running = true;

	bool m_uifunc_set = false;

	bool m_update_font = false;
	std::string m_update_font_path;

	inline static std::unordered_map<std::string, ImGuiWindow*> m_window_from_name = {};
	inline static std::unordered_map<GLFWwindow*, ImGuiWindow*> m_window_from_hwnd = {};

	std::function<void(ImGuiIO* io)> m_uifunc = DefaultUIWindow;

	std::thread m_window_thread;

	GLFWwindow* m_window = {};
	WNDCLASSEXA m_wc = {};

	// imgui
	ImGuiIO* m_io = {};
	ImGuiStyle* m_style = {};

};

}