make_package(jam res/buildings/bighouse.png res/buildings/coal_plant.png res/buildings/house_deco.png res/buildings/house_wall.png res/buildings/nuclear_plant.png res/buildings/power_tower.png res/buildings/shop.png res/buildings/solar.png res/buildings/town_hall.png)
add_executable(jam src/main.cc ${CMAKE_CURRENT_BINARY_DIR}/jam.c)
target_link_libraries(jam game2d)