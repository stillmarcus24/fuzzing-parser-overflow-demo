/* Replays the same RNG sequence to capture and dump the first crashing input as a file,
 * so the crash is reproducible from bytes, not just from a seed. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct { uint8_t type; uint8_t name[64]; uint32_t vlen; } record_t;
static int would_overflow(const uint8_t*b,size_t n){ if(n<2)return 0; return b[1] > 63; }
int main(void){ srand(1); uint8_t buf[256];
  for(long i=0;i<5000000;i++){ size_t n=rand()%200+1; for(size_t j=0;j<n;j++)buf[j]=rand()&0xff;
    if(would_overflow(buf,n)){ FILE*f=fopen("crash-001.bin","wb"); fwrite(buf,1,n,f); fclose(f);
      printf("crash input: %zu bytes, type=0x%02x, declared name length=%u (buffer is 64)\n",n,buf[0],buf[1]); return 0; } }
  return 1; }
