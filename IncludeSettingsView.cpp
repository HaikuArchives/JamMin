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
//	File Name:		IncludeSettingsView.cpp
//	Author(s):		Guido Casiraghi
//	Description:	
//------------------------------------------------------------------------------

//TODO
// - implementare la conversione path assoluto/relativo in "AddRefsToList"


#include "JamMin.h"

//#include "TargetSettingsWin.h"
//#include "debugMacros.h"

//XXX - to be moved into a shared header
const uint32 INCLUDE_SETTINGS_CHANGED = 'isch';
//messages for this view
const uint32
	ADD_SYS_PATH_BUTTON		= 'aspb',
	REMOVE_SYS_PATH_BUTTON	= 'rspb',
	ADD_LOCAL_PATH_BUTTON	= 'alpb',
	REMOVE_LOCAL_PATH_BUTTON	= 'rlpb',
	SYS_PATH_REFS_RECEIVED	= 'sprr',
	LOCAL_PATH_REFS_RECEIVED	= 'lprr';

//constants for GUI
const float
	kDistanceFromBorder		= 20.0,
	kControlHeight			= 30.0,
	kDividerWidth			= 120.0,
	kControlsDistance		= 15.0,
	kButtonWidth			= 60.0,
	kButtonHeight			= 30.0;
	
const char 
	*kAddSysPathString = "Additional paths for system headers"
		" (in the form: #include <header>)",
	*kAddLocalPathString = "Additional paths for local headers"
		" (in the form: #include \"header\")";


IncludeSettingsView::IncludeSettingsView(BRect frame, const char *name,
			uint32 resizingMode = B_FOLLOW_ALL, uint32 flags = B_WILL_DRAW)
//XXX - check resizingMode and flags
	:
	BView(frame, name, resizingMode, flags)	
{
	//initialize user interface

	//background color
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	//////////////////// system headers //////////////////////////////////////////
	//fAddSysPathString
	BRect r = Bounds();
	r.left += kDistanceFromBorder;
	r.right -= kDistanceFromBorder;
	r.bottom = r.top + kControlHeight;
	fAddSysPathString = new BStringView(r, "add sys path string", kAddSysPathString);
	AddChild(fAddSysPathString);

	//fSysPathList
	r.top = r.bottom + kControlsDistance;
	r.bottom = r.top + 3 * kControlHeight;
	r.right -= kButtonWidth + kDistanceFromBorder + kControlsDistance;
	fSysPathList = 	new BListView(r, "sys path list", B_SINGLE_SELECTION_LIST);
	fSysPathScroll = new BScrollView("sys path scroll", fSysPathList,
		B_FOLLOW_LEFT | B_FOLLOW_TOP, 0, true, true);
	AddChild(fSysPathScroll);
	//bottons
	//fAddSysButton
	r.left = r.right + kControlsDistance + B_V_SCROLL_BAR_WIDTH;
	r.right = r.left + kButtonWidth;
	r.bottom = r.top + kButtonHeight;
	fAddSysButton = new BButton(r, "add sys path button", "Add",
		new BMessage(ADD_SYS_PATH_BUTTON));
	AddChild(fAddSysButton);
	//fRemoveSysButton
	r.top = r.bottom + kControlsDistance;
	r.bottom = r.top + kButtonHeight;
	fRemoveSysButton = new BButton(r, "remove sys path button", "Remove",
		new BMessage(REMOVE_SYS_PATH_BUTTON));
	AddChild(fRemoveSysButton);

	//////////////////// local headers //////////////////////////////////////////
	r = Bounds();
	r.left += kDistanceFromBorder;
	r.right -= kDistanceFromBorder;
	r.top = fSysPathList->Bounds().bottom + 4 * kControlsDistance + B_V_SCROLL_BAR_WIDTH;
	r.bottom = r.top + kControlHeight;
	fAddLocalPathString = new BStringView(r, "add local path string", kAddLocalPathString);
	AddChild(fAddLocalPathString);
	//fLocalPathList
	r.top = r.bottom + kControlsDistance;
	r.bottom = r.top + 3 * kControlHeight;
	r.right -= kButtonWidth + kDistanceFromBorder + kControlsDistance;
	fLocalPathList = 	new BListView(r, "local path list", B_SINGLE_SELECTION_LIST);
	fLocalPathScroll = new BScrollView("local path scroll", fLocalPathList,
		B_FOLLOW_LEFT | B_FOLLOW_TOP, 0, true, true);
	AddChild(fLocalPathScroll);
	//bottons
	//fAddLocalButton
	r.left = r.right + kControlsDistance + B_V_SCROLL_BAR_WIDTH;
	r.right = r.left + kButtonWidth;
	r.bottom = r.top + kButtonHeight;
	fAddLocalButton = new BButton(r, "add local path button", "Add",
		new BMessage(ADD_LOCAL_PATH_BUTTON));
	AddChild(fAddLocalButton);
	//fRemoveLocalButton
	r.top = r.bottom + kControlsDistance;
	r.bottom = r.top + kButtonHeight;
	fRemoveLocalButton = new BButton(r, "remove local path button", "Remove",
		new BMessage(REMOVE_LOCAL_PATH_BUTTON));
	AddChild(fRemoveLocalButton);
	
	//fSelectSysDirPanel
	fSelectSysDirPanel = new BFilePanel(
		B_OPEN_PANEL,     // It's an open dialog 
		NULL,             // we set panel target to "this" in AttachedToWindow function
		NULL,             // No specific wishes for where the dialog starts
		B_DIRECTORY_NODE, // You only can select directories
		false,            // No multiple selection
		new BMessage(SYS_PATH_REFS_RECEIVED), //XXX - are we the owner of this message?
		NULL,             // We want all dirs
		true,             // Modal panel
		true);            // Hide when done 

	//fSelectLocalDirPanel
	fSelectLocalDirPanel = new BFilePanel(
		B_OPEN_PANEL,     // It's an open dialog 
		NULL,             // we set panel target to "this" in AttachedToWindow function
		NULL,             // No specific wishes for where the dialog starts
		B_DIRECTORY_NODE, // You only can select directories
		false,            // No multiple selection
		new BMessage(LOCAL_PATH_REFS_RECEIVED), //XXX - are we the owner of this message?
		NULL,             // We want all dirs
		true,             // Modal panel
		true);            // Hide when done 
		
	fSelectSysDirPanel2 = new JMOpenFilePanel(new BMessage(SYS_PATH_REFS_RECEIVED), B_DIRECTORY_NODE);
	fSelectLocalDirPanel2 = new JMOpenFilePanel(new BMessage(LOCAL_PATH_REFS_RECEIVED), B_DIRECTORY_NODE);
}


IncludeSettingsView::~IncludeSettingsView()
{
	//XXX - delect objects
}


void
IncludeSettingsView::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case ADD_SYS_PATH_BUTTON:
			DPRINT("IncludeSettingsView::MessageReceived: ADD_SYS_PATH_BUTTON");
			//fSelectSysDirPanel->Show();
			fSelectSysDirPanel2->Show();
			break;
		case REMOVE_SYS_PATH_BUTTON:
			DPRINT("IncludeSettingsView::MessageReceived: REMOVE_SYS_PATH_BUTTON");
			RemoveSelectedItem(fSysPathList);
			break;
		case ADD_LOCAL_PATH_BUTTON:
			DPRINT("IncludeSettingsView::MessageReceived: ADD_LOCAL_PATH_BUTTON");
			//fSelectLocalDirPanel->Show();
			fSelectLocalDirPanel2->Show();
			break;
		case REMOVE_LOCAL_PATH_BUTTON:
			DPRINT("IncludeSettingsView::MessageReceived: REMOVE_LOCAL_PATH_BUTTON");
			RemoveSelectedItem(fLocalPathList);
			break;
		case SYS_PATH_REFS_RECEIVED:
			DPRINT("IncludeSettingsView::MessageReceived: SYS_PATH_REFS_RECEIVED");
			AddRefsToList(fSysPathList, message);
			break;
		case LOCAL_PATH_REFS_RECEIVED:
			DPRINT("IncludeSettingsView::MessageReceived: LOCAL_PATH_REFS_RECEIVED");
			AddRefsToList(fLocalPathList, message);
			break;
		default:
			BView::MessageReceived(message);
			break;
	}
}


void
IncludeSettingsView::AttachedToWindow()
{
	DPRINT("IncludeSettingsView::AttachedToWindow()");	
	BMessenger thisMess(this); //messenger's target is "this" object
	fSelectSysDirPanel->SetTarget(thisMess);
	fSelectLocalDirPanel->SetTarget(thisMess);

	fSelectSysDirPanel2->SetTarget(thisMess);
	fSelectLocalDirPanel2->SetTarget(thisMess);

	fAddSysButton->SetTarget(this);
	fRemoveSysButton->SetTarget(this);
	fAddLocalButton->SetTarget(this);
	fRemoveLocalButton->SetTarget(this);

}


void
IncludeSettingsView::SetSysPaths(const char* paths)
{
	PopulatePathList(fSysPathList, paths);
}


void
IncludeSettingsView::SetLocalPaths(const char* paths)
{
	PopulatePathList(fLocalPathList, paths);
}


BString
IncludeSettingsView::GetSysPaths()
{
	return GetListContent(fSysPathList);
}


BString	
IncludeSettingsView::GetLocalPaths()
{
	return GetListContent(fLocalPathList);	
}


BString
IncludeSettingsView::GetListContent(BListView* list)
{
	BString pathString("");
	BListItem* item;
	int32 nItems = list->CountItems();
	for (int32 i = 0; i < nItems; i++) {
		item = list->ItemAt(i);
		pathString += ((BStringItem*)item)->Text();
		pathString += " ";
	}
	return pathString;
}


void
IncludeSettingsView::AddRefsToList(BListView* list, BMessage* message, bool relative = false)
{
	//XXX - it is possible for now to select only one directory each time.
	entry_ref ref;
	BEntry entry;
	BPath path;
	BString pathString;
	status_t err;
	int32 ref_num;
	bool relpath = false;
	ref_num = 0;
	//find boolean in the message
	err =  message->FindBool("relpath", &relpath);
	if (err != B_OK) {
		DPRINT("IncludeSettingsView::AddRefsToList: boolean not found");
	}

	//add this entry to the list view
	do {
		if ( (err = message->FindRef("refs", ref_num, &ref)) != B_OK ) {
			return;
		}
		entry.SetTo(&ref, true);
		//we return if for some strange reason the entry is unavailable
		if (entry.InitCheck() != B_OK) return;
		entry.GetPath(&path);
		//XXX - if "relative" convert path
		DPRINT("IncludeSettingsView::AddRefsToList: entry name = " << path.Path());
		//convert path to relative if requested
		if (relpath) {
			//find the window we are attached to...
			DPRINT("IncludeSettingsView::AddRefsToList: add relative path");
			TargetConfig* c = ((TargetSettingsWin*) Window() )->TConfig();
			assert(c != NULL); //we *must* be attached to a window here
			c->RelPathToWorkingDir(path.Path(), &pathString);
		} else {
			DPRINT("IncludeSettingsView::AddRefsToList: add absolute path");
			pathString = path.Path();
		}
		list->AddItem(new BStringItem(pathString.String()));
		//tell the window that this view has changed
		CommunicateChanges();

		ref_num++;
	} while (1);	

}


void
IncludeSettingsView::PopulatePathList(BListView* list, const char* paths)
{
	//prerequisites: "paths" string is a list of dirs separated by white spaces.
	//the last character is a white space.
	BString pathString(paths);
	BString itemName;
	int32 slen = pathString.CountChars();
	int32 index = 0, lastBlank = -1;
	//scan the string for white spaces
	while (index < slen) {	
		//word boundaries are white spaces
		if (pathString[index] == ' ') {
			//add the string from 'lastBlank' to 'index' - 1 to the list view
			pathString.CopyInto(itemName, lastBlank + 1, index - lastBlank - 1);
			list->AddItem(new BStringItem(itemName.String()));
			lastBlank = index;
		}
		index++;
	}
}


void
IncludeSettingsView::RemoveSelectedItem(BListView* list)
{
	int32 index = list->CurrentSelection();
	if (index >= 0) {
		list->RemoveItem(index);
		//tell the window that this view has changed
		CommunicateChanges();
	}
}


void
IncludeSettingsView::CommunicateChanges()
{
	BMessenger messenger((BHandler*)Window()); //the handler is the window this view is attached to
	BMessage message(INCLUDE_SETTINGS_CHANGED); //we are the owner of the message
	messenger.SendMessage(&message); //no reply
}


