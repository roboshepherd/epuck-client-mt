
#include "ThreadMain.h"


// player client stuff
PlayerClient* pc;
Position2dProxy* p2d;
IrProxy* irp;
// device state
volatile RobotDevice::eState state;

// thread stuff
condition_variable link_cond, device_cond;
mutex link_mut, device_mut;
volatile bool link_busy;
volatile bool device_available;


boost::xtime xsecs(int secs)
{
    //Create an xtime that is secs seconds from now
    boost::xtime ret;
    ret.sec += secs;
    return ret;
}

// check e-puck state in every 30 sec
void check_device_state(EpuckPlayerClient *epuck)
{
  work:
  {

    boost::unique_lock<boost::mutex> lock(link_mut);

    while(link_busy) {
      link_cond.wait(lock);
      printf("%ld ++++++++++++: waiting for link to be ready\n", GetTimeInSecond());
    }

    state = epuck->GetClientState(pc);
    printf("%ld +++++++++++++: Robot state: %d \n", GetTimeInSecond(), (int )state);
    if (state == RobotDevice::AVAILABLE) {
      {
        boost::lock_guard<boost::mutex> lock(device_mut);
        device_available = true;
      }
      device_cond.notify_one();
    }

  }
  Sleep(3000);
  goto work;

}

void navigate_to_task()
{
  {
    boost::lock_guard<boost::mutex> lock(link_mut);
    link_busy = true;
  }
  link_cond.notify_one();

  printf("%ld $$$$$$$$$$$$$$: navigating ------xxxxx\n", GetTimeInSecond());
  Sleep(500);
   // set device link free
  {
    boost::lock_guard<boost::mutex> lock(link_mut);
    link_busy = false;
  }
  link_cond.notify_one();
   printf("%ld, XXXXXXXXXXXX:task ended ---------<<<<<\n", GetTimeInSecond());
}


void do_task()
{
work:
  {
      // TODO: navigate for a set time
      printf("%ld, XXXXXXXXXXX: checking device availability...\n", GetTimeInSecond());
      //sleep(2);
      boost::unique_lock<boost::mutex> lock(device_mut);
      while(!device_available) {
        device_cond.wait(lock);
        printf("%ld, XXXXXXXXXX: waiting for device to be available\n", GetTimeInSecond());
      }
      thread task_thread = thread(navigate_to_task);
      task_thread.detach();

      printf("%ld, XXXXXXXXXXXX:task started ---------<<<<<\n", GetTimeInSecond());

  }
  Sleep(1000);
  goto work;
}


int main (int argc, char ** argv) {

    EpuckPlayerClient epuck(argv[1], argv[2]);
    // init client stuff
    epuck.mClientID = argv[1];
    epuck.mClientPort = atoi(argv[2]);
    epuck.InitRobotDevice();
    epuck.InitShopTasks(MAXSHOPTASK);
    epuck.SetupStaticTaskLocations(MAXSHOPTASK, TASKS_CENTERS);
//    epuck.InitLogFiles();
//    epuck.LogExptConfig();


workloop:
    try
    {
        // create client
        PlayerClient client("localhost", epuck.mClientPort);
        epuck.InitClient(&client);
        // create devices
        Position2dProxy pose2d(&client);
        IrProxy ir(&client);
        // player pointers
        pc = &client;
        p2d = &pose2d;
        irp = &ir;

        // init
        device_available = false;
        link_busy = false;

        thread thread_1 = thread(check_device_state, &epuck);
        thread thread_2 = thread(do_task);

        for(;;){
          // Sense device state
//          state =  epuck.GetClientState(pc);
//          // Act
//          epuck.TriggerStateAction(pc, p2d, irp, state);
//
//          // Wait a bit
//          Sleep(STEP_TIME);
        }

          thread_1.join();
          thread_2.join();

    } catch (PlayerError pe) {
        std::cerr << pe << std::endl;
        epuck.mRobotDevice.mStateStep++;
        goto workloop;
    }



    return 0;
}
