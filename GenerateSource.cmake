if(NOT DEFINED input_file OR NOT DEFINED output_file)
    message(FATAL_ERROR "You must define input_file and output_file")
endif()

file(READ ${input_file} file_contents HEX)
string(REGEX REPLACE "(..)" "\\\\x\\1" formatted_contents "${file_contents}")

file(WRITE ${output_file} "#ifndef SOURCE_HPP\n#define SOURCE_HPP\n\n")
file(APPEND ${output_file} "static constexpr const char source[] = \"${formatted_contents}\";\n\n")
file(APPEND ${output_file} "#endif // SOURCE_HPP\n")
