// file TargetConfig.h

#ifndef __H_TARGETCONFIG
#define __H_TARGETCONFIG

//#include <File.h>
#include <List.h>
#include <Message.h>
#include <Archivable.h>
#include <String.h>
#include <Path.h>
//#include <cassert>

enum TargetType {
	UNKNOWN,
	BE_APP,
	SHARED_LIBRARY,
	STATIC_LIBRARY,
	KERNEL_DRIVER
};

enum WarningOpt {
	W_NONE,
	W_ALL
};

enum OptimizeOpt {
	O_NONE,
	O_SOME,
	O_FULL
};


class TargetConfig : public BArchivable
{
	public:
		/* methods */
						TargetConfig();
						TargetConfig(BMessage *archive); //BArchivable
		virtual			~TargetConfig();
		
		/***** GetXXX ******************/		
		const char*		GetName();
		TargetType		GetTargetType();
		
		const char*		GetSourceAt(int32 index);
		const char*		GetResourceAt(int32 index);
		const char*		GetLibAt(int32 index); //new
		const char*		GetSysLibAt(int32 index); //obsolete
		const char*		GetUserLibAt(int32 index); //obsolete

		const char*		GetLibPaths();
		const char*		GetSysIncludePaths();
		const char*		GetLocalIncludePaths();
		OptimizeOpt		GetOptimizeOptions();
		const char*		GetDefines();
		WarningOpt		GetWarningOptions();
		bool			SymbolsEnabled();
		bool			DebuggerEnabled();
		const char*		GetCompilerFlags();
		const char* 	GetLinkerFlags();
		
		int32			GetHowManySources();
		int32			GetHowManyResources();
		int32			GetHowManyLibraries(); //new
		int32			GetHowManySysLibs(); //obsolete
		int32			GetHowManyUserLibs(); //obsolete
		
		const char*		GetWorkingDir();
		status_t		RelPathToWorkingDir(const char* absPath, BString* relPath);
		
		/***** SetXXX ******************/						
		status_t		SetName(const char* name);
		status_t		SetTargetType(TargetType type);

		status_t		AddSourceFile(const char* filePath);
		status_t		AddResourceFile(const char* resPath);
		status_t		AddLibrary(const char* libPath); //new
		status_t		AddSystemLib(const char* libPath); //obsolete
		status_t		AddUserLib(const char* libPath); //obsolete
		
		status_t		RemoveFile(const char* filePath);
		status_t		RemSourceFile(const char* filePath);
		status_t		RemResourceFile(const char* resPath);
		status_t		RemLibrary(const char* libPath); //new
		status_t		RemSystemLib(const char* libPath); //obsolete
		status_t		RemUserLib(const char* libPath); //obsolete

		status_t		SetLibPaths(const char* paths);
		status_t		SetSysIncludePaths(const char* paths);
		status_t		SetLocalIncludePaths(const char* paths);
		status_t		SetOptimizeOptions(OptimizeOpt options);
		status_t		SetDefines(const char* defs);
		status_t		SetWarningOptions(WarningOpt options);
		
		void			EnableSymbols(bool enable=true);
		void			EnableDebugger(bool enable=true);		
			
		status_t		SetCompilerFlags(const char* flags);
		status_t		SetLinkerFlags(const char* flags);
		
		void			SetWorkingDir(const char* dir);
	
		/***** MISC ******************/						
		void			PrintToStream();
		status_t		InitCheck();

		//BArchivable
		static BArchivable*	Instantiate(BMessage *archive);
		virtual status_t	Archive(BMessage *archive, bool deep = true) const;

		
	private:
		/* members */
		BString		fName;
		TargetType	fType;
		BList		fSourceFileList;
		BList		fResourceFileList;
		BList		fLibraryList; //new
		BList		fSystemLibraryList; //obsolete
		BList		fUserLibraryList; //obsolete
		
		int32		fNumberOfSources;
		int32		fNumberOfResources;
		int32		fNumberOfLibraries; //new
		int32		fNumberOfSysLibs; //obsolete
		int32		fNumberOfUserLibs; //obsolete
		
		BString		fLibPaths;
		BString		fSystemIncludePaths;
		BString		fLocalIncludePaths;
		OptimizeOpt	fOptimizeOptions;
		BString		fDefines;
		WarningOpt	fWarningOptions;
		bool		fSymbolsEnabled;
		bool		fDebuggerEnabled;
		BString		fCompilerFlags;
		BString		fLinkerFlags;
		BString		fDriverPath;
		
		BPath		fWorkingDir;
		
		status_t	fStatus;
		
		/* methods */
		
		status_t	AddNameToList(const char* name, BList* list);
		status_t	RemNameFromList(const char* name, BList* list);
		void		DeleteListItems(BList* list);
		void		PrintListContent(const char* listName, BList* list);
		bool		ListHasFile(const char* fileName, BList* list);
		status_t	InstantiateFileList(BMessage* archive, const char* archivedName,
			BList* list);
		status_t	ArchiveFileList(const char* archivedName, BMessage* archive, 
			const BList* list) const;
		void GetNextPathToken(BString* token, BString* path, int32* start,
			int32* stop, int32* len);
};

#endif //__H_TARGETCONFIG
