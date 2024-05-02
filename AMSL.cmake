function(add_amsl_target target_name target_source_file)
    set(SOURCES
            include/amsl.hpp include/utils.hpp include/string.hpp include/lexer.hpp include/token.hpp
            include/parser.hpp include/expression.hpp include/ptr_wrapper.hpp include/compiler.hpp include/executor.hpp
            include/analyzed_expression.hpp include/analyzer.hpp include/encoder.hpp include/bytes.hpp
            include/traits.hpp include/encodable.hpp
    )
    add_executable(${target_name} src/main.cpp ${SOURCES})

    set(generated_source_file "${target_source_file}.hpp")
    set(absolute_generated_source_file "${CMAKE_SOURCE_DIR}/${target_source_file}.hpp")
    set(generate_source_file_target_name "GenerateSource-${target_name}")

    target_include_directories(${target_name} PRIVATE include)
    target_compile_options(${target_name} PRIVATE "-fconstexpr-depth=1000000" "-ftemplate-depth=1000000")
    target_compile_definitions(${target_name} PRIVATE "-DSOURCE_FILE=\"${absolute_generated_source_file}\"")

    add_custom_command(
            OUTPUT ${generated_source_file}
            COMMAND ${CMAKE_COMMAND} -Dinput_file=${CMAKE_SOURCE_DIR}/${target_source_file} -Doutput_file=${absolute_generated_source_file} -P "${CMAKE_SOURCE_DIR}/GenerateSource.cmake"
            DEPENDS ${target_source_file}
            COMMENT "Generating C++ ready source file ${generated_source_file}"
    )

    add_custom_target(
            ${generate_source_file_target_name} ALL
            DEPENDS ${generated_source_file}
    )

    add_custom_command(
            TARGET ${target_name}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove ${absolute_generated_source_file}
            COMMENT "Cleaning up ${generated_source_file}"
    )

    add_dependencies(${target_name} ${generate_source_file_target_name})

    set(introspection_target_name "${target_name}-introspect")
    add_executable(${introspection_target_name} src/introspect.cpp ${SOURCES})
    target_include_directories(${introspection_target_name} PRIVATE include)
    target_compile_options(${introspection_target_name} PRIVATE "-fconstexpr-depth=1000000" "-ftemplate-depth=1000000")
    target_compile_definitions(${introspection_target_name} PRIVATE "-DSOURCE_FILE=\"${absolute_generated_source_file}\"")
    add_dependencies(${introspection_target_name} ${generate_source_file_target_name})
endfunction()
