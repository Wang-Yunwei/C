/**
 * ********************************************************************
 * Created by WangYunwei [2024-10-09] 基础相机功能
 * ********************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "dji_logger.h"
#include "dji_aircraft_info.h"
#include "ms_camera_basic_function.h"

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
#define INTERVAL_PHOTOGRAPH_ALWAYS_COUNT (255)
#define INTERVAL_PHOTOGRAPH_INTERVAL_INIT_VALUE (1) // unit: s
#define SDCARD_TOTAL_SPACE_IN_MB (32 * 1024)
#define SDCARD_PER_PHOTO_SPACE_IN_MB (4)
#define SDCARD_PER_SECONDS_RECORD_SPACE_IN_MB (2)

/* Private variables ---------------------------------------------------------*/
static T_DjiCameraCommonHandler s_commonHandler;
static T_DjiMutexHandle s_commonMutex = {0};
static T_DjiCameraSystemState s_cameraState = {0};
static E_DjiCameraShootPhotoMode s_cameraShootPhotoMode = DJI_CAMERA_SHOOT_PHOTO_MODE_SINGLE;
static T_DjiCameraPhotoTimeIntervalSettings s_cameraPhotoTimeIntervalSettings = {INTERVAL_PHOTOGRAPH_ALWAYS_COUNT, INTERVAL_PHOTOGRAPH_INTERVAL_INIT_VALUE};
static E_DjiCameraBurstCount s_cameraBurstCount = DJI_CAMERA_BURST_COUNT_2;
static T_DjiCameraSDCardState s_cameraSDCardState = {0};
static T_DjiMutexHandle s_zoomMutex = {0};
static T_DjiMutexHandle s_tapZoomMutex = NULL;

/* Private functions declaration ---------------------------------------------*/
/**
 * 获取相机类负载设备当前状态的回调函数
 * systemState: 指向用于存储相机类负载设备当前状态的内存空间
 *
 * T_DjiCameraSystemState
 *      E_DjiCameraMode cameraMode;                           相机类负载设备当前的模式, 详情请参见: E_DjiCameraMode
 *      E_DjiCameraShootingState shootingState;               相机类负载设备当前拍照的模式,详情请参见：E_DjiCameraShootingState
 *      bool isStoring;                                       相机类负载设备是否正在存储媒体文件
 *      bool isShootingIntervalStart;                         相机类负载设备是否正在定时拍照
 *      uint16_t currentPhotoShootingIntervalTimeInSeconds;   指定相机类负载设备定时拍照倒计时,当该该数值递减为0 时将触发相机拍照,单位: s
 *      uint16_t currentPhotoShootingIntervalCount;           指定相机类负载设备定时拍照时拍摄的照片
 *      bool isRecording;                                     相机类负载设备是否正在录像
 *      uint16_t currentVideoRecordingTimeInSeconds;          指定相机类负载设备录像时间的进度,单位: s
 *      bool isOverheating;                                   相机是否过热
 *      bool hasError;                                        相机是否出现错误
 */
static T_DjiReturnCode GetSystemState(T_DjiCameraSystemState *systemState)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    *systemState = s_cameraState;

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 设置相机类负载设备工作模式的回调函数
 *      在相机类负载设备执行某项任务时无法切换工作模式
 * mode: 相机工作的模式
 *
 * E_DjiCameraMode
 *      DJI_CAMERA_MODE_SHOOT_PHOTO = 0,  拍照模式
 *      DJI_CAMERA_MODE_RECORD_VIDEO = 1, 录像模式
 *      DJI_CAMERA_MODE_PLAYBACK = 2,     媒体文件回放
 */
static T_DjiReturnCode SetMode(E_DjiCameraMode mode)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    s_cameraState.cameraMode = mode;
    USER_LOG_INFO("set camera mode:%d", mode);

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 获取相机类负载设备当前工作模式的回调函数
 * mode: 指向用于存储相机类负载设备工作模式的内存空间
 *
 * E_DjiCameraMode
 *      DJI_CAMERA_MODE_SHOOT_PHOTO = 0,  拍照模式
 *      DJI_CAMERA_MODE_RECORD_VIDEO = 1, 录像模式
 *      DJI_CAMERA_MODE_PLAYBACK = 2,     媒体文件回放
 */
static T_DjiReturnCode GetMode(E_DjiCameraMode *mode)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    *mode = s_cameraState.cameraMode;

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 控制相机类负载设备开始录像的回调函数
 */
static T_DjiReturnCode StartRecordVideo(void)
{
    T_DjiReturnCode djiStat;
    T_DjiReturnCode returnCode = DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    djiStat = osalHandler->MutexLock(s_commonMutex);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", djiStat);
        return djiStat;
    }

    if (s_cameraState.isRecording != false)
    {
        USER_LOG_ERROR("camera is already in recording state");
        returnCode = DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT_IN_CURRENT_STATE;
        goto out;
    }

    s_cameraState.isRecording = true;
    USER_LOG_INFO("start record video");

out:
    djiStat = osalHandler->MutexUnlock(s_commonMutex);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", djiStat);
        return djiStat;
    }

    return returnCode;
}

/**
 * 控制相机类负载设备停止录像的回调函数
 */
static T_DjiReturnCode StopRecordVideo(void)
{
    T_DjiReturnCode djiStat;
    T_DjiReturnCode returnCode = DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    djiStat = osalHandler->MutexLock(s_commonMutex);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", djiStat);
        return djiStat;
    }

    if (s_cameraState.isRecording != true)
    {
        USER_LOG_ERROR("camera is not in recording state");
        returnCode = DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT_IN_CURRENT_STATE;
        goto out;
    }

    s_cameraState.isRecording = false;
    s_cameraState.currentVideoRecordingTimeInSeconds = 0;
    USER_LOG_INFO("stop record video");

out:
    djiStat = osalHandler->MutexUnlock(s_commonMutex);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", djiStat);
        return djiStat;
    }

    return returnCode;
}

/**
 * 控制相机类负载设备开始拍摄照片的回调函数
 */
static T_DjiReturnCode StartShootPhoto(void)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    USER_LOG_INFO("start shoot photo");
    s_cameraState.isStoring = true;

    if (s_cameraShootPhotoMode == DJI_CAMERA_SHOOT_PHOTO_MODE_SINGLE)
    {
        s_cameraState.shootingState = DJI_CAMERA_SHOOTING_SINGLE_PHOTO;
    }
    else if (s_cameraShootPhotoMode == DJI_CAMERA_SHOOT_PHOTO_MODE_BURST)
    {
        s_cameraState.shootingState = DJI_CAMERA_SHOOTING_BURST_PHOTO;
    }
    else if (s_cameraShootPhotoMode == DJI_CAMERA_SHOOT_PHOTO_MODE_INTERVAL)
    {
        s_cameraState.shootingState = DJI_CAMERA_SHOOTING_INTERVAL_PHOTO;
        s_cameraState.isShootingIntervalStart = true;
        s_cameraState.currentPhotoShootingIntervalTimeInSeconds = s_cameraPhotoTimeIntervalSettings.timeIntervalSeconds;
    }

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 控制相机类负载设备停止拍摄照片的回调函数
 */
static T_DjiReturnCode StopShootPhoto(void)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    USER_LOG_INFO("stop shoot photo");
    s_cameraState.shootingState = DJI_CAMERA_SHOOTING_PHOTO_IDLE;
    s_cameraState.isStoring = false;
    s_cameraState.isShootingIntervalStart = false;

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 设置相机类负载设备拍照模式的回调函数
 * mode: 相机拍照的模式
 *
 * E_DjiCameraShootPhotoMode
 *      DJI_CAMERA_SHOOT_PHOTO_MODE_SINGLE = 1,     单张拍摄模式
 *      DJI_CAMERA_SHOOT_PHOTO_MODE_BURST = 4,      连拍模式
 *      DJI_CAMERA_SHOOT_PHOTO_MODE_INTERVAL = 6,   间隔拍摄模式
 */
static T_DjiReturnCode SetShootPhotoMode(E_DjiCameraShootPhotoMode mode)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    s_cameraShootPhotoMode = mode;
    USER_LOG_INFO("set shoot photo mode:%d", mode);

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 获取相机类负载设备当前拍照模式的回调函数
 * mode: 指向用于存储相机类负载设备拍照模式的内存空间
 *
 * E_DjiCameraShootPhotoMode
 *      DJI_CAMERA_SHOOT_PHOTO_MODE_SINGLE = 1,     单张拍摄模式
 *      DJI_CAMERA_SHOOT_PHOTO_MODE_BURST = 4,      连拍模式
 *      DJI_CAMERA_SHOOT_PHOTO_MODE_INTERVAL = 6,   间隔拍摄模式
 */
static T_DjiReturnCode GetShootPhotoMode(E_DjiCameraShootPhotoMode *mode)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    *mode = s_cameraShootPhotoMode;

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 设置相机类负载设备在连拍模式下连拍张数的回调函数
 * burstCount: 相机连拍张数
 *
 * E_DjiCameraBurstCount
 *      DJI_CAMERA_BURST_COUNT_2     = 2,      在连拍模式下每次连续拍摄的照片张数为 2
 *      DJI_CAMERA_BURST_COUNT_3     = 3,      在连拍模式下每次连续拍摄的照片张数为 3
 *      DJI_CAMERA_BURST_COUNT_5     = 5,      在连拍模式下每次连续拍摄的照片张数为 5
 *      DJI_CAMERA_BURST_COUNT_7     = 7,      在连拍模式下每次连续拍摄的照片张数为 7
 *      DJI_CAMERA_BURST_COUNT_10    = 10,     在连拍模式下每次连续拍摄的照片张数为 10, 仅被 X4S 相机、X5S 相机、Phantom 4 Pro 相机支持
 *      DJI_CAMERA_BURST_COUNT_14    = 14,     在连拍模式下每次连续拍摄的照片张数为 14, 仅被 X4S 相机、X5S 相机、Phantom 4 Pro 相机支持
 *      DJI_CAMERA_BURST_COUNT_KNOWN = 0xFF,   在连拍模式下每次连续拍摄的照片张数为 unknown
 */
static T_DjiReturnCode SetPhotoBurstCount(E_DjiCameraBurstCount burstCount)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    s_cameraBurstCount = burstCount;
    USER_LOG_INFO("set photo burst count:%d", burstCount);

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 获取相机类负载设备当前连拍张数的回调函数
 * burstCount: 指向用于存储相机类负载设备连拍张数的内存空间
 *
 * E_DjiCameraBurstCount
 *      DJI_CAMERA_BURST_COUNT_2     = 2,      在连拍模式下每次连续拍摄的照片张数为 2
 *      DJI_CAMERA_BURST_COUNT_3     = 3,      在连拍模式下每次连续拍摄的照片张数为 3
 *      DJI_CAMERA_BURST_COUNT_5     = 5,      在连拍模式下每次连续拍摄的照片张数为 5
 *      DJI_CAMERA_BURST_COUNT_7     = 7,      在连拍模式下每次连续拍摄的照片张数为 7
 *      DJI_CAMERA_BURST_COUNT_10    = 10,     在连拍模式下每次连续拍摄的照片张数为 10, 仅被 X4S 相机、X5S 相机、Phantom 4 Pro 相机支持
 *      DJI_CAMERA_BURST_COUNT_14    = 14,     在连拍模式下每次连续拍摄的照片张数为 14, 仅被 X4S 相机、X5S 相机、Phantom 4 Pro 相机支持
 *      DJI_CAMERA_BURST_COUNT_KNOWN = 0xFF,   在连拍模式下每次连续拍摄的照片张数为 unknown
 */
static T_DjiReturnCode GetPhotoBurstCount(E_DjiCameraBurstCount *burstCount)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    *burstCount = s_cameraBurstCount;

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 设置相机类负载设备在定时拍照模式下拍照间隔的回调函数
 *      在定时拍照模式下拍照的时间间隔范围为2～255,当时间间隔设为255 时,相机类负载设备将会持续拍摄照片
 * settings: 设置相机类负载设备在定时拍照模式下拍照的时间间隔
 *
 * T_DjiCameraPhotoTimeIntervalSettings
 *      uint8_t captureCount;           指定间隔设置的总拍摄计数, 0: 保留, 1~254: 数字, 255: 一直拍摄直到停止
 *      uint16_t timeIntervalSeconds;   指定两次拍摄之间的间隔时间, 单位: s
 */
static T_DjiReturnCode SetPhotoTimeIntervalSettings(T_DjiCameraPhotoTimeIntervalSettings settings)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    s_cameraPhotoTimeIntervalSettings.captureCount = settings.captureCount;
    s_cameraPhotoTimeIntervalSettings.timeIntervalSeconds = settings.timeIntervalSeconds;
    USER_LOG_INFO("set photo interval settings count:%d seconds:%d", settings.captureCount,
                  settings.timeIntervalSeconds);
    s_cameraState.currentPhotoShootingIntervalCount = settings.captureCount;

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 获取相机类负载设备在定时拍照模式下拍照间隔的回调函数
 * settings: 指向用于存储相机类负载设备在定时拍照模式下拍照间隔时间的内存空间
 *
 * T_DjiCameraPhotoTimeIntervalSettings
 *      uint8_t captureCount;           指定间隔设置的总拍摄计数, 0: 保留, 1~254: 数字, 255: 一直拍摄直到停止
 *      uint16_t timeIntervalSeconds;   指定两次拍摄之间的间隔时间, 单位: s
 */
static T_DjiReturnCode GetPhotoTimeIntervalSettings(T_DjiCameraPhotoTimeIntervalSettings *settings)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    memcpy(settings, &s_cameraPhotoTimeIntervalSettings, sizeof(T_DjiCameraPhotoTimeIntervalSettings));

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 获取相机类负载设备中当前SD 卡状态的回调函数
 * sdCardState: 指向用于存储相机类负载SD 卡状态的内存空间
 *
 * T_DjiCameraSDCardState
 *      bool isInserted;   相机类负载设备中是否已插入SD 卡
 *      bool isVerified;   相机类负载设备中的SD 卡是否被认证
 *      bool isInitializing;   相机类负载设备中SD 卡是否已初始化
 *      bool isReadOnly;   相机类负载设备中的SD 卡是否处于只读模式
 *      bool isFormatting;   相机类负载设备中的SD 卡正在格式化
 *      bool isFull;   相机类负载设备中的SD 卡内存已满
 *      bool isInvalidFormat;   相机类负载设备中的SD 卡已格式化
 *      bool hasError;   相机类负载设备中的SD 卡出现错误
 *      uint32_t totalSpaceInMB;   相机类负载设备中SD 卡的总容量, 单位: MB
 *      uint32_t remainSpaceInMB;   相机类负载设备中SD 卡的剩余容量, 单位: MB
 *      uint32_t availableCaptureCount;   当前相机类负载设备中SD 卡可存储的相片数量
 *      uint32_t availableRecordingTimeInSeconds;   当前相机类负载设备中SD 卡可存储视频的长度, 单位: s
 */
static T_DjiReturnCode GetSDCardState(T_DjiCameraSDCardState *sdCardState)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    memcpy(sdCardState, &s_cameraSDCardState, sizeof(T_DjiCameraSDCardState));

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 控制相机类负载设备格式化 SD 卡的回调函数
 */
static T_DjiReturnCode FormatSDCard(void)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    USER_LOG_INFO("format sdcard");

    memset(&s_cameraSDCardState, 0, sizeof(T_DjiCameraSDCardState));
    s_cameraSDCardState.isInserted = true;                                                                                  // 相机类负载设备中是否已插入SD 卡
    s_cameraSDCardState.isVerified = true;                                                                                  // 相机类负载设备中的 SD 卡是否被认证
    s_cameraSDCardState.totalSpaceInMB = SDCARD_TOTAL_SPACE_IN_MB;                                                          // 相机类负载设备中 SD 卡的总容量,单位: MB
    s_cameraSDCardState.remainSpaceInMB = SDCARD_TOTAL_SPACE_IN_MB;                                                         // 相机类负载设备中 SD 卡的剩余容量,单位: MB
    s_cameraSDCardState.availableCaptureCount = SDCARD_TOTAL_SPACE_IN_MB / SDCARD_PER_PHOTO_SPACE_IN_MB;                    // 当前相机类负载设备中 SD 卡可存储的相片数量
    s_cameraSDCardState.availableRecordingTimeInSeconds = SDCARD_TOTAL_SPACE_IN_MB / SDCARD_PER_SECONDS_RECORD_SPACE_IN_MB; // 当前相机类负载设备中 SD 卡可存储视频的长度,单位: s

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode F_Camera_Basic_Function()
{
    T_DjiReturnCode returnCode;
    char ipAddr[DJI_IP_ADDR_STR_SIZE_MAX] = {0};
    uint16_t port = 0;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiAircraftInfoBaseInfo aircraftInfoBaseInfo = {0};

    returnCode = osalHandler->MutexCreate(&s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("创建互斥锁用于锁定点击缩放参数错误: 0x%08llX", returnCode);
        return returnCode;
    }

    returnCode = osalHandler->MutexCreate(&s_zoomMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("创建互斥锁用于锁定点击缩放参数错误: 0x%08llX", returnCode);
        return returnCode;
    }

    returnCode = osalHandler->MutexCreate(&s_tapZoomMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("创建互斥锁用于锁定点击缩放参数错误: 0x%08llX", returnCode);
        return returnCode;
    }

    // 相机类功能模块初始化
    returnCode = DjiPayloadCamera_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("初始化相机类功能模块失败, 错误编码:0x%08llX", returnCode);
    }

    /* 初始化 SD卡 参数 */
    s_cameraSDCardState.isInserted = true;                                                                                  // 相机类负载设备中是否已插入SD 卡
    s_cameraSDCardState.isVerified = true;                                                                                  // 相机类负载设备中的 SD 卡是否被认证
    s_cameraSDCardState.totalSpaceInMB = SDCARD_TOTAL_SPACE_IN_MB;                                                          // 相机类负载设备中 SD 卡的总容量,单位: MB
    s_cameraSDCardState.remainSpaceInMB = SDCARD_TOTAL_SPACE_IN_MB;                                                         // 相机类负载设备中 SD 卡的剩余容量,单位: MB
    s_cameraSDCardState.availableCaptureCount = SDCARD_TOTAL_SPACE_IN_MB / SDCARD_PER_PHOTO_SPACE_IN_MB;                    // 当前相机类负载设备中 SD 卡可存储的相片数量
    s_cameraSDCardState.availableRecordingTimeInSeconds = SDCARD_TOTAL_SPACE_IN_MB / SDCARD_PER_SECONDS_RECORD_SPACE_IN_MB; // 当前相机类负载设备中 SD 卡可存储视频的长度,单位: s

    /* 注册相机公共处理程序 */
    s_commonHandler.GetSystemState = GetSystemState; // 获取负载设备系统当前的状
    // 实现设置相机类负载设备模式的功能
    s_commonHandler.SetMode = SetMode;
    s_commonHandler.GetMode = GetMode;
    // 实现开始或停止录像的功能
    s_commonHandler.StartRecordVideo = StartRecordVideo;
    s_commonHandler.StopRecordVideo = StopRecordVideo;
    // 实现开始或停止拍照的功能
    s_commonHandler.StartShootPhoto = StartShootPhoto;
    s_commonHandler.StopShootPhoto = StopShootPhoto;
    //  实现设置相机类负载设备的拍照功能
    s_commonHandler.SetShootPhotoMode = SetShootPhotoMode;
    s_commonHandler.GetShootPhotoMode = GetShootPhotoMode;
    s_commonHandler.SetPhotoBurstCount = SetPhotoBurstCount;
    s_commonHandler.GetPhotoBurstCount = GetPhotoBurstCount;
    s_commonHandler.SetPhotoTimeIntervalSettings = SetPhotoTimeIntervalSettings;
    s_commonHandler.GetPhotoTimeIntervalSettings = GetPhotoTimeIntervalSettings;
    // 实现 SD 卡管理功能
    s_commonHandler.GetSDCardState = GetSDCardState;
    s_commonHandler.FormatSDCard = FormatSDCard;

    // 注册相机类基础功能
    returnCode = DjiPayloadCamera_RegCommonHandler(&s_commonHandler);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("camera register common handler error:0x%08llX", returnCode);
    }

    // 预估可拍照张数和可录像时长的功能
    if (s_cameraState.isRecording)
    {
        s_cameraState.currentVideoRecordingTimeInSeconds++;
        s_cameraSDCardState.remainSpaceInMB = s_cameraSDCardState.remainSpaceInMB - SDCARD_PER_SECONDS_RECORD_SPACE_IN_MB;
        if (s_cameraSDCardState.remainSpaceInMB > SDCARD_TOTAL_SPACE_IN_MB)
        {
            s_cameraSDCardState.remainSpaceInMB = 0;
            s_cameraSDCardState.isFull = true;
        }
    }

    return returnCode;
}