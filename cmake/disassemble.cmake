if(NOT DEFINED OBJDUMP OR NOT DEFINED INPUT OR NOT DEFINED OUTPUT)
    message(FATAL_ERROR "Missing disassembly arguments")
endif()

execute_process(
    COMMAND "${OBJDUMP}" -d "${INPUT}"
    OUTPUT_FILE "${OUTPUT}"
    ERROR_VARIABLE error_output
    RESULT_VARIABLE result
)

if(NOT result EQUAL 0)
    message(FATAL_ERROR
        "Disassembly failed: ${result}\n${error_output}")
endif()
