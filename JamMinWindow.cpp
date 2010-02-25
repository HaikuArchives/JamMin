//------------------------------------------------------------------------------
//	Copyright (c) 2003 Guido Casiraghi
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
//	File Name:		JamMinWindow.cpp
//	Author(s):		Guido Casiraghi
//	Description:	
//------------------------------------------------------------------------------


//TODO
// - aggiungere tutte le finestre di dialogo necessarie - FATTO
// - aggiungere una finestra di log che avvisa delle operazione non riuscite, come aggiunta file...
// - doppio click elementi della lista
// - capire perche' "AddFileToProject" ha inserito "/boot/home/Desktop" 
// - implementare gestione zoom
// - va in crash se: da 'open' si sceglie un file che non e' corretto ("unflatten file error").
//   --> Crash quando si chiude l'applicazione


#include "JamMin.h"

//#include "JamMinWindow.h"
//#include "JamMinApp.h"
//#include "ProjFileItem.h"
//#include "debugMacros.h"
//#include <Entry.h>
//#include <Path.h>
//#include <Messenger.h>
//#include <FilePanel.h>
//#include <Alert.h>
//#include <ClassInfo.h> //per cast_as
//#include <FindDirectory.h> //per find_directory()
//#include <Roster.h>
////#include <TypeConstants.h> //for B_TYPE_RECT
//
//#include <stdio.h> //per printf, FILE
//#include <stdlib.h> //per malloc
//#include <string.h> //per strcmp
//#include <iostream.h>
//#include <cassert>

const char* kRectAttrName =	"JMWinRect";


//default constructor
JamMinWindow::JamMinWindow(const char *uWindowTitle)
:
	BWindow(
		BRect(64, 64, 320, 256),
		uWindowTitle,
		B_DOCUMENT_WINDOW,
		0	)
{
	//first of all, create empty TargetConfig object
	fConfig = new TargetConfig;
	fSaveMessage = NULL; //we still don't know where to save the configuration
	fStatus = B_OK;

	//initialize interface
	InitWindow();
	
	//XXX - should this be called here?
	Show();
}


JamMinWindow::JamMinWindow(entry_ref* ref) :
	BWindow(
	BRect(64, 64, 320, 256),
	"JamMan Window",
	B_DOCUMENT_WINDOW, B_NOT_CLOSABLE,
	0	)
{
	BFile file;
	BMessage archive;
	status_t ret;

	fConfig = NULL;
	fSaveMessage = NULL;
	fStatus = B_OK;
	
	//instantiate fConfig from BMessage stored in file that 'ref' points to
	if ( (ret = file.SetTo(ref, B_READ_ONLY)) != B_OK ) {
		DPRINT("JamMinWindow: opened ref unreadable");
		fStatus = ret;
		return;
	}
	ret = archive.Unflatten(&file);
	if (ret != B_OK) {
		DPRINT("JamMinWindow: unflatten file error");
		fStatus = ret;
		UnflattenErrorAlert();
		return;
	}
	BArchivable* unflatArchive = instantiate_object(&archive);
	if (!unflatArchive) {
		DPRINT("JamMinWindow: instantiate_object error");
		fStatus = B_ERROR;
		UnflattenErrorAlert();
		return;
	}
	fConfig = cast_as(unflatArchive, TargetConfig);
	if (!fConfig) {
		DPRINT("JamMinWindow: casting from BArchivable to TargetConfig error");
		fStatus = B_ERROR;
		UnflattenErrorAlert();
		return;
	}
	#ifdef DEBUG
	fConfig->PrintToStream();
	#endif
	
	//save file name and path
	fSaveMessage = new BMessage(B_SAVE_REQUESTED);
	if (!fSaveMessage) {
		DPRINT("JamMinWindow: fSaveMessage == NULL");
		fStatus = B_NO_MEMORY;
		return;
	}
	BEntry entry(ref, true); // XXX - non symlinks!?
	BEntry parent;
	entry_ref parent_ref;
	char name[B_FILE_NAME_LENGTH];
	entry.GetParent(&parent);
	entry.GetName(name);
	parent.GetRef(&parent_ref);
	fSaveMessage->AddRef("directory", &parent_ref);
	fSaveMessage->AddString("name", name);
	DPRINT("JamMinWindow: fSaveMessage name = " << name);
	
	//set working directory in target configuration
	fConfig->SetWorkingDir(GetWorkingDirPath().Path());
	DPRINT("JamMinWindow: fConfig working dir " << GetWorkingDirPath().Path());
	
	//
	BRect frame;
	ssize_t size;
	size = file.ReadAttr(kRectAttrName, B_RECT_TYPE, 0, static_cast<void*>(&frame), sizeof(frame));
	if (size == sizeof(frame)) { //reading file attribute was succesfull
		DPRINT("JamMinWindow: setting frame");
		frame.PrintToStream();
		MoveTo(frame.left, frame.top);
		ResizeTo(frame.Width(), frame.Height());
	}
	
	SetTitle(name);
	
	InitWindow();
	
	SetPanelDirs();
	
	DPRINT("JamMinWindow: Show()");
	Show();
}


JamMinWindow::~JamMinWindow()
{
	DPRINT("JamMinWindow::~JamMinWindow");

	Unregister();
	//detach views before deleting them
	//BWindow::RemoveChild(fMenubar);
	//BWindow::RemoveChild(fFileListView);
//	fMenubar->RemoveSelf();
//	fFileListView->RemoveSelf();
//	
//	
//	delete fConfig;
//	delete fSaveMessage;
//	delete fMenubar;	//from be book: Frees all the items and submenus in the entire
//						//menu hierarchy, and all memory allocated by the BMenuBar	
//	
//	//delete listview objects
//	int32 nItems = fFileListView->FullListCountItems();
//	BListItem* item;
//	for (int32 i= 0; i < nItems; i++) {
//		item = fFileListView->FullListItemAt(i);
//		delete item;
//	}
//	delete fSourceGroup;
//	delete fResourceGroup;
//	delete fLibraryGroup;
//	
	//delete fFileListView; //this crashes the app:
	
	delete fSavePanel;
	delete fExportPanel;
	delete fOpenPanel;
	delete fOpenPanel2;
}

void
JamMinWindow::InitWindow()
{
	//create menus
	InitMenu();
	
	//create list view and groups
	InitFileList();

	//populate file list only if  we are
	//opening an existing project configuration

	if (fSaveMessage) PopulateFileList();
	

	//create fSettingsWin
	//fSettingsWin = new TargetSettingsWin("Target Settings");
	
	//fSavePanel
	fSavePanel = new BFilePanel(B_SAVE_PANEL, new BMessenger(this), NULL, B_FILE_NODE, false);
	BMessage exportMess(JAMFILE_REF_RECEIVED);
	fExportPanel = new BFilePanel(B_SAVE_PANEL, new BMessenger(this), NULL, B_FILE_NODE, false, &exportMess);
	fOpenPanel = new BFilePanel;
	fOpenPanel->SetTarget(this); //this window is the target of "add files..." menu
	fOpenPanel2 = new JMOpenFilePanel;
	fOpenPanel2->SetTarget(this);
	
	
	//Register...
	Register();

	//don't call Show() here
}


void
JamMinWindow::InitMenu()
{
	/* create menu and attach to window */
	fMenubar = new BMenuBar(BRect(0,0,0,0), "fMenubar"); //NOTE: this view is automatic
	BWindow::AddChild(fMenubar);
	/***** create  file menu *****/
	BMenuItem* item;
	BMenu* filemenu = new BMenu("File");
	fMenubar->AddItem(filemenu);
	filemenu->AddItem(new BMenuItem("New Target" B_UTF8_ELLIPSIS, new BMessage(MENU_FILE_NEW_TARGET), 'N'));
	filemenu->AddItem(item = new BMenuItem("Open" B_UTF8_ELLIPSIS, new BMessage(MENU_FILE_OPEN), 'O'));
	item->SetTarget(be_app); //we want be_app to menage 'open' messages
	
	filemenu->AddSeparatorItem();
	
	filemenu->AddItem(new BMenuItem("Save As" B_UTF8_ELLIPSIS, new BMessage(MENU_FILE_SAVEAS), 'S'));
	filemenu->AddItem(new BMenuItem("Export Jamfile" B_UTF8_ELLIPSIS, new BMessage(MENU_FILE_EXPORT_JAMFILE), 'E'));
	filemenu->AddItem(new BMenuItem("Close", new BMessage(MENU_FILE_CLOSE)));

	filemenu->AddSeparatorItem();
	
	filemenu->AddItem(new BMenuItem("Quit", new BMessage(MENU_FILE_QUIT)));
			
	/***** create target menu  *****/
	BMenu* targetmenu = new BMenu("Target");
	fMenubar->AddItem(targetmenu);
	targetmenu->AddItem(new BMenuItem("Add Files" B_UTF8_ELLIPSIS, new BMessage(MENU_TARGET_ADD_FILES)));

	targetmenu->AddSeparatorItem();
	
	targetmenu->AddItem(new BMenuItem("Remove Selected Items", new BMessage(MENU_TARGET_REMOVE_ITEMS)));
	targetmenu->AddSeparatorItem();	
	targetmenu->AddItem(new BMenuItem("Settings" B_UTF8_ELLIPSIS, new BMessage(MENU_TARGET_SETTINGS)));
}


void
JamMinWindow::InitFileList()
{
	BRect r(Bounds());

	r.right -= B_V_SCROLL_BAR_WIDTH;
	r.bottom -= B_V_SCROLL_BAR_WIDTH;
	r.top = fMenubar->Bounds().Height() + 1; //subtract fMenubar height	

	fFileListView = new BOutlineListView(r, "target_file_list",
		B_MULTIPLE_SELECTION_LIST, B_FOLLOW_ALL);
	
	//add groups
	fFileListView->AddItem( fSourceGroup = new BStringItem("Source Files") );
	fFileListView->AddItem( fLibraryGroup = new BStringItem("Libraries") );
	fFileListView->AddItem( fResourceGroup = new BStringItem("Resource Files") );
//	fFileListView->AddItem( fSysLibGroup = new BStringItem("User Libraries") );
//	fFileListView->AddItem( fUserLibGroup = new BStringItem("System Libraries") );
//	fFileListView->AddItem( fOtherGroup = new BStringItem("Other Files") );

	//set the message that is sent to the window when the user double-clicks an item
	fFileListView->SetInvocationMessage(new BMessage(LISTVIEW_FILE_INVOKED)); //the message belongs to the view
	//create the scroll view and add it
	fScrollFileListView = new BScrollView("scroll_file_view", fFileListView,
		B_FOLLOW_ALL, 0, true, true);
	BWindow::AddChild(fScrollFileListView);
}


void
JamMinWindow::PopulateFileList()
{
	assert(fSaveMessage != NULL); //it must not be NULL here
	assert(fConfig != NULL);
	
	const char* archivedName;
	int32 nFiles;
	
	//source files
	nFiles = fConfig->GetHowManySources();
	for ( int32 i = 0; i < nFiles; i++ ) {
		archivedName = fConfig->GetSourceAt(i);
		AddArchivedName(archivedName, fSourceGroup, &TargetConfig::RemSourceFile);
	}
	//resource files
	nFiles = fConfig->GetHowManyResources();
	for ( int32 i = 0; i < nFiles; i++ ) {
		archivedName = fConfig->GetResourceAt(i);
		AddArchivedName(archivedName, fResourceGroup, &TargetConfig::RemResourceFile);
	}
	//libraries
	nFiles = fConfig->GetHowManyLibraries();
	for ( int32 i = 0; i < nFiles; i++ ) {
		archivedName = fConfig->GetLibAt(i);
		AddArchivedName(archivedName, fLibraryGroup, &TargetConfig::RemLibrary);
	}
	//other files
	//XXX - to be finished...
}


void JamMinWindow::MessageReceived(BMessage * Message)
{
	switch(Message->what)
	{
		case MENU_FILE_NEW_TARGET:
			DPRINT("JamMinWindow::MessageReceived: MENU_FILE_NEW_TARGET");
			new JamMinWindow("Untitled");
			break;
		case MENU_FILE_OPEN:
			DPRINT("JamMinWindow::MessageReceived: MENU_FILE_OPEN");
			break;
		case MENU_FILE_SAVEAS:
			DPRINT("JamMinWindow::MessageReceived: MENU_FILE_SAVEAS");
			fSavePanel->Show();
			break;
		case MENU_FILE_EXPORT_JAMFILE:
			DPRINT("JamMinWindow::MessageReceived: MENU_FILE_EXPORT_JAMFILE");
			if (!fSaveMessage) {
				ShowSaveAlert();
			} else {
				fExportPanel->Show();
			}
			break;
		case MENU_FILE_CLOSE:
			DPRINT("JamMinWindow::MessageReceived: MENU_FILE_CLOSE");
			Quit();
			break;
		case MENU_FILE_QUIT:
			DPRINT("JamMinWindow::MessageReceived: MENU_FILE_QUIT");
			be_app->PostMessage(B_QUIT_REQUESTED);
			break;
		case MENU_TARGET_ADD_FILES:
			DPRINT("JamMinWindow::MessageReceived: MENU_TARGET_ADD_FILES");
			if (!fSaveMessage) {
				ShowSaveAlert();
			} else {
				//fOpenPanel->Show();
				fOpenPanel2->Show();
			}
			break;
		case MENU_TARGET_REMOVE_ITEMS:
			DPRINT("JamMinWindow::MessageReceived: MENU_TARGET_REMOVE_ITEMS");
			//we can remove something only if we have already added something, i.e.
			//only if have ^at least^ saved the configuration
			if (fSaveMessage != NULL) {
				RemoveSelectedItems();
			}
			break;
		case MENU_TARGET_BUILD:
			DPRINT("JamMinWindow::MessageReceived: MENU_TARGET_BUILD");
			break;
		case MENU_TARGET_SETTINGS:
			DPRINT("JamMinWindow::MessageReceived: MENU_TARGET_SETTINGS");
			if (!fSaveMessage) {
				ShowSaveAlert();
			} else {
				fSettingsWin = new TargetSettingsWin("Target Settings", fConfig);
			}
			break;
		case B_SAVE_REQUESTED:
			DPRINT("JamMinWindow::MessageReceived: B_SAVE_REQUESTED");
			Save(Message);
			break;
		case JAMFILE_REF_RECEIVED:
			DPRINT("JamMinWindow::MessageReceived: JAMFILE_REF_RECEIVED");
			ExportJamFile(Message);
			break;
		case B_REFS_RECEIVED:
		{	//this is to add files to project
			DPRINT("JamMinWindow::MessageReceived: B_REFS_RECEIVED");
		
			entry_ref ref;
			status_t err;
			int32 ref_num;
			ref_num = 0;
			bool relpath = false; //default if something goes wrong
			
			err =  Message->FindBool("relpath", &relpath);
			if (err != B_OK) {
				DPRINT("message B_REFS_RECEIVED: boolean not found");
			}
			while ( (err = Message->FindRef("refs", ref_num, &ref)) == B_OK ) {
				DPRINT("ref " << ref_num << " received");
				//insert this ref to project
				AddToProject(&ref, relpath);
				ref_num++;
			}	
			break;		
		}
		case LISTVIEW_FILE_INVOKED:
			DPRINT("JamMinWindow::MessageReceived: LISTVIEW_FILE_INVOKED");
			OpenSelectedItem();
			break;
		case B_SIMPLE_DATA:
			DPRINT("JamMinWindow::MessageReceived: B_SIMPLE_DATA");
			//handle drag'n'drop
			HandleDragMessage(Message);
			break;
		default:
			//DPRINT("JamMinWindow::MessageReceived: other message");
			BWindow::MessageReceived(Message);
			break;
	} //switch
}


void
JamMinWindow::HandleDragMessage(BMessage* message)
{
	entry_ref ref;
	status_t err;
	int32 ref_num = 0;
	do {
		if ( (err = message->FindRef("refs", ref_num, &ref)) != B_OK ) {
			return;
		}
		DPRINT("JamMinWindow::HandleDragMessage: ref " << ref_num << " received");
		if (fSaveMessage) //we can add files only if the project has been already saved
			AddToProject(&ref, true); //store relative path by default for dragged files
		ref_num++;
	} while (1);		
}


//XXX - useless - to be removed
void JamMinWindow::FrameResized(float width, float height)
{
	BWindow::FrameResized(width, height);
}


void
JamMinWindow::ShowSaveAlert()
{
	BAlert* saveNowAlert = new BAlert("save before add",
		"You must save the project before adding files to it. Save now?",
		"No", "Yes", NULL,
		B_WIDTH_AS_USUAL, B_EVEN_SPACING, B_INFO_ALERT);
	saveNowAlert->SetShortcut(0, B_ESCAPE);
	int32 button_index = saveNowAlert->Go();
	switch (button_index) {
		case 0:
			break;
		case 1:
			fSavePanel->Show();
			break;
		default:
			break;
	}
}


bool JamMinWindow::QuitRequested()
{
	DPRINT("JamMinWindow::QuitRequested");
	/*******************************************************************************************
	if (!fSaveMessage) {
		DPRINT("JamMinWindow::QuitRequested: quitting without saving?");
		//we show the classic "Save/Don't Save/Cancel" dialog window		
		BAlert* saveAlert = new BAlert("title", "Save changes to Untitled?",
			"Cancel", "Don't save", "Save", B_WIDTH_AS_USUAL, B_OFFSET_SPACING, B_WARNING_ALERT);
		saveAlert->SetShortcut(0, B_ESCAPE); //see Be Book
		int32 button_index = saveAlert->Go();
		switch (button_index) {
			case 0:
				DPRINT("JamMinWindow::QuitRequested: Cancel pressed");
				return false;
				break;
			case 1:
				DPRINT("JamMinWindow::QuitRequested: Don\'t save pressed");
				return true;
				break;
			case 2:
				DPRINT("JamMinWindow::QuitRequested: Save pressed");
				fSavePanel->Show();
				return false;
				break;
			default:
				//we should never be here
				return false;
				break;			
		}
		
	} else {
		//simply return true if we have a valid fSaveMessage, destructor unregisters this window
		//but first save!
		//XXX - saving here is only needed if configuration has changed through fTargetSettingsWin.
		//find another way??
		Save(NULL);
		return true;
	}
	*******************************************************************************************/
	//we don't need to save anything. Configuration file is saved each time the user modifies something.
	return true;
}


/* virtual */ void
JamMinWindow::Zoom(BPoint origin, float width, float height)
{
	int32 nItems = fFileListView->CountItems();
	float h = fSourceGroup->Height() * nItems + fMenubar->Frame().Height() + B_V_SCROLL_BAR_WIDTH + 5;
	float w = fSourceGroup->Width() + B_V_SCROLL_BAR_WIDTH;
	//DPRINT("JamMinWindow::Zoom: " << items << " " << fSourceGroup->Height() << " " << fSourceGroup->Width() );
	//BListItem* item;
	//const char* text;
	//int len, maxlen = 0;
	//for (int i = 0; i < nItems; i++) {
	//	len = strlen( fFileListView->ItemAt(i)->Text() );
	//}
	
	//XXX - to be finished
	
	//ResizeTo(w, h);
}


//XXX - check return status
status_t
JamMinWindow::Save(BMessage* message)
{
	DPRINT("JamMinWindow::Save");
	entry_ref ref;
	status_t err;
	const char* name;
	BPath path;
	BEntry entry;
	BMessage archive;
	BFile file;
	
	// if a NULL is passed we use the cached info
	if (!message) {
		message = fSaveMessage;
		if (!message) {
			DPRINT("JamMinWindow::Save: fSaveMessage == NULL");
			return B_ERROR;
		}
	}
	
	if ( (err = message->FindRef("directory", &ref)) != B_OK ) return err;	
	if ( (err = message->FindString("name", &name)) != B_OK ) return err;
	if ( (err = entry.SetTo(&ref)) != B_OK ) return err;	
	
	entry.GetPath(&path);
	path.Append(name);

	DPRINT("JamMinWindow::Save: file to be saved = " << path.Path());
	
	//archive target config into 'archive' BMessage
	//DPRINT("JamMinWindow::Save: fConfig before archiving");
	//fConfig->PrintToStream();
	err = fConfig->Archive(&archive);
	if (err != B_OK) {
		return err;
	}
	
	//save to file
	if ( (err = file.SetTo(path.Path(), B_WRITE_ONLY | B_CREATE_FILE)) == B_OK ) {
		err = archive.Flatten(&file);
		if (err != B_OK) {
			DPRINT("JamMinWindow::Save: Flatten() failed");
			return err;
		}
		//save the BRect of this window as a file attribute
		BRect frame = Frame();
		file.WriteAttr(kRectAttrName, B_RECT_TYPE, 0, &frame, sizeof(BRect));
	}
	SetTitle(name);
	//is this the first time that we save this project?
	if (!fSaveMessage) {
		fSaveMessage = new BMessage(*message);
		SetPanelDirs();
	} else if (fSaveMessage != message) {
		//if fSaveMessage not null, we are saving to a new file
		delete fSaveMessage;
		fSaveMessage = new BMessage(*message);
		SetPanelDirs();
	}
	//XXX - it is not necessary to call the following two functions each time the file is saved
	fConfig->SetWorkingDir(GetWorkingDirPath().Path());
	DPRINT("JamMinWindow::Save(): fConfig working dir " << GetWorkingDirPath().Path());
	return err;	
}


void
JamMinWindow::SetPanelDirs()
{
	//set directory of all panels to the working dir
	assert(fSaveMessage != NULL); //configuration must be already saved
	BPath wd(GetWorkingDirPath());
	fOpenPanel2->SetPanelDirectory(wd.Path());
	fExportPanel->SetPanelDirectory(wd.Path());
	fSavePanel->SetPanelDirectory(wd.Path());
}


void
JamMinWindow::Register()
{
	BMessenger messenger(ApplicationSignature);
	BMessage message(WINDOW_REGISTRY_ADD);
	messenger.SendMessage(&message, this);
}


void
JamMinWindow::Unregister()
{
	BMessenger messenger(ApplicationSignature);
	messenger.SendMessage(new BMessage(WINDOW_REGISTRY_SUB));
}


/* AddToProject: called when users choose "Target/Add Files..." menu
 */
void
JamMinWindow::AddToProject(entry_ref* ref, bool relpath)
{
	status_t err;
	entry_ref parent_ref;
	entry_ref working_dir_ref;
	BEntry parent;
	BEntry workingDir;
	BPath fullPath;
	BString	archName;
	char name[B_FILE_NAME_LENGTH];
	
	if (!fSaveMessage) {
		DPRINT("JamMinWindow::AddToProject: fSaveMessage is null!");
		return;
	}
	//get file name and parent directory from 'ref'	
	BEntry entry(ref, true); // XXX - non symlinks!?
	if (!entry.Exists() || !entry.IsFile()) {
		//something strange happened, or this is not a file
		DPRINT("JamMinWindow::AddToProject: ref entry doesn't exist or is not a file!");
		return;
	}
	fullPath.SetTo(&entry);
	archName = fullPath.Path();
	entry.GetParent(&parent);
	entry.GetName(name);
	//get working dir for this target, i.e. 'directory' saved in fSaveMessage

	BPath wdPath, parentPath;
	wdPath = GetWorkingDirPath();
	parentPath.SetTo(&parent);
	DPRINT("working dir = " << wdPath.Path());
	DPRINT("parent dir = " << parentPath.Path());
	
	//we now test if this is a file that we can add to the project
	//first we find ^file name extension^
	//XXX - use Storage Kit functions instead

	BString fileName(name);
	BString nameSuffix;
	err = GetFileExtension(&nameSuffix, &fileName);
	if (err != B_OK) {
		//file name extension not found. It is required
		return;
	}
	//if the user is adding a system library and she wants to store relative path,
	//probably this is a mistake. Ask her to confirm
	if (relpath && IsSystemLibraryPath(&parentPath)) {
		relpath = SysLibAlert();
	}
	
	//check if the user wants the path to be stored as relative to working dir
	if (relpath) {
		DPRINT("JamMinWindow::AddToProject: user wants relative path");
		fConfig->RelPathToWorkingDir(fullPath.Path(), &archName);
	} else {
		DPRINT("JamMinWindow::AddToProject: user wants absolute path")
	}
	
	DPRINT("JamMinWindow::AddToProject: adding name = " << name << ", archName = " << archName.String() << ", fullPath = "
		<< fullPath.Path())
	//XXX - we are only considering name ext here, we should also test MIME type	
	//compare nameSuffix and decide where to put it
	//case-insensitive compare
	if ( nameSuffix.ICompare("cpp") == 0 || nameSuffix.ICompare("c") == 0 || 
	     nameSuffix.ICompare("cxx") == 0) {
		//it is a source file
		DPRINT("JamMinWindow::AddToProject: 'ref' is source file");
		err = AddToTargetAndGroup(name, archName.String(), &fullPath, &TargetConfig::AddSourceFile,
			&TargetConfig::RemSourceFile, fSourceGroup);
		
	} else if ( nameSuffix.ICompare("rsrc") == 0 ) {
		//add to resource files
		DPRINT("JamMinWindow::AddToProject: 'ref' is resource file");
		err = AddToTargetAndGroup(name, archName.String(), &fullPath, &TargetConfig::AddResourceFile,
			&TargetConfig::RemResourceFile, fResourceGroup);
	} else if ( nameSuffix.ICompare("so") == 0 || nameSuffix.ICompare("a") == 0 ) {
		//this is a library
		DPRINT("JamMinWindow::AddToProject: 'ref' is library");
		err = AddToTargetAndGroup(name, archName.String(), &fullPath, &TargetConfig::AddLibrary,
			&TargetConfig::RemLibrary, fLibraryGroup);
	} else { //file extension unknown
		DPRINT("JamMinWindow::AddToProject: 'ref' extension unknown");			
		BAlert* errFileTypeAlert = new BAlert("unknown file extension", "Unknown file extension.",
			"Ok");
		errFileTypeAlert->Go();
	}// if name extension
}


status_t
JamMinWindow::AddToTargetAndGroup(
	const char* name,
	const char* archName,
	BPath* fullPath,
	status_t (TargetConfig::*AddFun)(const char*), 
	status_t (TargetConfig::*RemFun)(const char*), BListItem* group)
{
	status_t err;
	err = (fConfig->*AddFun)(archName);
	if (err != B_OK) return err;
	DPRINT("JamMinWindow::AddToTargetAndGroup: " << archName << " added to target conf");
	err = AddFileToGroup(name, archName, fullPath, group);
	if (err != B_OK) {
		(fConfig->*RemFun)(archName);
		Save(NULL);
		return err;
	}
	DPRINT("JamMinWindow::AddToTargetAndGroup: " << name << " added to target group");
	//save configuration and return
	return Save(NULL);
}


/* GetFileExtension:
 * returns B_OK if extension was found, B_ERROR otherwise
 */
status_t
JamMinWindow::GetFileExtension(BString* extension, const BString* fileName)
{
	assert(extension != NULL);
	assert(fileName != NULL);
	int32 nameLength = fileName->Length();
	int32 dotIndex = nameLength;
	
	while (fileName->ByteAt(dotIndex) != '.' && dotIndex >= 0) {
		dotIndex--;
	}
	
	if (dotIndex <= 0) {
		DPRINT("JamMinWindow::GetFileExtension: name extension not found");
		return B_ERROR;
	}
	//copy 'fileName' extension into 'extension' string
	fileName->CopyInto(*extension, dotIndex + 1, nameLength - dotIndex);
	DPRINT("File extension = " << extension->String());
	return B_OK;
}


void
JamMinWindow::RemoveSelectedItems()
{
	DPRINT("JamMinWindow::RemoveSelectedItems()");
	//for each group call "RemoveSelectedGroupItems"
	RemoveSelectedGroupItems(fSourceGroup);
	RemoveSelectedGroupItems(fResourceGroup);
	RemoveSelectedGroupItems(fLibraryGroup);
//	RemoveSelectedGroupItems(fSysLibGroup);
//	RemoveSelectedGroupItems(fUserLibGroup);
//	RemoveSelectedGroupItems(fOtherGroup);
}


void
JamMinWindow::RemoveSelectedGroupItems(BListItem* group)
{
	assert(group != NULL);
	int32 nItems = fFileListView->CountItemsUnder(group, true);	//'true' means only one level.
																//'false' makes non sense here.
	DPRINT("JamMinWindow::RemoveSelectedGroupItems: nItems = " << nItems);
	BListItem* item;
	const char* archivedName;
	int32 i = 0;
	bool changedConfig = false;
	//for (int32 i = 0; i < nItems; i++) {
	while(i < nItems) {
		item = fFileListView->ItemUnderAt(group, true, i);
		DPRINT("JamMinWindow::RemoveSelectedGroupItems: ITEM at " << i);
		assert(item != NULL); //must be not NULL here
		if (item->IsSelected()) {
			//XXX - use C++ explicit cast here
			archivedName = ((ProjFileItem*)item)->GetArchivedName();
			assert(archivedName != NULL);
			//remove this name from configuration
			DPRINT("JamMinWindow::RemoveSelectedGroupItems(): removing ITEM " << i <<
				", file " << archivedName);
			fConfig->RemoveFile(archivedName);
			//remove the BListItem from this list view and free memory
			fFileListView->RemoveItem(item);
			//XXX - pay attention to this cast too!
			delete((ProjFileItem*)item);
			changedConfig = true;
			//the following instructions force the while loop to restart from index 0
			//XXX - this is not stricly necessary, must be optimized
			i = 0;
			nItems--;
		} else {
			//if this item is not selected, go to the next one
			i++;
		}
	}
	//now we save the configuration if it has changed
	if (changedConfig) Save(NULL);
}


status_t
JamMinWindow::ExportJamFile(BMessage* message)
{
	//precondition: jamfile must be in the working dir
	entry_ref ref;
	status_t err;
	const char* name;
	BPath path;
	BEntry entry;

	if ( (err = message->FindRef("directory", &ref)) != B_OK ) return err;	
	if ( (err = message->FindString("name", &name)) != B_OK ) return err;
	if ( (err = entry.SetTo(&ref)) != B_OK ) return err;	
	
	entry.GetPath(&path);
	//now test if path is the working dir
	if ( strcmp(path.Path(), GetWorkingDirPath().Path()) != 0) { //alert the user and return error
		BAlert* jamfileAlert = new BAlert("jamfile alert", "You must export the jamfile to the working directory.",
			"Ok");
		jamfileAlert->Go();

		return B_ERROR;
	}
	
	path.Append(name);

	DPRINT("JamMinWindow::ExportJamFile: file to be saved = " << path.Path());
	JamFile file(path.Path(), fConfig);
	if ( (err = file.InitCheck()) != B_OK ) {
		DPRINT("JamMinWindow::ExportJamFile: Jamfile InitCheck error");
		return err;
	}
	if ( (err = file.PrintToFile()) != B_OK ) {
		DPRINT("JamMinWindow::ExportJamFile: Jamfile PrintToFile error");
		return err;
	}
	return B_OK;
}


BPath
JamMinWindow::GetWorkingDirPath()
{
	assert(fSaveMessage != NULL); //it must not be NULL at this point
	entry_ref working_dir_ref;
	BEntry workingDir;
	BPath wdPath;
	
	status_t err = fSaveMessage->FindRef("directory", &working_dir_ref);
	if (err != B_OK) {
		DPRINT("JamMinWindow::InitFileList: fSaveMessage has no directory");
		return BPath(); //empty path XXX - this can be  dangerous?
	}
	
	workingDir.SetTo(&working_dir_ref);
	wdPath.SetTo(&workingDir);
	return wdPath;
}


/* AddFileToGroup:
 * It is called 1) from 'AddToProject' or 2) from 'PopulateFileList'. In either cases we must 
 * assure that the entry exists. Otherwise we don't add it and return B_ERROR. Callers handle this 
 * return value differently
 */
status_t
JamMinWindow::AddFileToGroup(const char* name, const char* archName, BPath* fullPath, BListItem* group)
{
	assert(group != NULL);
	DPRINT("JamMinWindow::AddFileToGroup: adding " << archName << ", full path = " << fullPath->Path());
	//now we can add the item to the group
	fFileListView->AddUnder( new ProjFileItem(name, archName, fullPath), group );
	
	return B_OK;
}


status_t
JamMinWindow::AddArchivedName(
	const char* archName,
	BListItem* group,
	status_t (TargetConfig::*RemFun)(const char*))
{
	//XXX - this function is too complicated
	//check if is relative path
	BPath filePath;
	status_t ret;

	if (IsRelativePath(archName)) {
		//build absoulte path from working dir 
		BString pathString;
		BPath wdPath = GetWorkingDirPath();
		pathString = wdPath.Path();
		pathString += "/";
		pathString += archName;
		filePath.SetTo( pathString.String(), NULL, true ); //enable 'normalize'	
	} else {
		filePath.SetTo(archName); //the path must be already normalized
	}
	if ( (ret = filePath.InitCheck()) != B_OK ) {
		DPRINT("JamMinWindow::AddArchivedName: file path error: " << filePath.Path());
		UnarchiveNameErrorAlert(filePath.Path());
		(fConfig->*RemFun)(archName);
		Save(NULL);
		return ret;
	}
	//we must assure here that the entry exists (file can be removed, or configuration
	//is corrupted...
	BEntry fileEntry(filePath.Path());
	//XXX - if 'entry' is a directory something strange happened
	//XXX - symbolic links are not allowed. Should this be fixed
	if ( !fileEntry.Exists() || fileEntry.IsDirectory() || fileEntry.IsSymLink() ) {
		DPRINT("JamMinWindow::AddArchivedName: file entry error: " << filePath.Path());
		UnarchiveNameErrorAlert(filePath.Path());
		(fConfig->*RemFun)(archName);
		Save(NULL);
		return B_ERROR;
	}
	DPRINT("JamMinWindow::AddArchivedName: unarchived name = " << filePath.Path());
	
	return AddFileToGroup(filePath.Leaf(), archName, &filePath, group);
}


bool
JamMinWindow::IsRelativePath(const char* path)
{
	if (*path == '/')
		return false;
	else
		return true;
}


bool
JamMinWindow::IsSystemLibraryPath(BPath* libPath)
{
	//use 'find_directory' to check if this is a path for
	//system libraries
	status_t ret;
	BPath path;
	
	ret = find_directory(B_BEOS_LIB_DIRECTORY, &path);
	//if (ret == B_OK) {
		if ( strcmp(libPath->Path(), path.Path()) == 0 ) return true;
	//}
	ret = find_directory(B_COMMON_LIB_DIRECTORY, &path);
	if (ret == B_OK) {
		if ( strcmp(libPath->Path(), path.Path()) == 0 ) return true;
	}
	
	ret = find_directory(B_USER_LIB_DIRECTORY, &path); //the same as B_COMMON_LIB_DIRECTORY
	if (ret == B_OK) {
		if ( strcmp(libPath->Path(), path.Path()) == 0 ) return true;
	}
	//if we are here none of the precedent paths was valid
	DPRINT("JamMinWindow::IsSystemLibraryPath: not a system path");
	return false;
}


bool
JamMinWindow::SysLibAlert() {
	bool ret = false;
	BAlert* sysLibAlert = new BAlert("syslib alert",
		"It seems that you are adding a system library. You should not store the relative path. Store absolute path instead?",
		"No", "Yes", NULL,
		B_WIDTH_AS_USUAL, B_EVEN_SPACING, B_INFO_ALERT);
	sysLibAlert->SetShortcut(0, B_ESCAPE);
	int32 button_index = sysLibAlert->Go();
	switch (button_index) {
		case 0: //No
			ret = true;
			break;
		case 1: //Yes
			ret = false; //relpath = false
			break;
		default:
			break;
	}
	return ret;
}


void
JamMinWindow::UnflattenErrorAlert()
{
	BAlert* unflatErrorAlert = new BAlert("unflatten file error", "This is not a valid configuration file.",
		"Ok");
	unflatErrorAlert->Go();
}


void
JamMinWindow::UnarchiveNameErrorAlert(const char* path)
{
	BString alert = "It is not possible to add file ";
	alert += path;
	alert += ".";
	BAlert* unarchErrorAlert = new BAlert("unarchive name error", alert.String(),
		"Ok");
	unarchErrorAlert->Go();

}

void
JamMinWindow::OpenSelectedItem()
{
	DPRINT("JamMinWindow::OpenSelectedItem");
	int32 i = fFileListView->FullListCurrentSelection();
	if (i < 0) return; //no file is selected, something strange happened
	BListItem* item = fFileListView->FullListItemAt(i);
	BListItem* super = fFileListView->Superitem(item);
	if (super == NULL) return; //the item is at the outermost level of the outline, it can't be opened
	//now check if this is a group that we can invoke
	if (super == fSourceGroup || super == fResourceGroup) {
		ProjFileItem* projItem = dynamic_cast<ProjFileItem*>(item);
		if (projItem) { 
			entry_ref fileRef;
			BEntry entry(projItem->GetFilePath());
			if (entry.InitCheck() != B_OK) return;
			entry.GetRef(&fileRef);
			be_roster->Launch(&fileRef);
		}
	}
}
