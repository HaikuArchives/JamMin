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
//	File Name:		LinkerSettingsView.cpp
//	Author(s):		Guido Casiraghi
//	Description:	
//------------------------------------------------------------------------------



#include "JamMin.h"

//#include "TargetSettingsWin.h"
////#include "LinkerSettingsView.h"
//#include "debugMacros.h"
//
//#include <cassert>


//constant for messages
//XXX - to be moved into a shared header
const uint32 LINKER_SETTINGS_CHANGED = 'lsch';
const uint32 LINKER_SETTINGS_ADD_BUTTON = 'lsab';
const uint32 LINKER_SETTINGS_REMOVE_BUTTON = 'lsrb';
const uint32 LINKER_SETTINGS_REFS_RECEIVED = 'lsrr';

//constants for user interface
const float
	kDistanceFromBorder = 20.0,
	kControlHeight = 30.0,
	kDividerWidth = 120.0,
	kControlsDistance = 15.0,
	kButtonWidth = 60.0,
	kButtonHeight = 30.0;
	
const char* kAddPathString = "Additional paths for libraries that follow"
	"the libXXX.so naming scheme:";

LinkerSettingsView::LinkerSettingsView(BRect frame, const char *name,
			uint32 resizingMode = B_FOLLOW_ALL, uint32 flags = B_WILL_DRAW)
//XXX - check resizingMode and flags
	:
	BView(frame, name, resizingMode, flags)	
{
	//inizialize interface

	//background color
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	//fFlagsText
	BRect r = Bounds();
	r.left += kDistanceFromBorder;
	r.right -= kDistanceFromBorder;
	r.top += kDistanceFromBorder;
	r.bottom = r.top + kControlHeight;
	fFlagsText = new BTextControl(r, "linker flags text control", "Additional Flags:",
		NULL, new BMessage(LINKER_SETTINGS_CHANGED));
	fFlagsText->SetDivider(kDividerWidth);
	fFlagsText->SetModificationMessage(new BMessage(LINKER_SETTINGS_CHANGED));
	AddChild(fFlagsText);

	//fAddPathString
	r.top = r.bottom + kControlsDistance;
	r.bottom = r.top + kControlHeight;
	fAddPathString = new BStringView(r, "add path string", kAddPathString);
	AddChild(fAddPathString);

	//fLibPathList
	r.top = r.bottom + kControlsDistance;
	r.bottom = r.top + 3 * kControlHeight;
	r.right -= kButtonWidth + kDistanceFromBorder + kControlsDistance;

//	r.right -= B_V_SCROLL_BAR_WIDTH;
//	r.bottom -= B_V_SCROLL_BAR_WIDTH;

	fLibPathList = 	new BListView(r, "lib path list", B_SINGLE_SELECTION_LIST);
	fLibPathScroll = new BScrollView("lib path scroll", fLibPathList,
		B_FOLLOW_LEFT | B_FOLLOW_TOP, 0, true, true);
	AddChild(fLibPathScroll);
	
	//fAddButton
	r.left = r.right + kControlsDistance + B_V_SCROLL_BAR_WIDTH;
	r.right = r.left + kButtonWidth;
	r.bottom = r.top + kButtonHeight;
	fAddButton = new BButton(r, "add lib path button", "Add", new BMessage(LINKER_SETTINGS_ADD_BUTTON));
	AddChild(fAddButton);
	
	//fRemoveButton
	r.top = r.bottom + kControlsDistance;
	r.bottom = r.top + kButtonHeight;
	fRemoveButton = new BButton(r, "remove lib path button", "Remove", new BMessage(LINKER_SETTINGS_REMOVE_BUTTON));
	AddChild(fRemoveButton);
	
	//fSelectDirPanel
	fSelectDirPanel = new BFilePanel(
		B_OPEN_PANEL,     // It's an open dialog 
		NULL,             // we set panel target to "this" in AttachedToWindow function
		NULL,             // No specific wishes for where the dialog starts
		B_DIRECTORY_NODE, // You only can select directories
		false,            // No multiple selection
		//NULL,			//default message
		new BMessage(LINKER_SETTINGS_REFS_RECEIVED), //XXX - are we the owner of this message?
		NULL,             // We want all dirs
		true,             // Modal panel
		true);            // Hide when done
	
	fSelectDirPanel2 = new JMOpenFilePanel(new BMessage(LINKER_SETTINGS_REFS_RECEIVED), B_DIRECTORY_NODE);
}


LinkerSettingsView::~LinkerSettingsView()
{
	//XXX - delect objects
}

void
LinkerSettingsView::AttachedToWindow()
{
	//BControl::SetTarget() must be called after BView::AddChild().
	//The best place to call it is here
	DPRINT("LinkerSettingsView::AttachedToWindow()");	
	BMessenger thisMess(this); //messenger's target is "this" object
	//fSelectDirPanel->SetTarget(thisMess);
	fSelectDirPanel2->SetTarget(thisMess);
	fAddButton->SetTarget(this);
	fRemoveButton->SetTarget(this);
}


void
LinkerSettingsView::MessageReceived(BMessage* message)
{
	//DPRINT("LinkerSettingsView::MessageReceived");
	switch (message->what) {
		case LINKER_SETTINGS_ADD_BUTTON:
			DPRINT("LinkerSettingsView::MessageReceived: LINKER_SETTINGS_ADD_BUTTON");
			//fSelectDirPanel->Show();
			fSelectDirPanel2->Show();
			break;
		case LINKER_SETTINGS_REMOVE_BUTTON:
			DPRINT("LinkerSettingsView::MessageReceived: LINKER_SETTINGS_REMOVE_BUTTON");
			RemoveSelectedItem();
			break;
		case LINKER_SETTINGS_REFS_RECEIVED:
			DPRINT("LinkerSettingsView::MessageReceived: LINKER_SETTINGS_REFS_RECEIVED");
			AddRefsToList(message);
			break;
		default:
			DPRINT("LinkerSettingsView::MessageReceived: other message");
			BView::MessageReceived(message);
			break;
	}
}


void
LinkerSettingsView::AddRefsToList(BMessage* message)
{
	//see clsApp::RefsReceived()
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
		DPRINT("LinkerSettingsView::AddRefsToList: boolean not found");
	}

	//add this entry to the list view
	do {
		if ( (err = message->FindRef("refs", ref_num, &ref)) != B_OK ) {
			return;
		}
		//DPRINT("LinkerSettingsView::AddRefsToList: ref " << ref_num << " received");
		entry.SetTo(&ref, true);
		//we return if for some strange reason the entry is unavailable
		if (entry.InitCheck() != B_OK) return;
		entry.GetPath(&path);
		DPRINT("LinkerSettingsView::AddRefsToList: entry name = " << path.Path());
		
		//convert path to relative if requested
		if (relpath) {
			//find the window we are attached to...
			DPRINT("LinkerSettingsView::AddRefsToList: add relative path");
			TargetConfig* c = ((TargetSettingsWin*) Window() )->TConfig();
			assert(c != NULL); //we *must* be attached to a window here
			c->RelPathToWorkingDir(path.Path(), &pathString);
		} else {
			DPRINT("LinkerSettingsView::AddRefsToList: add absolute path");
			pathString = path.Path();
		}

		fLibPathList->AddItem(new BStringItem(pathString.String()));
		//tell the window that this view has changed
		CommunicateChanges();

		ref_num++;
	} while (1);	
}


void
LinkerSettingsView::SetAdditionalFlags(const char* flags)
{
	fFlagsText->SetText(flags);
}


void
LinkerSettingsView::SetAdditionalPaths(const char* paths)
{
	PopulatePathList(paths);
}


const char *
LinkerSettingsView::GetAdditionalFlags()
{
	return fFlagsText->Text();
}


BString
LinkerSettingsView::GetAdditionalPaths()
{
	//convert list view items into a string
	BString pathString("");
	BListItem* item;
	int32 nItems = fLibPathList->CountItems();
	for (int32 i = 0; i < nItems; i++) {
		item = fLibPathList->ItemAt(i);
		pathString += ((BStringItem*)item)->Text();
		pathString += " ";
	}
	//DPRINT("LinkerSettingsView::GetAdditionalPaths(): " << pathString.String());
	return pathString;
}


void
LinkerSettingsView::PopulatePathList(const char* paths)
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
			fLibPathList->AddItem(new BStringItem(itemName.String()));
			lastBlank = index;
		}
		index++;
	}
	//we don't need to add the last word because the last character is always blank
}


void
LinkerSettingsView::RemoveSelectedItem()
{
	int32 index = fLibPathList->CurrentSelection();
	if (index >= 0) {
		fLibPathList->RemoveItem(index);
		//tell the window that this view has changed
		CommunicateChanges();
	}
}


void
LinkerSettingsView::CommunicateChanges()
{
	BMessenger messenger((BHandler*)Window()); //the handler is the window this view is attached to
	BMessage message(LINKER_SETTINGS_CHANGED); //we are the owner of the message
	messenger.SendMessage(&message); //no reply
}
