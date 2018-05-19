#ifndef CHARN_CONFIG
#define CHARN_CONFIG

typedef struct CharnConfig {
	int verbose;
	int errorsoftfail;
} CharnConfig;

int init_config(CharnConfig *cc);

// dir may be NULL, and file may specify directories, but file must be
// non-NULL. If dir is non-NULL, it will be concatenated to file. Returns 0 if
// the file was successfully loaded, parsed, and used.
int load_config(CharnConfig *cc, const char *dir, const char *file);

#endif
