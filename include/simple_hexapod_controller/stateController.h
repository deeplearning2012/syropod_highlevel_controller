
#ifndef SIMPLE_HEXAPOD_CONTROLLER_STATE_CONTROLLER_H
#define SIMPLE_HEXAPOD_CONTROLLER_STATE_CONTROLLER_H
/** 
 *  \file    state_controller.h
 *  \brief   Top level controller that handles state of hexapod. Part of simple hexapod controller.
 *
 *  \author Fletcher Talbot
 *  \date   January 2017
 *  \version 0.5.0
 *
 *  CSIRO Autonomous Systems Laboratory
 *  Queensland Centre for Advanced Technologies
 *  PO Box 883, Kenmore, QLD 4069, Australia
 *
 *  (c) Copyright CSIRO 2017
 *
 *  All rights reserved, no part of this program may be used
 *  without explicit permission of CSIRO
 *
 */
#include "standardIncludes.h"
#include "parametersAndStates.h"

#include "walkController.h"
#include "poseController.h"
#include "model.h"

#include "debugOutput.h"
#include "dynamixelMotorInterface.h"
#include "impedanceController.h"

//#include "std_msgs/Bool.h"
//#include "std_msgs/Int8.h"
//#include "std_msgs/Float32MultiArray.h"
//#include "geometry_msgs/Twist.h"
//#include "geometry_msgs/Vector3.h"
//#include "geometry_msgs/Point.h"
//#include "sensor_msgs/JointState.h"
//#include "sensor_msgs/Joy.h"
//#include "sensor_msgs/Imu.h"
//#include "simple_hexapod_controller/legState.h"

#define MAX_MANUAL_LEGS 2

class StateController
{
public:  
  StateController(ros::NodeHandle nodeHandle);
  ~StateController();
  
  inline Parameters* getParameters(void) { return &params_; };
  inline bool getUserInputFlag(void) { return user_input_flag_; }
  
  inline bool receivingImuData(void) { return imu_data_subscriber_; };
  inline bool receivingTipForces(void) { return tip_force_subscriber_; };
  inline bool receivingJointStates(void) { return joint_state_subscriber_; };
  inline bool areJointPostionsInitialised(void) { return joint_positions_initialised; };
  
  // Initialisation functions  
  void init(void);
  void initParameters(void);
  void initGaitParameters(GaitDesignation gaitSelection);  
  void initModel(bool use_default_joint_positions = false);
  void resetDebug(void) { debug_.reset(); };

  // Loop and state functions
  void loop();
  void impedanceControl();
  void transitionSystemState();
  void runningState();
  void adjustParameter();
  void changeGait();
  void legStateToggle();
  void publishDesiredJointState();  
  
  // Debugging functions  
  void publishLegState();
  void publishBodyVelocity();
  void publishPose();
  void publishIMUData();  
  void publishRotationPoseError();
  void publishTranslationPoseError();
  void RVIZDebugging(bool static_display = false);

  // Callbacks
  void bodyVelocityInputCallback(const geometry_msgs::Twist &input);
  void primaryTipVelocityInputCallback(const geometry_msgs::Point &input);
  void secondaryTipVelocityInputCallback(const geometry_msgs::Point &input);
  void bodyPoseInputCallback(const geometry_msgs::Twist &input);
  void systemStateCallback(const std_msgs::Int8 &input);
  void gaitSelectionCallback(const std_msgs::Int8 &input);
  void posingModeCallback(const std_msgs::Int8 &input);
  void cruiseControlCallback(const std_msgs::Int8 &input);
  void autoNavigationCallback(const std_msgs::Int8 &input);
  void parameterSelectionCallback(const std_msgs::Int8 &input);
  void parameterAdjustCallback(const std_msgs::Int8 &input);
  void primaryLegSelectionCallback(const std_msgs::Int8 &input);
  void secondaryLegSelectionCallback(const std_msgs::Int8 &input);
  void primaryLegStateCallback(const std_msgs::Int8 &input);
  void secondaryLegStateCallback(const std_msgs::Int8 &input);
  void poseResetCallback(const std_msgs::Int8 &input);
  void imuCallback(const sensor_msgs::Imu &data);
  void jointStatesCallback(const sensor_msgs::JointState &joint_states);
  void tipForceCallback(const sensor_msgs::JointState &raw_tip_forces); 
  
private:
  ros::NodeHandle n_;
  ros::Subscriber desired_velocity_subscriber_;
  ros::Subscriber primary_tip_velocity_subscriber_;
  ros::Subscriber secondary_tip_velocity_subscriber_;
  ros::Subscriber desired_pose_subscriber_;
  ros::Subscriber system_state_subscriber_;
  ros::Subscriber gait_selection_subscriber_;
  ros::Subscriber posing_mode_subscriber_;
  ros::Subscriber cruise_control_mode_subscriber_;
  ros::Subscriber auto_navigation_mode_subscriber_;
  ros::Subscriber parameter_selection_subscriber_;
  ros::Subscriber parameter_adjustment_subscriber_;
  ros::Subscriber primary_leg_selection_subscriber_;
  ros::Subscriber primary_leg_state_subscriber_;
  ros::Subscriber secondary_leg_selection_subscriber_;
  ros::Subscriber secondary_leg_state_subscriber_;
  ros::Subscriber pose_reset_mode_subscriber_;
  ros::Subscriber imu_data_subscriber_;
  ros::Subscriber tip_force_subscriber_;
  ros::Subscriber joint_state_subscriber_;
  ros::Publisher pose_publisher_;
  ros::Publisher imu_data_publisher_;
  ros::Publisher body_velocity_publisher_;
  ros::Publisher rotation_pose_error_publisher_;
  ros::Publisher translation_pose_error_publisher_;
  
  Model* model_;
  DynamixelMotorInterface* interface_;
  WalkController* walker_;
  PoseController* poser_;
  ImpedanceController* impedance_;
  DebugOutput debug_;
  Parameters params_;

  SystemState system_state_ = WAITING_FOR_USER;
  SystemState new_system_state_ = WAITING_FOR_USER;

  GaitDesignation gait_selection_ = GAIT_UNDESIGNATED;
  PosingMode posing_mode_ = NO_POSING;
  CruiseControlMode cruise_control_mode_ = CRUISE_CONTROL_OFF;
  AutoNavigationMode auto_navigation_mode_ = AUTO_NAVIGATION_OFF;

  ParameterSelection parameter_selection_ = NO_PARAMETER_SELECTION;
  AdjustableParameter* parameter_being_adjusted_;

  LegDesignation primary_leg_selection_ = LEG_UNDESIGNATED;
  LegDesignation secondary_leg_selection_ = LEG_UNDESIGNATED;
  LegState primary_leg_state_ = WALKING;
  LegState secondary_leg_state_ = WALKING;
  Leg* primary_leg_;
  Leg* secondary_leg_;     
  
  int manual_leg_count_ = 0;

  bool user_input_flag_ = false;
  bool gait_change_flag_ = false;
  bool toggle_primary_leg_state_ = false;
  bool toggle_secondary_leg_state_ = false;
  bool parameter_adjust_flag_ = false;
  bool new_parameter_set_ = false;
  bool joint_positions_initialised = false;
  bool transition_state_flag_ = false;  

  Vector2d linear_velocity_input_;
  double angular_velocity_input_ = 0;
  Vector3d primary_tip_velocity_input_;
  Vector3d secondary_tip_velocity_input_;
  Vector2d linear_cruise_velocity_;
  double angular_cruise_velocity_; 

  std::map<int, Leg*>::iterator leg_it_;  
};

#endif /* SIMPLE_HEXAPOD_CONTROLLER_STATE_CONTROLLER_H */