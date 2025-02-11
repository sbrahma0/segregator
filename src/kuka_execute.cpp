#include<ros/ros.h>
#include<geometry_msgs/Twist.h>
#include<geometry_msgs/PoseStamped.h>
#include<sensor_msgs/JointState.h>
#include<trajectory_msgs/JointTrajectory.h>
#include<trajectory_msgs/JointTrajectoryPoint.h>
// #include<kdl/chain.hpp>
// #include<std_msgs/UInt8.h>
// #include<std_msgs/Bool.h>
// #include<std_msgs/Float64.h>
// #include<std_msgs/Int16.h>
// #include<tf/transform_listener.h>
// #include<tf/tfMessage.h>
#include <ros/console.h>
// #include<fstream>
using namespace std;
///////// INPUT DATA ///////////
const int number_of_points=27;

double position_joints[number_of_points][7]={
														 {0.11304, -0.148999 ,-0.0240472, -1.50173, -0.0803639 ,1.78921, 0.709907},
						 							 	 {0.841234, 0.156756, -0.0240041, -1.59303, -0.331495, 1.7242, 1.45576},
														 {0.94299, 0.361689, -0.0241217, -1.565, -0.381891, 1.72069, 1.59206},
														 {1.01621, 0.562788, -0.0240305, -1.44739, -0.389202, 1.75551, 1.6758},
														 {1.12011, 0.945067, -0.0239639 ,-0.992652, -0.43529 ,2.05422 ,1.69758},
														 {1.12523 ,1.08735 ,-0.0239481 ,-0.984023 ,-0.464947 ,2.04897 ,1.73575},
														 {1.12734 ,1.21338 ,-0.0239469 ,-0.940171 ,-0.477568 ,2.04324 ,1.76287},
														 {1.12839 ,1.29962 ,-0.0239418 ,-0.891568 ,-0.489425 ,2.05656 ,1.77248},
														 {-0.547112 ,-0.0364138 ,-0.0240231 ,-1.61642 ,0.143479 ,1.64779 ,0.072405},
														 {0-0.723117 ,0.107328 ,-0.0240372 ,-1.57788 ,0.235416 ,1.66626 ,-0.110447},
														 {-0.802335 ,0.196745 ,-0.024021 ,-1.58381 ,0.316252 ,1.64804 ,-0.0941228},
														 {-0.9361 ,0.490167 ,-0.0241308 ,-1.47198 ,0.390213 ,1.68363 ,-0.258662},
														 {-1.05596 ,0.856617 ,-0.0240937 ,-1.152 ,0.426323 ,1.85255 ,-0.351709},
														 {-1.06665 ,1.01331 ,-0.0240794 ,-1.12253 ,0.491639 ,1.91097 ,-0.384245},
														 {-1.0735 ,1.23486 ,-0.0240799 ,-1.03048 ,0.533087 ,1.93605 ,-0.426374},
														 {0.0830938 ,0.132447 ,-0.0241137 ,-1.22266 ,-0.0733511 ,1.78476 ,0.68156},
														 {0.487685 ,0.254231 ,-0.0241097 ,-1.31378 ,-0.16789 ,1.67193 ,1.09351},
														 {0.644638 ,0.392561 ,-0.0241106 ,-1.27402 ,-0.296526 ,1.70351 ,1.25495},
														 {0.845218 ,0.696448 ,-0.0241012 ,-1.0184 ,-0.444223 ,1.84389 ,1.43824},
														 {0.88336 ,0.870323 ,-0.0240998 ,-1.04884 ,-0.504423 ,1.79146 ,1.53713},
														 {0.937387 ,1.15576 ,-0.0241004 ,-0.829932 ,-0.576727 ,1.90936 ,1.57824},
														 {0.934635 ,1.39481 ,-0.024099 ,-0.748035 ,-0.78711 ,2.09259 ,1.56194},
														 {-0.560011 ,0.315765 ,-0.0241222 ,-1.27585 ,0.265554 ,1.66056 ,0.0925606},
														 {-0.674904 ,0.472105 ,-0.024119 ,-1.34369 ,0.343034 ,1.55141 ,-0.0737514},
														 {-0.844908 ,0.794037 ,-0.0241172 ,-1.0456 ,0.453951 ,1.7557 ,-0.209802},
														 {-0.908478 ,1.15845 ,-0.0241177 ,-0.879889 ,0.600354 ,1.86066 ,-0.301069},
														 {-0.926267 ,1.42551 ,-0.0241183 ,-0.685301 ,0.795119 ,2.09248 ,-0.255848}

													 };


////////////////// Hand_eye_calcualtor ////////////
double E_base_ee[number_of_points][7];
double E_cam_marker[number_of_points][7];



/////////// SUBSCRIBERS ///////////////////////
//Get Pose of IIWA
geometry_msgs::PoseStamped pos;
void get_pos(const geometry_msgs::PoseStamped & _data)
{
	pos = _data;

}

///////////// MAIN /////////
int main(int argc, char * argv[])
{
	ros::init(argc, argv, "eye_in_hand_calib");
	ros::NodeHandle nh;
	ros::Rate loop_rate(.5);
	//Define Subscriber to Read IIWA Pose
    auto pos_sub = nh.subscribe("/iiwa/state/CartesianPose",.10, get_pos);
	//Define IIWA Joint Publisher
	std::string command_topic = "/iiwa/PositionJointInterface_trajectory_controller/command";
	auto cmd_pub = nh.advertise<trajectory_msgs::JointTrajectory>(command_topic,.10);
	//Define Listener
	// tf::TransformListener listener;
	// tf::StampedTransform transform;
	// tf::Transform inv_transform;
//Define Joint Message
	trajectory_msgs::JointTrajectory msg5;
	msg5.header.seq = 0;
	msg5.header.stamp.sec = 0;
	msg5.header.stamp.nsec = 0;
	msg5.header.frame_id = "";
	msg5.joint_names.push_back("iiwa_joint_1");
	msg5.joint_names.push_back("iiwa_joint_2");
	msg5.joint_names.push_back("iiwa_joint_3");
	msg5.joint_names.push_back("iiwa_joint_4");
	msg5.joint_names.push_back("iiwa_joint_5");
	msg5.joint_names.push_back("iiwa_joint_6");
	msg5.joint_names.push_back("iiwa_joint_7");
	msg5.points.resize(1);
	int ind = 0;
	msg5.points[ind].velocities.resize(7);
	msg5.points[ind].effort.resize(7);
	 for (size_t j = 0; j < 7; ++j) {
	 	msg5.points[ind].velocities[j]=0.0;
	 	msg5.points[ind].effort[j] = 0.0;
	 }
	msg5.points[ind].time_from_start = ros::Duration(1.0);
	while(ros::ok())
	{
		loop_rate.sleep();
		for(int i=0;i<number_of_points;i++)
		{
			msg5.points[ind].positions.resize(7);
			msg5.points[ind].positions[0] = position_joints[i][0];
			msg5.points[ind].positions[1] = position_joints[i][1];
			msg5.points[ind].positions[2] = position_joints[i][2];
			msg5.points[ind].positions[3] = position_joints[i][3];
			msg5.points[ind].positions[4] = position_joints[i][4];
			msg5.points[ind].positions[5] = position_joints[i][5];
			msg5.points[ind].positions[6] = position_joints[i][6];
	    ROS_INFO_STREAM("Going to Scan Position: " <<i+1);
			cmd_pub.publish(msg5);
		    ros::spinOnce();
			loop_rate.sleep();
			ros::spinOnce();
			loop_rate.sleep();


		}
		ros::spinOnce();
		loop_rate.sleep();
		ros::shutdown();
	}
	return 0;
}
