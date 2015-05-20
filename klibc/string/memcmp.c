#include <string.h>

int memcmp(const void* aptr, const void* bptr, size_t size)
{
  size_t i;
	const unsigned char* a = (const unsigned char*) aptr;
	const unsigned char* b = (const unsigned char*) bptr;
	for ( i = 0; i < size; i++ )
		if ( a[i] < b[i] )
			return -1;
		else if ( b[i] < a[i] )
			return 1;
	return 0;
}
