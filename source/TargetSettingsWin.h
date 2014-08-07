//file TargetSettings.h

#ifndef __H_TARGET_SETTINGS
#define __H_TARGET_SETTINGS


//#include "globals.h"
//#include "constants.h"
#include "JMOpenFilePanel.h"
#include "GeneralSettingsView.h"
#include "CompilerSettingsView.h"
#include "LinkerSettingsView.h"
#include "IncludeSettingsView.h"

#include "TargetConfig.h"

//#include <Application.h>
#include <View.h>
#include <ListView.h>
//#include <ListItem.h>
//#include <OutlineListView.h>
//#include <StringView.h>
//#include <ScrollView.h>
//#include <Menu.h>
//#include <MenuItem.h>
//#include <MenuBar.h>
#include <Message.h>
//#include <Button.h>
//#include <TextControl.h>
//#include <TextView.h>
#include <Window.h>


	
class TargetSettingsWin : public BWindow
{
public:
	TargetSettingsWin(const char *uWindowTitle, TargetConfig *targetConfig);
	virtual					~TargetSettingsWin();
	virtual void			MessageReceived(BMessage *Message);
	virtual bool			QuitRequested();
	TargetConfig*			TConfig();
private:
	//members
	TargetConfig*			fConfig;
	BView*					MainView;
	BListView*				fSettingsListView;
	
	BView*					fCurrentSettingsView;
	GeneralSettingsView*	fGeneralSettingsView;
	CompilerSettingsView*	fCompilerSettingsView;
	LinkerSettingsView*		fLinkerSettingsView;
	IncludeSettingsView*	fIncludeSettingsView;
	
	bool					fChangedConfig;
	bool					fGeneralSettingsChanged;
	bool					fCompilerSettingsChanged;
	bool					fLinkerSettingsChanged;
	bool					fIncludeSettingsChanged;
	
	//XXX - to be filled
	
	//functions
	bool					AreCompulsorySet();
	status_t				SaveChangedSettings();
	void					PopulateSettings();
	void					ShowSelectedView(int32 index);
	void					MakeCurrent(BView* view);
	void					Register();
	void					Unregister();

};

#endif //__H_TARGET_SETTINGS
