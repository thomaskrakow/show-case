
#include "ICOMThreadPool.h"
#include "Janitor.h"
#include "ICOMMacros.h"
#include "ICOMExceptionCollection.h"

#include <assert.h>
#include <string.h>

using namespace ICOM::Framework;
	
ICOMThreadPool::ICOMThreadPool(int maxThreads) : 
	fMaxThreads(maxThreads),
	fSignalHandle(0),
	fSignalHandleAll(0)
{
	assert( maxThreads > 0 );
}

ICOMThreadPool::~ICOMThreadPool() 
{
	this->join();

	if (fSignalHandle) {
		ICOMFramework::fgSignalManager->destroySignal(fSignalHandle);
	}
	if (fSignalHandleAll) {
		ICOMFramework::fgSignalManager->destroySignal(fSignalHandleAll);
	}
}


void*	ICOMThreadPool::wrapper_for_calling_run(void* arg)
{	
	Argument*		argument = (Argument*)arg;
	Runnable*		torun = argument->runnable;
	ICOMThreadPool* threadPool = argument->threadPool;

	ICOMThreadPool::ThreadSurveillance surveillance(argument);

	try {
		
		torun->run();
		if (torun->getAutoDelete()) {
			delete torun;
		}
		return 0;
	}catch (ICOM::Framework::ICOMException& exc){
		threadPool->exceptionThrown(exc);
	}
}

SignalManager::State	ICOMThreadPool::start(Runnable* runnable,int milliSeconds,bool autoDelete)
{
	/* Schauen wir erstmal nach, ob wir �berhaupt einen Thread erzeugen d�rfen, wenn nein, dann warten wir bis wir einen
	 * Thread erzeugen d�rfen. 
	 */
	this->waitForFreePoolCapacity(milliSeconds);

	runnable->setAutoDelete(autoDelete);

	Argument* arg = new Argument;
	arg->runnable = runnable;
	arg->threadPool = this;

	{
		ICOM::Framework::MutexLock mutex(&fMutex);
		fArgumentsOfRunningThreads.insert(arg);
		if (fSignalHandleAll && fArgumentsOfRunningThreads.size() == 1) {
			ICOMFramework::fgSignalManager->destroySignal(fSignalHandleAll);
			fSignalHandleAll = 0;
		}
		if (fSignalHandleAll == 0) {
			fSignalHandleAll = ICOMFramework::fgSignalManager->createSignal();
		}
	}
	arg->threadHandle = ICOMFramework::fgThreadManager->createThread(&wrapper_for_calling_run,arg);
	return SignalManager::State_Success;
}

SignalManager::State	ICOMThreadPool::start(const char* cmdline, int milliSeconds)
{
	/* Schauen wir erstmal nach, ob wir �berhaupt einen Thread erzeugen d�rfen, wenn nein, dann warten wir bis wir einen
	 * Thread erzeugen d�rfen.
	 */
	this->waitForFreePoolCapacity(milliSeconds);

	Argument* arg = new Argument;
	arg->runnable = 0;
	arg->threadPool = this;

	{
		ICOM::Framework::MutexLock mutex(&fMutex);
		fArgumentsOfRunningThreads.insert(arg);
		if (fSignalHandleAll && fArgumentsOfRunningThreads.size() == 1) {
			ICOMFramework::fgSignalManager->destroySignal(fSignalHandleAll);
			fSignalHandleAll = 0;
		}
		if (fSignalHandleAll == 0) {
			fSignalHandleAll = ICOMFramework::fgSignalManager->createSignal();
		}
	}
	arg->threadHandle = ICOMFramework::fgThreadManager->createProcess(cmdline);
	return SignalManager::State_Success;
}

SignalManager::State	ICOMThreadPool::join(int milliSeconds)
{
	SignalManager::State state = SignalManager::State_Success;
	if(fSignalHandleAll){
		state = ICOMFramework::fgSignalManager->wait(fSignalHandleAll, milliSeconds);
		if( state == SignalManager::State_Success){
			ICOMFramework::fgSignalManager->destroySignal(fSignalHandleAll);
			fSignalHandleAll = 0;
		}
	}
	this->rethrow();
	return state;
}

SignalManager::State	ICOMThreadPool::waitForFreePoolCapacity(int milliSeconds)
{
	{
		ICOM::Framework::MutexLock mutex(&fMutex);
		if (fArgumentsOfRunningThreads.size() >= fMaxThreads && fSignalHandle == 0) {
			fSignalHandle = ICOMFramework::fgSignalManager->createSignal();
		}
	}
	if (fSignalHandle) {
		SignalManager::State state = ICOMFramework::fgSignalManager->wait(fSignalHandle, milliSeconds);
		if (state == SignalManager::State_TimeOut) {
			return SignalManager::State_TimeOut;
		}
		ICOM::Framework::MutexLock mutex(&fMutex);
		ICOMFramework::fgSignalManager->destroySignal(fSignalHandle);
		fSignalHandle = 0;
	}
	this->rethrow();
}

void	ICOMThreadPool::exceptionThrown(ICOM::Framework::ICOMException& exc)
{
	ICOM::Framework::MutexLock mutex(&fMutex);
	/*throw exc;*/
	fExceptionsThrownByThread.push(exc);
}


 
void	ICOMThreadPool::rethrow()
{
	ICOM::Framework::MutexLock mutex(&fMutex);
	if (!fExceptionsThrownByThread.empty()) {

		/*
		* Auch wenn mehrere Exceptions in den Threads geworfen wurden, wird nur die letzte Exception geworfen.
		* Meist wird auch nur eine Exception in diesem Stack enthalten sein.
		*/

		if (fExceptionsThrownByThread.size() > 1) {

			ICOMExceptionCollection exc(__FILE__, __LINE__);
			while (!fExceptionsThrownByThread.empty()) {
				exc.add(fExceptionsThrownByThread.top());
				fExceptionsThrownByThread.pop();
			}
			throw exc;

		}
		else {
			ICOM::Framework::ICOMException exc(fExceptionsThrownByThread.top());
			fExceptionsThrownByThread.pop();
			throw exc;
		}

		
	}
}

ICOMThreadPool::ThreadSurveillance::ThreadSurveillance(Argument* argument) :
	fArgument(argument)
{
	assert(argument != 0);
}

ICOMThreadPool::ThreadSurveillance::~ThreadSurveillance()
{
	ICOM::Framework::MutexLock mutex(&fArgument->threadPool->fMutex);
	fArgument->threadPool->fArgumentsOfRunningThreads.erase(fArgument);
	if (fArgument->threadPool->fSignalHandle) {
		ICOMFramework::fgSignalManager->fire(fArgument->threadPool->fSignalHandle);
	}
	
	if (fArgument->threadPool->fSignalHandleAll && fArgument->threadPool->fArgumentsOfRunningThreads.empty()) {
		ICOMFramework::fgSignalManager->fire(fArgument->threadPool->fSignalHandleAll);
	}
	ICOM_DELETE_VARIABLE(fArgument);
}
