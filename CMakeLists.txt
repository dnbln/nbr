cmake_minimum_required(VERSION 3.22)
project(nbr C)

set(CMAKE_C_STANDARD 17)

add_executable(nbr main.c cfg_node.h nbr_driver.h nbr_driver.c cfg.c cfg.h opt_profile.c opt_profile.h optimize.c optimize.h config.h interpret.c interpret.h)
