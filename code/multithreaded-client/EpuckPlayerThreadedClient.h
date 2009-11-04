#ifndef Header_EpuckPlayerThreadedClient
#define  EpuckPlayerThreadedClient

#include <string>
#include <cstdlib>
#include <iostream>

#include <libplayerc++/playerc++.h>
using namespace PlayerCc;

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
using namespace boost::interprocess;

#include <boost/thread.hpp>
using namespace boost;


#include "EpuckPlayerClient.h"
#include "RobotDevice.h"
#include "SHMConfig.h"
#include "Sleep.h"
#include "EpuckNavigator.h"

#define DEFAULT_PLAYER_PORT 6600

class ThreadedClient
{
  public:
    ThreadedClient(char* robotid, char* serverport,
    PlayerClient *client, Position2dProxy *p2d, IrProxy *irp):
      mStopRequested(false), mLinkReady(false),
      mDeviceAvailable(false), mDoingTask(false),
      mEpuck(robotid, serverport)
    {
        InitThreadedClient(robotid, DEFAULT_PLAYER_PORT);
        RunWorkLoop(client, p2d, irp);
    }

    ~ThreadedClient()
    {
      mStopRequested = true;
      mTaskManagerThread.join();
      mStateUpdaterThread.join();
    }

    void InitThreadedClient(char* id, int port)
    {
          // init client stuff
    mEpuck.mClientID = id;
    mEpuck.mClientPort = port;
    mEpuck.InitRobotDevice();
    mEpuck.InitShopTasks(MAXSHOPTASK);
    mEpuck.SetupStaticTaskLocations(MAXSHOPTASK, TASKS_CENTERS);
    // mEpuck.InitLogFiles();
    // mEpuck.LogExptConfig();
    }
    void RunWorkLoop(PlayerClient *client, Position2dProxy *p2d,\
      IrProxy *irp)
    {
      try
      {
        // create player client
        PlayerClient pc("localhost", mEpuck.mClientPort);
        mEpuck.InitClient(&pc);
        // create devices
        Position2dProxy pose2d(&pc);
        IrProxy ir(&pc);
        // player pointers
        client = &pc;
        p2d = &pose2d;
        irp = &ir;

        mStateUpdaterThread = new boost::thread(boost::bind(&ThreadedClient::UpdateState, this));
        mTaskManagerThread = boost::bind(&ThreadedClient::ManageTask, this);

        for(;;){
          // Wait a bit
      //          Sleep(STEP_TIME);
        }

      } catch (PlayerError pe) {
        std::cerr << pe << std::endl;
        //mEpuck.mRobotDevice.mStateStep++;
      }
    }


  private:
    bool mStopRequested; //  graceful stop
    bool mLinkReady; // to access e-puck
    bool mDeviceAvailable; // current state of e-puck
    bool mDoingTask; // to manage a fire-n-forget type task worker thread

    EpuckPlayerClient mEpuck; // single thread client

    boost::mutex mLinkMutex, mDeviceMutex, mTaskMutex;
    boost::thread mStateUpdaterThread, mTaskManagerThread;

    void UpdateState();
    void ManageTask();
    void DoTask();

};

#endif
