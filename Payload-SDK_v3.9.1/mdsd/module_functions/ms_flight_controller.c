/**
 * ********************************************************************
 * Created by WangYunwei [2024-10-15] 飞行控制
 * ********************************************************************
 */

/* Includes ------------------------------------------------------------------*/

#include "dji_logger.h"
#include "psdk_typedef.h"
#include "dji_flight_controller.h"

#include "ms_flight_controller.h"

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

/**
 * 初始化飞行控制模块
 *      注: 使用飞行器飞行控制功能需要先初始化飞行控制模块, 再设置飞行器的基础参数, 最后再使用 Joystick 中的功能;
 *          - 当完全脱离遥控器使用 PSDK 控制飞机时, 需要通过订阅飞机端的关键数据, 严格检查飞机的当前状态是否允许进行飞行控制;
 *          - 务必检查当前电量、返航点、失控行为、RTK 开关、避障开关等关键飞行参数, 此外还需要严格遵守当地的飞行器法规要求;
 *          - 如美国需要强制上报 RID 信息, 否则会导致飞机阻飞, 需要第三方开发者保证 RID 信息的准确性, 在飞行任务过程中,需要实时监控飞行器实时的
 *          - 状态和告警信息, 用来辅助远程驾驶员或者程序的决策, 参考 “信息管理” 和 “HMS 功能” 章节
 *
 * T_DjiFlightControllerRidInfo
 *      dji_f64_t latitude;     // 单位: 度
 *      dji_f64_t longitude;    // 单位: 度
 *      uint16_t altitude;
 */
static void F_Init(T_DjiFlightControllerRidInfo ridInfo)
{
    T_DjiReturnCode returnCode = DjiFlightController_Init(ridInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("初始化飞行控制模块失败, 错误码: 0x%08llX", returnCode);
    }
}

/**
 * 反初始化飞行控制模块
 */
static void F_DeInit()
{
    T_DjiReturnCode returnCode = DjiFlightController_DeInit();
}

/**
 * 启用/禁用 RTK 位置功能 - 设置
 *
 * E_DjiFlightControllerRtkPositionEnableStatus
 *      DJI_FLIGHT_CONTROLLER_DISABLE_RTK_POSITION = 0, // 无人机将使用GPS数据而不是 RTK 数据执行需要位置信息的动作（如航点、返航...）
 *      DJI_FLIGHT_CONTROLLER_ENABLE_RTK_POSITION = 1,  // 无人机将使用RTK数据而不是 GPS 数据执行需要位置信息的动作（如航点、返航...)
 */
static void F_RtkPositionEnableStatus_Set(E_DjiFlightControllerRtkPositionEnableStatus rtkEnableStatus)
{
    T_DjiReturnCode returnCode = DjiFlightController_SetRtkPositionEnableStatus(rtkEnableStatus);
}

/**
 * RTK 启用状态 - 获取
 */
static void F_RtkPositionEnableStatus_Get()
{
    E_DjiFlightControllerRtkPositionEnableStatus rtkEnableStatus;
    T_DjiReturnCode returnCode = DjiFlightController_GetRtkPositionEnableStatus(rtkEnableStatus);
}

/**
 * 失联动作 - 设置
 *      注: 当遥控器和 OSDK 均失联时, 此设置才生效, 仅支持 M30
 *
 * E_DjiFlightControllerRCLostAction
 *      DJI_FLIGHT_CONTROLLER_RC_LOST_ACTION_HOVER = 0,     // 无人机在失去遥控信号时悬停
 *      DJI_FLIGHT_CONTROLLER_RC_LOST_ACTION_LANDING = 1,   // 无人机在失去遥控信号时着陆
 *      DJI_FLIGHT_CONTROLLER_RC_LOST_ACTION_GOHOME = 2,    // 无人机在失去遥控信号时返航
 */
static void F_RCLostAction_Set(E_DjiFlightControllerRCLostAction rcLostAction)
{
    T_DjiReturnCode returnCode = DjiFlightController_SetRCLostAction(rcLostAction);
}

/**
 * 遥控器失联动作 - 获取
 *      注: 当遥控器和 OSDK 均失联时, 此设置才生效, 仅支持 M30
 */
static void F_RCLostAction_Get()
{
    E_DjiFlightControllerRCLostAction rcLostAction;
    T_DjiReturnCode returnCode = DjiFlightController_GetRCLostAction(&rcLostAction);
}

/**
 * 启用/禁用水平视觉障碍物避让(前、后、左、右) - 设置
 *
 * E_DjiFlightControllerObstacleAvoidanceEnableStatus
 *      DJI_FLIGHT_CONTROLLER_DISABLE_OBSTACLE_AVOIDANCE = 0,   // 无人机不会在指定的方向执行障碍物避让
 *      DJI_FLIGHT_CONTROLLER_ENABLE_OBSTACLE_AVOIDANCE = 1,    // 无人机将在指定的方向执行障碍物避让
 */
static void F_HorizontalVisualObstacleAvoidanceEnableStatus_Set(E_DjiFlightControllerObstacleAvoidanceEnableStatus horizontalObstacleAvoidanceEnableStatus)
{
    T_DjiReturnCode returnCode = DjiFlightController_SetHorizontalVisualObstacleAvoidanceEnableStatus(horizontalObstacleAvoidanceEnableStatus);
}

/**
 * 水平视觉障碍物避让(前、后、左、右) - 获取
 */
static void F_HorizontalVisualObstacleAvoidanceEnableStatus_Get()
{
    E_DjiFlightControllerObstacleAvoidanceEnableStatus horizontalObstacleAvoidanceEnableStatus;
    T_DjiReturnCode returnCode = DjiFlightController_GetHorizontalVisualObstacleAvoidanceEnableStatus(&horizontalObstacleAvoidanceEnableStatus);
}

/**
 * 开启/关闭水平雷达避障功能 - 设置
 *      注: 仅当成功安装 CSM 雷达后生效
 *
 * E_DjiFlightControllerObstacleAvoidanceEnableStatus   // 同 F_HorizontalVisualObstacleAvoidanceEnableStatus_Set()
 */
static void F_HorizontalRadarObstacleAvoidanceEnableStatus_Set(E_DjiFlightControllerObstacleAvoidanceEnableStatus horizontalObstacleAvoidanceEnableStatus)
{
    T_DjiReturnCode returnCode = DjiFlightController_SetHorizontalRadarObstacleAvoidanceEnableStatus(horizontalObstacleAvoidanceEnableStatus);
}

/**
 * 水平雷达避障功能的状态 - 获取
 */
static void F_HorizontalRadarObstacleAvoidanceEnableStatus_Get()
{
    E_DjiFlightControllerObstacleAvoidanceEnableStatus horizontalObstacleAvoidanceEnableStatus;
    T_DjiReturnCode returnCode = DjiFlightController_GetHorizontalRadarObstacleAvoidanceEnableStatus(&horizontalObstacleAvoidanceEnableStatus);
}

/**
 * 开启/关闭上视避障功能 - 设置
 *
 * E_DjiFlightControllerObstacleAvoidanceEnableStatus   // 同 F_HorizontalVisualObstacleAvoidanceEnableStatus_Set()
 */
static void F_UpwardsVisualObstacleAvoidanceEnableStatus_Set(E_DjiFlightControllerObstacleAvoidanceEnableStatus upwardsObstacleAvoidanceEnableStatus)
{
    T_DjiReturnCode returnCode = DjiFlightController_SetUpwardsVisualObstacleAvoidanceEnableStatus(upwardsObstacleAvoidanceEnableStatus);
}

/**
 * 上视避障功能的状态 - 获取
 */
static void F_UpwardsVisualObstacleAvoidanceEnableStatus_Get()
{
    E_DjiFlightControllerObstacleAvoidanceEnableStatus upwardsObstacleAvoidanceEnableStatus;
    T_DjiReturnCode returnCode = DjiFlightController_GetUpwardsVisualObstacleAvoidanceEnableStatus(&upwardsObstacleAvoidanceEnableStatus);
}

/**
 * 开启/关闭向上雷达避障功能 - 设置
 *      注: 仅当成功安装 CSM 雷达后生效
 *
 * E_DjiFlightControllerObstacleAvoidanceEnableStatus   // 同 F_HorizontalVisualObstacleAvoidanceEnableStatus_Set()
 */
static void F_UpwardsRadarObstacleAvoidanceEnableStatus_Set(E_DjiFlightControllerObstacleAvoidanceEnableStatus upwardsObstacleAvoidanceEnableStatus)
{
    T_DjiReturnCode returnCode = DjiFlightController_SetUpwardsRadarObstacleAvoidanceEnableStatus(upwardsObstacleAvoidanceEnableStatus);
}

/**
 * 向上雷达避障功能的状态 - 获取
 */
static void F_UpwardsRadarObstacleAvoidanceEnableStatus_Get()
{
    E_DjiFlightControllerObstacleAvoidanceEnableStatus upwardsObstacleAvoidanceEnableStatus;
    T_DjiReturnCode returnCode = DjiFlightController_GetUpwardsRadarObstacleAvoidanceEnableStatus(&upwardsObstacleAvoidanceEnableStatus);
}

/**
 * 开启/关闭向下视觉避障功能 - 设置
 *
 * E_DjiFlightControllerObstacleAvoidanceEnableStatus   // 同 F_HorizontalVisualObstacleAvoidanceEnableStatus_Set()
 */
static void F_SetDownwardsVisualObstacleAvoidanceEnableStatus_Set(E_DjiFlightControllerObstacleAvoidanceEnableStatus downwardsObstacleAvoidanceEnableStatus)
{
    T_DjiReturnCode returnCode = DjiFlightController_SetDownwardsVisualObstacleAvoidanceEnableStatus(downwardsObstacleAvoidanceEnableStatus);
}

/**
 * 向下视觉避障功能的状态 - 获取
 */
static void F_DownwardsVisualObstacleAvoidanceEnableStatus_Get()
{
    E_DjiFlightControllerObstacleAvoidanceEnableStatus downwardsObstacleAvoidanceEnableStatus;
    T_DjiReturnCode returnCode = DjiFlightController_GetDownwardsVisualObstacleAvoidanceEnableStatus(&downwardsObstacleAvoidanceEnableStatus);
}

/**
 * 紧急制动飞行
 *      注: 当无人机在地面时, 它会停止电机并在 APP 上显示 “hms description”;
 *          当无人机在空中时, 仅在APP上显示 “hms description”, 使用此接口后需调用 "DjiFlightController_CancelArrestFlying" 退出该状态, 无人机才可再次飞行
 */
static void F_ArrestFlying()
{
    T_DjiReturnCode returnCode = DjiFlightController_ArrestFlying();
}

/**
 * 退出紧急制动状态
 *      注: 在紧急制动后, 无人机如需继续飞行则需先退出该状态
 */
static void F_CancelArrestFlying()
{
    T_DjiReturnCode returnCode = DjiFlightController_CancelArrestFlying();
}

/**
 * 无人机在地面时启动电机
 */
static void F_TurnOnMotors()
{
    T_DjiReturnCode returnCode = DjiFlightController_TurnOnMotors();
}

/**
 * 无人机在地面时关闭电机
 */
static void F_TurnOffMotors()
{
    T_DjiReturnCode returnCode = DjiFlightController_TurnOffMotors();
}

/**
 * 在任何情况下紧急停止电机
 *      注: 需使用此接口发送禁用指令退出锁定电机状态
 *
 * char debugMsg:   // 将调试信息注入飞行控制固件以进行记录，有大小限制
 * E_DjiFlightControllerEmergencyStopMotor cmd: // 在空中启用紧急停机功能非常危险, 可能导致飞机坠毁
 *      DJI_FLIGHT_CONTROLLER_ENABLE_EMERGENCY_STOP_MOTOR = 0x01,   // 执行紧急停机操作
 */
static void F_EmergencyStopMotor(E_DjiFlightControllerEmergencyStopMotor cmd, char debugMsg[EMERGENCY_STOP_MOTOR_MSG_MAX_LENGTH])
{
    T_DjiReturnCode returnCode = DjiFlightController_EmergencyStopMotor(cmd, debugMsg);
}

/**
 * 当无人机在地面时请求起飞
 */
static void F_StartTakeoff()
{
    T_DjiReturnCode returnCode = DjiFlightController_StartTakeoff();
}

/**
 * 当无人机在空中时请求降落
 */
static void F_StartLanding()
{
    T_DjiReturnCode returnCode = DjiFlightController_StartLanding();
}

/**
 * 当无人机正在降落时请求取消降落
 */
static void F_CancelLanding()
{
    T_DjiReturnCode returnCode = DjiFlightController_CancelLanding();
}

/**
 * 当无人机距离地面0.7米时确认着陆
 *      注: 当飞机与地面的距离小于0.7米时, 飞机会暂停降落并等待用户确认, 此 API 用于确认降落, 如果地面不适合降落, 用户必须使用遥控器手动控制其降落或强制降落
 */
static void F_StartConfirmLanding()
{
    T_DjiReturnCode returnCode = DjiFlightController_StartConfirmLanding();
}

/**
 * 在任何情况下都强制着陆
 *      注: 此API将忽略智能降落功能。使用此API，飞机将直接降落（不会在0.7米处停止等待用户指令）。注意：这可能导致飞机坠毁！!
 */
static void F_StartForceLanding()
{
    T_DjiReturnCode returnCode = DjiFlightController_StartForceLanding();
}

/**
 * 自定义 GPS (非 RTK) 的 home 点位置 - 设置
 *      注: 设置自定义起飞点失败的原因可能包括, 新的起飞点与上一个起飞点之间的距离超过 MAX_FLY_RADIUS(20km), 开机后记录初始起飞点失败
 *
 * T_DjiFlightControllerHomeLocation homeLocation: 包括纬度和经度的 home 点位置
 *      dji_f64_t latitude;
 *      dji_f64_t longitude;
 */
static void F_SetHomeLocationUsingGPSCoordinates_Set(T_DjiFlightControllerHomeLocation homeLocation)
{
    T_DjiReturnCode returnCode = DjiFlightController_SetHomeLocationUsingGPSCoordinates(homeLocation);
}

/**
 * 使用当前飞机的 GPS（非 RTK）位置设置 home 点位置 - 设置
 *      注: 设置起飞点失败的原因可能包括： 无人机的GPS等级未达到记录起飞点的条件。 开机后记录初始起飞点失败
 */
static void F_HomeLocationUsingCurrentAircraftLocation_Set()
{
    T_DjiReturnCode returnCode = DjiFlightController_SetHomeLocationUsingCurrentAircraftLocation();
}

/**
 * 返航高度 - 设置
 *      注: 如果无人机的当前高度高于返航高度的设置值，无人机将使用当前高度返航。否则，它将爬升到返航高度的设置，然后执行返航操作。返航高度设置为20m ~ 1500m
 *
 * E_DjiFlightControllerGoHomeAltitude altitude: //单位: 米,量程20~500
 */
static void F_GoHomeAltitude_Set(E_DjiFlightControllerGoHomeAltitude altitude)
{
    T_DjiReturnCode returnCode = DjiFlightController_SetGoHomeAltitude(altitude);
}

/**
 * 返航高度 - 获取
 */
static void F_GoHomeAltitude_Get()
{
    E_DjiFlightControllerGoHomeAltitude altitude;
    T_DjiReturnCode returnCode = DjiFlightController_GetGoHomeAltitude(&altitude);
}

/**
 * 获取国家码
 *
 * uint16_t countryCode: 指向缓冲区的指针, 用于返回国家代码, 国家代码表示飞行器当前所在的国家或地区, 具体的国家代码含义, 请参考 ISO 3166-1 代码表
 */
static void F_CountryCode_Get()
{
    uint16_t *countryCode;
    T_DjiReturnCode returnCode = DjiFlightController_GetCountryCode(&countryCode);
}

/**
 * 当无人机在空中时，请求返航动作
 */
static void F_StartGoHome()
{
    T_DjiReturnCode returnCode = DjiFlightController_StartGoHome();
}

/**
 * 在无人机返航时, 请求取消返航动作
 */
static void F_CancelGoHome()
{
    T_DjiReturnCode returnCode = DjiFlightController_CancelGoHome();
}

/**
 * 获取无人机的摇杆控制权限
 *      注: 使用摇杆控制无人机之前，必须成功获取摇杆控制权限，遥控器必须处于 P 档;
 *          在调用飞行器控制接口之前, 需要先调用 DjiFlightController_ObtainJoystickCtrlAuthority 接口来获取飞行器的控制权, 在飞行器控制权生效前, 飞行器控制命令接口将无法生效;
 */
static void F_ObtainJoystickCtrlAuthority()
{
    T_DjiReturnCode returnCode = DjiFlightController_ObtainJoystickCtrlAuthority();
}

/**
 * 释放无人机摇杆控制权限
 *      注: 遥控器必须处于 P 档
 */
static void F_ReleaseJoystickCtrlAuthority()
{
    T_DjiReturnCode returnCode = DjiFlightController_ReleaseJoystickCtrlAuthority();
}

/**
 * 使用回调函数订阅摇杆控制权限切换事件
 *      注: 当摇杆控制权限切换事件发生时, 将触发此函数
 */
static void F_RegJoystickCtrlAuthorityEventCallback()
{
    T_DjiReturnCode returnCode = DjiFlightController_RegJoystickCtrlAuthorityEventCallback(NULL);
}

/**
 * 请求执行摇杆动作
 *
 * T_DjiFlightControllerJoystickCommand
 *      dji_f32_t x;    // 对x轴的控制
 *      dji_f32_t y;    // 对y轴的控制
 *      dji_f32_t z;    // 对z轴的控制, 上为正
 *      dji_f32_t yaw;  // 偏航位置/速度控制, 相对于地面坐标系
 */
static void F_ExecuteJoystickAction(T_DjiFlightControllerJoystickCommand joystickCommand)
{
    T_DjiReturnCode returnCode = DjiFlightController_ExecuteJoystickAction(joystickCommand);
}

/**
 * 请求紧急制动动作
 */
static void F_ExecuteEmergencyBrakeAction()
{
    T_DjiReturnCode returnCode = DjiFlightController_ExecuteEmergencyBrakeAction();
}

/**
 * 请求取消紧急制动动作
 *      注: 仅在M30上支持。如果使用了 DjiFlightController_ExecuteEmergencyBrakeAction()，则需要使 DjiFlightController_CancelEmergencyBrakeAction() 以允许无人机再次执行动作
 */
static void F_CancelEmergencyBrakeAction()
{
    T_DjiReturnCode returnCode = DjiFlightController_CancelEmergencyBrakeAction();
}

/**
 * 获取飞机的通用信息
 *
 * T_DjiFlightControllerGeneralInfo // 存储含有用户给出的字符数组的序列号的结构体
 */
static void F_GetGeneralInfo()
{
    T_DjiFlightControllerGeneralInfo generalInfo;
    T_DjiReturnCode returnCode = DjiFlightController_GetGeneralInfo(&generalInfo);
}

/**
 * 该命令决定PSDK运行时是否执行RC丢失操作 - 设置
 *      注: 此设置仅在遥控器丢失且 OSDK 连接时影响无人机的行为。 如果命令为启用，当遥控器丢失但OSDK正在运行时，无人机不会执行遥控器丢失的操作； 如果命令为禁用，当遥控器丢失但OSDK正在运行时，无人机会执行遥控器丢失的操作； 当遥控器和OSDK都丢失时，无人机将执行遥控器丢失的操作，无论命令为何。 默认命令为禁用。
 */
static void F_RCLostActionEnableStatus_Set(E_DjiFlightControllerRCLostActionEnableStatus command)
{
    T_DjiReturnCode returnCode = DjiFlightController_SetRCLostActionEnableStatus(command);
}

/**
 *
 */
static void F_EnableRCLostActionStatus_Get()
{
    E_DjiFlightControllerRCLostActionEnableStatus command;
    T_DjiReturnCode returnCode = DjiFlightController_GetEnableRCLostActionStatus(&command);
}

/**
 * 注册回调函数触发 FTS 事件
 *      注: FTS 的触发时机由飞行器决定, FTS 的触发执行动作需要在回调函数中实现, 并且必须返回正确的返回值, 否则飞行器将会持续被触发
 */
static void F_RegTriggerFtsEventCallback()
{
    T_DjiReturnCode returnCode = DjiFlightController_RegTriggerFtsEventCallback(TriggerFtsEventCallback callback);
}

/* Exported functions definition ---------------------------------------------*/

T_DjiReturnCode F_Flight_Controller()
{
}