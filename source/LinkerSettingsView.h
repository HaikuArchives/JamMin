//LinkerSettingsView.h

#ifndef LINKER_SETTINGS_H
#define LINKER_SETTINGS_H

#include <View.h>
#include <Rect.h>
#include <Message.h>
#include <TextControl.h>
#include <Button.h>
#include <ListView.h>
#include <ScrollView.h>
#include <StringView.h>
#include <FilePanel.h>
//#include <Messenger.h>
//#include <Handler.h>
#include <String.h>
#include <Entry.h>
#include <Path.h>

//#include "JMOpenFilePanel.h"

class LinkerSettingsView: public BView
{
	public:
					LinkerSettingsView(BRect frame, const char *name,
						uint32 resizingMode = B_FOLLOW_ALL, uint32 flags = B_WILL_DRAW);
		virtual		~LinkerSettingsView();
		void		MessageReceived(BMessage* message);
		void		AttachedToWindow();
		
		void		SetAdditionalFlags(const char* flags);
		void		SetAdditionalPaths(const char* paths); //path names separeted by a space
		const char*	GetAdditionalFlags();
		BString		GetAdditionalPaths(); //path names separeted by a space
		
		
	private:
		BTextControl*	fFlagsText;
		BStringView*	fAddPathString;
		BScrollView*	fLibPathScroll;
		BListView*		fLibPathList;
		BButton*		fAddButton;
		BButton*		fRemoveButton;
		BFilePanel*		fSelectDirPanel;
		JMOpenFilePanel*	fSelectDirPanel2;
		
		/* functions*/
		void			AddRefsToList(BMessage* message);
		void			PopulatePathList(const char* paths);
		void			RemoveSelectedItem();
		void			CommunicateChanges();
};


#endif //LINKER_SETTINGS_H