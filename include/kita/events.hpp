#pragma once

namespace kita
{
	class kita_instance;
}

namespace kita::events
{	

	struct on_close;
	struct on_render;
	struct on_key;
	struct on_glfwerr;
	struct on_pre_render; 
	namespace details 
	{

		enum class event_type
		{
			GENERIC,
			ON_CLOSE,
			ON_RENDER,
			GLFW_ERROR,
			GLFW_KEY,
			ON_PRE_RENDER,
		};

		struct basic_event
		{
			event_type type;
			kita_instance * instance;
		};

		using on_close_cb_t = void(*)(on_close *);
		using on_render_cb_t = void(*)(on_render *);
		using on_key_cb_t = void(*)(on_key *);
		using on_glfwerr_cb_t = void(*)(on_glfwerr *);
		using on_pre_render_cb_t = void(*)(on_pre_render *);
	}

	// ----------------------------------------------------------

	struct on_close : public details::basic_event
	{
		bool cancel = false;
	};


	// ----------------------------------------------------------

	struct on_render : public details::basic_event
	{
		
	};
	

	// ----------------------------------------------------------

	struct on_glfwerr : public details::basic_event
	{
		int error;
		const char * description;
	};


	// ----------------------------------------------------------

	struct on_key : public details::basic_event
	{
		int key;
		int scancode;
		int action;
		int mods;
	};

	// ----------------------------------------------------------

	struct on_pre_render : public details::basic_event
	{
		bool render = true;
	};
};