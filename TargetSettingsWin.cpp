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
//	File Name:		TargetSettingsView.cpp
//	Author(s):		Guido Casiraghi
//	Description:	
//------------------------------------------------------------------------------

#include "JamMin.h"

//#include "TargetSettingsWin.h"
//#include "GeneralSettingsView.h"
//#include "TargetConfig.h"
//#include "JamMinWindow.h"
//#include "JamMinApp.h"
//#include "debugMacros.h"
//#include <iostream>
//#include <Alert.h>
//#include <cassert>


//constants for application messages
//const uint32 WINDOW_REGISTRY_ADD =		'WRad';
//const uint32 WINDOW_REGISTRY_SUB =		'WRsb';
//const uint32 WINDOW_REGISTRY_ADDED =	'WRdd';
//

//constants for window messages
const uint32 PANEL_LIST_ITEM_SELECTED = 'lsse';
const uint32 OK_BUTTON_PRESSED = 'okbu';
const uint32 CANCEL_BUTTON_PRESSED = 'cabu';

//constants for messages that come from settings views
//XXX - to be put in a header file
const uint32 GENERAL_SETTINGS_CHANGED = 'gsch',
	COMPILER_SETTINGS_CHANGED = 'csch',
	LINKER_SETTINGS_CHANGED = 'lsch',
	INCLUDE_SETTINGS_CHANGED = 'isch';
//XXX - remove the following
const uint32 LINKER_SETTINGS_ADD_BUTTON = 'lsab';
const uint32 LINKER_SETTINGS_REMOVE_BUTTON = 'lsrb';

//private constants for this window
enum {
	GENERAL_SETTINGS_INDEX = 0,
	INCLUDE_PATHS_INDEX,
	COMPILER_OPTIONS_INDEX,
	LINKER_OPTIONS_INDEX
};

TargetSettingsWin::TargetSettingsWin(const char *uWindowTitle, TargetConfig *targetConfig)
:
	BWindow(
		BRect(64, 64, 680, 450),
		uWindowTitle,
		B_FLOATING_WINDOW_LOOK,
		/*B_NORMAL_WINDOW_FEEL*/ B_MODAL_APP_WINDOW_FEEL,
		B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_NOT_CLOSABLE,
		0 )
{
	assert(targetConfig != NULL);
	
	MainView = new BView(BWindow::Bounds(), NULL, B_FOLLOW_ALL, 0);
	if (MainView == NULL)
	{
		AppReturnValue = B_NO_MEMORY;
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;		
	}
	MainView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	BWindow::AddChild(MainView);
	
	//members
	fConfig = targetConfig;
	fChangedConfig = false;
	fGeneralSettingsChanged = false;
	fCompilerSettingsChanged = false;
	fLinkerSettingsChanged = false;
	fIncludeSettingsChanged = false;
	
	//list of available settings
	BRect r = MainView->Bounds();
	r.left += 15;
	r.bottom = r.top + (r.bottom - r.top)/2;
	r.top += 15;
	
	r.right = r.left + 100;
	fSettingsListView = new BListView(r, "settings list");
	fSettingsListView->SetSelectionMessage(new BMessage(PANEL_LIST_ITEM_SELECTED));
	//XXX - the following doesn't work. Why?
	fSettingsListView->Select(0); //select the first item, i.e. "general settings"
	MainView->AddChild(fSettingsListView);
	fSettingsListView->AddItem(new BStringItem("General"));
	fSettingsListView->AddItem(new BStringItem("Include Paths"));
	fSettingsListView->AddItem(new BStringItem("Compiler Options"));
	fSettingsListView->AddItem(new BStringItem("Linker Options"));
	
	/////////////////////////////////////////////////////////////////////
	//create all settings panels. Use the same rect for all views.
	r = MainView->Bounds();
	r.top += 15;
	r.right -= 15;
	r.bottom -= 50;
	r.left += fSettingsListView->Bounds().Width() + 30;
	//create GeneralSettingsView and make it the current view
	fGeneralSettingsView = new GeneralSettingsView(r, "general settings view");
	fCurrentSettingsView = fGeneralSettingsView;
	MainView->AddChild(fGeneralSettingsView);

	fCompilerSettingsView = new CompilerSettingsView(r, "compiler settings view");
	fLinkerSettingsView = new LinkerSettingsView(r, "linker settings view");
	fIncludeSettingsView = new IncludeSettingsView(r, "include settings view");

	/////////////////////////////////////////////////////////////////////
		
	//insert 'OK' and 'Cancel' buttons
	r = BWindow::Bounds();
	r.top = fGeneralSettingsView->Bounds().bottom + 30;
	r.bottom -= 15;
	r.right -=15;
	r.left = r.right - 75;
	r.PrintToStream();
	//OK
	BButton* okButton = new BButton(r, "OK button", "OK", new BMessage(OK_BUTTON_PRESSED));
	MainView->AddChild(okButton);
	//Cancel
	r.right = r.left - 15;
	r.left = r.right - 75;
	r.PrintToStream();
	BButton* cancelButton = new BButton(r, "Cancel button", "Cancel", new BMessage(CANCEL_BUTTON_PRESSED));
	MainView->AddChild(cancelButton);
	
	/////////////////////////////////////////////////////////////////////
	
	//initialize views with configuration settings
	PopulateSettings();
	//tell the application that there's a new window
	Register();
	//last but not least...
	Show();
}


TargetSettingsWin::~TargetSettingsWin()
{
	//delete all views
	fCurrentSettingsView->RemoveSelf(); //first remove current view
	delete fGeneralSettingsView;
	delete fCompilerSettingsView;
	delete fLinkerSettingsView;
	delete fIncludeSettingsView;

	Unregister();
}

void
TargetSettingsWin::MessageReceived(BMessage *Message)
{
	//questi messaggi devono aggiornare l'oggetto 'TargetConfig'
	switch (Message->what) {
		case PANEL_LIST_ITEM_SELECTED:
		{
			DPRINT("TargetSettingsWin::MessageReceived: PANEL_LIST_ITEM_SELECTED");
			int32 index;
			status_t err = Message->FindInt32("index", &index);
			if (err == B_OK) {
				ShowSelectedView(index);
			}
			break;
		}
		case OK_BUTTON_PRESSED:
			DPRINT("TargetSettingsWin::MessageReceived: OK_BUTTON_PRESSED");
			if (AreCompulsorySet()) {
				SaveChangedSettings();
				fConfig->PrintToStream();
				Quit();
			} else {
				//show a dialog for the user to inform that name and type must be specified
				BAlert* alert = new BAlert("specify name and type alert",
					"You must specify one name and one type for this target.",
					"Ok", NULL, NULL,
					B_WIDTH_AS_USUAL, B_EVEN_SPACING, B_INFO_ALERT);
					alert->SetShortcut(0, B_ESCAPE);
					/* int32 button_index = */ alert->Go();
			}
			break;
		case CANCEL_BUTTON_PRESSED:
			DPRINT("TargetSettingsWin::MessageReceived: CANCEL_BUTTON_PRESSED");
			//close this window
			Quit();
			break;
		case GENERAL_SETTINGS_CHANGED:
			DPRINT("TargetSettingsWin::MessageReceived: GENERAL_SETTINGS_CHANGED");
			//fChangedConfig = true;
			fGeneralSettingsChanged = true;
			break;
		case COMPILER_SETTINGS_CHANGED:
			DPRINT("TargetSettingsWin::MessageReceived: COMPILER_SETTINGS_CHANGED");
			fCompilerSettingsChanged = true;
			break;
		case INCLUDE_SETTINGS_CHANGED:
			DPRINT("TargetSettingsWin::MessageReceived: INCLUDE_SETTINGS_CHANGED");
			fIncludeSettingsChanged = true;
			break;
		case LINKER_SETTINGS_CHANGED:
			DPRINT("TargetSettingsWin::MessageReceived: LINKER_SETTINGS_CHANGED");
			fLinkerSettingsChanged = true;
			break;
		case LINKER_SETTINGS_ADD_BUTTON:
		case LINKER_SETTINGS_REMOVE_BUTTON:
			//XXX - to be removed only for testing
			//---> is it possible for the view to manage directly these messages?
			DPRINT("TargetSettingsWin::MessageReceived: LINKER_SETTINGS_{ADD,REMOVE}_BUTTON");
			fLinkerSettingsView->MessageReceived(Message);
			break;
		default:
			DPRINT("TargetSettingsWin::MessageReceived: other message");
			BWindow::MessageReceived(Message);
		  break;
	} //switch

}


bool TargetSettingsWin::QuitRequested()
{
	return true;
}



bool
TargetSettingsWin::AreCompulsorySet()
{
	//check if name and type are ok
	if (fGeneralSettingsView->GetTargetName() == NULL
		|| fGeneralSettingsView->GetTargetType() == UNKNOWN)
		return false;
	else
		return true;
}


status_t
TargetSettingsWin::SaveChangedSettings()
{
	//write changed settings into the configuration
	status_t ret;
	//do it for each panel that has changed
	if (fGeneralSettingsChanged) {
		//general settings
		DPRINT("TargetSettingsWin::SaveChangedSettings: general settings changed");
		ret = fConfig->SetName(fGeneralSettingsView->GetTargetName());
		if (ret != B_OK) return ret;
		ret = fConfig->SetTargetType(fGeneralSettingsView->GetTargetType());
		if (ret != B_OK) return ret;
		fGeneralSettingsChanged = false; //XXX - this is useless, we are quitting
	}
	if (fCompilerSettingsChanged) {
		//compiler settings
		DPRINT("TargetSettingsWin::SaveChangedSettings: compiler settings changed");
		ret = fConfig->SetOptimizeOptions(fCompilerSettingsView->GetOptimizationLevel());
		if (ret != B_OK) return ret;
		ret = fConfig->SetWarningOptions(fCompilerSettingsView->GetWarningsOption());
		if (ret != B_OK) return ret;
		fConfig->EnableSymbols(fCompilerSettingsView->SymbolsEnabled());
		fConfig->EnableDebugger(fCompilerSettingsView->DebuggerEnabled());
		ret = fConfig->SetDefines(fCompilerSettingsView->GetDefines());
		if (ret != B_OK) return ret;
		ret = fConfig->SetCompilerFlags(fCompilerSettingsView->GetAdditionalFlags());
		if (ret != B_OK) return ret;
		fCompilerSettingsChanged = false; //XXX - this is useless, we are quitting
	}
	if (fLinkerSettingsChanged) {
		DPRINT("TargetSettingsWin::SaveChangedSettings: linker settings changed");
		ret = fConfig->SetLinkerFlags(fLinkerSettingsView->GetAdditionalFlags());
		if (ret != B_OK) return ret;
		ret = fConfig->SetLibPaths(fLinkerSettingsView->GetAdditionalPaths().String());
		if (ret != B_OK) return ret;
		fLinkerSettingsChanged = false;
	}
	//XXX - add other panels
	if (fIncludeSettingsChanged){
		DPRINT("TargetSettingsWin::SaveChangedSettings: include settings changed");
		ret = fConfig->SetSysIncludePaths(fIncludeSettingsView->GetSysPaths().String());
		if (ret != B_OK) return ret;
		ret = fConfig->SetLocalIncludePaths(fIncludeSettingsView->GetLocalPaths().String());
		if (ret != B_OK) return ret;
		fIncludeSettingsChanged = false;
	}
	return B_OK;
}


void
TargetSettingsWin::PopulateSettings()
{
	//add contents to each settings panel
	//general settings
	fGeneralSettingsView->SetTargetName(fConfig->GetName());
	fGeneralSettingsView->SetTargetType(fConfig->GetTargetType());
	
	//compiler settings
	fCompilerSettingsView->SetOptimizationLevel(fConfig->GetOptimizeOptions());
	fCompilerSettingsView->SetWarningsOption(fConfig->GetWarningOptions());
	fCompilerSettingsView->EnableSymbols(fConfig->SymbolsEnabled());
	fCompilerSettingsView->EnableDebugger(fConfig->DebuggerEnabled());
	fCompilerSettingsView->SetDefines(fConfig->GetDefines());
	fCompilerSettingsView->SetAdditionalFlags(fConfig->GetCompilerFlags());

	//linker settings
	fLinkerSettingsView->SetAdditionalFlags(fConfig->GetLinkerFlags());
	fLinkerSettingsView->SetAdditionalPaths(fConfig->GetLibPaths());
	//DPRINT("Additional Paths = " << fLinkerSettingsView->GetAdditionalPaths().String());
	
	//include settings
	fIncludeSettingsView->SetSysPaths(fConfig->GetSysIncludePaths());
	fIncludeSettingsView->SetLocalPaths(fConfig->GetLocalIncludePaths());
}


void
TargetSettingsWin::ShowSelectedView(int32 index)
{
	DPRINT("TargetSettingsWin::ShowSelectedView: index = " << index);
	switch (index)
	{
		case GENERAL_SETTINGS_INDEX:
			MakeCurrent(fGeneralSettingsView);
			break;
		case INCLUDE_PATHS_INDEX:
			MakeCurrent(fIncludeSettingsView);
			break;
		case COMPILER_OPTIONS_INDEX:
			MakeCurrent(fCompilerSettingsView);
			break;
		case LINKER_OPTIONS_INDEX:
			MakeCurrent(fLinkerSettingsView);
			break;
		default:
			break;
	}
}


inline void
TargetSettingsWin::MakeCurrent(BView* view)
{
		if (fCurrentSettingsView != view) {
				MainView->RemoveChild(fCurrentSettingsView);
				fCurrentSettingsView = view;
				MainView->AddChild(view);
		}
}


TargetConfig *
TargetSettingsWin::TConfig()
{
	return fConfig;
}


void
TargetSettingsWin::Register()
{
	BMessenger messenger(ApplicationSignature);
	BMessage message(WINDOW_REGISTRY_ADD);
	messenger.SendMessage(&message, this);
}


void
TargetSettingsWin::Unregister()
{
	BMessenger messenger(ApplicationSignature);
	messenger.SendMessage(new BMessage(WINDOW_REGISTRY_SUB));
}
