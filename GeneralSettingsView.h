//file GeneralSettingsView.h

#ifndef GENERAL_SETTINGS_H
#define GENERAL_SETTINGS_H

#include "TargetConfig.h" //for TargetType enum

#include <View.h>
#include <Rect.h>
#include <Message.h>
#include <TextControl.h>
#include <MenuField.h>
//#include <Menu.h>
//#include <PopUpMenu.h>

#include <MenuItem.h>

class GeneralSettingsView: public BView
{
	public:
					GeneralSettingsView(BRect frame, const char *name,
						uint32 resizingMode = B_FOLLOW_ALL, uint32 flags = B_WILL_DRAW);
		virtual		~GeneralSettingsView();
		virtual		void MessageReceived(BMessage* message);
		TargetType	GetTargetType();
		const char*	GetTargetName();
		void		SetTargetType(TargetType type);
		void		SetTargetName(const char* name);		
		
		
	private:
		BTextControl*	fTargetNameText;
		BMenuField*		fTargetTypeMenu;
		//bool			fChangedConfig;
};
#endif //GENERAL_SETTINGS_H