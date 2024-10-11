/**
 * ********************************************************************
 * Created by WangYunwei [2024-10-09] 基础相机管理
 * ********************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "dji_logger.h"
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
 *      DJI_CAMERA_MANAGER_WORK_MODE_SHOOT_PHOTO = 0,            拍照模式, 用户可在此模式下拍照
 *      DJI_CAMERA_MANAGER_WORK_MODE_RECORD_VIDEO = 1,           录像模式, 用户可在此模式下录制视频
 *      DJI_CAMERA_MANAGER_WORK_MODE_PLAYBACK = 2,               回放模式, 用户可预览和删除照片和视频
 *                                                                        适用于: Phantom 3 专业版、X3、X5、X5R、Phantom 4
 *                                                                        不支持: Z30、X5S、X4S、Phantom 4 Pro、Mavic Pro、Phantom 3标准版、Phantom 3 高级版、Phantom 3 4K 和 Osmo 系列
 *      DJI_CAMERA_MANAGER_WORK_MODE_MEDIA_DOWNLOAD = 3,         媒体下载模式, 用户可以下载媒体到移动设备, 不支持: 挂载在飞机上的 X5 和 X5R 摄像机
 *      DJI_CAMERA_MANAGER_WORK_MODE_BROADCAST = 4,              此模式可以录制视频, 此模式下, 分辨率和帧率设置为 1080i50(PAL) 或 720p60(NTSC) 照片拍摄仅能在视频录制时进行
 *                                                                         只支持通过更换模式为 RECORD_VIDEO 来退出直播模式, 仅Inspire 2 支持此模式
 *      DJI_CAMERA_MANAGER_WORK_MODE_WORK_MODE_UNKNOWN = 0xFF,   摄像机工作模式, 未知
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
 *      DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_SINGLE = 0x01,        单拍模式
 *      DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_HDR = 0x02,           HDR 模式, X5、X5R、XT、Z30、Phantom 4 Pro、X4S 和 X5S 相机不支持此模式
 *      DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_BURST = 0x04,         连拍模式, XT 相机不支持
 *      DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_AEB = 0x05,           AEB 模式, 可快速拍摄多张(默认三张) 不同曝光值的照片, XT 和 Z30 相机不支持
 *      DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_INTERVAL = 0x06,      定时拍摄模式, JPEG 的最小拍摄间隔为 2s, RAW 和 RAW+JPEG 的最小拍摄间隔为 10s, 部分机型的 RAW 和 RAW+JPEG 拍摄最小间隔为 5s
 *      DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_RAW_BURST = 0x09,     RAW 连拍模式, 仅 X5S 摄像机支持
 *      DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_REGIONAL_SR = 0x16,   区域拍摄模式, 支持 H20/H20T
 *      DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_UNKNOWN = 0xFF,       拍照模式, 未知
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
 */
static void F_StartShootPhoto_Start(E_DjiCameraManagerShootPhotoMode mode)
{
    T_DjiReturnCode returnCode = DjiCameraManager_StartShootPhoto(mountPosition, mode);
}

/**
 * 拍照 - 停止
 *      注: StartShootPhoto 被调用，当前处于拍摄模式
 */
static void F_StopShootPhoto_Stop()
{
    T_DjiReturnCode returnCode = DjiCameraManager_StopShootPhoto(mountPosition);
}

/**
 * 相机拍摄状态 - 获取
 *      注: L1/P1 不支持此接口
 *
 * E_DjiCameraManagerCapturingState
 *      DJI_CAMERA_MANAGER_CAPTURING_STATE_IDLE = 0,     未拍摄
 *      DJI_CAMERA_MANAGER_CAPTURING_STATE_SINGLE = 1,   单次拍摄中
 *      DJI_CAMERA_MANAGER_CAPTURING_STATE_MULTI = 2,    多次拍摄中
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
 *      DJI_CAMERA_BURST_COUNT_2  = 2,         在连拍模式下每次连续拍摄的照片张数为 2
 *      DJI_CAMERA_BURST_COUNT_3 = 3,          在连拍模式下每次连续拍摄的照片张数为 3
 *      DJI_CAMERA_BURST_COUNT_5 = 5,          在连拍模式下每次连续拍摄的照片张数为 5
 *      DJI_CAMERA_BURST_COUNT_7 = 7,          在连拍模式下每次连续拍摄的照片张数为 7
 *      DJI_CAMERA_BURST_COUNT_10 = 10,        在连拍模式下每次连续拍摄的照片张数为 10, 仅被X4S、X5S、Phantom 4 Pro 相机支持
 *      DJI_CAMERA_BURST_COUNT_14 = 14,        在连拍模式下每次连续拍摄的照片张数为 14, 仅被X4S、X5S、Phantom 4 Pro 相机支持
 *      DJI_CAMERA_BURST_COUNT_KNOWN = 0xFF,   在连拍模式下每次连续拍摄的照片张数为 unknown
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
 */
static void F_PhotoAEBCount_Set(E_DjiCameraManagerPhotoAEBCount count)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetPhotoAEBCount(mountPosition, count);
}

/**
 * AEB(自动曝光拍摄) 模式下的连拍数量 - 获取
 */
static void F_PhotoAEBCount_Get()
{
    E_DjiCameraManagerPhotoAEBCount count;
    T_DjiReturnCode returnCode = DjiCameraManager_GetPhotoAEBCount(mountPosition, &count);
}

/**
 * 间隔拍摄模式的参数 - 设置
 *      注: 在此拍照模式下, 相机将拍摄一张照片,等待指定时间间隔,然后继续拍摄,直至完成所需数量的照片,该模式也支持热成像相机
 *
 * T_DjiCameraPhotoTimeIntervalSettings
 *      uint8_t captureCount; 指定间隔设置的总拍摄计数 0: 保留, 1~254: 数字, 255: 一直拍摄直到停止
 *      uint16_t timeIntervalSeconds; 指定两次拍摄之间的间隔时间，单位: s
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
 *      注: 设置镜头对焦模式,自动对焦模式下,目标点为焦点; 手动对焦模式下,若启用了对焦助手,目标点为缩小区域
 * 
 * E_DjiCameraManagerFocusMode
 *      DJI_CAMERA_MANAGER_FOCUS_MODE_MANUAL = 0,       手动对焦模式, 在此模式下,用户可通过调整对焦环值来调整焦距
 *      DJI_CAMERA_MANAGER_FOCUS_MODE_AUTO = 1,         自动对焦模式, 对于 Z30 相机,焦点会自动计算,对于其他相机,用户可以设置一个对焦目标以自动计算焦点
 *      DJI_CAMERA_MANAGER_FOCUS_MODE_AFC = 2,          连续自动对焦模式, 仅支持固件版本为 V01.03.0000 或更高的 Mavic Pro、X4S 相机、Mavic 2 Zoom 相机和 Mavic 2 Pro 相机
 *      DJI_CAMERA_MANAGER_FOCUS_MODE_UNKNOWN = 0xFF,   相机对焦模式未知
 */
static void F_(E_DjiCameraManagerFocusMode focusMode)
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
 * 
 * T_DjiCameraManagerFocusPosData
 *      dji_f32_t focusX;   指定水平区域坐标, 该参数介于 0 和 1 之间。点 [0.0, 0.0] 表示屏幕的左上角
 *      dji_f32_t focusY;   指定垂直区域坐标, 此参数介于 0 和 1 之间。
 */
static void F_FocusPoint_Set(T_DjiCameraManagerFocusPosData focusPosData)
{
    T_DjiReturnCode returnCode = DjiCameraManager_SetFocusTarget(mountPosition, focusPosData);
}

/**
 * 选定相机挂载位置的对焦点 - 获取
 */
static void F_FocusPoint_Get()
{
    T_DjiCameraManagerFocusPosData tapFocusPos;
    T_DjiReturnCode returnCode = DjiCameraManager_GetFocusTarget(mountPosition, &tapFocusPos);
}

































/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode F_Camera_Basic_Manage()
{
    T_DjiReturnCode returnCode;

    // 初始化相机管理功能模块
    returnCode = DjiCameraManager_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("初始化相机管理器失败, 错误编码: 0x%08X\r\n", returnCode);
    }

    /**
     * 设置或获取相机参数
     *
     * E_DjiCameraManagerISO
     *       DJI_CAMERA_MANAGER_ISO_AUTO = 0x00, 自动设置 ISO 值并非所有相机的手动模式下可用
     *       DJI_CAMERA_MANAGER_ISO_100 = 0x03, ISO 值设置为 100
     *       DJI_CAMERA_MANAGER_ISO_200 = 0x04, ISO 值设置为 200
     *       DJI_CAMERA_MANAGER_ISO_400 = 0x05, ISO 值设置为 400
     *       DJI_CAMERA_MANAGER_ISO_800 = 0x06, ISO 值设置为 800
     *       DJI_CAMERA_MANAGER_ISO_1600 = 0x07, ISO 值设置为 1600
     *       DJI_CAMERA_MANAGER_ISO_3200 = 0x08, ISO 值设置为 3200
     *       DJI_CAMERA_MANAGER_ISO_6400 = 0x09, ISO 值设置为 6400
     *       DJI_CAMERA_MANAGER_ISO_12800 = 0x0A, ISO 值设置为 12800
     *       DJI_CAMERA_MANAGER_ISO_25600 = 0x0B, ISO 值设置为 25600
     *       DJI_CAMERA_MANAGER_ISO_FIXED = 0xFF, ISO 值由相机固件固定,当相机色彩设置为 D_LOG 时,相机将把 ISO 固定在一个特定的值以优化性能
     */
    E_DjiCameraManagerISO getIso;
    returnCode = DjiCameraManager_GetISO(mountPosition, &getIso);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS && returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND)
    {
        USER_LOG_ERROR("获得安装位置 %d 相机的iso失败, 错误编码: 0x%08X", mountPosition, returnCode);
        return returnCode;
    }
    E_DjiCameraManagerISO setIso;
    if (getIso == setIso)
    {
        USER_LOG_INFO("安装位置 %d 相机的iso已经是您所期望的", mountPosition);
        return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    }
    returnCode = DjiCameraManager_SetISO(mountPosition, setIso);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS && returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND)
    {
        USER_LOG_ERROR("设置安装位置 %d 相机的iso %d 失败, 错误码: 0x%08X", mountPosition, setIso, returnCode);
    }

    /**
     * 控制相机执行指定的动作
     *
     * E_DjiCameraManagerShootPhotoMode
     * - 单拍模式
     *      DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_SINGLE = 0x01
     *
     * - HDR 模式, X5、X5R、XT、Z30、Phantom 4 Pro、X4S 和 X5S 相机不支持此模式
     *      DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_HDR = 0x02
     *
     * - 连拍模式, XT 相机不支持
     *      DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_BURST = 0x04
     *
     * - AEB 模式, 可快速拍摄多张（默认三张）不同曝光值的照片。XT 和 Z30 相机不支持
     *      DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_AEB = 0x05
     *
     * - 定时拍摄模式, JPEG 的最小拍摄间隔为 2s, RAW 和 RAW+JPEG 的最小拍摄间隔为 10s, 部分机型的 RAW 和 RAW+JPEG 拍摄最小间隔为 5s
     *      DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_INTERVAL = 0x06
     *
     * - RAW 连拍模式, 仅 X5S 摄像机支持
     *      DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_RAW_BURST = 0x09
     *
     * - 区域拍摄模式, 支持 H20/H20T
     *      DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_REGIONAL_SR = 0x16
     *
     * - 拍照模式未知
     *      DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_UNKNOWN = 0xFF
     */
    returnCode = DjiCameraManager_StartShootPhoto(mountPosition, DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_SINGLE);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("安装位置 %d 相机拍摄照片失败, 错误码: 0x%08X", mountPosition, returnCode);
    }

    return returnCode;
}