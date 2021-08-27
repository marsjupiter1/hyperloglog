#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
	
	//-----------------------------------------------------------------------------
	
	void MurmurHash3_x86_32 (const void *key, int len, uint32_t seed, void *out);
	
	void MurmurHash3_x86_128(const void *key, int len, uint32_t seed, void *out);
	
	void MurmurHash3_x64_128(const void *key, int len, uint32_t seed, void *out);
	
	//-----------------------------------------------------------------------------
	
#ifdef __cplusplus
}
#endif