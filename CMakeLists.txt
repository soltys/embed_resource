cmake_minimum_required(VERSION 3.5)
project(EmbedResource)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

function(embed_resources out_var)
    set(result)
    foreach(in_f ${ARGN})
        file(RELATIVE_PATH src_f ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/${in_f}")
        set(out_f "${PROJECT_BINARY_DIR}/${in_f}.hpp")
        get_filename_component(out_f_dir "${out_f}" DIRECTORY)
        add_custom_command(OUTPUT ${out_f}
                COMMAND ${CMAKE_COMMAND} -E make_directory "${out_f_dir}"
                COMMAND embed-resource "${out_f}" "${src_f}"
                DEPENDS "${in_f}" embed-resource
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMENT "Building binary file for embedding ${out_f}"
                VERBATIM)
        list(APPEND result "${out_f}")
    endforeach()
    set(${out_var} "${result}" PARENT_SCOPE)
    get_target_property(EMBED_RESOURCES_SOURCE_DIR embed-resource SOURCE_DIR)
    include_directories(${out_f_dir} ${EMBED_RESOURCES_SOURCE_DIR})
endfunction()

add_executable(embed-resource embedresource.cpp Resource.h)
target_link_libraries(embed-resource)
