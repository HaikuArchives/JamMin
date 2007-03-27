//------------------------------------------------------------------------------
//	Copyright (c) 2003, Guido Casiraghi
//
//	Permission is hereby granted, free of charge, to any person obtaining a
//	copy of this software and associated documentation files (the "Software"),
//	to deal in the Software without restriction, including without limitation
//	the rights to use, copy, modify, merge, publish, distribute, sublicense,
//	and/or sell copies of the Software, and to permit persons to whom the
//	Software is furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in
//	all copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//	DEALINGS IN THE SOFTWARE.
//
//	File Name:		TargetConfig.cpp
//	Author(s):		Guido Casiraghi
//	Description:	
//------------------------------------------------------------------------------

#include "JamMin.h"

//#include "TargetConfig.h"
//#include <iostream> //per cout
//#include <stdio.h>	//per printf
//#include <stdlib.h> //per malloc
//#include "debugMacros.h"


//help function
BString IntegerToString(int32 n);



/********** public methods ***************/

//to be completed...
TargetConfig::TargetConfig()
{
	//initialize all non-aggregate members
	fNumberOfSources	= 0;
	fNumberOfResources	= 0;
	fNumberOfLibraries = 0;
	////////remove//////////
	fNumberOfSysLibs	= 0;
	fNumberOfUserLibs	= 0;
	///////////////////////
	fType = UNKNOWN;
	fOptimizeOptions = O_FULL;
	fWarningOptions = W_ALL;

	fDebuggerEnabled = false;
	fSymbolsEnabled = false;
	
	fStatus = B_OK;
}//costruttore normale


TargetConfig::TargetConfig(BMessage *archive)
{
	DPRINT("TargetConfig::TargetConfig(BMessage*)");
	status_t ret;
	const char* charBuffer;
	int32 type, optimize, warnings;
	
	fStatus = B_OK;
	
	//target name
	ret = archive->FindString("_TargetConfig:name", &charBuffer);
	if (ret != B_OK) return;
	fName.SetTo(charBuffer);
	//target type - XXX it is casted from int32
	ret = archive->FindInt32("_TargetConfig:type", &type);
	if (ret != B_OK) return;
	fType = (TargetType)type;
	
	//file lists...
	InstantiateFileList(archive, "_TargetConfig:sourcefile", &fSourceFileList);
	InstantiateFileList(archive, "_TargetConfig:resourcefile", &fResourceFileList);
	InstantiateFileList(archive, "_TargetConfig:lib", &fLibraryList);
	//////remove///////
	InstantiateFileList(archive, "_TargetConfig:syslib", &fSystemLibraryList);
	InstantiateFileList(archive, "_TargetConfig:userlib", &fUserLibraryList);
	//////////////////
	
	
	//number of files
	ret = archive->FindInt32("_TargetConfig:nsources", &fNumberOfSources);
	if (ret != B_OK) return;
	ret = archive->FindInt32("_TargetConfig:nresources", &fNumberOfResources);
	if (ret != B_OK) return;
	ret = archive->FindInt32("_TargetConfig:nlibs", &fNumberOfLibraries);
	if (ret != B_OK) return;
	//////////remove//////////
	ret = archive->FindInt32("_TargetConfig:nsyslibs", &fNumberOfSysLibs);
	if (ret != B_OK) return;
	ret = archive->FindInt32("_TargetConfig:nuserlibs", &fNumberOfUserLibs);
	if (ret != B_OK) return;
	/////////////////////////
	
	//lib paths
	ret = archive->FindString("_TargetConfig:libpaths", &charBuffer);
	if (ret != B_OK) return;
	fLibPaths.SetTo(charBuffer);
	//system and local include paths
	ret = archive->FindString("_TargetConfig:syspaths", &charBuffer);
	if (ret != B_OK) return;
	fSystemIncludePaths.SetTo(charBuffer);
	ret = archive->FindString("_TargetConfig:localpaths", &charBuffer);
	if (ret != B_OK) return;
	fLocalIncludePaths.SetTo(charBuffer);
	//optimize options - XXX enum casted to int32
	ret = archive->FindInt32("_TargetConfig:optim", &optimize);
	if (ret != B_OK) return;
	fOptimizeOptions = (OptimizeOpt)optimize;
	//defines
	ret = archive->FindString("_TargetConfig:defs", &charBuffer);
	if (ret != B_OK) return;
	fDefines.SetTo(charBuffer);
	//warnings - XXX enum casted to int32
	ret = archive->FindInt32("_TargetConfig:warn", &warnings);
	if (ret != B_OK) return;
	fWarningOptions = (WarningOpt) warnings;
	//symbols and debugger enabled
	ret = archive->FindBool("_TargetConfig:symbols", &fSymbolsEnabled);
	if (ret != B_OK) return;	
	ret = archive->FindBool("_TargetConfig:debugger", &fDebuggerEnabled);
	if (ret != B_OK) return;
	//compiler and linker flags
	ret = archive->FindString("_TargetConfig:cflags", &charBuffer);
	if (ret != B_OK) return;
	fCompilerFlags.SetTo(charBuffer);
	ret = archive->FindString("_TargetConfig:lflags", &charBuffer);
	if (ret != B_OK) return;
	fLinkerFlags.SetTo(charBuffer);
	ret = archive->FindString("_TargetConfig:drvpath", &charBuffer);
	if (ret != B_OK) return;
	fDriverPath.SetTo(charBuffer);

	DPRINT("TargetConfig::TargetConfig(BMessage*) terminated correctly");		
} //costruttore 


TargetConfig::~TargetConfig()
{
	//delete object allocated in the heap i.e. BStrings in BLists
	DeleteListItems(&fSourceFileList);
	DeleteListItems(&fResourceFileList);
	DeleteListItems(&fLibraryList);
	//////remove////////////
	DeleteListItems(&fSystemLibraryList);
	DeleteListItems(&fUserLibraryList);
	///////////////////////
} //distruttore

///////////////////////// METODI GET ////////////////////////////////////////////////////////////
const char *
TargetConfig::GetName()
{
	return fName.String();
}


TargetType
TargetConfig::GetTargetType()
{
	return fType;
}


const char *
TargetConfig::GetSourceAt(int32 index)
{
	BString *string = (BString *)fSourceFileList.ItemAt(index);
	if (!string) return NULL;
	return string->String();
}


const char *
TargetConfig::GetResourceAt(int32 index)
{
	BString *string = (BString *)fResourceFileList.ItemAt(index);
	if (!string) return NULL;
	return string->String();
}


const char *
TargetConfig::GetLibAt(int32 index)
{
	BString *string = (BString *)fLibraryList.ItemAt(index);
	if (!string) return NULL;
	return string->String();
}


const char *
TargetConfig::GetSysLibAt(int32 index)
{
	BString *string = (BString *)fSystemLibraryList.ItemAt(index);
	if (!string) return NULL;
	return string->String();
}


const char *
TargetConfig::GetUserLibAt(int32 index)
{
	BString *string = (BString *)fUserLibraryList.ItemAt(index);
	if (!string) return NULL;
	return string->String();
}


const char *
TargetConfig::GetLibPaths()
{
	return fLibPaths.String();
}


const char *
TargetConfig::GetSysIncludePaths()
{
	return fSystemIncludePaths.String();
}


const char *
TargetConfig::GetLocalIncludePaths()
{
	return fLocalIncludePaths.String();
}


OptimizeOpt
TargetConfig::GetOptimizeOptions()
{
	return fOptimizeOptions;
}


const char *
TargetConfig::GetDefines()
{
	return fDefines.String();
}


WarningOpt
TargetConfig::GetWarningOptions()
{
	return fWarningOptions;
}


bool
TargetConfig::SymbolsEnabled()
{
	return fSymbolsEnabled;
}


bool
TargetConfig::DebuggerEnabled()
{
	return fDebuggerEnabled;
}


const char *
TargetConfig::GetCompilerFlags()
{
	return fCompilerFlags.String();
}


const char *
TargetConfig::GetLinkerFlags()
{
	return fLinkerFlags.String();
}


int32
TargetConfig::GetHowManySources()
{
	return fNumberOfSources;
}


int32
TargetConfig::GetHowManyResources()
{
	return fNumberOfResources;
}


int32
TargetConfig::GetHowManyLibraries()
{
	return fNumberOfLibraries;
}


int32
TargetConfig::GetHowManySysLibs()
{
	return fNumberOfSysLibs;
}


int32
TargetConfig::GetHowManyUserLibs()
{
	return fNumberOfUserLibs;
}


///////////////////////// METODI SET ////////////////////////////////////////////////////////////


status_t
TargetConfig::SetName(const char* name)
{
	fName.SetTo(name);
	return B_OK;
}


status_t
TargetConfig::SetTargetType(TargetType type)
{
	fType = type;
	return B_OK;
}


status_t
TargetConfig::AddSourceFile(const char *filePath)
{
	status_t ret = AddNameToList(filePath, &fSourceFileList);
	if (ret == B_OK) fNumberOfSources++;
	return ret;
}


status_t
TargetConfig::AddResourceFile(const char *resPath)
{
	status_t ret =  AddNameToList(resPath, &fResourceFileList);
	if (ret == B_OK) fNumberOfResources++;
	return ret;
}


status_t
TargetConfig::AddLibrary(const char* libPath)
{
	status_t ret =  AddNameToList(libPath, &fLibraryList);
	if (ret == B_OK) fNumberOfLibraries++;
	return ret;
}


status_t
TargetConfig::AddSystemLib(const char *libPath)
{
	status_t ret = AddNameToList(libPath, &fSystemLibraryList);
	if (ret == B_OK) fNumberOfSysLibs++;
	return ret;
}


status_t
TargetConfig::AddUserLib(const char *libPath)
{
	status_t ret = AddNameToList(libPath, &fUserLibraryList);
	if (ret == B_OK) fNumberOfUserLibs++;
	return ret;
}


/* RemoveFile():
 * searches 'filePath' in all file lists and removes it if it was found
 */
status_t
TargetConfig::RemoveFile(const char* filePath)
{
	if ( RemSourceFile(filePath) == B_OK) return B_OK;
	if ( RemResourceFile(filePath) == B_OK) return B_OK;
	if ( RemLibrary(filePath) == B_OK) return B_OK;
	if ( RemSystemLib(filePath) == B_OK) return B_OK;
	if ( RemUserLib(filePath) == B_OK) return B_OK;
	return B_ERROR;
}

status_t
TargetConfig::RemSourceFile(const char* filePath)
{
	status_t ret = RemNameFromList(filePath, &fSourceFileList);
	if (ret == B_OK) fNumberOfSources--;
	return ret;
}


status_t
TargetConfig::RemResourceFile(const char* resPath)
{
	status_t ret =  RemNameFromList(resPath, &fResourceFileList);
	if (ret == B_OK) fNumberOfResources--;
	return ret;
}


status_t
TargetConfig::RemLibrary(const char* libPath)
{
	status_t ret =  RemNameFromList(libPath, &fLibraryList);
	if (ret == B_OK) fNumberOfLibraries--;
	return ret;
}


status_t
TargetConfig::RemSystemLib(const char* libPath)
{
	status_t ret = RemNameFromList(libPath, &fSystemLibraryList);
	if (ret == B_OK) fNumberOfSysLibs--;
	return ret;

}


status_t
TargetConfig::RemUserLib(const char* libPath)
{
	status_t ret = RemNameFromList(libPath, &fUserLibraryList);
	if (ret == B_OK) fNumberOfUserLibs--;
	return ret;

}


status_t
TargetConfig::SetLibPaths(const char *paths)
{
	fLibPaths.SetTo(paths);
	return B_OK;
}


status_t
TargetConfig::SetSysIncludePaths(const char *paths)
{
	fSystemIncludePaths.SetTo(paths);
	return B_OK;
}


status_t
TargetConfig::SetLocalIncludePaths(const char *paths)
{
	fLocalIncludePaths.SetTo(paths);
	return B_OK;
}


status_t
TargetConfig::SetOptimizeOptions(OptimizeOpt options)
{
	fOptimizeOptions = options;
	return B_OK;
}


status_t
TargetConfig::SetDefines(const char* defs)
{
	fDefines.SetTo(defs);
	return B_OK;
}


status_t
TargetConfig::SetWarningOptions(WarningOpt options)
{
	fWarningOptions = options;
	return B_OK;
}


void
TargetConfig::EnableSymbols(bool enable=true)
{
	fSymbolsEnabled = enable;
}


void
TargetConfig::EnableDebugger(bool enable=true)
{
	fDebuggerEnabled = enable;
}


status_t
TargetConfig::SetCompilerFlags(const char* flags)
{
	fCompilerFlags.SetTo(flags);
	return B_OK;
}


status_t
TargetConfig::SetLinkerFlags(const char* flags)
{
	fLinkerFlags.SetTo(flags);
	return B_OK;
}

////////////////////////////////////////////


void
TargetConfig::PrintToStream()
{
	BString type, warnings, optimize;
	switch (fType) {
		case UNKNOWN:
			type.SetTo("UNKNOWN");
			break;
		case BE_APP:
			type.SetTo("BE_APP");
			break;
		case SHARED_LIBRARY:
			type.SetTo("SHARED_LIBRARY");
			break;
		case STATIC_LIBRARY:
			type.SetTo("STATIC_LIBRARY");
			break;
		case KERNEL_DRIVER:
			type.SetTo("KERNEL_DRIVER");
			break;
		default:
			type.SetTo("");
			break;
	}
	switch (fWarningOptions) {
		case W_NONE:
			warnings.SetTo("W_NONE");
			break;
		case W_ALL:
			warnings.SetTo("W_ALL");
			break;
		default:
			warnings.SetTo("");
	}
	switch (fOptimizeOptions) {
		case O_NONE:
			optimize.SetTo("O_NONE");
			break;
		case O_SOME:
			optimize.SetTo("O_SOME");
			break;
		case O_FULL:
			optimize.SetTo("O_FULL");
			break;
		default:
			optimize.SetTo("");
			break;
	}
	
	std::cout << "Target Name = " << GetName() << endl;
	std::cout << "Target Type = " << type.String() << endl;

	PrintListContent("Source File List", &fSourceFileList);
	PrintListContent("Resource File List", &fResourceFileList);
	PrintListContent("Library List", &fLibraryList);
	PrintListContent("System Library List", &fSystemLibraryList);
	PrintListContent("User Library List", &fUserLibraryList);

	std::cout << "Number of source files = " << fNumberOfSources << endl;
	std::cout << "Number of resource files = " << fNumberOfResources << endl;
	std::cout << "Number of libraries = " << fNumberOfLibraries << endl;
	std::cout << "Number of system libraries = " << fNumberOfSysLibs << endl;
	std::cout << "Number of user libraries = " << fNumberOfUserLibs << endl;
	
	std::cout << "Library Paths = " << fLibPaths.String() << endl;
	std::cout << "System Include Paths = " << fSystemIncludePaths.String() << endl;
	std::cout << "Local Include Paths = " << fLocalIncludePaths.String() << endl;

	std::cout << "Optimize Options = " << optimize.String() << endl;
	std::cout << "Defines = " << fDefines.String() << endl;
	std::cout << "Warning Options = " << warnings.String() << endl;
	std::cout << "Symbols " << (fSymbolsEnabled ? "enabled" : "not enabled" ) << endl;
	std::cout << "Debugger " << (fDebuggerEnabled ? "enabled" : "not enabled" ) << endl;
	
	std::cout << "Compiler Flags = " << fCompilerFlags.String() << endl;
	std::cout << "Linker Flags = " << fLinkerFlags.String() << endl;
	std::cout << "Driver Path = " << fDriverPath.String() << endl;

	//std::cout << "Lib Paths = " << GetLibPaths() << endl;
}


void
TargetConfig::PrintListContent(const char* listName, BList* list)
{
	BString* item;
	int32 i;
	//XXX - the following is not clear
	for ( i = 0; (item = (BString *)list->ItemAt(i)); i++ ) {
		std::cout << listName << " item " << i << " = " << item->String() << endl;
	}
	if (i == 0)
		std::cout << listName << " is empty" << endl;
}


////////////////////////////////////////////


/* AddNameToList()
 * Return:
 * - B_ERROR: fileName is already present in the list
 * - B_NO_MEMORY: can't allocate new memory
 * - B_OK: fileName is inserted correctly 
 *
 */
status_t
TargetConfig::AddNameToList(const char* fileName, BList* list)
{
	//check first if fileName is already present in the list
	if ( ListHasFile(fileName, list) ) return B_ERROR;	
	//allocate memory for string
	BString* newFile = new BString(fileName); 
	if (!newFile) return B_NO_MEMORY;
	//add newFile to list
	if (list->AddItem(newFile))
		return B_OK;
	else
		return B_NO_MEMORY;
}


status_t
TargetConfig::RemNameFromList(const char* name, BList* list)
{
	BString* item;
	//XXX - the following is not clear
	for ( int32 i = 0; (item = (BString *)list->ItemAt(i)); i++ ) {
		if  ( ((BString *)item)->Compare(name) == 0 ) {
			//we have found the name at position 'i', remove it, delete object and return
			if (list->RemoveItem(i)) {
				delete item;
				return B_OK;
			} else {
				//for some strange reason RemoveItem returned false
				return B_ERROR;
			}
		}		
	} //for
	//if we are here, we haven't found the file name
	return B_ERROR;
}


bool
TargetConfig::ListHasFile(const char* fileName, BList* list)
{
	BString* item;
	//XXX - the following code is similar to RemNameFromList
	for ( int32 i = 0; (item = (BString *)list->ItemAt(i)); i++ ) {
		if  ( ((BString *)item)->Compare(fileName) == 0 ) {
			//the file name is already present
			return true;
		}
	}
	//if we are here, we haven't found fileName
	return false;	
}


void
TargetConfig::DeleteListItems(BList* list)
{
	BString* item;
	//XXX - the following is not clear
	for ( int32 i = 0; (item = (BString *)list->ItemAt(i)); i++ )
		delete item;
}


//implementazione di BArchivable
/* static */ BArchivable *
TargetConfig::Instantiate(BMessage *archive)
{
	DPRINT("TargetConfig::Instantiate()");
	//controlla se si tratta di un 'archive' corretto
	if ( !validate_instantiation(archive, "TargetConfig") ) {
		return NULL;
	}
	DPRINT("TargetConfig::Instantiate(): validate_instantiation ok");
	return new TargetConfig(archive);
}


status_t
TargetConfig::InstantiateFileList(
	BMessage* archive, const char* archivedName, BList* list)
{
	if (archive == NULL || archivedName == NULL || list == NULL)
		return B_ERROR;

	BString archivedItemName;
	const char* charBuffer;
	status_t ret;
	
	for ( int32 i = 0; /* */ ;i++ ) {
		archivedItemName = archivedName;
		archivedItemName += IntegerToString(i).String();
		ret = archive->FindString(archivedItemName.String(), &charBuffer);
		if (ret != B_OK) break;
		DPRINT("TargetConfig::InstantiateFileList(): " << archivedItemName.String()
			<< " = " << charBuffer);
		//insert charBuffer in the list - XXX should check return value
		AddNameToList(charBuffer, list);
	}

	return B_OK;
}


status_t
TargetConfig::Archive(BMessage *archive, bool deep = true) const
{
	status_t ret;
	//call base class Archive function first
	ret = BArchivable::Archive(archive, deep);
	if (ret != B_OK) return ret;
	ret = archive->AddString("class", "TargetConfig");
	
	//add members to archive
	//XXX - enums are casted to int32
	//target name
	ret = archive->AddString("_TargetConfig:name", fName.String());
	if (ret != B_OK) return ret;
	//target type
	ret = archive->AddInt32("_TargetConfig:type", fType);
	if (ret != B_OK) return ret;
	
	//file lists...
	ArchiveFileList("_TargetConfig:sourcefile", archive, &fSourceFileList);
	ArchiveFileList("_TargetConfig:resourcefile", archive, &fResourceFileList);
	ArchiveFileList("_TargetConfig:lib", archive, &fLibraryList);
	ArchiveFileList("_TargetConfig:syslib", archive, &fSystemLibraryList);
	ArchiveFileList("_TargetConfig:userlib", archive, &fUserLibraryList);
	
	
	//number of files
	ret = archive->AddInt32("_TargetConfig:nsources", fNumberOfSources);
	if (ret != B_OK) return ret;
	ret = archive->AddInt32("_TargetConfig:nresources", fNumberOfResources);
	if (ret != B_OK) return ret;
	ret = archive->AddInt32("_TargetConfig:nlibs", fNumberOfLibraries);
	if (ret != B_OK) return ret;
	ret = archive->AddInt32("_TargetConfig:nsyslibs", fNumberOfSysLibs);
	if (ret != B_OK) return ret;
	ret = archive->AddInt32("_TargetConfig:nuserlibs", fNumberOfUserLibs);
	if (ret != B_OK) return ret;
	
	//lib paths
	ret = archive->AddString("_TargetConfig:libpaths", fLibPaths.String());
	if (ret != B_OK) return ret;
	//system and local include paths
	ret = archive->AddString("_TargetConfig:syspaths", fSystemIncludePaths.String());
	if (ret != B_OK) return ret;
	ret = archive->AddString("_TargetConfig:localpaths", fLocalIncludePaths.String());
	if (ret != B_OK) return ret;
	//optimize options - XXX enum casted to int32
	ret = archive->AddInt32("_TargetConfig:optim", fOptimizeOptions);
	if (ret != B_OK) return ret;
	//defines
	ret = archive->AddString("_TargetConfig:defs", fDefines.String());
	if (ret != B_OK) return ret;
	//warnings - XXX enum casted to int32
	ret = archive->AddInt32("_TargetConfig:warn", fWarningOptions);
	if (ret != B_OK) return ret;
	//symbols and debugger enabled
	ret = archive->AddBool("_TargetConfig:symbols", fSymbolsEnabled);
	if (ret != B_OK) return ret;	
	ret = archive->AddBool("_TargetConfig:debugger", fDebuggerEnabled);
	if (ret != B_OK) return ret;
	//compiler and linker flags
	ret = archive->AddString("_TargetConfig:cflags", fCompilerFlags.String());
	if (ret != B_OK) return ret;
	ret = archive->AddString("_TargetConfig:lflags", fLinkerFlags.String());
	if (ret != B_OK) return ret;
	ret = archive->AddString("_TargetConfig:drvpath", fDriverPath.String());
	if (ret != B_OK) return ret;
	
	return ret;
}

//ATTENTION: since ArchiveFileList is called from Archive (const function), BList pointer must
//be const because it point to a class members
status_t
TargetConfig::ArchiveFileList(const char* archivedName, BMessage* archive, const BList* list) const
{
	BString* item;
	BString archivedItemName;
	status_t ret;
	for ( int32 i = 0; (item = (BString *)list->ItemAt(i)); i++ ) {
		archivedItemName = archivedName;
		archivedItemName += IntegerToString(i).String();
		DPRINT("TargetConfig::ArchiveFileList(): " << archivedItemName.String());
		ret = archive->AddString(archivedItemName.String(), item->String());
		if (ret != B_OK) return ret;
	}
	return B_OK;
}


void
TargetConfig::SetWorkingDir(const char* dir)
{
	fWorkingDir.SetTo(dir);
}


const char *
TargetConfig::GetWorkingDir()
{
	return fWorkingDir.Path();
}


status_t
TargetConfig::InitCheck()
{
	return fStatus;
}


//GetWorkingDir()
//builds relPath as absPath relative to working directory
//Input:
//absPath: file or directory absolute path
//relPath: pointer to empty path to be filled
//Output:
//B_ERROR: if fWorkingDir is empty or something wrong happened
//B_OK: otherwise
status_t
TargetConfig::RelPathToWorkingDir(const char* absPath, BString* relPath)
{
	assert(absPath != NULL);
	if (fWorkingDir.InitCheck() == B_NO_INIT) return B_ERROR;
	//copy paths into strings
	BString wdString(fWorkingDir.Path());
	BString pathString(absPath);
	BString relPathString;
	//initialize variables
	BString wdTok, pathTok; //path tokens
	int32 wdstart = -1, wdstop = -1;  //counters for wdString
	int32 pstart = -1, pstop = -1; //counters for pathString
	int32 wdlen, pathlen;
	int32 firstdiff = 0; //the index after which the two paths are different 
	wdlen = wdString.CountChars();
	pathlen = pathString.CountChars();
	DPRINT("TargetConfig::RelPathToWorkingDir: wdString = " << wdString.String()
		<< ", pathString = " << pathString.String());
	DPRINT("TargetConfig::RelPathToWorkingDir: wdlen  = " << wdlen << ", pathlen = " << pathlen);
	//first check if paths are absoulte
	if (wdString[0] != '/' || pathString[0] != '/') return B_ERROR;
	while (wdstop < wdlen-1 || pstop < pathlen-1) { // while the two strings have not been parsed
		wdTok = pathTok = ""; //reset
		GetNextPathToken(&wdTok, &wdString, &wdstart, &wdstop, &wdlen);
		GetNextPathToken(&pathTok, &pathString, &pstart, &pstop, &pathlen);
		if (wdTok == "" && firstdiff == 0) {
			//first case: wdString has been completely parsed. pathString is a subdirectory
			//e.g. wdString = /boot/aa/bb, pathString = /boot/aa/bb//cc/dd
			firstdiff = pstart;
			DPRINT("TargetConfig::RelPathToWorkingDir: wdTok == NULL && firstdiff == 0");
		} else if (wdTok == "" && firstdiff > 0) {
			DPRINT("TargetConfig::RelPathToWorkingDir: wdTok == NULL && firstdiff > 0");
			//XXX - remove this, it does nothing
		} else if ( (wdTok.ICompare(pathTok) != 0) || firstdiff > 0 ) {
			//second case: wdTok is a valid token, pathTok may be NULL. The two strings differ at some
			//point
			DPRINT("TargetConfig::RelPathToWorkingDir: wdTok.ICompare(pathTok) != 0) || firstdiff > 0");
			relPathString += "../";
			if (firstdiff == 0) firstdiff = wdstart;
		}
			//otherwise: do nothing
		

	}
	//we have parsed all the strings. Now append pathString from firstdiff index to the end
	//into relPathString
	if (firstdiff != 0 && firstdiff < pathlen) { //otherwise wdString == pathString or
												// wd is a subdirectory of this path
		BString rem;
		DPRINT("TargetConfig::RelPathToWorkingDir: copy " << pathlen - firstdiff << " chars from "
			<< firstdiff);
		pathString.CopyInto(rem, firstdiff, pathlen - firstdiff);
			DPRINT("TargetConfig::RelPathToWorkingDir: rem string = " << rem.String());
			relPathString += rem;
	} 
	
	DPRINT("TargetConfig::RelPathToWorkingDir: relPathString = " << relPathString.String());
	*relPath = relPathString.String();
	return B_OK;
}


void
TargetConfig::GetNextPathToken(BString* token, BString* path, int32* start, int32* stop, int32* len)
{
	if (*stop >= (*len - 1)) return; //end of string
	*start  = *stop + 2; //*start+1 character should be a slash
	*stop = *start;
	*token = "";
	DPRINT("TargetConfig::GetNextPathToken: start = " << *start << ", stop = " << *stop);
	//scan the string until end or slash is found
	while (*stop < *len && path->ByteAt(*stop) != '/') {
		++*stop;
	}
	//if (path->ByteAt(*stop) == '/' || *stop == *len) { //one of these two things must be true here!
		--*stop;
		path->CopyInto(*token, *start, *stop - *start + 1);
		DPRINT("TargetConfig::GetNextPathToken: token = " << token->String());
	//}
}


//////////////////////////////////////////////////////////////////////////////
//adapted from k&r
//converts only positive numbers
BString
IntegerToString(int32 n)
{
	int32 i, sign;
	BString s;
	char c;
	
	if ((sign = n) < 0)
		n = -n;
	i = 0;
	do {
		c = n % 10 + '0';
		//append c to s one time
		s.Prepend(c, 1);
		//i++;
	
	} while ((n /= 10) > 0);
	
	return s;
}
