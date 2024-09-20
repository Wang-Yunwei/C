# 设置 CMake 最低版本
cmake_minimum_required(VERSION 3.22)

# 设置项目名称
project(C_Exercise VERSION 1.0.0 LANGUAGES C)

# -------------------------- 第三方库 --------------------------

# 引入 mosquitto 库
set(MOSQUITTO_INCLUDE_DIRS resource/mosquitto/include)
message(">>> MOSQUITTO_INCLUDE_DIRS: ${MOSQUITTO_INCLUDE_DIRS}")
# file(GLOB_RECURSE MOSQUITTO_LIB resource/mosquitto/lib/libmosquitto.so)
# 命令在 CMake 中每次只能查找一个库文件, 如果你需要查找多个库文件, 你需要多次调用 find_library
find_library(MOSQUITTO_LIB NAMES libmosquitto.so PATHS resource/mosquitto/lib NO_DEFAULT_PATH)
message(">>> MOSQUITTO_LIB: ${MOSQUITTO_LIB}")

# 引入 ffmpeg 库
set(FFMPEG_INCLUDE_DIRS resource/ffmpeg/include)
message(">>> FFMPEG_INCLUDE_DIRS: ${FFMPEG_INCLUDE_DIRS}")
# 使用 file(GLOB) 生成库文件列表
file(GLOB_RECURSE FFMPEG_LIBS resource/ffmpeg/lib/*.so)
message(">>> FFMPEG_LIBS: ${FFMPEG_LIBS}")

# 引入 psdk 库
execute_process(COMMAND uname -m OUTPUT_VARIABLE DEVICE_SYSTEM_ID) # 获取设备架构
# 判断设备架构
if (DEVICE_SYSTEM_ID MATCHES x86_64)
    set(TOOLCHAIN_NAME x86_64-linux-gnu-gcc)
    add_definitions(-DPLATFORM_ARCH_x86_64=1)
else()
    message(FATAL_ERROR "FATAL: Please confirm your platform.")
endif()
set(PAYLOAD_SDK_INCLUDE_DIRS resource/psdk_lib/include)
# 命令在 CMake 中每次只能查找一个库文件, 如果你需要查找多个库文件, 你需要多次调用 find_library
find_library(PAYLOAD_SDK_LIB NAMES libpayloadsdk.a PATHS resource/psdk_lib/lib/x86_64-linux-gnu-gcc NO_DEFAULT_PATH)

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
    "${MODULE_APP_UTILS}/UdpSocket.c"
)
# 包含头文件
target_include_directories(${PROJECT_NAME} PRIVATE
    ${MOSQUITTO_INCLUDE_DIRS}
    ${FFMPEG_INCLUDE_DIRS}
    ${PAYLOAD_SDK_INCLUDE_DIRS}
)
# 链接库文件
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${MOSQUITTO_LIB}
    ${FFMPEG_LIBS}
    ${PAYLOAD_SDK_LIB}
)
