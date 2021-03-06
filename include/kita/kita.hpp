#pragma once

#include <GLFW/glfw3.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <imgui.h>
#include <kita/events.hpp>
#include <mutex>

namespace kita
{
	enum class kita_state
	{
		UNINITIALIZED,
		INITIALIZED,
		RUN = INITIALIZED,
		REQ_DESTROY, // A destroy request should only be sent if kita is initialized (this needs the event/render loop to be processed)
		DESTROY,

		GLFW_INIT_FAIL,
	};

	enum class kita_mode
	{
		WINDOW,
		OVERLAY,
		IM_DOCKING // TODO: for ImGui dockings
	};

	class kita_instance
	{
	public:
		kita_instance(const char * title_, int width_, int height_, events::details::on_glfwerr_cb_t err_hnd_ = nullptr);
		kita_instance(kita_instance &) = delete;
		kita_instance(const kita_instance &) = delete;
		kita_instance(kita_instance &&) = delete;
		kita_instance(const kita_instance &&) = delete;
		~kita_instance();

		// Runs the event loop
		auto run() -> kita_instance &;

		// Sets the window position, if no parameter is provided
		// kita will center the window
		auto position(int x = -1, int y = -1) -> kita_instance &;

		auto show() -> kita_instance &;
		auto hide() -> kita_instance &;
		auto minimize() -> kita_instance &;
		auto restore() -> kita_instance &;
		auto maximize() -> kita_instance &;
		auto close() -> kita_instance &;

		auto toggle_mode() -> kita_instance &;
		auto overlay_mode() -> kita_instance &;
		auto window_mode() -> kita_instance &;

		auto title(const char * title) -> kita_instance &;
		auto title(void) -> const char *;

		// Registers an on_close event
		auto callback(events::details::on_close_cb_t handler) -> kita_instance &;
		auto callback(events::details::on_render_cb_t handler) -> kita_instance &;
		auto callback(events::details::on_glfwerr_cb_t handler) -> kita_instance &;
		auto callback(events::details::on_key_cb_t handler) -> kita_instance &;
		auto callback(events::details::on_pre_render_cb_t handler) -> kita_instance &;

		template <typename... vargs_t> requires (sizeof...(vargs_t) != 0)
		auto callbacks(vargs_t... vargs) -> kita_instance &
		{
			if (state == kita_state::INITIALIZED)	
				((callback(vargs)), ...);

			return *this;
		}

		template <typename T>
		auto operator+=(T rhs) -> kita_instance &
		{
			return callback(rhs);
		}

		operator bool() const noexcept;

	private:
		auto destroy(kita_state s = kita_state::REQ_DESTROY) -> bool;
		static auto glfw_error_dispatcher(int err_code, const char * description) -> void;
		static auto glfw_key_dispatcher(GLFWwindow* window, int key, int scancode, int action, int mods) -> void;

	private:
		int width {}, height {};
		const char * class_name {};
		char wnd_title[256] {};
		
		kita_mode mode { kita_mode::WINDOW };
		kita_state state { kita_state::UNINITIALIZED };

		GLFWwindow * window {};

		events::details::on_close_cb_t      on_close_cb  {};
		events::details::on_render_cb_t     on_render_cb {};
		events::details::on_key_cb_t        on_key_cb    {};
		events::details::on_pre_render_cb_t on_pre_render_cb {};
		inline static events::details::on_glfwerr_cb_t on_glfwerr  {};

	private:
		int trans_w = width, trans_h = height, trans_x = -1, trans_y = -1;
		inline static int kita_instances = 0;
		inline static std::mutex mut;
	};

}