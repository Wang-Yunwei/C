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
 *
 * T_DjiWayPointV2MissionSettings
 *      uint32_t missionID;
 *      uint8_t repeatTimes;
 *      E_DJIWaypointV2MissionFinishedAction finishedAction;    // 航点任务完成后将采取行动
 *          DJI_WAYPOINT_V2_FINISHED_NO_ACTION,
 *          DJI_WAYPOINT_V2_FINISHED_GO_HOME,
 *          DJI_WAYPOINT_V2_FINISHED_AUTO_LANDING,
 *          DJI_WAYPOINT_V2_FINISHED_GO_TO_FIRST_WAYPOINT,
 *          DJI_WAYPOINT_V2_FINISHED_CONTINUE_UNTIL_STOP
 *      dji_f32_t maxFlightSpeed;
 *      dji_f32_t autoFlightSpeed;
 *      E_DJIWaypointV2MissionActionWhenRcLost actionWhenRcLost;
 *          DJI_WAYPOINT_V2_MISSION_STOP_WAYPOINT_V2_AND_EXECUTE_RC_LOST_ACTION,
 *          DJI_WAYPOINT_V2_MISSION_KEEP_EXECUTE_WAYPOINT_V2,
 *      E_DJIWaypointV2MissionGotoFirstWaypointMode gotoFirstWaypointMode;
 *          DJI_WAYPOINT_V2_MISSION_GO_TO_FIRST_WAYPOINT_MODE_SAFELY,
 *          DJI_WAYPOINT_V2_MISSION_GO_TO_FIRST_WAYPOINT_MODE_POINT_TO_POINT,
 *      T_DjiWaypointV2 *mission;
 *          dji_f64_t longitude;
 *          dji_f64_t latitude;
 *          dji_f32_t relativeHeight;   // 相对起飞高度
 *          E_DJIWaypointV2FlightPathMode waypointType;
 *          E_DJIWaypointV2HeadingMode headingMode;
 *          T_DjiWaypointV2Config config;
 *          uint16_t dampingDistance;
 *          dji_f32_t heading;
 *          E_DJIWaypointV2TurnMode turnMode;
 *          T_DjiWaypointV2RelativePosition pointOfInterest;
 *          dji_f32_t maxFlightSpeed;
 *          dji_f32_t autoFlightSpeed;
 *      uint16_t missTotalLen;
 *      T_DJIWaypointV2ActionList actionList;
 *          T_DJIWaypointV2Action *actions; // 此类表示“DJIWaypointV2Mission”的动作, 它决定了在执行航路点任务时如何执行操作
 *              uint16_t actionId;
 *              T_DJIWaypointV2Trigger trigger;
 *                  uint8_t actionTriggerType;
 *                  union {
 *                      T_DJIWaypointV2SampleReachPointTriggerParam sampleReachPointTriggerParam;
 *                      T_DJIWaypointV2AssociateTriggerParam associateTriggerParam;
 *                      T_DJIWaypointV2TrajectoryTriggerParam trajectoryTriggerParam;
 *                      T_DJIWaypointV2IntervalTriggerParam intervalTriggerParam;
 *                  };
 *              T_DJIWaypointV2Actuator actuator;
 *                  uint8_t actuatorType;
 *                  uint8_t actuatorIndex;
 *                  union {
 *                      T_DJIWaypointV2CameraActuatorParam cameraActuatorParam;
 *                      T_DJIWaypointV2GimbalActuatorParam gimbalActuatorParam;
 *                      T_DJIWaypointV2AircraftControlParam aircraftControlActuatorParam;
 *                  };
 *          uint16_t actionNum;
 */
static void F_UploadMission()
{
    T_DjiWayPointV2MissionSettings info = {0};
    T_DjiReturnCode returnCode = DjiWaypointV2_UploadMission(&info);
}

/**
 * 开始执行任务
 */
static void F_Start()
{
    T_DjiReturnCode returnCode = DjiWaypointV2_Start();
}

/**
 * 停止任务
 */
static void F_Stop()
{
    T_DjiReturnCode returnCode = DjiWaypointV2_Stop();
}

/**
 * 暂停任务
 */
static void F_Pause()
{
    T_DjiReturnCode returnCode = DjiWaypointV2_Pause();
}

/**
 * 恢复任务
 */
static void F_Resume()
{
    T_DjiReturnCode returnCode = DjiWaypointV2_Resume();
}

/**
 * 从飞控获取全局巡航速度设置 - 获取
 */
static void F_GlobalCruiseSpeed_Get()
{
    T_DjiWaypointV2GlobalCruiseSpeed cruiseSpeed;
    T_DjiReturnCode returnCode = DjiWaypointV2_GetGlobalCruiseSpeed(&cruiseSpeed);
}

/**
 * 将全局巡航速度设置为飞控
 *
 * T_DjiWaypointV2GlobalCruiseSpeed
 *      dji_f32_t
 */
static void F_GlobalCruiseSpeed_Set(T_DjiWaypointV2GlobalCruiseSpeed cruiseSpeed)
{
    T_DjiReturnCode returnCode = DjiWaypointV2_SetGlobalCruiseSpeed(cruiseSpeed);
}

/**
 * 使用回调函数订阅 waypointV2 事件
 *
 * T_DjiWaypointV2MissionEventPush
 *      uint8_t event;
 *      uint32_t FCTimestamp;
 *      U_DjiWaypointV2EventData data;
 *          uint16_t actionId;
 *          uint8_t preActuatorState;
 *          uint8_t curActuatorState;
 *          uint32_t result;
 */
static T_DjiReturnCode F_WaypointV2EventCbFunc_Callback(T_DjiWaypointV2MissionEventPush eventData)
{
}
static void F_RegisterMissionEventCallback()
{
    T_DjiReturnCode returnCode = DjiWaypointV2_RegisterMissionEventCallback(F_WaypointV2EventCbFunc_Callback);
}

/**
 * 使用回调函数订阅 waypointV2 任务状态
 *
 * T_DjiWaypointV2MissionStatePush
 *      uint16_t curWaypointIndex;
 *      uint8_t state;
 *      uint16_t velocity;
 */
static T_DjiReturnCode F_WaypointV2StateCbFunc_Callback(T_DjiWaypointV2MissionStatePush stateData)
{
}
static void F_()
{
    T_DjiReturnCode returnCode = DjiWaypointV2_RegisterMissionStateCallback(F_WaypointV2StateCbFunc_Callback);
}

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode F_WAYPOINT_V2()
{
}