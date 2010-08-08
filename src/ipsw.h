#include <plist/plist.h>

extern void check_ipsw(const char *s_ipsw);
extern int ipsw_extract_build_manifest(const char* ipsw, plist_t* buildmanifest);
extern plist_t build_manifest_get_build_identity(plist_t build_manifest);
extern int ipsw_extract_to_memory(const char* ipsw, const char* infile, char** pbuffer, uint32_t* psize);

#define BUFSIZE 0x100000

typedef struct {
	struct zip* zip;
} ipsw_archive;
