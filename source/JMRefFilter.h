//file JMRefFilter.h

#ifndef JM_REFFILTER_H
#define JM_REFFILTER_H

#include "debugMacros.h"
#include <FilePanel.h>
#include <String.h>
#include <string.h>

//const char* kJamminFiletype	= "application/x-vnd.generic-JamMin-File";
//const char* kJamminFileExt	= "jmin";

#define kJamminFiletype	"application/x-vnd.generic-JamMin-File"
#define kBeDirType		"application/x-vnd.Be-directory"
#define kBeSymLinkType	"application/x-vnd.Be-symlink"
#define kBeVolumeType	"application/x-vnd.Be-volume"
#define kJamminFileExt	"jmin"

class JMRefFilter : public BRefFilter {


	bool Filter(const entry_ref *ref,
		BNode *node,
		struct stat_beos *st,
		const char *filetype)
	{
		//DPRINT("JMRefFilter::Filter: filetype = " << filetype);
		if ( strcmp(kJamminFiletype, filetype) == 0
			|| strcmp(kBeDirType, filetype) == 0
			|| strcmp(kBeSymLinkType, filetype) == 0
			|| strcmp(kBeVolumeType, filetype) == 0) return true;
		//file type is not correct. Check extension
		BString name(ref->name);
		int32 index;
		if ( (index = name.FindLast(kJamminFileExt)) == B_ERROR) return false;
		//is this really the file extension?
		if (index != (name.CountChars() - (int32)strlen(kJamminFileExt)) ) return false;
		return true;
	}
};

#endif //JM_REFFILTER_H
