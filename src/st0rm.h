#ifndef ST0RM_h
	#define ST0RM_h
	#include <glib.h>

	extern gchar *ipsw;

	// Can this device be jailbroken directly?
	extern int pwnable;

	// The target, really only used for calling dfu-util...
	extern const char *target;

	// A human-readable target string...
	extern const char *hr_target_str;

	// Flag to see if there actually is a baseband TO update...
	extern int bb_can_up;
#endif
