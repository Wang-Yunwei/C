# 设置 CMake 最低版本
cmake_minimum_required(VERSION 3.22)

# 设置项目名称
project(C_Exercise VERSION 1.0.0 LANGUAGES C)

# -------------------------- 第三方库 --------------------------

# 引入 mosquitto 库
set(MOSQUITTO_INCLUDE_DIRS resource/mosquitto/include)
message(">>> MOSQUITTO_INCLUDE_DIRS: ${MOSQUITTO_INCLUDE_DIRS}")
file(GLOB_RECURSE MOSQUITTO_LIB resource/mosquitto/lib/libmosquitto.so)
message(">>> MOSQUITTO_LIB: ${MOSQUITTO_LIB}")

# 引入 ffmpeg 库
set(FFMPEG_INCLUDE_DIRS resource/ffmpeg/include)
message(">>> FFMPEG_INCLUDE_DIRS: ${FFMPEG_INCLUDE_DIRS}")
file(GLOB_RECURSE FFMPEG_LIBS resource/ffmpeg/lib/*.so)
message(">>> FFMPEG_LIBS: ${FFMPEG_LIBS}")

# -------------------------- 第三方库 --------------------------

set(MODULE_APP_MAIN src/main.c)
set(MODULE_APP_UTILS src/utils)

# 添加可执行文件
add_executable(${PROJECT_NAME}
    ${MODULE_APP_MAIN}
    "${MODULE_APP_UTILS}/MQClient.c"
    "${MODULE_APP_UTILS}/FFmpegClient.c"
    "${MODULE_APP_UTILS}/TcpClient.c"
    "${MODULE_APP_UTILS}/TcpServer.c"
)
# 包含头文件
target_include_directories(${PROJECT_NAME} PRIVATE
    ${MOSQUITTO_INCLUDE_DIRS}
    ${FFMPEG_INCLUDE_DIRS}
)
# 链接库文件
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${MOSQUITTO_LIB}
    ${FFMPEG_LIBS}
)
