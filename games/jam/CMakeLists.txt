make_package(jam res/gimp.png res/gimp.jpg res/grass.png res/test.json)
add_executable(jam src/main.cc ${CMAKE_CURRENT_BINARY_DIR}/jam.c)
target_link_libraries(jam game2d)