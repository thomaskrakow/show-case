
#include "JanitorFile.h"
#include "ICOMString.h"
#include "ICOMFramework.h"
#include "FileManager.h"
#include "ICOMMacros.h"

using namespace ICOM::Framework;
	
JanitorFile::JanitorFile(const char*	filename) : 
	fFileName(0)
{
	fFileName = ICOMString::replicate(filename);
}



JanitorFile::~JanitorFile() 
{
	if (fFileName) {
		ICOMFramework::fgFileManager->removeFile(fFileName);
		ICOM_DELETE_VARIABLE(fFileName);
	}
}

void	JanitorFile::reset(const char* file)
{
	if (fFileName) {
		ICOMFramework::fgFileManager->removeFile(fFileName);
		ICOM_DELETE_VARIABLE(fFileName);
	}
	fFileName = ICOMString::replicate(file);
}

const char*		JanitorFile::getFileName() const
{
	return fFileName;
}








 
