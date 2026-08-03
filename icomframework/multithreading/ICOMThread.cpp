
#include "ICOMThread.h"




using namespace ICOM::Framework;
	
ICOMThread::ICOMThread(Runnable*	runnable) : 
	fRunnable(runnable),
	fThreadHandle(0)
{
}



ICOMThread::~ICOMThread() 
{
	this->join();
}

		
void			ICOMThread::start() 
{
	fThreadHandle = ICOMFramework::fgThreadManager->createThread(&wrapper_for_calling_run,fRunnable);
}

void			ICOMThread::join(int milliSeconds) 
{
	if( fThreadHandle ){
		ICOMFramework::fgThreadManager->joinThread(fThreadHandle,milliSeconds);
	}
}

void			ICOMThread::terminate() 
{
	if( fThreadHandle ){
		ICOMFramework::fgThreadManager->terminateThread(fThreadHandle);
	}
}


void*	ICOMThread::wrapper_for_calling_run(void* runnable)
{
	Runnable*	torun = (ICOM::Framework::Runnable*)runnable;

	torun->run();

	return 0;
}








 
