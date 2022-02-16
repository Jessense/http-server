aux_source_directory(. DIR_LIB_SRC)
add_library(mylib ${DIR_LIB_SRC})
target_link_libraries(mylib pthread)