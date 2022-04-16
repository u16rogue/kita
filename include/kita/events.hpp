#pragma once

namespace kita
{
	class kita_instance;
}

namespace kita::events
{
	enum class event_type
	{
		GENERIC,
		ON_CLOSE,
		ON_RENDER,
		GLFW_ERROR,
		GLFW_KEY
	};

	struct basic_event
	{
		event_type type;
		kita::kita_instance * instance;
	};
	
	// ----------------------------------------------------------

	struct on_close : public basic_event
	{
		bool cancel = false;
	};
	using on_close_cb_t = void(*)(on_close *);

	// ----------------------------------------------------------

	struct on_render : public basic_event
	{
	};
	using on_render_cb_t = void(*)(on_render *);

	// ----------------------------------------------------------

	struct on_glfwerr : public basic_event
	{
		int error;
		const char * description;
	};
	using on_glfwerr_cb_t = void(*)(on_glfwerr *);

	// ----------------------------------------------------------

	struct on_key : public basic_event
	{
		int key;
		int scancode;
		int action;
		int mods;
	};
	using on_key_cb_t = void(*)(on_key *);

};