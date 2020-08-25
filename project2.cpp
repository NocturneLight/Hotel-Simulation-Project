#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <queue>

// Create our define statements or macros here.
#define NUM_GUEST 25
#define NUM_EMPLO 2

// Create classes here.
class Guest
{
  public:
    // For the MasterGuest object only.
    volatile int masterIndex = 0;

    // For the regular guests mostly.
    volatile int ident;
    volatile int suite;
    volatile int lugga;
    volatile bool needHelp;


    // The Guest exclusive functions go here. Each
    // function simply outputs something to the
    // screen, save for the constructor and destructor.
    Guest()
    {
    }

    ~Guest()
    {
    }

    void GuestCreated(volatile int id)
    {
      printf("Guest %i created\n", id);
    }

    void EnterHotel(volatile int id, volatile int bags)
    {
      if (bags == 1)
      {
        printf("Guest %i enters hotel with %i bag\n", id, bags);
      }
      else
      {
        printf("Guest %i enters hotel with %i bags\n", id, bags);
      }
    }

    void ReceiveKey(volatile int id, volatile int room, volatile int empId)
    {
      printf("Guest %i receives room key for room %i from front desk employee %i\n", id, room, empId);
    }

    void EnterRoom(volatile int id, volatile int room)
    {
      printf("Guest %i enters room %i\n", id, room);
    }

    void RequestHelp(volatile int id)
    {
      printf("Guest %i requests help with bags\n", id);
    }

    void ReceiveBags(volatile int id, volatile int bhId)
    {
      printf("Guest %i receives bags from bellhop %i and gives tip\n", id, bhId);
    }

    void Retire(volatile int id)
    {
      printf("Guest %i retires for the evening\n", id);
    }

    void Join(volatile int id)
    {
      printf("Guest %i joined\n", id);
    }
};

class FrontDesk
{
  public:
    // Variables for just the MasterFDesk.
    volatile int guestCount = 0;
    volatile int roomCount = 0;

    // Variables more specifically for localFrontDesk.
    volatile int ident;

	// The FrontDesk functions go here.
	// Each function simply prints a message to screen.
	// The exceptions are the constructor and destructor,
	// which do nothing.
    FrontDesk()
    {
    }

    ~FrontDesk()
    {
    }

    void FrontDeskCreated(volatile int id)
    {
      printf("Front desk employee %i created\n", id);
    }

    void AssignRoom(volatile int id, volatile int gId)
    {
      printf("Front desk employee %i registers guest %i and assigns room %i\n", id, gId, ++this->roomCount);
    }
};

class Bellhop
{
  public:
	// Variables specifically for the MasterBellhop.
	volatile int guestCount;
	volatile int guestIdent;

	// Variables for the localBellhop.
	volatile int ident;


	// Create our Bellhop functions here. Each of them, save
	// for the class constructor and destructor, output to the 
	// screen something.
	Bellhop()
	{
	}

	~Bellhop()
	{
	}

	void BellhopCreated(volatile int id)
	{
	  printf("Bellhop %i created\n", id);
	}

	void ReceiveBags(volatile int id, volatile int gId)
	{
	  printf("Bellhop %i receives bags from guest %i\n", id, gId);
	}

	void DeliverBags(volatile int id, volatile int gId)
	{
      printf("Bellhop %i delivers bags to guest %i\n", id, gId);
	}
};


// Create our threads here.
pthread_t g[NUM_GUEST];
pthread_t fd[NUM_EMPLO];
pthread_t bh[NUM_EMPLO];

// Create our semaphores here.
sem_t guestArrival;
sem_t guestMutex;
sem_t roomGiven;
sem_t helpGuest;
sem_t bellhopMutex;
sem_t bellhopFinished;
sem_t bellhopProceed;
sem_t bellhopFinished2;

// Create any global variables and objects here.
Guest *MasterGuest = new Guest();
FrontDesk *MasterFDesk = new FrontDesk();
Bellhop *MasterBellhop = new Bellhop();

void *FrontDeskFunc(void *arg)
{
  // Create variables here.
  FrontDesk *localFrontDesk = new FrontDesk();


   //	   Create the Front Desk Employees.      //
  ////////////////////////////////////////////////
  // Assign the localFrontDesk an identity corresponding
  // to the i-th value in the main function's front desk
  // thread creation loop.
  localFrontDesk->ident = (long)arg;

  // Output to the screen that the front desk employee was
  // created.
  MasterFDesk->FrontDeskCreated(localFrontDesk->ident);
  ////////////////////////////////////////////////


  while (true)
  {

    // Wait for the Guest to respond.
    sem_wait(&guestArrival);


     //	    Assigning the Room to Guest.       //
    ////////////////////////////////////////////
    // Output to the screen the room assignment for the guest by the front
    // desk employee.
    MasterFDesk->AssignRoom(localFrontDesk->ident, MasterGuest->ident);

    // Assign the front desk's id to a value that can be
    // reached by the guest.
    MasterFDesk->ident = localFrontDesk->ident;

    // Signal to the guest that they're finished.
    sem_post(&roomGiven);
    ////////////////////////////////////////////



     //	        Loop Exit Conditions	       //
    ////////////////////////////////////////////
	// Increment the guestCount class variable.
    MasterFDesk->guestCount++;

	// If we've looped a number of times equal to
	// the number of guests - 1, break the loop.
    if (MasterFDesk->guestCount > NUM_GUEST - 1)
    {
      break;
    }
    ////////////////////////////////////////////
  }

  // Have the thread gracefully exit when or if it reaches here.
  pthread_exit(NULL);
}

void *BellhopFunc(void *arg)
{
  // Create our variables here.
  Bellhop *localBellhop = new Bellhop();


   //	        Create the Bellhop.		         //
  ////////////////////////////////////////////////
  // Store the id of the bellhop in the localBellhop
  // object for future use.
  localBellhop->ident = (long)arg;

  // Output to the screen that the bellhop has been
  // created.
  MasterBellhop->BellhopCreated(localBellhop->ident);
  ////////////////////////////////////////////////

  while (true)
  {
    // Wait until the guest signals the bellhop.
    sem_wait(&helpGuest);


     //	      Receive the Guest's Bags.	       //
    ////////////////////////////////////////////
    // Output to the screen that the bellhop has received the guest's bags.
    MasterBellhop->ReceiveBags(localBellhop->ident, MasterBellhop->guestIdent);

    // Post the semaphore to tell the guest they're done.
    sem_post(&bellhopFinished);

    // Wait for a response from the guest.
    sem_wait(&bellhopProceed);

    // Output that the bellhop delivered the luggage to the guest.
    MasterBellhop->DeliverBags(localBellhop->ident, MasterBellhop->guestIdent);

    // Place localBellhop's id in a space that the guest can access.
    MasterBellhop->ident = localBellhop->ident;

    // Tell the guest that they're finished.
    sem_post(&bellhopFinished2);
    ////////////////////////////////////////////



     //	        Loop Exit Conditions.	       //
    ////////////////////////////////////////////
	// Increment the guestCount class variable.
    MasterBellhop->guestCount++;

	// Break if we've looped a number of times equal
	// to the number of guests - 1.
    if (MasterBellhop->guestCount > NUM_GUEST - 1)
    {
      break;
    }
    ////////////////////////////////////////////
  }

  // Have the thread gracefully exit if or when it reaches here.
  pthread_exit(NULL);
}

void *GuestFunc(void *arg)
{
  // Create variables here.
  volatile int bagNum;
  volatile int ideNum;
  Guest *localGuest = new Guest();


   //		  Create the Guest.                   //
  /////////////////////////////////////////////////
  // Create a random number between 0 and 5, including 0 and 5
  // and temporarily store it and the arg variable in bagNum
  // and ideNum respectively.
  bagNum = rand() % 6;
  ideNum = (long)arg;

  // Set localGuest to true or false depending on whether or not
  // the guest's number of bags is greater than 2.
  bagNum > 2 ? localGuest->needHelp = true : localGuest->needHelp = false;

  // Then store the numbers in localGuest's variables for future use.
  localGuest->ident = ideNum;
  localGuest->lugga = bagNum;

  // Output to the screen that a guest has been created.
  MasterGuest->GuestCreated(localGuest->ident);
  /////////////////////////////////////////////////


  // Print out that the guest has entered the hotel.
  MasterGuest->EnterHotel(localGuest->ident, localGuest->lugga);


   //	        Call for Front Desk.		     //
  ////////////////////////////////////////////////
  // Only 1 guest goes through at a time.
  sem_wait(&guestMutex);

  // The guest puts it's information into MasterGuest
  // for front desk to access.
  MasterGuest->ident = localGuest->ident;

  // Then a signal is sent to the FrontDeskFunc to run.
  sem_post(&guestArrival);

  // The guest waits for front desk to finish before
  // proceeding.
  sem_wait(&roomGiven);

  // Output to the screen that the guest has received their room key.
  MasterGuest->ReceiveKey(localGuest->ident, MasterFDesk->roomCount, MasterFDesk->ident);

  // Localize the room count.
  localGuest->suite = MasterFDesk->roomCount;

  // When the front desk finishes, post the semaphore to
  // allow the next guest into the critical section.
  sem_post(&guestMutex);
  ////////////////////////////////////////////////
	

  // Involve the bellhop if a guest has more than 2 bags,
  // else the guest just enters their room and retires for the evening.
  if (localGuest->needHelp)
  {
     // Check if Bellhop is Needed and Call Them.  //
    ////////////////////////////////////////////////
    // Output that the guest needs help with luggage.
    MasterGuest->RequestHelp(localGuest->ident);

    // Only one guest may go through at a time.
    sem_wait(&bellhopMutex);

    // Give the guest id to bellhop so it can use it.
    MasterBellhop->guestIdent = localGuest->ident;

    // Start the bellhop function.
    sem_post(&helpGuest);

    // Wait until the Bellhop finishes.
    sem_wait(&bellhopFinished);

    // Output that the guest has entered their room.
    MasterGuest->EnterRoom(localGuest->ident, localGuest->suite);

    //Signal to the bellhop to proceed.
    sem_post(&bellhopProceed);

    // Wait until the bellhop finishes.
    sem_wait(&bellhopFinished2);

    // Output that the Guest received their luggage and tipped the bellhop.
    MasterGuest->ReceiveBags(localGuest->ident, MasterBellhop->ident);

    // Post to let the next guest through.
    sem_post(&bellhopMutex);
    ////////////////////////////////////////////


    // Output that the guest has retired for the evening.
    MasterGuest->Retire(localGuest->ident);
  }
  else
  {
    // Output that the guest has entered their room.
    MasterGuest->EnterRoom(localGuest->ident, localGuest->suite);

    // Output that the guest has retired for the evening.
    MasterGuest->Retire(localGuest->ident);
  }	

  // Have the thread gracefully exit if it reaches here.
  pthread_exit(NULL);
}

int main()
{
  // Initialize variables, semaphores, and other things here.
  sem_init(&guestArrival, 0, 0);
  sem_init(&guestMutex, 0, 1);
  sem_init(&roomGiven, 0, 0);
  sem_init(&helpGuest, 0, 0);
  sem_init(&bellhopMutex, 0, 1);
  sem_init(&bellhopFinished, 0, 0);
  sem_init(&bellhopProceed, 0, 0);
  sem_init(&bellhopFinished2, 0, 0);


  // Create the front desk threads.
  for (long i = 0; i < NUM_EMPLO; i++)
  {
    pthread_create(&fd[i], NULL, FrontDeskFunc, (void*)i);
  }

  // Create the bellhop threads.
  for (long i = 0; i < NUM_EMPLO; i++)
  {
    pthread_create(&bh[i], NULL, BellhopFunc, (void*)i);
  }

  // Create the guest threads.
  for (long i = 0; i < NUM_GUEST; i++)
  {
    pthread_create(&g[i], NULL, GuestFunc, (void*)i);
  }


  // Wait for all the guest threads to finish before exiting.
  for (long i = 0; i < NUM_GUEST; i++)
  {
    pthread_join(g[i], NULL);

    // Inform the user when each of the guest threads join.
    MasterGuest->Join(i);
  }

  // Inform the user when the program has ended.
  printf("Simulation ends\n");

  // Return from main to exit.
  return 0;
}