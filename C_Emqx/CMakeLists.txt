# 设置 CMake 最低版本要求
cmake_minimum_required(VERSION 3.22)

# 设置项目名称和版本
project(C_Emqx C)

# 添加包含目录, 使得编译器能够找到头文件
include_directories(/usr/local/include)

# 添加可执行文件
# 语法: add_executable(<name> [WIN32] [MACOSX_BUNDLE] [EXCLUDE_FROM_ALL] source1 source2 ... sourceN) 
add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} PRIVATE /usr/local/lib/libmosquitto.so)
