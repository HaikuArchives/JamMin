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
//	File Name:		GeneralSettingsView.cpp
//	Author(s):		Guido Casiraghi
//	Description:	
//------------------------------------------------------------------------------


//TODO
// - aggiungere 'fTargetType'
// - colore sfondo
// - implementare costruttore che ha come parametri "name" e "type" - NO: viene fatto tramite Get/Set
// 
// - implementare GetTargetType, GetTargetName - FATTO

#include "JamMin.h"

//#include "GeneralSettingsView.h"
//#include "TargetSettingsWin.h"
//#include "debugMacros.h"
//#include <cassert>

//const uint32 TARGET_NAME_CHANGED = 'tnch';
//const uint32 TARGET_TYPE_CHANGED = 'ttch';
const uint32 GENERAL_SETTINGS_CHANGED = 'gsch';
//const uint32 TYPE_MENU_BEAPP

//constants for user interface
const float
	kDistanceFromBorder = 20.0,
	kControlHeight = 30.0,
	kDividerWidth = 120.0,
	kControlsDistance = 15.0;

GeneralSettingsView::GeneralSettingsView(BRect frame, const char *name,
			uint32 resizingMode = B_FOLLOW_ALL, uint32 flags = B_WILL_DRAW)
//XXX - check resizingMode and flags
:
	BView(frame, name, resizingMode, flags)		
{
	//background color
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	//target name
	BRect r = Bounds();
	r.left += kDistanceFromBorder;
	r.right -= kDistanceFromBorder;
	r.top += kDistanceFromBorder;
	r.bottom = r.top + kControlHeight;
	fTargetNameText = new BTextControl(r, "name text control", "Target Name:",
		NULL, new BMessage(GENERAL_SETTINGS_CHANGED));
	fTargetNameText->SetDivider(kDividerWidth);
	fTargetNameText->SetModificationMessage(new BMessage(GENERAL_SETTINGS_CHANGED));
	AddChild(fTargetNameText);
	
	//target type
	BPopUpMenu* targetTypeMenu = new BPopUpMenu("choose one");
	targetTypeMenu->AddItem(new BMenuItem("Be App", new BMessage(GENERAL_SETTINGS_CHANGED)));
	targetTypeMenu->AddItem(new BMenuItem("Shared Library", new BMessage(GENERAL_SETTINGS_CHANGED)));
	targetTypeMenu->AddItem(new BMenuItem("Static Library", new BMessage(GENERAL_SETTINGS_CHANGED)));
	targetTypeMenu->AddItem(new BMenuItem("Kernel Driver", new BMessage(GENERAL_SETTINGS_CHANGED)));
	r.top = r.bottom + kControlsDistance;
	r.bottom = r.top + kControlHeight;
	fTargetTypeMenu = new BMenuField(r, "target type field", "Target Type:", targetTypeMenu);
	fTargetTypeMenu->SetDivider(kDividerWidth);
	AddChild(fTargetTypeMenu);
}


GeneralSettingsView::~GeneralSettingsView()
{
}


void
GeneralSettingsView::MessageReceived(BMessage* message)
{
	switch(message->what) {
		default:
			BView::MessageReceived(message);
			break;
	}
}


TargetType
GeneralSettingsView::GetTargetType()
{
	BMenu* menu = fTargetTypeMenu->Menu();
	assert(menu != NULL);
	BMenuItem* marked = menu->FindMarked();
	if (marked == NULL)
		return UNKNOWN;
	BString label(marked->Label());
	DPRINT("GeneralSettingsView::GetTargetType: label = " << label.String());
	if (label.Compare("Be App") == 0)
		return BE_APP;
	else if (label.Compare("Shared Library") == 0)
		return SHARED_LIBRARY;
	else if (label.Compare("Static Library") == 0)
		return STATIC_LIBRARY;
	else if (label.Compare("Kernel Driver") == 0)
		return KERNEL_DRIVER;
	else {
		DPRINT("GeneralSettingsView::GetTargetType: error: Unknown");
		return UNKNOWN;
	}	
}


const char *
GeneralSettingsView::GetTargetName()
{
	return fTargetNameText->Text();
}

void
GeneralSettingsView::SetTargetType(TargetType type)
{
	BMenu* menu = fTargetTypeMenu->Menu();
	assert(menu != NULL);
	BMenuItem* item; //the menu item we are going to select

	if (type == BE_APP) {
		item = menu->FindItem("Be App");
		assert(item != NULL);
		item->SetMarked(true);
	} else if (type == SHARED_LIBRARY) {
		item = menu->FindItem("Shared Library");
		assert(item != NULL);
		item->SetMarked(true);
	} else if (type == STATIC_LIBRARY) {
		item = menu->FindItem("Static Library");
		assert(item != NULL);
		item->SetMarked(true);
	} else if (type == KERNEL_DRIVER) {
		item = menu->FindItem("Kernel Driver");
		assert(item != NULL);
		item->SetMarked(true);	
	}
}


void
GeneralSettingsView::SetTargetName(const char* name)
{
	fTargetNameText->SetText(name);
}
