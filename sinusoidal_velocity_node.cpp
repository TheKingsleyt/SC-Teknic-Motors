#include <ros/ros.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

// Teknic SC SDK
#undef NET_CONTROLLER_MAX
#define NET_CONTROLLER_MAX 10
#include "pubSysCls.h"

using namespace sFnd;

#define TIME_TILL_TIMEOUT      10000
#define PI                     3.141592653589793
#define AMPLITUDE_CNTS         10000
#define FREQUENCY_HZ           0.5
#define ACC_LIM_RPM_PER_SEC    100000
#define VEL_LIM_RPM            400

int main(int argc, char** argv) {
    // Initialize ROS
    ros::init(argc, argv, "sinusoidal_velocity_node");
    ros::NodeHandle nh;
    ros::Rate loop_rate(50); // 50 Hz loop

    ROS_INFO("Starting Teknic sinusoidal velocity motion...");

    SysManager* myMgr = SysManager::Instance();
    std::vector<std::string> comHubPorts;
    size_t portCount = 0;

    try {
        SysManager::FindComHubPorts(comHubPorts);
        if (comHubPorts.empty()) {
            ROS_ERROR("No SC Hubs found.");
            return -1;
        }

        for (portCount = 0; portCount < comHubPorts.size() && portCount < NET_CONTROLLER_MAX; portCount++) {
            myMgr->ComHubPort(portCount, comHubPorts[portCount].c_str());
        }

        myMgr->PortsOpen(portCount);

        for (size_t i = 0; i < portCount; i++) {
            IPort &myPort = myMgr->Ports(i);
            for (size_t iNode = 0; iNode < myPort.NodeCount(); iNode++) {
                INode &theNode = myPort.Nodes(iNode);

                theNode.EnableReq(false);
                myMgr->Delay(200);

                theNode.Status.AlertsClear();
                theNode.Motion.NodeStopClear();
                theNode.EnableReq(true);

                double timeout = myMgr->TimeStampMsec() + TIME_TILL_TIMEOUT;
                while (!theNode.Motion.IsReady()) {
                    if (myMgr->TimeStampMsec() > timeout) {
                        ROS_ERROR("Node %zu failed to enable.", iNode);
                        return -2;
                    }
                }

                // Homing if needed
                if (theNode.Motion.Homing.HomingValid() && !theNode.Motion.Homing.WasHomed()) {
                    theNode.Motion.Homing.Initiate();
                    timeout = myMgr->TimeStampMsec() + TIME_TILL_TIMEOUT;
                    while (!theNode.Motion.Homing.WasHomed()) {
                        if (myMgr->TimeStampMsec() > timeout) {
                            ROS_ERROR("Node %zu homing timed out.", iNode);
                            return -3;
                        }
                    }
                    ROS_INFO("Node %zu homed successfully.", iNode);
                }

                // Setup motion parameters
                theNode.VelUnit(INode::RPM);
                theNode.AccUnit(INode::RPM_PER_SEC);
                theNode.Motion.AccLimit = ACC_LIM_RPM_PER_SEC;
                theNode.Motion.VelLimit = VEL_LIM_RPM;

                // CSV log file
                std::ofstream logFile("velocity_log.csv");
                logFile << "Time(s),Velocity(RPM)\n";

                double timeStart = myMgr->TimeStampMsec();

                while (ros::ok()) {
                    double t_sec = (myMgr->TimeStampMsec() - timeStart) / 1000.0;
                    double velocity = VEL_LIM_RPM * cos(2 * PI * FREQUENCY_HZ * t_sec);

                    theNode.Motion.MoveVelStart(velocity);
                    logFile << t_sec << "," << velocity << "\n";

                    ros::spinOnce();
                    loop_rate.sleep();
                }

                // On shutdown
                theNode.Motion.MoveVelStart(0);
                theNode.EnableReq(false);
                logFile.close();
                ROS_INFO("Node %zu motion stopped and log saved.", iNode);
            }
        }

        myMgr->PortsClose();
        ROS_INFO("All ports closed.");
    } catch (mnErr& theErr) {
        ROS_ERROR("Caught Teknic Error: addr=%d, code=0x%08x\n%s",
            theErr.TheAddr, theErr.ErrorCode, theErr.ErrorMsg);
        myMgr->PortsClose();
        return -4;
    }

    return 0;
}
