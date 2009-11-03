
#include "ThreadMain.h"


// player client stuff
PlayerClient* pc;
Position2dProxy* p2d;
IrProxy* irp;
// device state
RobotDevice::eState state;

// thread stuff
condition_variable cond;
mutex mut;
bool device_ready;

// check e-puck state in every 30 sec
void check_device_state(EpuckPlayerClient *epuck)
{
  boost::unique_lock<boost::mutex> lock(mut);

  while(!device_ready) {
    cond.wait(lock);
    printf("check device thread: waiting\n");
  }

  state = epuck->GetClientState(pc);
  printf(" check device thread: Robot state: %d \n", (int )state);
}

void do_task()
{
    // TODO: navigate for a set time
    printf("task started ---->>>>>>>\n");
    sleep(5);
//    for(int i=0; i < 10000000; i++) {
//    }
    printf("task done ---------<<<<<\n");
    // set device free
    {
      boost::lock_guard<boost::mutex> lock(mut);
      device_ready = true;
    }
    cond.notify_one();

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

    } catch (PlayerError pe) {
        std::cerr << pe << std::endl;
        epuck.mRobotDevice.mStateStep++;
        goto workloop;
    }


    //thread_2.join();
    //thread_1.join();
    return 0;
}
