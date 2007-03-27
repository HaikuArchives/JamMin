#ifndef __JAMMIN_WINDOW_H
#define __JAMMIN_WINDOW_H

//#include "globals.h"
//#include "constants.h"
#include "TargetSettingsWin.h"
#include "TargetConfig.h"
#include "JMOpenFilePanel.h"
//#include  "JamFile.h"

//#include <Application.h>
#include <View.h>
#include <ListView.h>
#include <OutlineListView.h>
#include <StringView.h>
#include <ScrollView.h>
#include <Menu.h>
#include <MenuItem.h>
#include <MenuBar.h>
#include <Message.h>
#include <Window.h>

//application message constants
const uint32 WINDOW_REGISTRY_ADD =		'WRad';
const uint32 WINDOW_REGISTRY_SUB =		'WRsb';
const uint32 WINDOW_REGISTRY_ADDED =	'WRdd';


// messaggi per il menu
//menu file
const uint32 MENU_FILE_NEW_TARGET =		'MFnt';
const uint32 MENU_FILE_OPEN =			'MFop';
const uint32 MENU_FILE_SAVEAS = 		'MFsa';
const uint32 MENU_FILE_EXPORT_JAMFILE =	'MFex';
const uint32 MENU_FILE_CLOSE =			'MFcl';
const uint32 MENU_FILE_QUIT	=			'MFqt';

//menu target
const uint32 MENU_TARGET_ADD_FILES =	'MTaf';
const uint32 MENU_TARGET_REMOVE_ITEMS = 'MTrm';
const uint32 MENU_TARGET_BUILD = 		'MTbd';
const uint32 MENU_TARGET_SETTINGS = 	'MTst';

//finestra Target Settings
const uint32 TARGET_NAME_CHANGED =		'TNch';
const uint32 TARGET_TYPE_CHANGED =		'TTch';
const uint32 TARGET_OPTIONS_CHANGED =	'TOch';
	
//Misc
const uint32 JAMFILE_REF_RECEIVED =		'JFrr';
const uint32 LISTVIEW_FILE_INVOKED =	'LVfi';



class JamMinWindow : public BWindow
{
public:
	JamMinWindow(const char *uWindowTitle);
	JamMinWindow(entry_ref* ref);
	~JamMinWindow();
	virtual void MessageReceived(BMessage *Message);
	virtual bool QuitRequested();
	virtual void FrameResized(float width, float height);
	virtual void Zoom(BPoint origin, float width, float height);
	status_t Save(BMessage* message);
private:
	/* members */
	TargetConfig* fConfig;
	TargetSettingsWin* fSettingsWin;
	//BListView* fListView;
	
	BMenuBar *fMenubar;
	BScrollView *fScrollFileListView;
	BFilePanel* fSavePanel;
	BFilePanel*	fExportPanel;
	BFilePanel*	fOpenPanel; 
	JMOpenFilePanel* fOpenPanel2; //to add files to project
	///////
	BOutlineListView *fFileListView;
	BListItem* fSourceGroup;
	//BStringItem* fSourceGroup;
	BListItem* fResourceGroup;
	BListItem* fLibraryGroup;
	BListItem* fSysLibGroup;
	BListItem* fUserLibGroup;
	BListItem* fOtherGroup;

	///////
	
	status_t fStatus;
	BMessage* fSaveMessage;
	
	
	//helper functions
	void		InitFileList(); //inserisce nella lista i nomi dei file														//da compilare
	void		InitMenu();
	void		InitWindow();
	void		PopulateFileList();
	void		SetPanelDirs();
	
	void		AddToProject(entry_ref* ref, bool relpath);
	status_t	AddToTargetAndGroup(const char* name, const char* archName, BPath* fullPath,
		status_t (TargetConfig::*AddFun)(const char*),
		status_t (TargetConfig::*RemFun)(const char*), BListItem* group);
	void		RemoveSelectedItems();
	void		RemoveSelectedGroupItems(BListItem* group);
	status_t	ExportJamFile(BMessage* message);
	bool		IsSystemLibraryPath(BPath* libPath);
	//bool		IsPathAllowed(BPath* workingDir, BPath* fileDir);
	BPath		GetWorkingDirPath();
	status_t	AddFileToGroup(const char* name, const char* archName, BPath* fullPath, BListItem* group);
	status_t	AddArchivedName(const char* archName, BListItem* group,
		status_t (TargetConfig::*RemFun)(const char*));
	void		HandleDragMessage(BMessage* message);
	inline bool	IsRelativePath(const char* path);
	//status_t	AbsToRelPath(BString* returnedPath, BPath* filePath);
	status_t	GetFileExtension(BString* extension, const BString* fileName);
	void		OpenSelectedItem();
	void		ShowSaveAlert();
	bool		SysLibAlert();
	void		UnflattenErrorAlert();
	void		UnarchiveNameErrorAlert(const char* path);
	
	void		Register();
	void		Unregister();
};

#endif //__JAMMIN_WINDOW_H
