cmake_minimum_required(VERSION 3.28...3.29)

# Project Version
set(PROJECT_VERSION 0.2.0)
project(EmbedResource
    VERSION ${PROJECT_VERSION})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(BOOST_REQUIRED_LIBRARIES program_options)
include(FetchContent)
FetchContent_Declare(
  Boost
  URL https://github.com/boostorg/boost/releases/download/boost-1.85.0/boost-1.85.0-cmake.zip  
  EXCLUDE_FROM_ALL
)
set(BOOST_INCLUDE_LIBRARIES ${BOOST_REQUIRED_LIBRARIES})
FetchContent_MakeAvailable(Boost)

foreach(lib_name ${BOOST_REQUIRED_LIBRARIES})
    set_target_properties(boost_${lib_name} PROPERTIES FOLDER "Dependencies")
endforeach()

configure_file("embededresource.h.in" "embededresource.h")

function(embed_resources out_var)
    set(flags)
    set(args)
    set(listArgs FILES)

    cmake_parse_arguments(er_arg "${flags}" "${args}" "${listArgs}" ${ARGN})

    if (NOT er_arg_FILES)
        message(FATAL_ERROR "[embed_resources]: FILES is a required argument")
    endif()

    set(er_ADDTIONAL_ARGS "")
    if(er_arg_CONTENT_AS_COMMENT)
        set(er_ADDTIONAL_ARGS "${er_ADDTIONAL_ARGS} -c")
    endif()

    set(result)
    foreach(in_f ${er_arg_FILES})
        file(RELATIVE_PATH src_f ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/${in_f}")
        set(out_f "${PROJECT_BINARY_DIR}/${in_f}.hpp")
        get_filename_component(out_f_dir "${out_f}" DIRECTORY)
        add_custom_command(OUTPUT ${out_f}
                COMMAND ${CMAKE_COMMAND} -E make_directory "${out_f_dir}"
                COMMAND embed_resource -i "${src_f}" -o "${out_f}" ${er_ADDTIONAL_ARGS}
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

add_executable(embed_resource embedresource.cpp embededresource.h Resource.h)
target_link_libraries(embed_resource Boost::program_options)
target_include_directories(embed_resource PRIVATE ${PROJECT_BINARY_DIR})

set_target_properties(embed_resource PROPERTIES PROJECT_LABEL "Embed Resource")
