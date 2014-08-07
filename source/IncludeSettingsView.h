//IncludeSettingsView.h

#ifndef INCLUDE_SETTINGS_H
#define INCLUDE_SETTINGS_H

#include <View.h>
#include <Rect.h>
#include <Message.h>
#include <TextControl.h>
#include <Button.h>
#include <ListView.h>
#include <FilePanel.h>
//#include <Entry.h>
//#include <Path.h>
#include <ScrollView.h>
#include <StringView.h>
#include <String.h>

class IncludeSettingsView: public BView
{
	public:
					IncludeSettingsView(BRect frame, const char *name,
						uint32 resizingMode = B_FOLLOW_ALL, uint32 flags = B_WILL_DRAW);
		virtual		~IncludeSettingsView();
		void		MessageReceived(BMessage* message);
		void		AttachedToWindow();

		void		SetSysPaths(const char* paths);
		void		SetLocalPaths(const char* paths);
		BString		GetSysPaths();
		BString		GetLocalPaths();
		
	private:
		BStringView*	fAddSysPathString;
		BStringView*	fAddLocalPathString;
		BScrollView*	fSysPathScroll;
		BScrollView*	fLocalPathScroll;
		BListView*		fSysPathList;
		BListView*		fLocalPathList;
		BButton*		fAddSysButton;
		BButton*		fRemoveSysButton;
		BButton*		fAddLocalButton;
		BButton*		fRemoveLocalButton;
		BFilePanel*		fSelectSysDirPanel;
		BFilePanel*		fSelectLocalDirPanel;
		JMOpenFilePanel*	fSelectSysDirPanel2;
		JMOpenFilePanel*	fSelectLocalDirPanel2;
		
		//functions
		void			AddRefsToList(BListView* list, BMessage* message, bool relative = false);
		void			PopulatePathList(BListView* list, const char* paths);
		void			RemoveSelectedItem(BListView* list);
		BString			GetListContent(BListView* list);
		void			CommunicateChanges();
		
};

#endif //INCLUDE_SETTINGS_H
