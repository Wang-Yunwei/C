/**
 * ********************************************************************
 * Created by WangYunwei [2024-10-09] 基础相机管理
 * ********************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "dji_logger.h"

#include "dji_camera_manager.h"

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

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

    return returnCode;
}