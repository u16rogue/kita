#pragma once

// THIS PART OF THE LIBRARY IS NOT MEANT FOR EXTERNAL USE
// Some implementations in here aren't that good and solely
// exist for internal use

namespace kita
{
	template <typename T, auto * deleter>
	struct unique_handle
	{
		~unique_handle()
		{
			if (v)
				deleter(v);
		}

		T v;
	};

	template <typename T>
	struct _defer
	{
		_defer(T cb_)
			: cb(cb_)
		{}

		~_defer()
		{
			cb();
		}

		T cb;
	};

	#define __KITA_COMB_(n, ln) \
		n##ln

	#define __KITA_DEFER_NAME(ln) \
		__KITA_COMB_(__defer_, ln)

	#define KITA_DEFER(lambda) \
		const auto __KITA_DEFER_NAME(__LINE__) = kita::_defer(lambda)
}