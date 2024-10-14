/**
 * ********************************************************************
 * Created by WangYunwei [2024-10-09] 基础相机管理
 * ********************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "ms_camera_manage.h"

/* Private types -------------------------------------------------------------*/
typedef struct
{
    E_DjiCameraType cameraType;
    char *cameraTypeStr;
} T_DjiTestCameraTypeStr;

/* Private constants ---------------------------------------------------------*/
static E_DjiMountPosition mountPosition = DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1; // 相机挂载点

/* Private variables ---------------------------------------------------------*/
static const T_DjiTestCameraTypeStr s_cameraTypeStrList[] = {
    {DJI_CAMERA_TYPE_UNKNOWN, "Unknown"},
    {DJI_CAMERA_TYPE_Z30, "Zenmuse Z30"},
    {DJI_CAMERA_TYPE_XT2, "Zenmuse XT2"},
    {DJI_CAMERA_TYPE_PSDK, "Payload Camera"},
    {DJI_CAMERA_TYPE_XTS, "Zenmuse XTS"},
    {DJI_CAMERA_TYPE_H20, "Zenmuse H20"},
    {DJI_CAMERA_TYPE_H20T, "Zenmuse H20T"},
    {DJI_CAMERA_TYPE_P1, "Zenmuse P1"},
    {DJI_CAMERA_TYPE_L1, "Zenmuse L1"},
    {DJI_CAMERA_TYPE_L2, "Zenmuse L2"},
    {DJI_CAMERA_TYPE_H20N, "Zenmuse H20N"},
    {DJI_CAMERA_TYPE_M30, "M30 Camera"},
    {DJI_CAMERA_TYPE_M30T, "M30T Camera"},
    {DJI_CAMERA_TYPE_M3E, "M3E Camera"},
    {DJI_CAMERA_TYPE_M3T, "M3T Camera"},
    {DJI_CAMERA_TYPE_M3D, "M3D Camera"},
    {DJI_CAMERA_TYPE_M3TD, "M3TD Camera"},
    {DJI_CAMERA_TYPE_H30, "H30 Camera"},
    {DJI_CAMERA_TYPE_H30T, "H30T Camera"},
};

/* Private functions declaration ---------------------------------------------*/
/**
 * 初始化摄像头管理模块
 */
static void F_Init()
{
    T_DjiReturnCode returnCode = DjiCameraManager_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("初始化相机管理器失败, 错误编码: 0x%08X\r\n", returnCode);
    }
}

/**
 * 反初始化摄像头管理模块
 */
static void F_DeInit()
{
    T_DjiReturnCode returnCode = DjiCameraManager_DeInit();
}

/**
 * 选定挂载位置的相机类型 - 获取
 *
 * E_DjiMountPosition 安装位置
 *      DJI_MOUNT_POSITION_UNKNOWN = 0
 *      DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1 = 1
 *      DJI_MOUNT_POSITION_PAYLOAD_PORT_NO2 = 2
 *      DJI_MOUNT_POSITION_PAYLOAD_PORT_NO3 = 3
 *      DJI_MOUNT_POSITION_EXTENSION_PORT = 4
 *      DJI_MOUNT_POSITION_EXTENSION_LITE_PORT = 5
 *
 * E_DjiCameraType 相机类型
 *      DJI_CAMERA_TYPE_UNKNOWN = 0, 相机类型为 unknown
 *      DJI_CAMERA_TYPE_Z30 = 20, 相机类型为 Z30
 *      DJI_CAMERA_TYPE_XT2 = 26, 相机类型为 XT2
 *      DJI_CAMERA_TYPE_PSDK = 31, 相机类型为基于 PSDK 的三方相机
 *      DJI_CAMERA_TYPE_XTS = 41, 相机类型为 XT S
 *      DJI_CAMERA_TYPE_H20 = 42, 相机类型为 H20
 *      DJI_CAMERA_TYPE_H20T = 43, 相机类型为 H20T
 *      DJI_CAMERA_TYPE_H20N = 61, 相机类型为 H20N
 *      DJI_CAMERA_TYPE_P1 = 50, 相机类型为 P1
 *      DJI_CAMERA_TYPE_L1, 相机类型为 L1
 *      DJI_CAMERA_TYPE_L2, 相机类型为 L2
 *      DJI_CAMERA_TYPE_M30, 相机类型为 M30
 *      DJI_CAMERA_TYPE_M30T, 相机类型为 M30T
 *      DJI_CAMERA_TYPE_M3E, 相机类型为 M3E
 *      DJI_CAMERA_TYPE_M3T, 相机类型为 M3T
 *      DJI_CAMERA_TYPE_M3D, 相机类型为 Matrice 3D
 *      DJI_CAMERA_TYPE_M3TD, 相机类型为 Matrice 3TD
 */
static void F_CameraType_Get()
{
    E_DjiCameraType cameraType;
    T_DjiReturnCode returnCode = DjiCameraManager_GetCameraType(mountPosition, &cameraType);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("获取安装位置 %d 相机类型失败, 错误编码: 0x%08X\r\n", mountPosition, returnCode);
    }
    for (int i = 0; i < sizeof(s_cameraTypeStrList) / sizeof(s_cameraTypeStrList[0]); i++)
    {
        if (s_cameraTypeStrList[i].cameraType == cameraType)
        {
            USER_LOG_INFO("安装位置 %d 摄像机的类型为 %s", mountPosition, s_cameraTypeStrList[i].cameraTypeStr);
        }
    }
}

/**
 * 选定挂载位置的相机固件版本 - 获取
 *
 * T_DjiCameraManagerFirmwareVersion
 *      uint8_t firmware_version[4]
 *          - #define DJI_VERSION_MAJOR     3   // DJI SDK 主版本号, 当有不兼容的 API 更改时, 范围从 0 到 99
 *          - #define DJI_VERSION_MINOR     8   // DJI SDK 次版本号, 当以向后兼容的方式添加功能时会发生变化, 范围从 0 到 99
 *          - #define DJI_VERSION_MODIFY    0   // DJI SDK 修改版本号, 当有向后兼容的错误修复更改时, 范围从 0 到 99。
 *          - #define DJI_VERSION_BETA      0   // DJI SDK beta 版本信息, 发布版本为0, 当beta版本发布变化时, 范围从 0 到 255
 *          - #define DJI_VERSION_BUILD     1929    // DJI SDK 版本构建信息, 当 jenkins 触发构建更改时, 范围从 0 到 65535
 */
static void F_FirmwareVersion_Get()
{
    T_DjiCameraManagerFirmwareVersion firmwareVersion;
    T_DjiReturnCode returnCode = DjiCameraManager_GetFirmwareVersion(mountPosition, &firmwareVersion);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("安装位置 %d 相机的固件版本失败, 错误编码: 0x%08X\r\n", mountPosition, returnCode);
    }
    USER_LOG_INFO("安装位置 %d 相机的固件是 V%d.%d.%d.%d\r\n", mountPosition, firmwareVersion.firmware_version[0], firmwareVersion.firmware_version[1], firmwareVersion.firmware_version[2], firmwareVersion.firmware_version[3]);
}

/**
 * 相机连接状态 - 获取
 */
static void F_CameraConnectStatus_Get()
{
    bool connectStatus;
    T_DjiReturnCode returnCode = DjiCameraManager_GetCameraConnectStatus(mountPosition, &connectStatus);
}

/**
 * 选定挂载位置相机的工作模式 - 设置
 *      注: 可将相机的工作模式设置为拍照、录像、回放、下载等, 当有任务正在进行时不可更改模式, 更改模式需要 1-2 s
 *
 * E_DjiCameraManagerWorkMode
 *      DJI_CAMERA_MANAGER_WORK_MODE_SHOOT_PHOTO = 0,           // 拍照模式, 用户可在此模式下拍照
 *      DJI_CAMERA_MANAGER_WORK_MODE_RECORD_VIDEO = 1,          // 录像模式, 用户可在此模式下录制视频
 *      DJI_CAMERA_MANAGER_WORK_MODE_PLAYBACK = 2,              // 回放模式, 用户可预览和删除照片和视频
 *                                                                      - 适用于: Phantom 3 专业版、X3、X5、X5R、Phantom 4
 *                                                                      - 不支持: Z30、X5S、X4S、Phantom 4 Pro、Mavic Pro、Phantom 3标准版、Phantom 3 高级版、Phantom 3 4K 和 Osmo 系列
 *      DJI_CAMERA_MANAGER_WORK_MODE_MEDIA_DOWNLOAD = 3,        // 媒体下载模式, 用户可以下载媒体到移动设备, 不支持: 挂载在飞机上的 X5 和 X5R 摄像机
 *      DJI_CAMERA_MANAGER_WORK_MODE_BROADCAST = 4,             // 此模式可以录制视频, 此模式下, 分辨率和帧率设置为 1080i50(PAL) 或 720p60(NTSC) 照片拍摄仅能在视频录制时进行
 *                                                                      - 只支持通过更换模式为 RECORD_VIDEO 来退出直播模式, 仅Inspire 2 支持此模式
 *      DJI_CAMERA_MANAGER_WORK_MODE_WORK_MODE_UNKNOWN = 0xFF,  // 摄像机工作模式, 未知
 */
static void F_Mode_Set(E_DjiCameraManagerWorkMode workMode)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetMode(mountPosition, workMode);
}

/**
 * 选定挂载位置相机的工作模式 - 获取
 */
static void F_Mode_Get()
{
    E_DjiCameraManagerWorkMode workMode;
    T_DjiReturnCode returnCode = DjiCameraManager_GetMode(mountPosition, &workMode);
}

/**
 * 选定挂载位置相机的拍照模式 - 设置
 *
 * E_DjiCameraManagerShootPhotoMode
 *      DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_SINGLE = 0x01,      // 单拍模式
 *      DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_HDR = 0x02,         // HDR 模式, X5、X5R、XT、Z30、Phantom 4 Pro、X4S 和 X5S 相机不支持此模式
 *      DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_BURST = 0x04,       // 连拍模式, XT 相机不支持
 *      DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_AEB = 0x05,         // AEB 模式, 可快速拍摄多张(默认三张) 不同曝光值的照片, XT 和 Z30 相机不支持
 *      DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_INTERVAL = 0x06,    // 定时拍摄模式, JPEG 的最小拍摄间隔为 2s, RAW 和 RAW+JPEG 的最小拍摄间隔为 10s, 部分机型的 RAW 和 RAW+JPEG 拍摄最小间隔为 5s
 *      DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_RAW_BURST = 0x09,   // RAW 连拍模式, 仅 X5S 摄像机支持
 *      DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_REGIONAL_SR = 0x16, // 区域拍摄模式, 支持 H20/H20T
 *      DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_UNKNOWN = 0xFF,     // 拍照模式, 未知
 */
static void F_ShootPhotoMode_Set(E_DjiCameraManagerShootPhotoMode mode)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetShootPhotoMode(mountPosition, mode);
}

/**
 * 选定挂载位置相机的拍照模式 - 获取
 */
static void F_ShootPhotoMode_Get()
{
    E_DjiCameraManagerShootPhotoMode takePhotoMode;
    T_DjiReturnCode returnCode = DjiCameraManager_GetShootPhotoMode(mountPosition, &takePhotoMode);
}

/**
 * 拍照 - 开始
 *      注: 相机必须处于拍照模式!
 *
 * E_DjiCameraManagerShootPhotoMode // 同 F_ShootPhotoMode_Set()
 */
static void F_StartShootPhoto(E_DjiCameraManagerShootPhotoMode mode)
{
    T_DjiReturnCode returnCode = DjiCameraManager_StartShootPhoto(mountPosition, mode);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("安装位置 %d 相机拍摄照片失败, 错误码: 0x%08X", mountPosition, returnCode);
    }
}

/**
 * 拍照 - 停止
 *      注: StartShootPhoto 被调用，当前处于拍摄模式
 */
static void F_StopShootPhoto()
{
    T_DjiReturnCode returnCode = DjiCameraManager_StopShootPhoto(mountPosition);
}

/**
 * 相机拍摄状态 - 获取
 *      注: L1/P1 不支持此接口
 *
 * E_DjiCameraManagerCapturingState
 *      DJI_CAMERA_MANAGER_CAPTURING_STATE_IDLE = 0,    // 未拍摄
 *      DJI_CAMERA_MANAGER_CAPTURING_STATE_SINGLE = 1,  // 单次拍摄中
 *      DJI_CAMERA_MANAGER_CAPTURING_STATE_MULTI = 2,   // 多次拍摄中
 */
static void F_CapturingState_Get()
{
    E_DjiCameraManagerCapturingState capturingState;
    T_DjiReturnCode returnCode = DjiCameraManager_GetCapturingState(mountPosition, &capturingState);
}

/**
 * 在连拍模式下设置连拍张数 - 设置
 *
 * E_DjiCameraBurstCount
 *      DJI_CAMERA_BURST_COUNT_2  = 2,          // 在连拍模式下每次连续拍摄的照片张数为 2
 *      DJI_CAMERA_BURST_COUNT_3 = 3,           // 在连拍模式下每次连续拍摄的照片张数为 3
 *      DJI_CAMERA_BURST_COUNT_5 = 5,           // 在连拍模式下每次连续拍摄的照片张数为 5
 *      DJI_CAMERA_BURST_COUNT_7 = 7,           // 在连拍模式下每次连续拍摄的照片张数为 7
 *      DJI_CAMERA_BURST_COUNT_10 = 10,         // 在连拍模式下每次连续拍摄的照片张数为 10, 仅被X4S、X5S、Phantom 4 Pro 相机支持
 *      DJI_CAMERA_BURST_COUNT_14 = 14,         // 在连拍模式下每次连续拍摄的照片张数为 14, 仅被X4S、X5S、Phantom 4 Pro 相机支持
 *      DJI_CAMERA_BURST_COUNT_KNOWN = 0xFF,    // 在连拍模式下每次连续拍摄的照片张数为 unknown
 */
static void F_PhotoBurstCount_Set(E_DjiCameraBurstCount count)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetPhotoBurstCount(mountPosition, count);
}

/**
 * 在连拍模式下获取连拍张数 - 获取
 */
static void F_PhotoBurstCount_Get()
{
    E_DjiCameraBurstCount count;
    T_DjiReturnCode returnCode = DjiCameraManager_GetPhotoBurstCount(mountPosition, &count);
}

/**
 * AEB(自动曝光拍摄) 模式下的连拍数量 - 设置
 * TODO - 该函数已弃用
 */
// static void F_PhotoAEBCount_Set(E_DjiCameraManagerPhotoAEBCount count)
// {
//     T_DjiReturnCode returnCode = DjiCameraManager_SetPhotoAEBCount(mountPosition, count);
// }

/**
 * AEB(自动曝光拍摄) 模式下的连拍数量 - 获取
 * TODO - 该函数已弃用
 */
// static void F_PhotoAEBCount_Get()
// {
//     E_DjiCameraManagerPhotoAEBCount count;
//     T_DjiReturnCode returnCode = DjiCameraManager_GetPhotoAEBCount(mountPosition, &count);
// }

/**
 * 间隔拍摄模式的参数 - 设置
 *      注: 在此拍照模式下, 相机将拍摄一张照片,等待指定时间间隔,然后继续拍摄,直至完成所需数量的照片,该模式也支持热成像相机
 *
 * T_DjiCameraPhotoTimeIntervalSettings
 *      uint8_t captureCount;   // 指定间隔设置的总拍摄计数 0: 保留, 1~254: 数字, 255: 一直拍摄直到停止
 *      uint16_t timeIntervalSeconds;   // 指定两次拍摄之间的间隔时间，单位: s
 */
static void F_PhotoTimeIntervalSettings_Set(T_DjiCameraPhotoTimeIntervalSettings intervalSetting)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetPhotoTimeIntervalSettings(mountPosition, intervalSetting);
}

/**
 * 间隔拍摄模式的参数 - 获取
 */
static void F_PhotoTimeIntervalSettings_Get()
{
    T_DjiCameraPhotoTimeIntervalSettings intervalSetting;
    T_DjiReturnCode returnCode = DjiCameraManager_GetPhotoTimeIntervalSettings(mountPosition, &intervalSetting);
}

/**
 * 间隔拍摄的剩余时间 - 获取
 *      注: L1/P1 不支持此 API
 */
static void F_IntervalShootingRemainTime_Get()
{
    uint8_t remainTime; // 剩余时间 (以秒为单位)
    T_DjiReturnCode returnCode = DjiCameraManager_GetIntervalShootingRemainTime(mountPosition, &remainTime);
}

/**
 * 选定相机挂载位置的相机对焦模式 - 设置
 *      注: 设置镜头对焦模式, 自动对焦模式下, 目标点为焦点; 手动对焦模式下,若启用了对焦助手,目标点为缩小区域
 *
 * E_DjiCameraManagerFocusMode
 *      DJI_CAMERA_MANAGER_FOCUS_MODE_MANUAL = 0,       // 手动对焦模式, 在此模式下,用户可通过调整对焦环值来调整焦距
 *      DJI_CAMERA_MANAGER_FOCUS_MODE_AUTO = 1,         // 自动对焦模式, 对于 Z30 相机,焦点会自动计算,对于其他相机,用户可以设置一个对焦目标以自动计算焦点
 *      DJI_CAMERA_MANAGER_FOCUS_MODE_AFC = 2,          // 连续自动对焦模式, 仅支持固件版本为 V01.03.0000 或更高的 Mavic Pro、X4S 相机、Mavic 2 Zoom 相机和 Mavic 2 Pro 相机
 *      DJI_CAMERA_MANAGER_FOCUS_MODE_UNKNOWN = 0xFF,   // 相机对焦模式未知
 */
static void F_FocusMode_Set(E_DjiCameraManagerFocusMode focusMode)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetFocusMode(mountPosition, focusMode);
}

/**
 * 选定相机挂载位置的对焦模式 - 获取
 */
static void F_FocusMode_Get()
{
    E_DjiCameraManagerFocusMode focusMode;
    T_DjiReturnCode returnCode = DjiCameraManager_GetFocusMode(mountPosition, &focusMode);
}

/**
 * 选定相机挂载位置的对焦点 - 设置
 *      注: 设置镜头对焦目标点, 自动对焦模式下,目标点为焦点; 手动对焦模式下, 若启用了对焦助手, 目标点为缩小区域
 *
 * T_DjiCameraManagerFocusPosData
 *      dji_f32_t focusX;   // 指定水平区域坐标, 该参数介于 0 和 1 之间; 点 [0.0, 0.0] 表示屏幕的左上角
 *      dji_f32_t focusY;   // 指定垂直区域坐标, 此参数介于 0 和 1 之间
 */
static void F_FocusTarget_Set(T_DjiCameraManagerFocusPosData focusPosData)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetFocusTarget(mountPosition, focusPosData);
}

/**
 * 选定相机挂载位置的对焦点 - 获取
 */
static void F_FocusTarget_Get()
{
    T_DjiCameraManagerFocusPosData tapFocusPos;
    T_DjiReturnCode returnCode = DjiCameraManager_GetFocusTarget(mountPosition, &tapFocusPos);
}

/**
 * 选定挂载位置相机的持续光学变焦 - 开启
 *      注: 以指定速度和方向开始改变镜头的焦距, 当达到最大或最小焦距,或调用 DjiCameraManager_StopContinuousOpticalZoom 时,焦距变化将停止
 *
 * E_DjiCameraZoomDirection
 *      DJI_CAMERA_ZOOM_DIRECTION_OUT = 0,  // 镜头向远方移动, 变焦倍数变小
 *      DJI_CAMERA_ZOOM_DIRECTION_IN = 1,   // 镜头向近处移动, 变焦倍数变大
 *
 * E_DjiCameraZoomSpeed
 *      DJI_CAMERA_ZOOM_SPEED_SLOWEST = 72, // 镜头变焦处于最慢速度
 *      DJI_CAMERA_ZOOM_SPEED_SLOW = 73,    // 镜头变焦处于慢速
 *      DJI_CAMERA_ZOOM_SPEED_MODERATELY_SLOW = 74, // 镜头变焦处于比普通速度稍慢的速度
 *      DJI_CAMERA_ZOOM_SPEED_NORMAL = 75,  // 镜头变焦处于正常速度
 *      DJI_CAMERA_ZOOM_SPEED_MODERATELY_FAST = 76, // 镜头的变焦速度比正常速度略快
 *      DJI_CAMERA_ZOOM_SPEED_FAST = 77,    // 镜头变焦处于快速
 *      DJI_CAMERA_ZOOM_SPEED_FASTEST = 78, // 镜头变焦处于最快速度
 */
static void F_StartContinuousOpticalZoom(E_DjiCameraZoomDirection zoomDirection, E_DjiCameraZoomSpeed zoomSpeed)
{
    T_DjiReturnCode returnCode = DjiCameraManager_StartContinuousOpticalZoom(mountPosition, zoomDirection, zoomSpeed);
}

/**
 * 选定挂载位置相机的持续光学变焦 - 停止
 *      注: 当在 DjiCameraManager_StartContinuousOpticalZoom 调用中处于焦距变化状态时, 调用此函数以停止焦距变化
 */
static void F_StopContinuousOpticalZoom()
{
    T_DjiReturnCode returnCode = DjiCameraManager_StopContinuousOpticalZoom(mountPosition);
}

/**
 * 选定挂载位置相机的光学变焦参数 - 设置
 *      注: 在此接口中, 将根据您设置的变焦参数进行变焦操作
 *
 * dji_f32_t factor: 变焦参数
 *
 * E_DjiCameraZoomDirection
 *      DJI_CAMERA_ZOOM_DIRECTION_OUT = 0,  // 镜头向远方移动,变焦倍数变小
 *      DJI_CAMERA_ZOOM_DIRECTION_IN = 1,   // 镜头向近处移动,变焦倍数变大
 */
static void F_OpticalZoomParam_Set(E_DjiCameraZoomDirection zoomDirection, dji_f32_t factor)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetOpticalZoomParam(mountPosition, zoomDirection, factor);
}

/**
 * 选定挂载位置相机的光学变焦参数 - 获取
 *
 * T_DjiCameraManagerOpticalZoomParam
 *      dji_f32_t currentOpticalZoomFactor;
 *      dji_f32_t maxOpticalZoomFactor;
 */
static void F_OpticalZoomParam_Get()
{
    T_DjiCameraManagerOpticalZoomParam opticalZoomParam;
    T_DjiReturnCode returnCode = DjiCameraManager_GetOpticalZoomParam(mountPosition, &opticalZoomParam);
}

/**
 * 指定挂载位置相机的红外变焦参数 - 设置
 *
 * dji_f32_t factor: 目标变焦参数
 */
static void F_InfraredZoomParam_Set(dji_f32_t factor)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetInfraredZoomParam(mountPosition, factor);
}

/**
 * 启用或禁用指定挂载位置相机的点击变焦功能 - 设置
 *      注: 只有在启用点击变焦功能后, 才能调用 TapZoomAtTarget
 *
 * bool param: 启用 / 禁用
 */
static void F_TapZoomEnabled_Set(bool param)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetTapZoomEnabled(mountPosition, param);
}

/**
 * 指定挂载位置相机的点击变焦状态 - 获取
 */
static void F_GetTapZoomEnabled_Get()
{
    bool param;
    T_DjiReturnCode returnCode = DjiCameraManager_GetTapZoomEnabled(mountPosition, &param);
}

/**
 * 指定挂载位置相机的点击变焦倍数 - 设置
 *      注: Tap-zoom 使用一个系数来改变缩放比例, TapZoom 的最终缩放比例为当前缩放比例 x 系数
 *
 * uint8_t tapZoomMultiplier: 倍数范围为 1~5, 倍数为 1 时,变焦不会改变
 */
static void F_TapZoomMultiplier_Set(uint8_t tapZoomMultiplier)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetTapZoomMultiplier(mountPosition, tapZoomMultiplier);
}

/**
 * 指定挂载位置相机的点击变焦倍数 - 获取
 */
static void F_TapZoomMultiplier_Get()
{
    uint8_t tapZoomMultiplier; // 倍数范围为 1~5, 倍数为 1 时,变焦不会改变
    T_DjiReturnCode returnCode = DjiCameraManager_GetTapZoomMultiplier(mountPosition, tapZoomMultiplier);
}

/**
 * 指定挂载位置相机的点击变焦点 - 设置
 *      注: 在目标处进行 Tap-zoom, 只有在启用了 TapZoom 时才能调用, 设置新目标后, 云台将旋转并将目标定位在屏幕中心, 同时相机将通过乘以 TapZoom 系数进行缩放
 *
 * T_DjiCameraManagerTapZoomPosData
 *      dji_f32_t focusX;   // 指定水平区域坐标, 该参数介于 0 和 1 之间; 点 [0.0, 0.0] 表示屏幕的左上角
 *      dji_f32_t focusY;   // 指定垂直区域坐标, 此参数介于 0 和 1 之间
 */
static void F_TapZoomAtTarget_Set(T_DjiCameraManagerTapZoomPosData tapZoomPos)
{
    T_DjiReturnCode returnCode = DjiCameraManager_TapZoomAtTarget(mountPosition, tapZoomPos);
}

/**
 * 相机焦距环的值范围 - 获取
 *
 * T_DjiCameraManagerRangeList
 *      uint8_t size;
 *      union {
 *          E_DjiCameraManagerPhotoStorageFormat photoStorageFormat[16];
 *              DJI_CAMERA_MANAGER_PHOTO_STORAGE_FORMAT_RAW = 0,
 *              DJI_CAMERA_MANAGER_PHOTO_STORAGE_FORMAT_JPEG = 1,
 *              DJI_CAMERA_MANAGER_PHOTO_STORAGE_FORMAT_RAW_JPEG = 2,
 *              DJI_CAMERA_MANAGER_PHOTO_STORAGE_FORMAT_YUV = 3,    // 保存为 YUV 格式的图片
 *              DJI_CAMERA_MANAGER_PHOTO_STORAGE_FORMAT_RJPEG = 7,  // 热辐射 JPEG
 *          E_DjiCameraManagerVideoStorageFormat videoStorageFormat[16];
 *              DJI_CAMERA_MANAGER_VIDEO_STORAGE_FORMAT_MOV = 0,
 *              DJI_CAMERA_MANAGER_VIDEO_STORAGE_FORMAT_MP4 = 1,
 *          E_DjiCameraManagerPhotoRatio photoRatioFormat[16];
 *              DJI_CAMERA_MANAGER_PHOTO_RATIO_4X3 = 0,
 *              DJI_CAMERA_MANAGER_PHOTO_RATIO_16X9 = 1,
 *              DJI_CAMERA_MANAGER_PHOTO_RATIO_3X2 = 2,
 *              DJI_CAMERA_MANAGER_PHOTO_RATIO_1X1 = 3,
 *              DJI_CAMERA_MANAGER_PHOTO_RATIO_18X3 = 4,
 *              DJI_CAMERA_MANAGER_PHOTO_RATIO_5X4 = 5,
 *          E_DjiCameraManagerStreamSource streamSource[16];
 *              DJI_CAMERA_MANAGER_SOURCE_DEFAULT_CAM = 0x0,
 *              DJI_CAMERA_MANAGER_SOURCE_WIDE_CAM = 0x1,
 *              DJI_CAMERA_MANAGER_SOURCE_ZOOM_CAM = 0x2,
 *              DJI_CAMERA_MANAGER_SOURCE_IR_CAM = 0x3,
 *              DJI_CAMERA_MANAGER_SOURCE_VISIBLE_CAM = 0x7,
 *          E_DjiCameraManagerStreamStorage streamStorage[16];  // 同 E_DjiCameraManagerStreamSource
 *          E_DjiCameraManagerNightSceneMode nightSceneMode[16];
 *              DJI_CAMERA_MANAGER_NIGHT_SCENE_MODE_DISABLE = 0,
 *              DJI_CAMERA_MANAGER_NIGHT_SCENE_MODE_ENABLE = 1,
 *              DJI_CAMERA_MANAGER_NIGHT_SCENE_MODE_AUTO = 2,
 *      };
 *      uint32_t minValue;
 *      uint32_t maxValue;
 */
static void F_FocusRingRange_Get()
{
    T_DjiCameraManagerRangeList rangeList;
    T_DjiReturnCode returnCode = DjiCameraManager_GetFocusRingRange(mountPosition, &rangeList);
}

/**
 * 相机焦距环的值 - 设置
 * uint16_t value：焦距环的值
 */
static void F_FocusRingValue_Set(uint16_t value)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetFocusRingValue(mountPosition, value);
}

/**
 * 相机焦距环的类型 - 获取
 */
static void F_FocusRingValue_Get()
{
    uint16_t *value;
    T_DjiReturnCode returnCode = DjiCameraManager_GetFocusRingValue(mountPosition, value);
}

/**
 * 选定的挂载位置相机的曝光模式 - 设置
 *      注: 不同的曝光模式定义了光圈、快门速度、ISO 是否可以自动或手动设置; 除手动模式外, 所有模式都可以更改曝光补偿
 *
 * E_DjiCameraManagerExposureMode
 *      DJI_CAMERA_MANAGER_EXPOSURE_MODE_PROGRAM_AUTO = 1,          // 自动程序模式
 *      DJI_CAMERA_MANAGER_EXPOSURE_MODE_SHUTTER_PRIORITY = 2,      // 快门优先模式
 *      DJI_CAMERA_MANAGER_EXPOSURE_MODE_APERTURE_PRIORITY = 3,     // 光圈优先模式
 *      DJI_CAMERA_MANAGER_EXPOSURE_MODE_EXPOSURE_MANUAL = 4,       // 手动模式
 *      DJI_CAMERA_MANAGER_EXPOSURE_MODE_EXPOSURE_UNKNOWN = 0xFF    // 曝光模式未知
 */
static void F_ExposureMode_Set(E_DjiCameraManagerExposureMode mode)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetExposureMode(mountPosition, mode);
}

/**
 * 选定的挂载位置相机的曝光模式 - 获取
 *      注: 不同的曝光模式定义了光圈、快门速度、ISO 是否可以自动或手动设置; 除手动模式外, 所有模式都可以更改曝光补偿
 */
static void F_ExposureMode_Get()
{
    E_DjiCameraManagerExposureMode mode;
    T_DjiReturnCode returnCode = DjiCameraManager_GetExposureMode(mountPosition, &mode);
}

/**
 * 选定的挂载位置相机的 ISO 值 - 设置
 *      注: 只有在相机曝光模式为手动模式时才可以设置 ISO 值
 *
 * E_DjiCameraManagerISO
 *      DJI_CAMERA_MANAGER_ISO_AUTO = 0x00,     // 自动设置 ISO 值, 并非所有相机的手动模式下可用
 *      DJI_CAMERA_MANAGER_ISO_100 = 0x03,      // ISO 值设置为 100
 *      DJI_CAMERA_MANAGER_ISO_200 = 0x04,      // ISO 值设置为 200
 *      DJI_CAMERA_MANAGER_ISO_400 = 0x05,      // ISO 值设置为 400
 *      DJI_CAMERA_MANAGER_ISO_800 = 0x06,      // ISO 值设置为 600
 *      DJI_CAMERA_MANAGER_ISO_1600 = 0x07,     // ISO 值设置为 1600
 *      DJI_CAMERA_MANAGER_ISO_3200 = 0x08,     // ISO 值设置为 3200
 *      DJI_CAMERA_MANAGER_ISO_6400 = 0x09,     // ISO 值设置为 64000
 *      DJI_CAMERA_MANAGER_ISO_12800 = 0x0A,    // ISO 值设置为 12800
 *      DJI_CAMERA_MANAGER_ISO_25600 = 0x0B,    // ISO 值设置为 25600
 *      DJI_CAMERA_MANAGER_ISO_FIXED = 0xFF,    // ISO 值由相机固件固定, 当相机色彩设置为 D_LOG 时, 相机将把 ISO 固定在一个特定的值以优化性能
 */
static void F_ISO_Set(E_DjiCameraManagerISO iso)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetISO(mountPosition, iso);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS && returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND)
    {
        USER_LOG_ERROR("设置安装位置 %d 相机的iso %d 失败, 错误码: 0x%08X", mountPosition, iso, returnCode);
    }
}

/**
 * 选定的挂载位置相机的 ISO 值 - 获取
 */
static void F_ISO_Get(E_DjiCameraManagerISO iso)
{
    E_DjiCameraManagerISO iso;
    T_DjiReturnCode returnCode = DjiCameraManager_GetISO(mountPosition, &iso);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS && returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND)
    {
        USER_LOG_ERROR("获得安装位置 %d 相机的iso失败, 错误编码: 0x%08X", mountPosition, returnCode);
        return returnCode;
    }
}

/**
 * 设置选定的挂载位置相机的光圈值 - 设置
 *      注: 曝光模式必须为 DJI_CAMERA_MANAGER_EXPOSURE_MODE_EXPOSURE_MANUAL 或 DJI_CAMERA_MANAGER_EXPOSURE_MODE_APERTURE_PRIORITY
 *
 * E_DjiCameraManagerAperture
 *      DJI_CAMERA_MANAGER_APERTURE_F_1_DOT_6 = 160,    // 光圈值为 f/1.6, 只有 Z30 相机支持
 *      DJI_CAMERA_MANAGER_APERTURE_F_1_DOT_7 = 170,    // 光圈值为 f/1.7
 *      DJI_CAMERA_MANAGER_APERTURE_F_1_DOT_8 = 180,    // 光圈值为 f/1.8
 *      DJI_CAMERA_MANAGER_APERTURE_F_2 = 200,          // 光圈值为 f/2
 *      DJI_CAMERA_MANAGER_APERTURE_F_2_DOT_2 = 220,    // 光圈值为 f/2.2
 *      DJI_CAMERA_MANAGER_APERTURE_F_2_DOT_4 = 240,    // 光圈值为 f/2.4, 只有 Z30 相机支持
 *      DJI_CAMERA_MANAGER_APERTURE_F_2_DOT_5 = 250,    // 光圈值为 f/2.5
 *      DJI_CAMERA_MANAGER_APERTURE_F_2_DOT_6 = 260,    // 光圈值为 f/2.6
 *      DJI_CAMERA_MANAGER_APERTURE_F_2_DOT_8 = 280,    // 光圈值为 f/2.8
 *      DJI_CAMERA_MANAGER_APERTURE_F_3_DOT_2 = 320,    // 光圈值为 f/3.2
 *      DJI_CAMERA_MANAGER_APERTURE_F_3_DOT_4 = 340,    // 光圈值为 f/3.4
 *      DJI_CAMERA_MANAGER_APERTURE_F_3_DOT_5 = 350,    // 光圈值为 f/3.5
 *      DJI_CAMERA_MANAGER_APERTURE_F_4 = 400,          // 光圈值为 f/4
 *      DJI_CAMERA_MANAGER_APERTURE_F_4_DOT_5 = 450,    // 光圈值为 f/4.5
 *      DJI_CAMERA_MANAGER_APERTURE_F_4_DOT_8 = 480,    // 光圈值为 f/4.8
 *      DJI_CAMERA_MANAGER_APERTURE_F_5 = 500,          // 光圈值为 f/5
 *      DJI_CAMERA_MANAGER_APERTURE_F_5_DOT_6 = 560,    // 光圈值为 f/5.6
 *      DJI_CAMERA_MANAGER_APERTURE_F_6_DOT_3 = 630,    // 光圈值为 f/6.3
 *      DJI_CAMERA_MANAGER_APERTURE_F_6_DOT_8 = 680,    // 光圈值为 f/6.8
 *      DJI_CAMERA_MANAGER_APERTURE_F_7_DOT_1 = 710,    // 光圈值为 f/7.1
 *      DJI_CAMERA_MANAGER_APERTURE_F_8 = 800,          // 光圈值为 f/8
 *      DJI_CAMERA_MANAGER_APERTURE_F_9 = 900,          // 光圈值为 f/9
 *      DJI_CAMERA_MANAGER_APERTURE_F_9_DOT_6 = 960,    // 光圈值为 f/9.6
 *      DJI_CAMERA_MANAGER_APERTURE_F_10 = 1000,        // 光圈值为 f/10
 *      DJI_CAMERA_MANAGER_APERTURE_F_11 = 1100,        // 光圈值为 f/11
 *      DJI_CAMERA_MANAGER_APERTURE_F_13 = 1300,        // 光圈值为 f/13
 *      DJI_CAMERA_MANAGER_APERTURE_F_14 = 1400,        // 光圈值为 f/14
 *      DJI_CAMERA_MANAGER_APERTURE_F_16 = 1600,        // 光圈值为 f/16
 *      DJI_CAMERA_MANAGER_APERTURE_F_18 = 1800,        // 光圈值为 f/18
 *      DJI_CAMERA_MANAGER_APERTURE_F_19 = 1900,        // 光圈值为 f/19
 *      DJI_CAMERA_MANAGER_APERTURE_F_20 = 2000,        // 光圈值为 f/20
 *      DJI_CAMERA_MANAGER_APERTURE_F_22 = 2200,        // 光圈值为 f/22
 *      DJI_CAMERA_MANAGER_APERTURE_F_UNKNOWN = 0xFFFF, // 光圈值为未知
 */
static void F_Aperture_Set(E_DjiCameraManagerAperture aperture)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetAperture(mountPosition, aperture);
}

/**
 * 选定的挂载位置相机的光圈值 - 获取
 */
static void F_Aperture_Get()
{
    E_DjiCameraManagerAperture aperture;
    T_DjiReturnCode returnCode = DjiCameraManager_GetAperture(mountPosition, &aperture);
}

/**
 * 指定挂载位置相机的快门速度 - 设置
 *      注: 设置相机的快门速度, 当相机处于 RECORD_VIDEO 模式时,快门速度不应低于视频帧率
 *          例如: 如果视频帧率为 30fps, 快门速度必须<= 1/30;
 *          前提条件: 只有在相机曝光模式为 DJI_CAMERA_MANAGER_EXPOSURE_MODE_EXPOSURE_MANUAL 或 DJI_CAMERA_MANAGER_EXPOSURE_MODE_SHUTTER_PRIORITY 时,才能设置快门速度
 *
 * E_DjiCameraManagerShutterSpeed
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_8000 = 0,    // 1/8000 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_6400 = 1,    // 1/6400 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_6000 = 2,    // 1/6000 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_5000 = 3,    // 1/5000 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_4000 = 4,    // 1/4000 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_3200 = 5,    // 1/3200 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_3000 = 6,    // 1/3000 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_2500 = 7,    // 1/2500 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_2000 = 8,    // 1/2000 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_1600 = 9,    // 1/1600 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_1500 = 10,   // 1/1500 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_1250 = 11,   // 1/1250 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_1000 = 12,   // 1/1000 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_800 = 13,    // 1/800 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_725 = 14,    // 1/725 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_640 = 15,    // 1/640 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_500 = 16,    // 1/500 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_400 = 17,    // 1/400 s
 *      JI_CAMERA_MANAGER_SHUTTER_SPEED_1_350 = 18,     // 1/350 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_320 = 19,    // 1/320 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_250 = 20,    // 1/250 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_240 = 21,    // 1/240 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_200 = 22,    // 1/200 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_180 = 23,    // 1/180 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_160 = 24,    // 1/160 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_125 = 25,    // 1/125 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_120 = 26,    // 1/120 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_100 = 27,    // 1/100 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_90 = 28, // 1/90 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_80 = 29, // 1/80 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_60 = 30, // 1/60 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_50 = 31, // 1/50 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_40 = 32, // 1/40 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_30 = 33, // 1/30 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_25 = 34, // 1/25 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_20 = 35, // 1/20 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_15 = 36, // 1/15 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_12DOT5 = 37, // 1/12.5
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_10 = 38, // 1/10 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_8 = 39,  // 1/8 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_6DOT25 = 40, // 1/6.25 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_5 = 41,  // 1/5 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_4 = 42,  // 1/4 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_3 = 43,  // 1/3 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_2DOT5 = 44,  // 1/2.5 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_2 = 45,  // 1/2 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_1DOT67 = 46, // 1/1.67 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_1DOT25 = 47, // 1/1.25 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1 = 48,    // 1.0 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1DOT3 = 49,    // 1.3 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_1DOT6 = 50,    // 1.6 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_2 = 51,    // 2.0 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_2DOT5 = 52,    // 2.5 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_3 = 53,    // 3.0 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_3DOT2 = 54,    // 3.2 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_4 = 55,    // 4.0 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_5 = 56,    // 5.0 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_6 = 57,    // 6.0 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_7 = 58,    // 7.0 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_8 = 59,    // 8.0 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_9 = 60,    // 9.0 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_10 = 61,   // 10.0 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_13 = 62,   // 13.0 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_15 = 63,   // 15.0 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_20 = 64,   // 20.0 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_25 = 65,   // 25.0 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_30 = 66,   // 30.0 s
 *      DJI_CAMERA_MANAGER_SHUTTER_SPEED_UNKNOWN = 0xFF,    // Unknown
 */
static void F_ShutterSpeed_Set(E_DjiCameraManagerShutterSpeed shutterSpeed)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetShutterSpeed(mountPosition, shutterSpeed);
}

/**
 * 指定挂载位置相机的快门速度 - 获取
 */
static void F_ShutterSpeed_Get()
{
    E_DjiCameraManagerShutterSpeed shutterSpeed;
    T_DjiReturnCode returnCode = DjiCameraManager_GetShutterSpeed(mountPosition, &shutterSpeed);
}

/**
 * 指定挂载位置相机的曝光补偿 - 设置
 *      注: 设置相机的曝光补偿, 要使用此功能, 请将相机曝光模式设置为快门、程序或光圈
 *          曝光模式为: DJI_CAMERA_MANAGER_EXPOSURE_MODE_EXPOSURE_MANUAL
 *                     DJI_CAMERA_MANAGER_EXPOSURE_MODE_SHUTTER_PRIORITY
 *                     DJI_CAMERA_MANAGER_EXPOSURE_APERTURE_PRIORITY
 *
 * E_DjiCameraManagerExposureCompensation
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_5_0 = 1,     // 相机曝光补偿为 -5.0ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_4_7 = 2,     // 相机曝光补偿为 -4.7ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_4_3 = 3,     // 相机曝光补偿为 -4.3ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_4_0 = 4,     // 相机曝光补偿为 -4.0ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_3_7 = 5,     // 相机曝光补偿为 -3.7ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_3_3 = 6,     // 相机曝光补偿为 -3.3ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_3_0 = 7,     // 相机曝光补偿为 -3.0ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_2_7 = 8,     // 相机曝光补偿为 -2.7ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_2_3 = 9,     // 相机曝光补偿为 -2.3ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_2_0 = 10,    // 相机曝光补偿为 -2.0ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_1_7 = 11,    // 相机曝光补偿为 -1.7ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_1_3 = 12,    // 相机曝光补偿为 -1.3ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_1_0 = 13,    // 相机曝光补偿为 -1.0ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_0_7 = 14,    // 相机曝光补偿为 -0.7ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_0_3 = 15,    // 相机曝光补偿为 -0.3ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_0_0 = 16,    // 相机曝光补偿为 0.0ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_0_3 = 17,    // 相机曝光补偿为 +0.3ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_0_7 = 18,    // 相机曝光补偿为 +0.7ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_1_0 = 19,    // 相机曝光补偿为 +1.0ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_1_3 = 20,    // 相机曝光补偿为 +1.3ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_1_7 = 21,    // 相机曝光补偿为 +1.7ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_2_0 = 22,    // 相机曝光补偿为 +2.0ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_2_3 = 23,    // 相机曝光补偿为 +2.3ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_2_7 = 24,    // 相机曝光补偿为 +2.7ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_3_0 = 25,    // 相机曝光补偿为 +3.0ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_3_3 = 26,    // 相机曝光补偿为 +3.3ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_3_7 = 27,    // 相机曝光补偿为 +3.7ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_4_0 = 28,    // 相机曝光补偿为 +4.0ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_4_3 = 29,    // 相机曝光补偿为 +4.3ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_4_7 = 30,    // 相机曝光补偿为 +4.7ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_5_0 = 31,    // 相机曝光补偿为 +5.0ev
 *      DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_FIXED = 0xFF,  // 相机曝光补偿为 fixed by the camera.
 */
static void F_ExposureCompensation_Set(E_DjiCameraManagerExposureCompensation ev)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetExposureCompensation(mountPosition, ev);
}

/**
 * 指定挂载位置相机的曝光补偿 - 获取
 */
static void F_ExposureCompensation_Get()
{
    E_DjiCameraManagerExposureCompensation ev;
    T_DjiReturnCode returnCode = DjiCameraManager_GetExposureCompensation(mountPosition, &ev);
}

/**
 * 自动曝光锁定 - 设置
 *
 * bool enable: 启用为 true, 禁用为 false
 */
static void F_AELockEnabled_Set(bool enable)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetAELockEnabled(mountPosition, enable);
}

/**
 * 自动曝光锁定状态 - 获取
 *      注: L1/P1 相机不支持此接口
 */
static void F_AELockEnabled_Get()
{
    bool enable;
    T_DjiReturnCode returnCode = DjiCameraManager_GetAELockEnabled(mountPosition, &enable);
}

/**
 * 重置相机设置
 */
static void F_ResetCameraSettings()
{
    T_DjiReturnCode returnCode = DjiCameraManager_ResetCameraSettings(mountPosition);
}

/**
 * 选定挂载位置相机录像 - 开始
 *      注: 相机必须处于 RECORD_VIDEO 模式, 对于热成像相机,用户可以在视频录制时拍摄单张照片
 */
static void F_RecordVideo_Start()
{
    T_DjiReturnCode returnCode = DjiCameraManager_StartRecordVideo(mountPosition);
}

/**
 * 选定挂载位置相机录像 - 停止
 *      注: 前提条件: 相机当前正在录像
 */
static void F_RecordVideo_Stop()
{
    T_DjiReturnCode returnCode = DjiCameraManager_StopRecordVideo(mountPosition);
}

/**
 * 相机录像状态 - 获取
 */
static void F_RecordingState_Get()
{
    E_DjiCameraManagerRecordingState recordingState;
    T_DjiReturnCode returnCode = DjiCameraManager_GetRecordingState(mountPosition, &recordingState);
}

/**
 * 录像时长 - 获取
 *      注: L1/P1 不支持此接口
 *
 * uint16_t recordingTime: 获取的录像时长,单位: 秒
 */
static void F_RecordingTime_Get()
{
    uint16_t recordingTime;
    T_DjiReturnCode returnCode = DjiCameraManager_GetRecordingTime(mountPosition, &recordingTime);
}

/**
 * 相机流媒体数据源范围 - 获取
 *
 * T_DjiCameraManagerRangeList  // 同 F_FocusRingRange_Get()
 */
static void F_StreamSourceRange_Get()
{
    T_DjiCameraManagerRangeList rangeList;
    T_DjiReturnCode returnCode = DjiCameraManager_GetStreamSourceRange(mountPosition, &rangeList);
}

/**
 * 选择相机流媒体数据源 - 设置
 *
 * E_DjiCameraManagerStreamSource
 *      DJI_CAMERA_MANAGER_SOURCE_DEFAULT_CAM = 0x0,
 *      DJI_CAMERA_MANAGER_SOURCE_WIDE_CAM = 0x1,
 *      DJI_CAMERA_MANAGER_SOURCE_ZOOM_CAM = 0x2,
 *      DJI_CAMERA_MANAGER_SOURCE_IR_CAM = 0x3,
 *      DJI_CAMERA_MANAGER_SOURCE_VISIBLE_CAM = 0x7,
 */
static void F_StreamSource_Set(E_DjiCameraManagerStreamSource streamSource)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetStreamSource(mountPosition, streamSource);
}

/**
 * 照片存储格式范围 - 获取
 *
 * T_DjiCameraManagerRangeList  // 同 F_FocusRingRange_Get()
 */
static void F_PhotoStorageFormatRange_Get()
{
    T_DjiCameraManagerRangeList rangeList;
    T_DjiReturnCode returnCode = DjiCameraManager_GetPhotoStorageFormatRange(mountPosition, &rangeList);
}

/**
 * 照片存储格式 - 设置
 *
 * E_DjiCameraManagerPhotoStorageFormat
 *      DJI_CAMERA_MANAGER_PHOTO_STORAGE_FORMAT_RAW = 0,
 *      DJI_CAMERA_MANAGER_PHOTO_STORAGE_FORMAT_JPEG = 1,
 *      DJI_CAMERA_MANAGER_PHOTO_STORAGE_FORMAT_RAW_JPEG = 2,
 *      DJI_CAMERA_MANAGER_PHOTO_STORAGE_FORMAT_YUV = 3, // 保存为 YUV 格式的图片
 *      DJI_CAMERA_MANAGER_PHOTO_STORAGE_FORMAT_RJPEG = 7, // 热辐射 JPEG
 */
static void F_PhotoFormat_Set(E_DjiCameraManagerPhotoStorageFormat format)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetPhotoFormat(mountPosition, format);
}

/**
 * 照片存储格式 - 获取
 */
static void F_PhotoFormat_Get(E_DjiCameraManagerPhotoStorageFormat format)
{
    E_DjiCameraManagerPhotoStorageFormat format;
    T_DjiReturnCode returnCode = DjiCameraManager_GetPhotoFormat(mountPosition, &format);
}

/**
 * 视频存储格式范围 - 获取
 *
 * T_DjiCameraManagerRangeList  // 同 F_FocusRingRange_Get()
 */
static void F_VideoFormatRange_Get()
{
    T_DjiCameraManagerRangeList rangeList;
    T_DjiReturnCode returnCode = DjiCameraManager_GetVideoFormatRange(mountPosition, &rangeList);
}

/**
 * 视频存储格式 - 设置
 *
 * E_DjiCameraManagerVideoStorageFormat
 *      DJI_CAMERA_MANAGER_VIDEO_STORAGE_FORMAT_MOV = 0,
 *      DJI_CAMERA_MANAGER_VIDEO_STORAGE_FORMAT_MP4 = 1,
 */
static void F_VideoStorageFormat_Set(E_DjiCameraManagerVideoStorageFormat format)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetVideoStorageFormat(mountPosition, format);
}

/**
 * 视频存储格式 - 获取
 */
static void F_VideoStorageFormat_Get()
{
    E_DjiCameraManagerVideoStorageFormat format;
    T_DjiReturnCode returnCode = DjiCameraManager_SetVideoStorageFormat(mountPosition, &format);
}

/**
 * 照片比例范围 - 获取
 *
 * T_DjiCameraManagerRangeList  // 同 F_FocusRingRange_Get()
 */
static void F_PhotoRatioRange_Get()
{
    T_DjiCameraManagerRangeList rangeList;
    T_DjiReturnCode returnCode = DjiCameraManager_GetPhotoRatioRange(mountPosition, &rangeList);
}

/**
 * 照片比例 - 设置
 *
 * E_DjiCameraManagerPhotoRatio
 *      DJI_CAMERA_MANAGER_PHOTO_RATIO_4X3 = 0,
 *      DJI_CAMERA_MANAGER_PHOTO_RATIO_16X9 = 1,
 *      DJI_CAMERA_MANAGER_PHOTO_RATIO_3X2 = 2,
 *      DJI_CAMERA_MANAGER_PHOTO_RATIO_1X1 = 3,
 *      DJI_CAMERA_MANAGER_PHOTO_RATIO_18X3 = 4,
 *      DJI_CAMERA_MANAGER_PHOTO_RATIO_5X4 = 5,
 */
static void F_PhotoRatio_Set(E_DjiCameraManagerPhotoRatio photoRatio)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetPhotoRatio(mountPosition, photoRatio);
}

/**
 * 照片比例 - 获取
 *
 * E_DjiCameraManagerPhotoRatio
 *      DJI_CAMERA_MANAGER_PHOTO_RATIO_4X3 = 0,
 *      DJI_CAMERA_MANAGER_PHOTO_RATIO_16X9 = 1,
 *      DJI_CAMERA_MANAGER_PHOTO_RATIO_3X2 = 2,
 *      DJI_CAMERA_MANAGER_PHOTO_RATIO_1X1 = 3,
 *      DJI_CAMERA_MANAGER_PHOTO_RATIO_18X3 = 4,
 *      DJI_CAMERA_MANAGER_PHOTO_RATIO_5X4 = 5,
 */
static void F_PhotoRatio_Get()
{
    E_DjiCameraManagerPhotoRatio photoRatio;
    T_DjiReturnCode returnCode = DjiCameraManager_GetPhotoRatio(mountPosition, &photoRatio);
}

/**
 * 视频分辨率和帧率 - 获取
 *
 * T_DjiCameraManagerVideoFormat
 *      E_DjiCameraManagerVideoResolution videoResolution;
 *          DJI_CAMERA_MANAGER_VIDEO_RESOLUTION_640X480P = 0,   // 640X480P
 *          DJI_CAMERA_MANAGER_VIDEO_RESOLUTION_1280X640P = 2,  // 1280X640P
 *          DJI_CAMERA_MANAGER_VIDEO_RESOLUTION_1280X720P = 4,  // 1280X720P
 *          DJI_CAMERA_MANAGER_VIDEO_RESOLUTION_1920X1080P = 10,    // 1920X1080P
 *          DJI_CAMERA_MANAGER_VIDEO_RESOLUTION_3840X2160P = 16,    // 3840X2160P
 *      E_DjiCameraManagerVideoFrameRate videoFrameRate;
 *          DJI_CAMERA_MANAGER_VIDEO_FRAME_RATE_15FPS = 0,  // 14.985
 *          DJI_CAMERA_MANAGER_VIDEO_FRAME_RATE_25FPS = 2,  // 25.000
 *          DJI_CAMERA_MANAGER_VIDEO_FRAME_RATE_30FPS = 3,  // 29.970
 *          DJI_CAMERA_MANAGER_VIDEO_FRAME_RATE_60FPS = 6,  // 59.940
 */
static void F_VideoResolutionFrameRate_Get()
{
    T_DjiCameraManagerVideoFormat videoParam;
    T_DjiReturnCode returnCode = DjiCameraManager_GetVideoResolutionFrameRate(mountPosition, &videoParam);
}

/**
 * 夜景模式范围 - 获取
 *
 * T_DjiCameraManagerRangeList  // 同 F_FocusRingRange_Get()
 */
static void F_NightSceneModeRange_Get()
{
    T_DjiCameraManagerRangeList rangeList;
    T_DjiReturnCode returnCode = DjiCameraManager_GetNightSceneModeRange(mountPosition, &rangeList);
}

/**
 * 夜景模式 - 设置
 *      注: 确保流媒体数据源为变焦或广角相机
 *
 * E_DjiCameraManagerNightSceneMode
 *      DJI_CAMERA_MANAGER_NIGHT_SCENE_MODE_DISABLE = 0,
 *      DJI_CAMERA_MANAGER_NIGHT_SCENE_MODE_ENABLE = 1,
 *      DJI_CAMERA_MANAGER_NIGHT_SCENE_MODE_AUTO = 2,
 */
static void F_NightSceneMode_Set(E_DjiCameraManagerNightSceneMode nightSceneMode)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetNightSceneMode(mountPosition, nightSceneMode);
}

/**
 * 夜景模式 - 获取
 */
static void F_NightSceneMode_Get()
{
    E_DjiCameraManagerNightSceneMode nightSceneMode;
    T_DjiReturnCode returnCode = DjiCameraManager_GetNightSceneMode(mountPosition, &nightSceneMode);
}

/**
 * 拍摄或录像时流媒体数据源的存储范围 - 获取
 *
 * T_DjiCameraManagerRangeList  // 同 F_FocusRingRange_Get()
 */
static void F_StreamStorageRange_Get()
{
    T_DjiCameraManagerRangeList rangeList;
    T_DjiReturnCode returnCode = DjiCameraManager_GetStreamStorageRange(mountPosition, &rangeList);
}

/**
 * 选择要存储的拍摄或录像流 - 设置
 *      注: 前提条件, 设置相机的工作模式以匹配流类型
 *
 * E_DjiCameraManagerCaptureOrRecording
 *      DJI_CAMERA_MANAGER_CAPTURE_OR_RECORDING_CAPTURE = 0,
 *      DJI_CAMERA_MANAGER_CAPTURE_OR_RECORDING_RECORDING = 1,
 *
 * T_DjiCameraManagerStreamList
 *      uint32_t size;
 *      E_DjiCameraManagerStreamSource streamSource[4];
 *          DJI_CAMERA_MANAGER_SOURCE_DEFAULT_CAM = 0x0,
 *          DJI_CAMERA_MANAGER_SOURCE_WIDE_CAM = 0x1,
 *          DJI_CAMERA_MANAGER_SOURCE_ZOOM_CAM = 0x2,
 *          DJI_CAMERA_MANAGER_SOURCE_IR_CAM = 0x3,
 *          DJI_CAMERA_MANAGER_SOURCE_VISIBLE_CAM = 0x7,
 *      E_DjiCameraManagerStreamStorage streamStorage[4];   // 结构, 同 E_DjiCameraManagerStreamSource
 */
static void F_CaptureRecordingStreams_Set(E_DjiCameraManagerCaptureOrRecording streamType, T_DjiCameraManagerStreamList *streamStorageList)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetCaptureRecordingStreams(mountPosition, streamType, streamStorageList);
}

/**
 * 要存储的拍摄或录像流 - 获取
 */
static void F_CaptureRecordingStreams_Get()
{
    E_DjiCameraManagerCaptureOrRecording streamType;
    T_DjiCameraManagerStreamList *streamStorageList;
    T_DjiReturnCode returnCode = DjiCameraManager_GetCaptureRecordingStreams(mountPosition, streamType, &streamStorageList);
}

/**
 * 启用 / 禁用同步分屏缩放功能 - 设置
 */
static void F_SynchronizedSplitScreenZoomEnabled_Set(bool enable)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetSynchronizedSplitScreenZoomEnabled(mountPosition, enable);
}

/**
 * 目录或文件的后缀名 - 设置
 *      注: 对于文件名, 该设置仅一次有效
 * const uint8_t *nameStr   // 自定义后缀名的内容
 * uint32_t nameSize    // 指示 nameStr 的大小, 该值必须在 1-239 范围内, 如果超出限制会将后缀名截断
 *
 * E_DjiCameraManagerExpandNameType
 *      DJI_CAMERA_MANAGER_EXPAND_NAME_TYPE_FILE = 1,
 *      DJI_CAMERA_MANAGER_EXPAND_NAME_TYPE_DIR = 2,
 */
static void F_CustomExpandName_Set(E_DjiCameraManagerExpandNameType nameType, const uint8_t *nameStr, uint32_t nameSize)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetCustomExpandName(mountPosition, nameType, nameStr, nameSize);
}

/**
 * 最近目录或文件名的自定义后缀 - 获取
 */
static void F_CustomExpandName_Get()
{
    E_DjiCameraManagerExpandNameType nameType;
    uint8_t *nameStr;
    uint32_t *nameSize;
    T_DjiReturnCode returnCode = DjiCameraManager_GetCustomExpandName(mountPosition, nameType, &nameStr, &nameSize);
}

/**
 * 下载选定的相机媒体文件列表
 *      注: 该接口为同步接口, 使用时会占用较多的 CPU 资源
 *
 * T_DjiCameraManagerFileList
 *      uint16_t totalCount;
 *      T_DjiCameraManagerFileListInfo *fileListInfo;
 *          char fileName[DJI_FILE_NAME_SIZE_MAX];  // DJI_FILE_NAME_SIZE_MAX = 256
 *          uint32_t fileSize;
 *          uint32_t fileIndex;
 *          T_DjiCameraManagerFileCreateTime createTime;
 *              uint8_t second;
 *              uint8_t minute;
 *              uint8_t hour;
 *              uint8_t day;
 *              uint8_t month;
 *              uint16_t year;
 *          E_DjiCameraMediaFileType type;
 *              DJI_CAMERA_FILE_TYPE_JPEG    = 0,
 *              DJI_CAMERA_FILE_TYPE_DNG     = 1,
 *              DJI_CAMERA_FILE_TYPE_MOV     = 2,
 *              DJI_CAMERA_FILE_TYPE_MP4     = 3,
 *              DJI_CAMERA_FILE_TYPE_TIFF    = 5,
 *              DJI_CAMERA_FILE_TYPE_LDRT    = 24,
 *              DJI_CAMERA_FILE_TYPE_RPT     = 25,
 *              DJI_CAMERA_FILE_TYPE_UNKNOWN = 255,
 *          T_DjiCameraManagerFileAttributeData attributeData;
 *          uint8_t subFileListTotalNum;
 *          T_DjiCameraManagerSubFileListInfo* subFileListInfo;
 *              char fileName[DJI_FILE_NAME_SIZE_MAX];  // 同上
 *              uint32_t fileSize;
 *              uint32_t fileIndex;
 *              T_DjiCameraManagerFileCreateTime createTime;
 *              E_DjiCameraMediaFileSubType type;
 *              T_DjiCameraManagerFileAttributeData attributeData;
 */
static void F_DownloadFileList()
{
    T_DjiCameraManagerFileList fileList;
    T_DjiReturnCode returnCode = DjiCameraManager_DownloadFileList(mountPosition, &fileList);
}

/**
 * 分块下载选定的相机媒体文件列表
 *      注: 该接口为同步接口, 使用时会占用较多的 CPU 资源
 *
 * T_DjiCameraManagerSliceConfig
 *      uint16_t sliceStartIndex;
 *      E_DjiCameraManagerFileListCountPerSlice;
 *          DJI_CAMERA_MANAGER_FILE_LIST_COUNT_60_PER_SLICE = 60,
 *          DJI_CAMERA_MANAGER_FILE_LIST_COUNT_120_PER_SLICE = 120,
 *          DJI_CAMERA_MANAGER_FILE_LIST_COUNT_ALL_PER_SLICE = 0xFFFF,
 *
 * T_DjiCameraManagerFileList   // 同 F_DownloadFileList()
 */
static void F_DownloadFileListBySlices()
{

    T_DjiCameraManagerSliceConfig sliceConfig;
    T_DjiCameraManagerFileList fileList;
    T_DjiReturnCode returnCode = DjiCameraManager_DownloadFileListBySlices(mountPosition, sliceConfig, &fileList);
}

/**
 * 注册选定相机下载文件数据的回调
 *
 * T_DjiDownloadFilePacketInfo
 *      E_DjiDownloadFileEvent downloadFileEvent;
 *          DJI_DOWNLOAD_FILE_EVENT_START,
 *          DJI_DOWNLOAD_FILE_EVENT_TRANSFER,
 *          DJI_DOWNLOAD_FILE_EVENT_END,
 *          DJI_DOWNLOAD_FILE_EVENT_START_TRANSFER_END,
 *      uint32_t fileIndex;
 *      uint32_t fileSize;
 *      dji_f32_t progressInPercent;
 */
T_DjiReturnCode F_ManagerDownloadFileData_Callback(T_DjiDownloadFilePacketInfo packetInfo, const uint8_t *data, uint16_t dataLen)
{
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}
void F_RegDownloadFileDataCallback()
{
    T_DjiReturnCode returnCode = DjiCameraManager_RegDownloadFileDataCallback(mountPosition, F_ManagerDownloadFileData_Callback);
}

/**
 * 通过文件索引下载选定的相机媒体文件
 *      注: 仅支持同时下载一个文件, 新文件的下载需要等待前一个文件下载完成, 此接口为同步接口, 在使用时占用更多的 CPU 资源, 如果下载文件失败超时时间为 3 秒
 */
static void F_DownloadFileByIndex(uint32_t fileIndex)
{
    T_DjiReturnCode returnCode = DjiCameraManager_DownloadFileByIndex(mountPosition, fileIndex);
}

/**
 * 通过文件索引和文件类型下载选定的相机媒体文件
 *      注: 此 API 仅支持 L2 相机, 一次支持下载一个文件, 下载完成后再开始下载另一个文件; 这个同步接口可能会导致较高的 CPU 使用率, 如果下载失败 3 秒后超时
 *
 * E_DjiCameraMediaFileSubType
 *      DJI_DOWNLOAD_FILE_ORG = 0,
 *      DJI_DOWNLOAD_FILE_LDR = 21,
 *      DJI_DOWNLOAD_FILE_SIG = 22,
 *      DJI_DOWNLOAD_FILE_RTK = 23,
 *      DJI_DOWNLOAD_FILE_CLC = 25,
 *      DJI_DOWNLOAD_FILE_CLI = 26,
 *      DJI_DOWNLOAD_FILE_IMU = 27,
 *      DJI_DOWNLOAD_FILE_RTL = 28,
 *      DJI_DOWNLOAD_FILE_RTB = 29,
 *      DJI_DOWNLOAD_FILE_RTS = 30,
 *      DJI_DOWNLOAD_FILE_RPOS = 31,
 */
static void F_(uint32_t index, E_DjiCameraMediaFileSubType fileType)
{
    T_DjiReturnCode returnCode = DjiCameraManager_DownloadSubFileByIndexAndSubType(mountPosition, index, fileType);
}

/**
 * 下载媒体文件前获取媒体文件下载权限
 */
static void F_ObtainDownloaderRights()
{
    T_DjiReturnCode returnCode = DjiCameraManager_ObtainDownloaderRights(mountPosition);
}

/**
 * 下载媒体文件后释放媒体文件下载权限
 *      注: 释放下载权限失败可能导致 Pilot 访问相册受限
 */
static void F_ReleaseDownloaderRights()
{
    T_DjiReturnCode returnCode = DjiCameraManager_ReleaseDownloaderRights(mountPosition);
}

/**
 * 格式化 SD 卡
 */
static void F_FormatStorage()
{
    T_DjiReturnCode returnCode = DjiCameraManager_FormatStorage(mountPosition);
}

/**
 * SD 卡的存储信息 - 获取
 *      注: L1/P1 相机不支持此 API
 *
 * T_DjiCameraManagerStorageInfo
 *      uint32_t totalCapacity;     总大小, 单位: MByte
 *      uint32_t remainCapacity;    剩余大小, 单位: MByte
 */
static void F_StorageInfo_Get()
{
    T_DjiCameraManagerStorageInfo storageInfo;
    T_DjiReturnCode returnCode = DjiCameraManager_GetStorageInfo(mountPosition, &storageInfo);
}

/**
 * 通过文件索引删除选定的相机媒体文件
 */
static void F_DeleteFileByIndex(uint32_t fileIndex)
{
    T_DjiReturnCode returnCode = DjiCameraManager_DeleteFileByIndex(mountPosition, fileIndex);
}

/**
 * 选定挂载位置的相机激光测距信息 - 获取
 *      注: 最大数据更新频率为 5Hz
 *
 * T_DjiCameraManagerLaserRangingInfo
 *      dji_f64_t longitude;    // 范围: [-180,180]
 *      dji_f64_t latitude;     // 范围: [-90,90]
 *      int32_t altitude;   // 单位: 0.1m
 *      int32_t distance;   // 单位: 0.1m
 *      int16_t screenX;    // 单位: 0.1%
 *      int16_t screenY;    // 单位: 0.1%
 *      bool enable_lidar;
 *      uint8_t exception;
 */
static void F_LaserRangingInfo_Get()
{
    T_DjiCameraManagerLaserRangingInfo laserRangingInfo;
    T_DjiReturnCode returnCode = DjiCameraManager_GetLaserRangingInfo(mountPosition, &laserRangingInfo);
}

/**
 * 选定挂载位置的相机点温度坐标 - 设置
 *
 * T_DjiCameraManagerPointThermometryCoordinate
 *      dji_f32_t pointX;   // 点温测的 x 坐标, 范围: 0-1
 *      dji_f32_t pointY;   // 点温测的 y 坐标, 范围: 0-1
 */
static void F_PointThermometryCoordinate_Set(T_DjiCameraManagerPointThermometryCoordinate pointCoordinate)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetPointThermometryCoordinate(mountPosition, pointCoordinate);
}

/**
 * 点温度结果 - 获取
 *      注: 在从相机获取点温度数据之前，必须先调用 DjiCameraManager_SetPointThermometryCoordinate() 函数
 *
 * T_DjiCameraManagerPointThermometryData
 *      dji_f32_t pointX;            // 点温测的 x 坐标，范围: 0-1
 *      dji_f32_t pointY;            // 点温测的 y 坐标，范围: 0-1
 *      dji_f32_t pointTemperature;  // 当前点的温度
 */
static void F_PointThermometryData_Get()
{
    T_DjiCameraManagerPointThermometryData pointThermometryData;
    T_DjiReturnCode returnCode = DjiCameraManager_GetPointThermometryData(mountPosition, &pointThermometryData);
}

/**
 * 选定挂载位置的相机区域温度坐标 - 设置
 *
 * T_DjiCameraManagerAreaThermometryCoordinate
 *      dji_f32_t areaTempLtX;  // 区域温测左上角的 x 坐标, 范围: 0-1
 *      dji_f32_t areaTempLtY;  // 区域温测左上角的 y 坐标, 范围: 0-1
 *      dji_f32_t areaTempRbX;  // 区域温测右下角的 x 坐标, 范围: 0-1
 *      dji_f32_t areaTempRbY;  // 区域温测右下角的 y 坐标, 范围: 0-1
 */
static void F_AreaThermometryCoordinate_Set(T_DjiCameraManagerAreaThermometryCoordinate areaCoordinate)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetAreaThermometryCoordinate(mountPosition, areaCoordinate);
}

/**
 * 区域温度结果 - 获取
 *      注: 在从相机获取区域温度数据之前，必须先调用 DjiCameraManager_SetAreaThermometryCoordinate() 函数
 *
 * T_DjiCameraManagerAreaThermometryData
 *      dji_f32_t areaTempLtX;          // 当前测温区域左上角的 x 坐标
 *      dji_f32_t areaTempLtY;          // 当前测温区域左上角的 y 坐标
 *      dji_f32_t areaTempRbX;          // 当前测温区域右下角的 x 坐标
 *      dji_f32_t areaTempRbY;          // 当前测温区域右下角的 y 坐标
 *      dji_f32_t areaAveTemp;          // 当前测温区域的平均温度
 *      dji_f32_t areaMinTemp;          // 当前测温区域的最低温度
 *      dji_f32_t areaMaxTemp;          // 当前测温区域的最高温度
 *      dji_f32_t areaMinTempPointX;    // 测温区域内最低温度的 x 坐标
 *      dji_f32_t areaMinTempPointY;    // 测温区域内最低温度的 y 坐标
 *      dji_f32_t areaMaxTempPointX;    // 测温区域内最高温度的 x 坐标
 *      dji_f32_t areaMaxTempPointY;    // 测温区域内最高温度的 y 坐标
 */
static void F_AreaThermometryData_Get()
{
    T_DjiCameraManagerAreaThermometryData areaThermometryData;
    T_DjiReturnCode returnCode = DjiCameraManager_GetAreaThermometryData(mountPosition, &areaThermometryData);
}

/**
 * FFC 模式 - 设置
 *
 * E_DjiCameraManagerFfcMode
 *      DJI_CAMERA_MANAGER_FFC_MODE_MANUAL = 0, // 手动
 *      DJI_CAMERA_MANAGER_FFC_MODE_AUTO = 1,   // 自动
 */
static void F_FfcMode_Set(E_DjiCameraManagerFfcMode ffcMode)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetFfcMode(mountPosition, ffcMode);
}

/**
 * 触发一次 FFC
 */
static void F_TriggerFfc()
{
    T_DjiReturnCode returnCode = DjiCameraManager_TriggerFfc(mountPosition);
}

/**
 * 红外相机增益模式 - 设置
 *
 * E_DjiCameraManagerIrGainMode
 *      DJI_CAMERA_MANAGER_IR_GAIN_MODE_AUTO = 0,   // 自动
 *      DJI_CAMERA_MANAGER_IR_GAIN_MODE_LOW = 1,    // 低
 *      DJI_CAMERA_MANAGER_IR_GAIN_MODE_HIGH = 2,   // 高
 */
static void F_InfraredCameraGainMode_Set(E_DjiCameraManagerIrGainMode gainMode)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetInfraredCameraGainMode(mountPosition, gainMode);
}

/**
 * 红外相机的温度范围 - 获取
 *
 * T_DjiCameraManagerIrTempMeterRange
 *      double lowGainTempMin;  // 低增益温度最小值
 *      double lowGainTempMax;  // 低增益温度最大值
 *      double highGainTempMin; // 高增益温度最小值
 *      double highGainTempMax; // 高增益温度最大值
 */
static void F_InfraredCameraGainModeTemperatureRange_Get()
{
    T_DjiCameraManagerIrTempMeterRange tempRange;
    T_DjiReturnCode returnCode = DjiCameraManager_GetInfraredCameraGainModeTemperatureRange(mountPosition, &tempRange);
}

/**
 * 相机测光模式 - 设置
 *
 * E_DjiCameraManagerMeteringMode
 *      DJI_CAMERA_MANAGER_METERING_MODE_CENTRAL = 0,   // 测光模式, 中心点
 *      DJI_CAMERA_MANAGER_METERING_MODE_AVERAGE = 1,   // 测光模式, 平均值
 *      DJI_CAMERA_MANAGER_METERING_MODE_SPOT = 2,      // 测光模式, 点
 */
static void F_MeteringMode_Set(E_DjiCameraManagerMeteringMode meteringMode)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetMeteringMode(mountPosition, meteringMode);
}

/**
 * 相机测光模式 - 获取
 */
static void F_MeteringMode_Get()
{
    E_DjiCameraManagerMeteringMode meteringMode;
    T_DjiReturnCode returnCode = DjiCameraManager_GetMeteringMode(mountPosition, &meteringMode);
}

/**
 * 测光点范围 - 获取
 */
static void F_MeteringPointRegionRange_Get()
{
    uint8_t *hrzNum; // 返回的水平范围值
    uint8_t *vtcNum; // 返回的垂直范围值
    T_DjiReturnCode returnCode = DjiCameraManager_GetMeteringPointRegionRange(mountPosition, &hrzNum, &vtcNum);
}

/**
 * 测光点 - 设置
 * uint8_t y: 垂直坐标值, 不得大于 vtcNum - 1
 * uint8_t x: 水平坐标值, 不得大于 hrzNum - 1
 */
static void F_MeteringPoint_Set(uint8_t x, uint8_t y)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetMeteringPoint(mountPosition, x, y);
}

/**
 * 当前测光点 - 获取
 */
static void F_MeteringPoint_Get()
{
    uint8_t x; // 返回的当前水平坐标
    uint8_t y; // 返回的当前垂直坐标
    T_DjiReturnCode returnCode = DjiCameraManager_GetMeteringPoint(mountPosition, &x, &y);
}

/**
 * 当前相机挂载位置的点云录制 - 开启
 */
static void F_RecordPointCloud_Start()
{
    T_DjiReturnCode returnCode = DjiCameraManager_StartRecordPointCloud(mountPosition);
}

/**
 * 当前相机挂载位置的点云录制 - 停止
 *      注: 前置条件, 相机当前正在录制
 */
static void F_RecordPointCloud_Stop()
{
    T_DjiReturnCode returnCode = DjiCameraManager_StopRecordPointCloud(mountPosition);
}

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode F_Camera_Manage()
{
    T_DjiReturnCode returnCode;

    // 初始化相机管理功能模块

    E_DjiCameraManagerISO getIso;
    E_DjiCameraManagerISO setIso;
    if (getIso == setIso)
    {
        USER_LOG_INFO("安装位置 %d 相机的iso已经是您所期望的", mountPosition);
        return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    }

    return returnCode;
}