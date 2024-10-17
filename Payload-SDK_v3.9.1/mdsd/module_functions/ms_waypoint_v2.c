/**
 * ********************************************************************
 * Created by WangYunwei [2024-10-15] 航点
 * ********************************************************************
 */

/* Includes ------------------------------------------------------------------*/

#include "dji_logger.h"
#include "dji_waypoint_v2.h"
#include "dji_waypoint_v2_type.h"

#include "ms_waypoint_v2.h"

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

/**
 * waypoint V2模块初始化
 */
static void F_Init()
{
    T_DjiReturnCode returnCode = DjiWaypointV2_Init();
}

/**
 * waypoint V2模块反初始化
 */
static void F_Deinit()
{
    T_DjiReturnCode returnCode = DjiWaypointV2_Deinit();
}

/**
 * 初始化任务模块设置
 */
static void F_UploadMission()
{
    T_DjiWayPointV2MissionSettings info = {0};
    T_DjiReturnCode DjiWaypointV2_UploadMission(&info);
}

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode F_WAYPOINT_V2()
{
}