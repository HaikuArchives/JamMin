//CompilerSettingsView.h

#ifndef COMPILER_SETTINGS_H
#define COMPILER_SETTINGS_H

#include "TargetConfig.h" //for enumerations

#include <View.h>
#include <Rect.h>
#include <Message.h>
#include <TextControl.h>
#include <Menu.h>
#include <MenuField.h>
//#include <PopUpMenu.h>
#include <MenuItem.h>
#include <CheckBox.h>

class CompilerSettingsView: public BView
{
	public:
				CompilerSettingsView(BRect frame, const char *name,
					uint32 resizingMode = B_FOLLOW_ALL, uint32 flags = B_WILL_DRAW);
		virtual	~CompilerSettingsView();
		
		void	SetOptimizationLevel(OptimizeOpt optLevel);
		void	SetWarningsOption(WarningOpt opt);
		void	EnableSymbols(bool enable);
		void	EnableDebugger(bool enable);
		void	SetDefines(const char* defs);
		void	SetAdditionalFlags(const char* flags);
		
		OptimizeOpt GetOptimizationLevel();
		WarningOpt	GetWarningsOption();
		bool		SymbolsEnabled();
		bool		DebuggerEnabled();
		const char*	GetDefines();
		const char*	GetAdditionalFlags();
		
	private:
		OptimizeOpt	fOptimizeOpt;
		WarningOpt	fWarningOpt;
		bool		fSymbolsEnabled;
		bool		fDebuggerEnabled;
		BString		fDefines;
		BString		fAdditionalFlags;
		
		BTextControl*	fFlagsText;
		BTextControl*	fDefinesText;
		BMenuField*		fOptimizationMenu;
		BMenuField*		fWarningsMenu;
		BCheckBox*		fSymbolsEnabledBox;
		BCheckBox*		fDebuggerEnabledBox;
		//check boxes...
};


#endif //COMPILER_SETTINGS_H