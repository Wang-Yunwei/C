/**
 * ********************************************************************
 * Created by WangYunwei [2024-10-09] 基础相机功能
 * ********************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "dji_logger.h"

#include "ms_camera_basic_function.h"

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode F_Camera_Basic_Function()
{
    T_DjiReturnCode returnCode;

    // 相机类功能模块初始化
    returnCode = DjiPayloadCamera_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("初始化相机类功能模块失败, 错误编码:0x%08llX", returnCode);
    }

    return returnCode;
}