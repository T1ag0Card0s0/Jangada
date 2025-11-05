set(JANGADA_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

add_subdirectory(${JANGADA_ROOT} ${CMAKE_BINARY_DIR}/jangada)

add_library(Jangada::kernel ALIAS jangada)

