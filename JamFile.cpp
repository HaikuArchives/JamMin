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
//	File Name:		JamFile.cpp
//	Author(s):		Guido Casiraghi
//	Description:	
//------------------------------------------------------------------------------

#include "JamMin.h"
//#include "JamFile.h"

JamFile::JamFile(const char* path, TargetConfig* t)
{
	assert(path != NULL);
	assert(t != NULL);
	fStatus = fFilePath.SetTo(path);
	fConfig = t;
}


JamFile::~JamFile()
{
}


status_t
JamFile::InitCheck()
{
	return fStatus;
}


status_t
JamFile::PrintToFile()
{
	status_t ret;
	BString out;
	BFile file;
	//create a new file or erase its data if it already exists
	ret = file.SetTo(fFilePath.Path(), B_WRITE_ONLY| B_CREATE_FILE | B_ERASE_FILE);
	if (ret != B_OK) return ret;
	
	//write configuration to the file
	if ( !file.IsWritable()) return B_PERMISSION_DENIED;

	PrintFileHeader(&file);
	
	//target name
	out.SetTo("NAME = ");
	out += fConfig->GetName();
	out +=  " ;\n";
	file.Write(out.String(), out.CountChars());
	//target type
	out.SetTo("TYPE = ");
	TargetType type = fConfig->GetTargetType();
	switch (type) {
		case BE_APP:
			out += "APP";
			break;
		case SHARED_LIBRARY:
			out += "SHARED";
			break;
		case STATIC_LIBRARY:
			out += "STATIC";
			break;
		case KERNEL_DRIVER:
			out += "DRIVER";
			break;
		default:
			break;
	}
	out += " ;\n";
	file.Write(out.String(), out.CountChars());
	
	//list of sources, resources and libraries
	PrintFileList(&file, "SRCS", &TargetConfig::GetSourceAt, fConfig->GetHowManySources());
	PrintFileList(&file, "RSRCS", &TargetConfig::GetResourceAt, fConfig->GetHowManyResources());
	//XXX - libraries are different
	//PrintFileList(&file, "LIBS", &TargetConfig::GetLibAt, fConfig->GetHowManyLibraries());
	PrintLibraryList(&file);

	//LIBPATHS
	out.SetTo("LIBPATHS = ");
	out += fConfig->GetLibPaths();
	out +=  " ;\n";
	file.Write(out.String(), out.CountChars());
	//system include paths
	out.SetTo("SYSTEM_INCLUDE_PATHS = ");
	out += fConfig->GetSysIncludePaths();
	out +=  " ;\n";
	file.Write(out.String(), out.CountChars());
	//local include paths
	out.SetTo("LOCAL_INCLUDE_PATHS = ");
	out += fConfig->GetLocalIncludePaths();
	out +=  " ;\n";
	file.Write(out.String(), out.CountChars());
	//optimize
	out.SetTo("OPTIMIZE = ");
	OptimizeOpt opt = fConfig->GetOptimizeOptions();
	switch (opt) {
		case O_NONE:
			out += "NONE";
			break;
		case O_SOME:
			out += "SOME";
			break;
		case O_FULL:
			out += "FULL";
			break;			
		default:
			break;
	}
	out +=  " ;\n";
	file.Write(out.String(), out.CountChars());
	
	//defines
	out.SetTo("DEFINES = ");
	out += fConfig->GetDefines();
	out +=  " ;\n";
	file.Write(out.String(), out.CountChars());
	//warnings
	out.SetTo("WARNINGS = ");
	WarningOpt warn = fConfig->GetWarningOptions();
	switch (warn) {
		case W_NONE:
			out += "NONE";
			break;
		case W_ALL:
			out += "ALL";
			break;
		default:
			break;
	}
	out +=  " ;\n";
	file.Write(out.String(), out.CountChars());
	//symbols
	out.SetTo("SYMBOLS = ");
	if (fConfig->SymbolsEnabled())
		out += "TRUE";
	out +=  " ;\n";
	file.Write(out.String(), out.CountChars());
	//debugger
	out.SetTo("DEBUGGER = ");
	if (fConfig->DebuggerEnabled())
		out += "TRUE";
	out +=  " ;\n";
	file.Write(out.String(), out.CountChars());	
	//compiler flags
	out.SetTo("COMPILER_FLAGS = ");
	out += fConfig->GetCompilerFlags();
	out +=  " ;\n";
	file.Write(out.String(), out.CountChars());		
	//linker flags
	out.SetTo("LINKER_FLAGS = ");
	out += fConfig->GetLinkerFlags();
	out +=  " ;\n";
	file.Write(out.String(), out.CountChars());
	//driver path
	out.SetTo("DRIVER_PATH = ");
	//out += fConfig->GetDriverPath() //XXX- missing
	out +=  " ;\n";
	file.Write(out.String(), out.CountChars());

	//include the Jamfile-engine
	out.SetTo("include $(BUILDHOME)/etc/Jamfile-engine ;");
	file.Write(out.String(), out.CountChars());
	
	return B_OK;
}


void
JamFile::PrintFileList(BFile* file, const char* varname,
	const char* (TargetConfig::*GetFileAt)(int32 i), int32 nFiles)
{
	//write to file something like: "SRCS = main.cpp source1.cpp ;"
	BString out(varname);
	out += " = ";
	for (int32 i = 0; i < nFiles; i++) {
		out += (fConfig->*GetFileAt)(i);
		out += " ";
	}
	out += ";\n";
	DPRINT(out.String());
	file->Write(out.String(), out.CountChars());
}


void
JamFile::PrintLibraryList(BFile* file)
{
	BString out("LIBS = ");
	BString stdOut, nostdOut; //the libraries that follow libXXX.so scheme and those that don't
	BString libname, leaf; //the library (absoulte or relative) path and the leaf name  
	int32 nFiles = fConfig->GetHowManyLibraries();
	int32 slashIndex;
	bool isStdName;

	for (int32 i = 0; i < nFiles; i++) {
		libname.SetTo(fConfig->GetLibAt(i));
		slashIndex = libname.FindLast('/');
		if (slashIndex == B_ERROR) { //slash not found
			//pass the whole string
			libname.CopyInto(leaf, 0, libname.CountChars());
			isStdName = StdLibName(&leaf);
		} else {
			//pass the string from slash to end
			libname.CopyInto(leaf, slashIndex+1, libname.CountChars()-slashIndex);
			isStdName = StdLibName(&leaf);
		}
		if (isStdName) {
			//"leaf" contains standard lib name
			stdOut += leaf.String();
			stdOut += " ";
		} else {
			//otherwise add archived name
			nostdOut += libname.String();
			nostdOut += " ";
		}
	}
	out += stdOut.String();
	out += nostdOut.String();
	out += ";\n";
	DPRINT("JamFile::PrintLibraryList: " << out.String());
	file->Write(out.String(), out.CountChars());
	
}

void
JamFile::PrintFileHeader(BFile* file)
{
	BString out (
		"## OpenBeOS Generic Jamfile v1.0 ##\n"
		"## Generated by JamMin ##\n\n"
	);
	file->Write(out.String(), out.CountChars());
}


bool
JamFile::StdLibName(BString* name)
{
	int32 i;
	//name must have a .so or .a suffix
	DPRINT("JamFile::StdLibName: leaf = " << name->String());
	//check if "name" has "lib" prefix
	const char* prefix = "lib";
	if (name->FindFirst(prefix) != 0) //if "lib" isn't the prefix
		return false;
	name->RemoveFirst(prefix);
	//find the suffix
	i = name->FindLast(".so");
	if (i == B_ERROR) //.so not found
		i = name->FindLast(".a");
	if (i != B_ERROR) {
		DPRINT("JamFile::StdLibName: i = " << i);
		name->Remove(i, name->CountChars()-i);
	}
	return true;
}


void
JamFile::Build()
{
}