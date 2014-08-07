//JMOpenFilePanel.h

#ifndef JM_OPEN_FILE_PANEL_H 
#define JM_OPEN_FILE_PANEL_H

#include <FilePanel.h>
#include <Message.h>
//#include <Messenger.h>
#include <Window.h>
#include <CheckBox.h>
//#include "debugMacros.h"

//const uint32 JM_OPEN_FILE_PANEL = 'jofp';
//const uint32 JM_SELECT_DIR_PANEL = 'jsdp';

class JMOpenFilePanel: public BFilePanel {
	public:
				JMOpenFilePanel(BMessage* message = NULL, uint32 node_flavors = B_FILE_NODE);
		//virtual void	WasHidden();
		void	SendMessage(const BMessenger *messenger, BMessage *message);
		
	private:
		BCheckBox*	fRelPathBox;
};

#endif //JM_OPEN_FILE_PANEL_H