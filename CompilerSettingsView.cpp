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
//	File Name:		CompilerSettingsView.cpp
//	Author(s):		Guido Casiraghi
//	Description:	
//------------------------------------------------------------------------------


#include "JamMin.h"
//#include "CompilerSettingsView.h"
//#include "TargetSettingsWin.h"
//#include "debugMacros.h"
//#include <cassert>

//constants for user interface
const float
	kDistanceFromBorder = 20.0,
	kControlHeight = 30.0,
	kDividerWidth = 120.0,
	kControlsDistance = 15;

//constants for messages
//XXX - to be moved into a common header
const uint32 COMPILER_SETTINGS_CHANGED = 'csch';

CompilerSettingsView::CompilerSettingsView(BRect frame, const char *name,
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
	fFlagsText = new BTextControl(r, "compiler flags text control", "Additional Flags:",
		NULL, new BMessage(COMPILER_SETTINGS_CHANGED));
	fFlagsText->SetDivider(kDividerWidth);
	fFlagsText->SetModificationMessage(new BMessage(COMPILER_SETTINGS_CHANGED));
	AddChild(fFlagsText);
	
	//fDefinesText
	r.top = r.bottom + kControlsDistance;
	r.bottom = r.top + kControlHeight;
	fDefinesText = new BTextControl(r, "compiler defines text control", "Compiler Defines:",
		NULL, new BMessage(COMPILER_SETTINGS_CHANGED));
	fDefinesText->SetDivider(kDividerWidth);
	fDefinesText->SetModificationMessage(new BMessage(COMPILER_SETTINGS_CHANGED));
	AddChild(fDefinesText);

	//fOptimizationMenu
	BPopUpMenu* optMenu = new BPopUpMenu("choose one");
	optMenu->AddItem(new BMenuItem("None", new BMessage(COMPILER_SETTINGS_CHANGED)));
	optMenu->AddItem(new BMenuItem("Some", new BMessage(COMPILER_SETTINGS_CHANGED)));
	optMenu->AddItem(new BMenuItem("Full", new BMessage(COMPILER_SETTINGS_CHANGED)));
	r.top = r.bottom + kControlsDistance;
	r.bottom = r.top + kControlHeight;
	fOptimizationMenu = new BMenuField(r, "optimization field", "Optimization Level:", optMenu);
	fOptimizationMenu->SetDivider(kDividerWidth);
	AddChild(fOptimizationMenu);

	//fWarningsMenu
	BPopUpMenu* warningMenu = new BPopUpMenu("choose one");
	warningMenu->AddItem(new BMenuItem("None", new BMessage(COMPILER_SETTINGS_CHANGED)));
	warningMenu->AddItem(new BMenuItem("All", new BMessage(COMPILER_SETTINGS_CHANGED)));
	r.top = r.bottom + kControlsDistance;
	r.bottom = r.top + kControlHeight;
	fWarningsMenu = new BMenuField(r, "warning field", "Compiler Warnings:", warningMenu);
	fWarningsMenu->SetDivider(kDividerWidth);
	AddChild(fWarningsMenu);
	
	//fSymbolsEnabledBox
	r.top = r.bottom + kControlsDistance;
	r.bottom = r.top + kControlHeight;
	fSymbolsEnabledBox = new BCheckBox(r, "symbols box", "Create Symbols", new BMessage(COMPILER_SETTINGS_CHANGED));
	AddChild(fSymbolsEnabledBox);
	DPRINT("CompilerSettingsView::CompilerSettingsView: fSymbolsEnabledBox = " << fSymbolsEnabledBox->Value());
	
	//fDebuggerEnabledBox
	r.top = r.bottom + kControlsDistance;
	r.bottom = r.top + kControlHeight;
	fDebuggerEnabledBox = new BCheckBox(r, "debugger box", "Enable Debugger", new BMessage(COMPILER_SETTINGS_CHANGED));
	AddChild(fDebuggerEnabledBox);
	DPRINT("CompilerSettingsView::CompilerSettingsView: fDebuggerEnabledBox = " << fDebuggerEnabledBox->Value());	
}


CompilerSettingsView::~CompilerSettingsView()
{
}


void
CompilerSettingsView::SetOptimizationLevel(OptimizeOpt optLevel)
{
	BMenu* menu = fOptimizationMenu->Menu();
	assert(menu != NULL);
	BMenuItem* item; //the menu item we are going to select
	if (optLevel == O_NONE) {
		item = menu->FindItem("None");
		assert(item != NULL);
		item->SetMarked(true);
	} else if (optLevel == O_SOME) {
		item = menu->FindItem("Some");
		assert(item != NULL);
		item->SetMarked(true);
	} else if (optLevel == O_FULL) {
		item = menu->FindItem("Full");
		assert(item != NULL);
		item->SetMarked(true);
	}
}


void
CompilerSettingsView::SetWarningsOption(WarningOpt opt)
{
	BMenu* menu = fWarningsMenu->Menu();
	assert(menu != NULL);
	BMenuItem* item; //the menu item we are going to select
	if (opt == W_NONE) {
		item = menu->FindItem("None");
		assert(item != NULL);
		item->SetMarked(true);
	} else if (opt == W_ALL) {
		item = menu->FindItem("All");
		assert(item != NULL);
		item->SetMarked(true);
	}
}


void
CompilerSettingsView::EnableSymbols(bool enable)
{
	if (enable)
		fSymbolsEnabledBox->SetValue(1);
	else
		fSymbolsEnabledBox->SetValue(0);
}


void
CompilerSettingsView::EnableDebugger(bool enable)
{
	if (enable)
		fDebuggerEnabledBox->SetValue(1);
	else
		fDebuggerEnabledBox->SetValue(0);
}


void
CompilerSettingsView::SetDefines(const char* defs)
{
	fDefinesText->SetText(defs);
}


void
CompilerSettingsView::SetAdditionalFlags(const char* flags)
{
	fFlagsText->SetText(flags);
}


OptimizeOpt
CompilerSettingsView::GetOptimizationLevel()
{
	BMenu* menu = fOptimizationMenu->Menu();
	assert(menu != NULL);
	BMenuItem* marked = menu->FindMarked();
	assert(marked != NULL);
	BString label(marked->Label());
	if (label.Compare("None") == 0) {
		return O_NONE;
	} else if (label.Compare("Some") == 0) {
		return O_SOME;
	} else if (label.Compare("Full") == 0) {
		return O_FULL;
	}	
}


WarningOpt
CompilerSettingsView::GetWarningsOption()
{
	BMenu* menu = fWarningsMenu->Menu();
	assert(menu != NULL);
	BMenuItem* marked = menu->FindMarked();
	assert(marked != NULL);
	BString label(marked->Label());
	if (label.Compare("None") == 0) {
		return W_NONE;
	} else if (label.Compare("All") == 0) {
		return W_ALL;
	}
}


bool
CompilerSettingsView::SymbolsEnabled()
{
	if (fSymbolsEnabledBox->Value())
		return true;
	else
		return false;
}


bool
CompilerSettingsView::DebuggerEnabled()
{
	if (fDebuggerEnabledBox->Value())
		return true;
	else
		return false;
}


const char *
CompilerSettingsView::GetDefines()
{
	return fDefinesText->Text();
}


const char *
CompilerSettingsView::GetAdditionalFlags()
{
	return fFlagsText->Text();
}
