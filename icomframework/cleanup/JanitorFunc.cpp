
#include "JanitorFunc.h"
#include "ICOMFramework.h"

using namespace ICOM::Framework;
	
JanitorFunc::JanitorFunc( void func() ) : 
	fFunc(func)
{
	
}



JanitorFunc::~JanitorFunc() 
{
	fFunc();
}


