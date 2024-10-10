/**
 * ********************************************************************
 * Created by WangYunwei [2024-10-09] 基础相机管理
 * ********************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "dji_logger.h"
#include "ms_camera_basic_manage.h"

/* Private types -------------------------------------------------------------*/
typedef struct
{
    E_DjiCameraType cameraType;
    char *cameraTypeStr;
} T_DjiTestCameraTypeStr;

/* Private constants ---------------------------------------------------------*/

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

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode F_Camera_Basic_Manage()
{
    T_DjiReturnCode returnCode;

    // 相机管理模块初始化
    returnCode = DjiCameraManager_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("初始化相机管理器失败, 错误编码: 0x%08X\r\n", returnCode);
    }

    /**
     * 获取相机类型和版本
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
    E_DjiMountPosition mountPosition = DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1;
    E_DjiCameraType cameraType;
    returnCode = DjiCameraManager_GetCameraType(mountPosition, &cameraType);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("获取安装位置 %d 相机类型失败, 错误编码: 0x%08X\r\n", mountPosition, returnCode);
        return returnCode;
    }
    for (int i = 0; i < sizeof(s_cameraTypeStrList) / sizeof(s_cameraTypeStrList[0]); i++)
    {
        if (s_cameraTypeStrList[i].cameraType == cameraType)
        {
            USER_LOG_INFO("安装位置 %d 摄像机的类型为 %s", mountPosition, s_cameraTypeStrList[i].cameraTypeStr);
        }
    }

    T_DjiCameraManagerFirmwareVersion firmwareVersion; // 摄像头模块光圈值
    returnCode = DjiCameraManager_GetFirmwareVersion(mountPosition, &firmwareVersion);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("安装位置 %d 相机的固件版本失败, 错误编码: 0x%08X\r\n", mountPosition, returnCode);
        return returnCode;
    }
    USER_LOG_INFO("安装位置 %d 相机的固件是 V%d.%d.%d.%d\r\n", mountPosition, firmwareVersion.firmware_version[0], firmwareVersion.firmware_version[1], firmwareVersion.firmware_version[2], firmwareVersion.firmware_version[3]);

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

    // 订阅相机实时三维点云数据 (仅支持 L2 相机, 可搭载在 M300 RTK、M350 RTK 上)

    // 下载和删除相机原始媒体文件

    return returnCode;
}