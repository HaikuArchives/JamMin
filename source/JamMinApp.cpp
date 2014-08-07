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
//	File Name:		JamMinApp.cpp
//	Author(s):		Guido Casiraghi
//	Description:	
//------------------------------------------------------------------------------


//TODO
// - fOpenFile should open only one file
// - fOpenFile should open only "application/JamMan" files


#include "JamMin.h"

//#include "JamMinApp.h"
//#include "debugMacros.h"
//#include <FilePanel.h>
//#include <Entry.h>
//#include <stdio.h> 
//#include <iostream>

JamMinApp::JamMinApp()
:
	BApplication(ApplicationSignature),
	fMainWindow(NULL)
{
	//window constructor calls 'Show()'
	try {
		//fMainWindow = new JamMinWindow("Untitled");
		fOpenPanel = new BFilePanel;
		fRefFilter = new JMRefFilter();
		fOpenPanel->SetRefFilter(fRefFilter);
		fWindowCount = 0; // no windows yet
		fIsLaunchingWithRef = false;
		
	} catch (bad_alloc)	{
		AppReturnValue = B_NO_MEMORY;
		be_app->PostMessage(B_QUIT_REQUESTED);
	}
}

JamMinApp::~JamMinApp()
{
	DPRINT("JamMinApp::~JamMinApp()");
	delete fOpenPanel;
	delete fRefFilter;
	if (fMainWindow != NULL)
		if (fMainWindow->LockWithTimeout(30000000) == B_OK)	//Must Lock() before calling Quit()
			fMainWindow->Quit();	//Never delete a BWindow, call Quit() instead.
}

void JamMinApp::ReadyToRun()
{
	//A good place to set up windows, etc., just before your application is about to Run().
	if (!fIsLaunchingWithRef)
		fMainWindow = new JamMinWindow("Untitled");
}

void JamMinApp::Pulse()
{
	//Provides a "heartbeat" for your application; a good place to blink cursors, etc.
	//You set the pulse rate in BApplication::SetPulseRate().
}


void JamMinApp::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case WINDOW_REGISTRY_ADD:
		{	
			//only increment window counter and reply to sender
			DPRINT("JamMinApp::MessageReceived(): WINDOW_REGISTRY_ADD");
			BMessage reply(WINDOW_REGISTRY_ADDED);
			fWindowCount++;
			message->SendReply(&reply);
			break;
		}
		case WINDOW_REGISTRY_SUB:
			DPRINT("JamMinApp::MessageReceived(): WINDOW_REGISTRY_SUB");
			fWindowCount--;
			if (!fWindowCount) {
				//XXX - be_app->PostMessage(..)?
				Quit();
			}
			break;
		case MENU_FILE_OPEN:
			DPRINT("JamMinApp::MessageReceived(): MENU_FILE_OPEN");
			fOpenPanel->Show();
			break;
			
		default:
			BApplication::MessageReceived(message);
			break;
	}//switch
}


void
JamMinApp::RefsReceived(BMessage* message)
{
	DPRINT("JamMinApp::RefsReceived()");
	
	entry_ref ref;
	status_t err;
	int32 ref_num;
	ref_num = 0;
	if (IsLaunching()) {
		DPRINT("JamMinApp::RefsReceived: launching with ref");
		fIsLaunchingWithRef = true;
	}
	do {
		if ( (err = message->FindRef("refs", ref_num, &ref)) != B_OK ) {
			return;
		}
		DPRINT("ref " << ref_num << " received");
		//call window constructor
		new JamMinWindow(&ref);
		ref_num++;
	} while (1);		
}
