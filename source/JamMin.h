//file JamMin.h

#include "JamMinApp.h"
#include "JamMinWindow.h"
#include "TargetSettingsWin.h"
#include "TargetConfig.h"
#include "JamFile.h"

#include "constants.h"
#include "globals.h"

#include "JMOpenFilePanel.h"
#include "ProjFileItem.h"
#include "JMRefFilter.h"

#include "CompilerSettingsView.h"
#include "GeneralSettingsView.h"
#include "LinkerSettingsView.h"
#include "IncludeSettingsView.h"

#include "debugMacros.h"

#include <Application.h>
#include <FilePanel.h>
#include <Entry.h>
#include <File.h>
#include <Path.h>
#include <Messenger.h>
#include <Alert.h>
#include <PopUpMenu.h>
#include <Button.h>
#include <TextControl.h>
#include <Menu.h>
#include <MenuItem.h>
#include <MenuBar.h>
#include <OutlineListView.h>
#include <StringView.h>
#include <ScrollView.h>
#include <ListItem.h>
#include <ClassInfo.h> //for cast_as
#include <FindDirectory.h> //per find_directory()
#include <Roster.h>
#include <Handler.h> //?
//#include <TypeConstants.h> //for B_TYPE_RECT

#include <iostream.h>
//#include <iostream>
#include <cassert>
#include <stdlib.h> //per malloc
#include <string.h> //per strcmp
#include <stdio.h> 
