make_package(jam res/red_glass.png res/green_glass.png)
add_executable(jam src/main.cc ${CMAKE_CURRENT_BINARY_DIR}/jam.c)
target_link_libraries(jam game2d)