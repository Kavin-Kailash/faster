#ifndef RP_HPP_
#define RP_HPP_

#define SCREEN_PRINT_RATE	0.5
#define PI 3.14159

// ROS includes
#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include "geometry_msgs/PoseStamped.h"
#include "geometry_msgs/PointStamped.h"
#include "geometry_msgs/PoseArray.h"
#include "geometry_msgs/TwistStamped.h"
#include "geometry_msgs/Vector3Stamped.h"
#include "tf/transform_datatypes.h"
#include "nav_msgs/Path.h"

#include <Eigen/Dense>

// custom messages
#include "acl_system/ViconState.h"
#include "acl_system/QuadGoal.h"
#include "acl_system/QuadState.h"
#include "acl_system/QuadFlightEvent.h"

// Global includes
#include <stdio.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <queue>

class REACT
{
public:
	REACT();

	ros::Publisher goal_pub, new_goal_pub, int_goal_pub, last_goal_pub, quad_goal_pub, pub_clean_scan;

	void scanCB(const sensor_msgs::LaserScan& msg);
	void stateCB(const acl_system::ViconState& msg);
	void global_goalCB(const geometry_msgs::PointStamped& msg);

	// void partition_scan(const sensor_msgs::LaserScan& msg);
	void vis_better_scan(const sensor_msgs::LaserScan& msg);

	void eventCB(const acl_system::QuadFlightEvent& msg);

	// ROS timed functions
	void sendGoal(const ros::TimerEvent&);

	void convert2ROS(Eigen::MatrixXd Goals);
	void pubROS();

	// Make these private after testing
	void sort_clusters(Eigen::MatrixXd& Sorted_Goals, Eigen::Vector3d last_goal, Eigen::MatrixXd Goals,  Eigen::Vector3d pose, Eigen::Vector3d goal);
	void partition_scan(Eigen::MatrixXd scan, Eigen::MatrixXd& Goals,int& partition, Eigen::Vector3d pose, Eigen::Vector3d goal);
	void find_times(std::vector<double>& t, Eigen::Matrix4d& X0, Eigen::Vector3d x, double vf);
	void eval_trajectory(Eigen::MatrixXd& Xc, Eigen::Matrix4d X0, Eigen::Matrix4d Y0, std::vector<double> t_x, std::vector<double> t_y, double t);
	void collision_check2(Eigen::MatrixXd X, std::vector<double> scan, Eigen::Vector3d goal, double buff, double v, bool& can_reach_goal);
	void convert_scan(sensor_msgs::LaserScan msg, Eigen::MatrixXd& scanE , std::vector<double>& scanV );
	void collision_check(Eigen::MatrixXd X, Eigen::MatrixXd scan, Eigen::Vector3d goal, double buff, double v, bool& can_reach_goal);
	void pick_cluster( Eigen::MatrixXd Sorted_Goals_, Eigen::Vector3d& local_goal_, Eigen::MatrixXd Xc);
	void saturate(double &var, double min, double max);
	

private:

	double inf_; // Infinity definition
	double thresh_, yaw_, dist_2_goal_, angle_2_goal_, angle_check_, msg_received_, cost_, cost_i_, angle_diff_, safe_distance_, min_cost_, buffer_;
	double num_samples_, angle_max_, angle_min_, angle_increment_;
	double angle_diff_last_, angle_seg_inc_;
	bool debug_, can_reach_goal_, corridor_free_;
	int down_sample_, num_of_clusters_, goal_index_, collision_counter_, collision_counter_corridor_;

	double spinup_time_, heading_, j_max_, a_max_, t0_;
	int quad_status_;
	acl_system::QuadState state_;
	acl_system::QuadFlightEvent quad_event_;


	std::ostringstream errorMsg, warnMsg;

	geometry_msgs::PoseArray goal_points_ros_ ;
	geometry_msgs::Pose temp_goal_point_ros_;


	// // // // //

	acl_system::QuadGoal quad_goal_;

	// Weird initialization
	std::vector<double> t_x_{std::vector<double>(3,0)};
	std::vector<double> t_y_{std::vector<double>(3,0)}; 

	std::vector<double> x0_{std::vector<double>(4,0)};
	std::vector<double> v0_{std::vector<double>(4,0)};
	std::vector<double> a0_{std::vector<double>(4,0)};
	// Note the last entry is always zero
	std::vector<double> j_{std::vector<double>(4,0)};

	std::vector<double> scanV_;
	std::vector<double> cost_v_;
	std::vector<double>::iterator it_;

	std::priority_queue<double, std::vector<double>, std::greater<double> > cost_queue_;


	Eigen::Matrix4d X0_; // [x0; v0; a0; j0]
	Eigen::Matrix4d Y0_;
	Eigen::Matrix4d X_;

	Eigen::MatrixXd Xc_; 
	Eigen::MatrixXd scanE_;
	Eigen::MatrixXd Goals_;
	Eigen::MatrixXd Sorted_Goals_;

	Eigen::Vector3d x_;
	Eigen::Vector3d y_;
	Eigen::Vector3d goal_;
	Eigen::Vector3d local_goal_;
	Eigen::Vector3d last_goal_;
	Eigen::Vector3d last_goal_V_;
	Eigen::Vector3d next_goal_V_;
	Eigen::Vector3d pose_;



	double vfx_, vfy_, t_, dt_, T_, r_, theta_, d_theta_, d_min_, tx_, ty_;
	double theta_1_, theta_2_;
	int index1_, index2_, partition_ ;
	double max_angle_, min_angle_, d_angle_;

	double r_i_, angle_i_, r_goal_;

	//## Logging and Debugging Functions

	void takeoff();
	void land();
	
};

#endif /* RP_HPP_ */