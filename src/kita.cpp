#include <kita/kita.hpp>
#include <kita/helpers.hpp>
#include <unordered_map>

static decltype(glClear) * kita_glClear = nullptr;
static decltype(glClearColor) * kita_glClearColor = nullptr;
static decltype(glViewport) * kita_glViewport = nullptr;
static bool kita_glfw_init = GLFW_FALSE;

// static std::unordered_map <GLFWwindow *, kita::kita_instance *> kita_error_callbacks;
static std::unordered_map <GLFWwindow *, kita::kita_instance *> kita_key_callbacks;

kita::kita_instance::kita_instance(const char * title_, int width_, int height_, events::details::on_glfwerr_cb_t err_hnd_)
	: class_name(title_), width(width_), height(height_)
{
	const std::lock_guard _lg(mut);

	state = kita_state::GLFW_INIT_FAIL;

	if (kita_instances == 0)
	{
		if (kita_glfw_init == GLFW_FALSE)
		{
			kita_glfw_init = glfwInit();
			glfwSetErrorCallback(glfw_error_dispatcher);
		}
		if (kita_glfw_init == GLFW_FALSE)
			return;
	}

	if (err_hnd_)
		callback(err_hnd_);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	window = glfwCreateWindow(width_, height_, title_, nullptr, nullptr);
	if (!window)
		return;

	KITA_DEFER([&] {
		if (state != kita_state::INITIALIZED)
			glfwDestroyWindow(window);
		if (kita_instances == 0 && kita_glfw_init)
			glfwTerminate();
	});

	glfwMakeContextCurrent(window);

	if (!kita_glClear || !kita_glClearColor || !kita_glViewport)
	{
		kita_glClear = (decltype(kita_glClear))glfwGetProcAddress("glClear");
		kita_glClearColor = (decltype(kita_glClearColor))glfwGetProcAddress("glClearColor");
		kita_glViewport  = (decltype(kita_glViewport))glfwGetProcAddress("glViewport");
	}

	if (!kita_glClear || !kita_glClearColor || !kita_glViewport)
		return;

	ImGui::CreateContext();
	ImGui::GetIO().IniFilename = nullptr;

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
	
	mode == kita_mode::WINDOW ? window_mode() : overlay_mode();

	++kita_instances;
	state = kita_state::INITIALIZED;
}

kita::kita_instance::~kita_instance()
{
	if (state != kita_state::INITIALIZED)
		destroy();
}

auto kita::kita_instance::run() -> kita_instance &
{
	if (state != kita_state::INITIALIZED)
		return *this;

	while (state == kita_state::RUN)
	{
		if (glfwWindowShouldClose(window))
		{
			if (on_close_cb)
			{
				events::on_close e {};
				e.cancel = false;
				e.instance = this;
				e.type = events::details::event_type::ON_CLOSE;
				on_close_cb(&e);
				if (e.cancel)
				{
					glfwSetWindowShouldClose(window, GLFW_FALSE);
				}
				else
				{
					destroy();
				}
			}
			else
			{
				destroy();
			}
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);

		if (mode == kita_mode::WINDOW)
		{
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2(display_w, display_h));

			ImGui::Begin("##kita_window", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus);

			if (on_render_cb)
			{
				events::on_render e {};
				e.type = events::details::event_type::ON_RENDER;
				e.instance = this;
				on_render_cb(&e);
			}
			
			ImGui::End();
		}
		else if (mode == kita_mode::OVERLAY)
		{
			static auto & io = ImGui::GetIO();
			static int last_wantcapture = -1;

			if (on_render_cb)
			{
				events::on_render e {};
				e.type = events::details::event_type::ON_RENDER;
				e.instance = this;
				on_render_cb(&e);
			}

			if (mode == kita_mode::OVERLAY && (last_wantcapture != io.WantCaptureMouse || last_wantcapture == -1))
			{
				last_wantcapture = io.WantCaptureMouse;
				glfwSetWindowAttrib(window, GLFW_MOUSE_PASSTHROUGH, last_wantcapture ? GLFW_FALSE : GLFW_TRUE);
			}
		}

		ImGui::Render();
		kita_glViewport(0, 0, display_w, display_h);
		kita_glClearColor(0.f, 0.f, 0.f, 0.f);
		kita_glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	if (state == kita_state::REQ_DESTROY)
		destroy(kita_state::DESTROY);

	return *this;
}

auto kita::kita_instance::position(int x, int y) -> kita_instance &
{
	if (state != kita_state::INITIALIZED)
		return *this;

	if (x == -1 && y == -1)
	{
		GLFWmonitor* monitors = glfwGetPrimaryMonitor();
		if (monitors)
		{
			int w, h;
			glfwGetMonitorWorkarea(monitors, &x, &y, &w, &h);
			glfwSetWindowPos(window, x + (w / 2 - width / 2), y + (h / 2 - height / 2));
		}
	}
	else
	{
		glfwSetWindowPos(window, x, y);
	}

	return *this;
}

auto kita::kita_instance::show() -> kita_instance &
{
	if (state != kita_state::INITIALIZED)
		return *this;

	glfwShowWindow(window);
	return *this;
}

auto kita::kita_instance::hide() -> kita_instance &
{
	if (state != kita_state::INITIALIZED)
		return *this;

	glfwHideWindow(window);
	return *this;
}

auto kita::kita_instance::minimize() -> kita_instance &
{
	if (state == kita_state::INITIALIZED)
		glfwIconifyWindow(window);
	return *this;
}

auto kita::kita_instance::restore() -> kita_instance &
{
	if (state == kita_state::INITIALIZED)
		glfwIconifyWindow(window);
	return *this;
}

auto kita::kita_instance::maximize() -> kita_instance &
{
	if (state == kita_state::INITIALIZED)
		glfwRestoreWindow(window);
	return *this;
}

auto kita::kita_instance::close() -> kita_instance &
{
	if (state == kita_state::INITIALIZED)
		destroy();
	return *this;
}

auto kita::kita_instance::toggle_mode() -> kita_instance &
{
	if (state == kita_state::INITIALIZED)
		mode == kita_mode::WINDOW ? overlay_mode() : window_mode();
	return *this;
}

auto kita::kita_instance::overlay_mode() -> kita_instance &
{
	if (state == kita_state::INITIALIZED)
	{
		mode = kita_mode::OVERLAY;
		glfwGetWindowSize(window, &trans_w, &trans_h);
		glfwGetWindowPos(window, &trans_x, &trans_y);

		int mc;
		GLFWmonitor ** monitors = glfwGetMonitors(&mc);
		for (int i = 0; i < (mc + 1); ++i)
		{
			int mx, my, mw, mh;
			glfwGetMonitorWorkarea(i == mc ? glfwGetPrimaryMonitor() : monitors[i], &mx, &my, &mw, &mh);
			if (trans_x >= mx && trans_x <= mx + mw && trans_y >= my && trans_y <= my + mh)
			{
				glfwSetWindowSize(window, mw, mh);
				glfwSetWindowPos(window, mx, my);
				glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
				break;
			}
		}
	}

	return *this;
}

auto kita::kita_instance::window_mode() -> kita_instance &
{
	if (state == kita_state::INITIALIZED)
	{
		mode = kita_mode::WINDOW;
		glfwSetWindowAttrib(window, GLFW_MOUSE_PASSTHROUGH, GLFW_FALSE);
		glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);

		glfwSetWindowSize(window, trans_w, trans_h);
		if (trans_x != -1 || trans_y != -1)
			glfwSetWindowPos(window, trans_x, trans_y);
	}

	return *this;
}

auto kita::kita_instance::title(const char * title) -> kita_instance &
{
	if (state != kita_state::INITIALIZED)
		return *this;

	if (title)
	{
		int i = 0;
		for (; i < sizeof(wnd_title) - 1 && *title; ++i)
		{
			wnd_title[i] = *title;
			++title;
		}

		wnd_title[i] = '\0';

		glfwSetWindowTitle(window, wnd_title);
	}

	return *this;
}

auto kita::kita_instance::title(void) -> const char *
{
	if (state != kita_state::INITIALIZED)
		return nullptr;

	return wnd_title;
}

auto kita::kita_instance::callback(events::details::on_close_cb_t handler) -> kita_instance &
{
	if (state == kita_state::INITIALIZED)
		on_close_cb = handler;

	return *this;
}

auto kita::kita_instance::callback(events::details::on_render_cb_t handler) -> kita_instance &
{
	if (state == kita_state::INITIALIZED)
		on_render_cb = handler;

	return *this;
}

auto kita::kita_instance::callback(events::details::on_glfwerr_cb_t handler) -> kita_instance &
{
	on_glfwerr = handler;
	// kita_error_callbacks[window] = this;
	return *this;
}

auto kita::kita_instance::callback(events::details::on_key_cb_t handler) -> kita_instance &
{
	if (state == kita_state::INITIALIZED)
	{
		on_key_cb = handler;
		kita_key_callbacks[window] = this;
		glfwSetKeyCallback(window, glfw_key_dispatcher);
	}

	return *this;	
}

kita::kita_instance::operator bool() const noexcept
{
	return state == kita_state::INITIALIZED;
}

auto kita::kita_instance::destroy(kita_state s) -> bool
{	
	state = s;

	// A destroy state means we have an initialized state,
	// therefore we cannot destroy those objects from here yet
	// what this does is that a destroy state is sent, the event
	// loop is broken and then the event loop with call this
	// again to finally destroy the objects we have initialized
	// since most likely we will be setting a destroy state from
	// and wndproc callback, if we destroy too early the render
	// thread might not receive the signal on time and try to access
	// an invalidated object. To summarize:
	// * check if its a req_destroy state signal
	// * if it is, just set the signal and let the event / render thread receive it first
	// * event / render thread calls this again but this time its a DESTROY state
	// * we can finally destroy the objects
	// We also need to do this since ReleaseDC can (or rather should) only be called from the thread that created it
	// so it would most likely be in the same thread that created this instance is running the run (render/event loop) method
	if (s != kita_state::REQ_DESTROY)
	{
		const std::lock_guard _lg(mut);
		--kita_instances;

		if (on_key_cb)
		{
			kita_key_callbacks[window] = nullptr;
		}

		if (window)
			glfwDestroyWindow(window);

		if (kita_instances == 0 && kita_glfw_init)
		{
			ImGui::DestroyContext();
			glfwTerminate();
		}
	}

	return true;
}

auto kita::kita_instance::glfw_error_dispatcher(int err_code, const char * description) -> void
{
	if (on_glfwerr)
	{
		events::on_glfwerr e {};
		e.error       = err_code;
		e.description = description;
		e.instance    = nullptr;
		e.type        = events::details::event_type::GLFW_ERROR;
		on_glfwerr(&e);
	}
}

auto kita::kita_instance::glfw_key_dispatcher(GLFWwindow * window, int key, int scancode, int action, int mods) -> void
{
	const auto & p = kita_key_callbacks.find(window);
	if (p != kita_key_callbacks.end() && p->second)
	{
		events::on_key e {};
		e.type     = events::details::event_type::GLFW_KEY;
		e.instance = p->second;
		e.action   = action;
		e.key      = key;
		e.action   = action;
		e.mods     = mods;

		p->second->on_key_cb(&e);
	}
}
