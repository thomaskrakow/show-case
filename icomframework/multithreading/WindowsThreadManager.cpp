
#include <Windows.h>

#include "WindowsThreadManager.h"
#include "WindowsThreadManagerException.h"


#include <assert.h>


using namespace ICOM::Framework;


WindowsThreadManager::WindowsThreadManager()
{
}


WindowsThreadManager::~WindowsThreadManager()
{
}

		
		

ThreadHandle	WindowsThreadManager::createThread(void *(*thread_func)(void*), void *arg)
{
	/*
	 * Let's create the thread
	 */
	ThreadHandle handle = (ThreadHandle)CreateThread(
						NULL,
						0,
						(LPTHREAD_START_ROUTINE)thread_func,
						(LPVOID)arg,
						0,
						0
						);
						
	/*
	 * Check if thread is created successfully.
	 */	
	if(handle == NULL){
		ICOMErrorHandlerFatal1(
			ICOM::Framework::ICOMFramework::fgErrorHandler,
			WindowsThreadManagerException,
			ErrorCodes::ERROR_CREATE_THREAD,
			ICOMString::toString(GetLastError()).c_str()
		);
	}
	
	/*
	 * return the handle
	 */
	return handle;
}

ThreadHandle				WindowsThreadManager::createProcess(const char* cmdline)
{
	/*
	 * https://stackoverflow.com/questions/42531/how-do-i-call-createprocess-in-c-to-launch-a-windows-executable   
	 * https://stackoverflow.com/questions/24012773/c-winapi-how-to-kill-child-processes-when-the-calling-parent-process-is-for
	 */

	BOOL bIsProcessInJob;
	BOOL bSuccess = IsProcessInJob(GetCurrentProcess(), NULL, &bIsProcessInJob);
	if (bSuccess == 0) {
		assert(false);
	}

	HANDLE hJob = CreateJobObject(NULL, NULL);
	if (hJob == NULL) {
		ICOMErrorHandlerFatal1(
			ICOM::Framework::ICOMFramework::fgErrorHandler,
			WindowsThreadManagerException,
			ErrorCodes::ERROR_CREATE_JOB_OBJECT,
			ICOMString::toString(::GetLastError()).c_str()
		)
	}

	JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };
	jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
	bSuccess = SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli));
	if (bSuccess == 0) {
		ICOMErrorHandlerFatal1(
			ICOM::Framework::ICOMFramework::fgErrorHandler,
			WindowsThreadManagerException,
			ErrorCodes::ERROR_SETINFORMATION_JOB_OBJECT,
			ICOMString::toString(::GetLastError()).c_str()
		)
	}

	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFO si = { 0 };
	si.cb = sizeof(si);
	DWORD dwCreationFlags = bIsProcessInJob ? CREATE_BREAKAWAY_FROM_JOB : 0;

	//dwCreationFlags |= CREATE_SECURE_PROCESS;

	if (!CreateProcess(NULL,   // No module name (use command line)
		(LPSTR)cmdline,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		dwCreationFlags,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		DWORD exitcode = 0;
		GetExitCodeProcess(pi.hProcess, &exitcode);
		ICOMErrorHandlerFatal3(
			ICOM::Framework::ICOMFramework::fgErrorHandler,
			WindowsThreadManagerException,
			ErrorCodes::ERROR_CREATE_PROCESS,
			cmdline,
			ICOMString::toString(exitcode).c_str(),
			ICOMString::toString(::GetLastError()).c_str()
		)
	}

	bSuccess = AssignProcessToJobObject(hJob, pi.hProcess);
	if (bSuccess == 0) {
		ICOMErrorHandlerFatal1(
			ICOM::Framework::ICOMFramework::fgErrorHandler,
			WindowsThreadManagerException,
			ErrorCodes::ERROR_ASSIGN_PROCESS_TO_JOBINFO,
			ICOMString::toString(::GetLastError()).c_str()
		)
	}

	return &pi;

}

ThreadManager::State			WindowsThreadManager::joinThread(ThreadHandle handle,int milliSeconds)
{
	assert(handle != 0);

	/*
	 * Joint to th thread.
	 */
	DWORD ret =	0;
	
	if( milliSeconds < 0){
		ret =	WaitForSingleObject((HANDLE)handle,INFINITE);
	}else{
		ret =	WaitForSingleObject((HANDLE)handle,(DWORD)milliSeconds);
	}
				
	/*
	 * Some error handling.
	 */
	if(ret != WAIT_OBJECT_0 && ret != WAIT_TIMEOUT ){
		ICOMErrorHandlerFatal2(
			ICOM::Framework::ICOMFramework::fgErrorHandler,
			WindowsThreadManagerException,
			ErrorCodes::ERROR_JOIN_THREAD,
			ICOMString::toString(ret).c_str(),
			ICOMString::toString(GetLastError()).c_str()
		);
	}

	if( ret == WAIT_TIMEOUT ){
		return ThreadManager::State_TimeOut;
	}

	return ThreadManager::State_Success;
}

ThreadManager::State			WindowsThreadManager::joinProcess(ThreadHandle handle, int milliSeconds)
{
	assert(handle != 0);

	/*
	 * Joint to th thread.
	 */
	DWORD ret = 0;

	PROCESS_INFORMATION* pi = (PROCESS_INFORMATION*)handle;

	if (milliSeconds < 0) {
		ret = WaitForSingleObject(pi->hProcess, INFINITE);
	}
	else {
		ret = WaitForSingleObject(pi->hProcess, (DWORD)milliSeconds);
	}

	/*
	 * Some error handling.
	 */
	if (ret != WAIT_OBJECT_0 && ret != WAIT_TIMEOUT) {
		ICOMErrorHandlerFatal2(
			ICOM::Framework::ICOMFramework::fgErrorHandler,
			WindowsThreadManagerException,
			ErrorCodes::ERROR_JOIN_THREAD,
			ICOMString::toString(ret).c_str(),
			ICOMString::toString(GetLastError()).c_str()
		);
	}

	if (ret == WAIT_TIMEOUT) {
		return ThreadManager::State_TimeOut;
	}
	CloseHandle(pi->hProcess);
	CloseHandle(pi->hThread);

	return ThreadManager::State_Success;
}


void			WindowsThreadManager::terminateThread(ThreadHandle handle)
{
	if(!TerminateThread( (HANDLE)handle, 0) ){
		ICOMErrorHandlerFatal1(
			ICOM::Framework::ICOMFramework::fgErrorHandler,
			WindowsThreadManagerException,
			ErrorCodes::ERROR_TERMINATE_THREAD,
			ICOMString::toString(GetLastError()).c_str()
		);
	}
}


void			WindowsThreadManager::terminateProcess(ThreadHandle handle)
{
	PROCESS_INFORMATION* pi = (PROCESS_INFORMATION*)handle;
	if (!TerminateProcess(pi->hProcess, 0)) {
		ICOMErrorHandlerFatal1(
			ICOM::Framework::ICOMFramework::fgErrorHandler,
			WindowsThreadManagerException,
			ErrorCodes::ERROR_TERMINATE_THREAD,
			ICOMString::toString(GetLastError()).c_str()
		);
	}
}

