/**
 * ********************************************************************
 * Created by WangYunwei [2024-10-14] 云台管理
 * ********************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "ms_gimbal_manage.h"

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
static E_DjiMountPosition mountPosition = DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1; // 云台挂载点

/* Private variables ---------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/
/**
 * 初始化云台管理模块
 *      注:  请在 DjiCore_Init 之后初始化此接口
 */
static void F_Init()
{
    T_DjiReturnCode returnCode = DjiGimbalManager_Init();
}

/**
 * 反初始化云台管理模块
 */
static void F_DeInit()
{
    T_DjiReturnCode returnCode = DjiGimbalManager_Deinit();
}

/**
 * 云台工作模式 - 设置
 *
 * E_DjiMountPosition
 *      DJI_MOUNT_POSITION_UNKNOWN = 0,
 *      DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1 = 1,
 *      DJI_MOUNT_POSITION_PAYLOAD_PORT_NO2 = 2,
 *      DJI_MOUNT_POSITION_PAYLOAD_PORT_NO3 = 3,
 *      DJI_MOUNT_POSITION_EXTENSION_PORT = 4,
 *      DJI_MOUNT_POSITION_EXTENSION_LITE_PORT = 5,
 *
 * E_DjiGimbalMode
 *      DJI_GIMBAL_MODE_FREE = 0,   // 自由模式, 在地面坐标系固定云台姿态, 忽略飞行器的运动
 *      DJI_GIMBAL_MODE_FPV = 1,    // FPV(第一人称视角)模式, 仅控制云台在地面坐标中的横滚和偏航角跟随飞行器
 *      DJI_GIMBAL_MODE_YAW_FOLLOW = 2, // 偏航跟随模式, 只控制云台在地面坐标的偏航角跟随飞行器
 */
static void F_Mode_Set(E_DjiGimbalMode mode)
{
    T_DjiReturnCode returnCode = DjiGimbalManager_SetMode(mountPosition, mode);
}

/**
 * 重置云台的俯仰和偏航角
 *
 * E_DjiGimbalResetMode
 *      DJI_GIMBAL_RESET_MODE_YAW = 1,  // 航向轴复位: 将云台航向轴的角度复位为无人机航向轴角度与云台航向轴微调角度的和
 *      DJI_GIMBAL_RESET_MODE_PITCH_AND_YAW = 3,    // 俯仰轴与航向轴复位: 将云台俯仰轴的角度复位为微调的角度, 将云台航向轴的角度复位为无人机航向轴角度与云台航向轴微调角度的和
 *      DJI_GIMBAL_RESET_MODE_PITCH_DOWNWARD_UPWARD_AND_YAW = 11,   // 重置云台的偏航轴和俯仰轴, 将偏航轴的角度重置为无人机偏航轴和微调角度的和重置俯仰轴为-90°与微调角度的和 (云台下置), 90°与微调角度的和 (云台上置)
 *      DJI_GIMBAL_RESET_MODE_PITCH_DOWNWARD_UPWARD = 12,   // 重置云台的偏航轴为-90°与微调角度的和 (云台下置), 90°与微调角度的和 (云台上置)
 */
static void F_Reset(E_DjiGimbalResetMode resetMode)
{
    T_DjiReturnCode returnCode = DjiGimbalManager_Reset(mountPosition, resetMode);
}

/**
 * 旋转云台角度
 *
 * T_DjiGimbalManagerRotation
 *      E_DjiGimbalRotationMode rotationMode;   // 云台旋转模式
 *          DJI_GIMBAL_ROTATION_MODE_RELATIVE_ANGLE = 0,    // 相对角度旋转模式, 表示根据当前角度旋转云台指定角度
 *          DJI_GIMBAL_ROTATION_MODE_ABSOLUTE_ANGLE = 1,    // 绝对角度旋转模式, 表示将云台旋转到地面坐标中的指定角度
 *          DJI_GIMBAL_ROTATION_MODE_SPEED = 2, // 速度旋转模式, 指定云台在地面坐标下的旋转速度
 *      dji_f32_t pitch;    // 俯仰角, 单位: 度
 *      dji_f32_t roll;     // 翻滚角, 单位: 度
 *      dji_f32_t yaw;      // 偏航角, 单位: 度
 *      dji_f64_t time;     // 预计执行时间, 单位: 秒
 */
static void F_Rotate(T_DjiGimbalManagerRotation rotation)
{
    T_DjiReturnCode returnCode = DjiGimbalManager_Rotate(mountPosition, rotation);
}

/**
 * 启用或禁用云台俯仰限位扩展 - 设置
 *      注: 云台的俯仰轴欧拉角限制切换为扩展限制角度或默认限制角度
 */
static void F_PitchRangeExtensionEnabled_Set(bool enabledFlag)
{
    T_DjiReturnCode returnCode = DjiGimbalManager_SetPitchRangeExtensionEnabled(mountPosition, enabledFlag);
}

/**
 * 云台控制器的最大速度百分比 - 设置
 *
 * uint8_t maxSpeedPercentage   // 最大速度值, 建议计算公式为 "spd = default_max_spd * x / 100", x 为默认最大速度值, 范围从 1 到 100
 * E_DjiGimbalAxis axis // 要设置的轴
 *      DJI_GIMBAL_AXIS_PITCH = 0,  // 俯仰轴
 *      DJI_GIMBAL_AXIS_ROLL = 1,   // 横滚轴
 *      DJI_GIMBAL_AXIS_YAW = 2,    // 偏航轴
 */
static void F_ControllerMaxSpeedPercentage_Set(E_DjiGimbalAxis axis, uint8_t maxSpeedPercentage)
{
    T_DjiReturnCode returnCode = DjiGimbalManager_SetControllerMaxSpeedPercentage(mountPosition, axis, maxSpeedPercentage);
}

/**
 * 云台控制器的平滑因子 - 设置
 *
 * uint8_t smoothingFactor: // 平滑因子, 值越大云台转动加速度越小, 建议计算公式为 "acc = 10000 * (0.8 ^ (1 + x)) deg/s^2", x 为平滑因子, 范围从 1 到 30
 * E_DjiGimbalAxis axis: // 要设置的轴, 同 F_ControllerMaxSpeedPercentage_Set()
 */
static void F_ControllerSmoothFactor_Set(E_DjiGimbalAxis axis, uint8_t smoothingFactor)
{
    T_DjiReturnCode returnCode = DjiGimbalManager_SetControllerSmoothFactor(mountPosition, axis, smoothingFactor);
}

/**
 * 恢复云台的出厂设置
 */
static void F_RestoreFactorySettings()
{
    T_DjiReturnCode returnCode = DjiGimbalManager_RestoreFactorySettings(mountPosition);
}

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode F_Gimbal_Manage()
{
}