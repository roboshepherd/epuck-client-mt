#include "EpuckPlayerThreadedClient.h"

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>

//void EpuckPlayerThreadedClient::InitClient(const char* id,  const char* port)
//{
//    // init client stuff
//    mEpuck.mClientID = id;
//    mEpuck.mClientPort = port;
//    mEpuck.InitRobotDevice();
//    mEpuck.InitShopTasks(MAXSHOPTASK);
//    mEpuck.SetupStaticTaskLocations(MAXSHOPTASK, TASKS_CENTERS);
//    // mEpuck.InitLogFiles();
//    // mEpuck.LogExptConfig();
//}

//void EpuckPlayerThreadedClient::RunWorkLoop(PlayerClient *client, Position2dProxy *p2d,\
// IrProxy *irp)
//{
//  try
//    {
//        // create player client
//        PlayerClient client("localhost", epuck.mClientPort);
//        epuck.InitClient(&client);
//        // create devices
//        Position2dProxy pose2d(&client);
//        IrProxy ir(&client);
//        // player pointers
//        pc = &client;
//        p2d = &pose2d;
//        irp = &ir;
//
//        mStateUpdaterThread = boost::bind(&ThreadedClient::UpdateState, this);
//        mTaskManagerThread = boost::bind(&ThreadedClient::ManageTask, this);
//
//        for(;;){
//          // Wait a bit
////          Sleep(STEP_TIME);
//        }
//
//    } catch (PlayerError pe) {
//        std::cerr << pe << std::endl;
//        //mEpuck.mRobotDevice.mStateStep++;
//    }
//}
