# Embed Resource

**FORK OF: https://github.com/cyrilcode/embed-resource main difference it creates `.hpp` files and not `.cpp` files**

Embed binary files and resources (such as GLSL Shader source files) into
C++ projects.

Include this repository in your CMake based project:

    git submodule add https://github.com/cyrilcode/embed-resource.git lib/embed-resource

Then add to your CMakeLists.txt for your project:

    include_directories(lib/embed-resource)
    add_subdirectory(lib/embed-resource)

Now you can add your resources, by calling the provided `embed_resources()` function in your
CMakeLists.txt file:

    embed_resources(MyResources shaders/vertex.glsl shaders/frag.glsl)

Then link to your binary by adding the created variable to your add_executable statement:

    add_executable(MyApp ${SOURCE_FILES} ${MyResources})

In your C++ project you can access your embed resources using the Resource class
provided in `Resource.h`. Here's an example:

    #include <iostream>
    using namespace std;

    #include "frag.glsl.hpp"

    int main() {

        Resource text = LOAD_RESOURCE(frag_glsl);
        cout << string(text.data(), text.size()) << endl;

        return EXIT_SUCCESS;
    }
