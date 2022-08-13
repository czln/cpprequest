project(unit-tests)

add_compile_options(-fsanitize=address)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR} ${PROJECT_NAME}_src)

message(STATUS "examples src: ${${PROJECT_NAME}_src}")
foreach(path ${${PROJECT_NAME}_src})
    #    get full path filename stripped suffix ".cpp"
    string(REGEX REPLACE ".*/" "" filename ${path})
    string(REGEX REPLACE "\.cpp$" "" out ${filename})

    link_directories(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
    message("debug: ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
    add_executable(${out} ${path})
    target_link_libraries(${out} cpprequest)
    target_link_options(${out} PRIVATE -fsanitize=address)
endforeach()
