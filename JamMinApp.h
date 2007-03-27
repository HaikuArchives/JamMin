#ifndef __JAMMIN_APP_H
#define __JAMMIN_APP_H
	
#include "globals.h"
#include "constants.h"
#include "JamMinWindow.h" //message constants
#include "JMRefFilter.h"

#define ApplicationSignature "application/x-vnd.generic-JamMin"

#include <Application.h>	

class JamMinApp : public BApplication {
	public:
		JamMinApp();
		~JamMinApp();
		virtual void ReadyToRun();
		virtual void Pulse();
		virtual void MessageReceived(BMessage *message);
		virtual void RefsReceived(BMessage* message);
	private:
		JamMinWindow*	fMainWindow;
		BFilePanel*		fOpenPanel;
		int32			fWindowCount;
		JMRefFilter*	fRefFilter;
		bool			fIsLaunchingWithRef;
};
	
#endif //__JAMMIN_APP_H
