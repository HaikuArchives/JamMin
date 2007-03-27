//JamFile.h
#ifndef __JAMFILE_H
#define __JAMFILE_H

#include "TargetConfig.h"
//#include "debugMacros.h"
//#include <File.h>
//#include <Entry.h>
//#include <Path.h>

#include <cassert>
#include <string.h> //for strlen

class JamFile {
	public:
					JamFile(const char* path, TargetConfig* t);
					~JamFile();
		status_t	InitCheck();
		status_t	PrintToFile();
		void		Build();
		
	private:
		status_t			fStatus;
		TargetConfig*	fConfig;
		BPath				fFilePath;
		
		/* methods */
		//the following takes a pointer to a TargetConfig member function
		void		PrintFileList(BFile* file, const char* varname,
			const char* (TargetConfig::*GetFileAt)(int32 i), int32 nFiles);
		void		PrintLibraryList(BFile* file);
		void		PrintFileHeader(BFile* file);
		bool		StdLibName(BString* name);
		

};
#endif //__JAMFILE_H