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
//	File Name:		JMOpenFilePanel.cpp
//	Author(s):		Guido Casiraghi
//	Description:	
//------------------------------------------------------------------------------


#include "JamMin.h"
//#include "JMOpenFilePanel.h"

JMOpenFilePanel::JMOpenFilePanel(BMessage* message = NULL, uint32 node_flavors = B_FILE_NODE)
	: BFilePanel(
		B_OPEN_PANEL,     
		NULL,             
		NULL,             // No specific wishes for where the dialog starts
		node_flavors,
		false,            // No multiple selection
		message,
		NULL,			// We want all files
		true,			// Modal panel
		true)			// Hide when done
{
	//fWindow = Window(); //this panel's window
	Window()->Lock();
	BView* background = Window()->ChildAt(0);
	BRect r;
	r = background->Bounds();
	r.top = r.bottom - 30;
	r.right += 15;
	r.right = r.left + 130;
	fRelPathBox = new BCheckBox(r, "rel path checkbox", "Store absolute path",
		 new BMessage('1234'), B_FOLLOW_LEFT | B_FOLLOW_BOTTOM);
	background->AddChild(fRelPathBox);
	Window()->Unlock();
}


void
JMOpenFilePanel::SendMessage(const BMessenger *messenger, BMessage *message)
{
	DPRINT("JMOpenFilePanel::SendMessage: fRelPathBox->Value() == " << fRelPathBox->Value());
	//add checkbox value to the message being sent
	bool relpath;
	if (fRelPathBox->Value() == 0)
		relpath = true;
	else
		relpath = false;
	message->AddBool("relpath", relpath);
	BFilePanel::SendMessage(messenger, message);
}
