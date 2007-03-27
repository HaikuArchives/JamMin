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
//	File Name:		ProjFileItem.h
//	Author(s):		Guido Casiraghi
//	Description:	
//------------------------------------------------------------------------------


#include "debugMacros.h"

#include <String.h>
#include <ListItem.h>
#include <Path.h>
#include <iostream>

class ProjFileItem : public BListItem 
{
	public:
		ProjFileItem(const char* displayName, const char* archName, BPath* fullPath)
			: BListItem()
		{
			fDisplayName.SetTo(displayName);
			fArchivedName.SetTo(archName);
			fFilePath.SetTo(fullPath->Path()); // 'path' must be already normalized
			kHighlight.red = 216;
			kHighlight.green = 216;
			kHighlight.blue = 216;
			kHighlight.alpha = 0; //ignored
			kBlackColor.red = kBlackColor.green = kBlackColor.blue = kBlackColor.alpha = 0;
			
		}
		
		//we have to override DrawItem function
		virtual void DrawItem(BView *owner, 
			BRect frame, 
			bool complete = false)
		{
				//DPRINT("ProjFileItem::DrawItem()");
				//we redraw the background if the item is selected or if complete is true
				if (IsSelected() || complete) {
					rgb_color color;
					if (IsSelected()) {
	            		color = kHighlight;
					}
					else {
			            color = owner->ViewColor();
					}
					owner->SetHighColor(color);
					owner->FillRect(frame);
      			}
      		
      		//now we draw the string
			//move the pen XXX - check appearance here
			owner->MovePenTo(frame.left+4, frame.bottom-2);
			if (IsEnabled()) {
				owner->SetHighColor(kBlackColor);
			} else {
				owner->SetHighColor(kBlackColor); //XXX - should be gray here
			}
			owner->DrawString(fDisplayName.String());
			
		}
		
		
		const char* GetArchivedName() { return fArchivedName.String(); }
		const char* GetFilePath() { return fFilePath.Path(); }
		const char*	Text() const { return fDisplayName.String(); }
		

	private:
		BString	fDisplayName;
		BString fArchivedName;	//the name that we use to archive this file
								//i.e. relative path to working dir
		BPath fFilePath; //the absolute file path in this moment
		rgb_color kHighlight; //background color when highlighted
		rgb_color kBlackColor;
};