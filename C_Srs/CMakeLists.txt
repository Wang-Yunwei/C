# 设置 CMake 最低版本
cmake_minimum_required(VERSION 3.22)

# 设置项目名称
project(C_Srs VERSION 1.0.0 LANGUAGES C)

# 设置 FFMPEG 根路径
set(FFMPEG_PATH /home/wyw/soft/ffmpeg)

# 设置 FFMPEG 头文件路径
set(FFMPEG_INCLUDE_DIRS "${FFMPEG_PATH}/include")

# 设置库文件的完整路径
set(FFMPEG_LIB_AV_CODEC "${FFMPEG_PATH}/lib/libavcodec.so")
set(FFMPEG_LIB_AV_FORMAT "${FFMPEG_PATH}/lib/libavformat.so")
set(FFMPEG_LIB_AV_UTIL "${FFMPEG_PATH}/lib/libavutil.so")

# 添加可执行文件
add_executable(${PROJECT_NAME} main.c)

target_include_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_INCLUDE_DIRS})

# 链接多个库
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${FFMPEG_LIB_AV_CODEC}   
    ${FFMPEG_LIB_AV_FORMAT} 
    ${FFMPEG_LIB_AV_UTIL} 
)

# 使用作用域
# PRIVATE: 链接的库仅对指定目标可见, 不会传递给依赖该目标的其他目标
# INTERFACE: 链接的库不对指定目标进行链接, 但会传递给依赖该目标的其他目标
# PUBLIC: 链接的库既对指定目标可见, 也会传递给依赖该目标的其他目标