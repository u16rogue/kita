# kita
[GitLab](https://gitlab.com/u16rogue/kita) • [GitHub](https://github.com/u16rogue/kita)<br>
Cross-platform quick deployment implementation of user interfaces with ImGui, GLFW and OpenGL.

<p align="center">
	<img src="https://gitlab.com/u16rogue/kita/-/raw/assets/ss.png"/>
</p>

```c++
#include <kita/kita.hpp>
auto main() -> int
{
	kita::kita_instance("kita", 300, 200)
		.position()
		.callback([](kita::events::on_render * e) { ImGui::Text("Hello world!"); })
		.title("kita title 2")
		.show()
		.run();

	return 0;
}
```
or
```c++
#include <kita/kita.hpp>

auto main() -> int
{
	auto ki = kita::kita_instance("kita", 300, 200);
	if (!ki)
		return 1;
	ki.position();
	ki += [](kita::events::on_render * e) {
		ImGui::Text("Hello world!");
	};
	ki.title("kita title 2");
	ki.show();
	ki.run();

	return 0;
}
```

## Events
* on_close
* on_render
* on_key
* on_glfwerr
* on_pre_render

## Display modes
* Overlay mode - The entire viewport is used for rendering, the on_render callback is used to render to the entire screen.
* Window mode - A single ImGui window represents the entire window, the on_render callback is used to render the content of that window.

## Preqrequisite
* cmake_minimum_required: 3.18
* Python (OPTIONAL! This is for the bootstrapper)
    * request
    * zipfile
    * os
    * shutil

## Usage
1. Add kita to your project `git submodule add --recursive --depth 1 -b master https://gitlab.com/u16rogue/kita.git <PATH>/kita`
	* (Optional) You might want to configure it as shallow with `git config -f .gitmodules submodule.<path>/kita.shallow true`
1. Run submodule update `git submodule update --init --recursive`
1. Run `kita_bootstrap.py` (This loads the latest ImGui release)
1. Include kita in your `CMakeLists.txt`
	```cmake
	# ...
	add_subdirectory("<PATH>/kita")
	# ...
	target_link_libraries(${PROJECT_NAME} PRIVATE kita)
	```
1. Access kita by `#include <kita/kita.hpp>`
1. Create an instance `kita::kita_instance(<title>, <width>, <height>)`

*kita is bound by a destructor! If the object is destroyed, the entire window and everything along it is also automatically destroyed!*

## Dependencies
*These dependencies are automatically installed*
* OpenGL 3
* [GLFW](https://github.com/glfw/glfw)
* [ImGui](https://github.com/ocornut/imgui)

## License
* GNU Lesser General Public License v2.1
