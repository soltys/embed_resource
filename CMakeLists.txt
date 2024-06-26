cmake_minimum_required(VERSION 3.28...3.29)

# Project Version
project(EmbedResource
    DESCRIPTION "Creates C++ Embeded Resources as .hpp files"
    HOMEPAGE_URL "https://github.com/soltys/embed_resource"
    VERSION 0.2.2)

configure_file("config.h.in" "${PROJECT_BINARY_DIR}/internal/config.h")

add_executable(embed_resource main.cpp EmbedResource.hpp args.hxx)
target_include_directories(embed_resource PRIVATE "${PROJECT_BINARY_DIR}")

set_target_properties(embed_resource PROPERTIES
                        PROJECT_LABEL "Embed Resource" 
                        CXX_STANDARD 17
                        CXX_STANDARD_REQUIRED YES
                        CXX_EXTENSIONS Off)

function(embed_resources out_var)
    set(flags CONTENT_AS_COMMENT)
    set(args)
    set(listArgs FILES)

    cmake_parse_arguments(ER_ARG "${flags}" "${args}" "${listArgs}" ${ARGN})

    if (NOT ER_ARG_FILES)
        message(FATAL_ERROR "[embed_resources]: FILES is a required argument")
    endif()

    if(${ER_ARG_CONTENT_AS_COMMENT})
        set(er_COMMENT_ARG "-c")
    endif()
    set(result)
    foreach(in_f ${ER_ARG_FILES})
        file(RELATIVE_PATH src_f ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/${in_f}")
        set(out_f "${PROJECT_BINARY_DIR}/${in_f}.hpp")
        get_filename_component(out_f_dir "${out_f}" DIRECTORY)
        add_custom_command(OUTPUT ${out_f}
                COMMAND ${CMAKE_COMMAND} -E make_directory "${out_f_dir}"
                COMMAND embed_resource ${er_COMMENT_ARG} -i "${src_f}" -o "${out_f}" 
                DEPENDS "${in_f}" embed_resource
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMENT "Building binary file for embedding ${out_f} from ${src_f}"
                VERBATIM)
        list(APPEND result "${out_f}")
    endforeach()
    set(${out_var} "${result}" PARENT_SCOPE)
    get_target_property(EMBED_RESOURCES_SOURCE_DIR embed_resource SOURCE_DIR)
    include_directories(${out_f_dir} ${EMBED_RESOURCES_SOURCE_DIR})
endfunction()

