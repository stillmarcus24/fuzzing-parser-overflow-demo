/* Minimal coverage-free mutational fuzzer: random inputs into parse_record until ASan
 * aborts. Deterministic seed so the crash reproduces. Real campaigns use libFuzzer/AFL++
 * for coverage guidance; this is enough to surface a shallow memory-safety bug fast. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct { uint8_t type; uint8_t name[64]; uint32_t vlen; } record_t;
int parse_record(const uint8_t*,size_t,record_t*);
int main(int argc,char**argv){
  unsigned seed = argc>1?atoi(argv[1]):1;
  srand(seed);
  uint8_t buf[256]; record_t r;
  for(long i=0;i<5000000;i++){
    size_t n = rand()%200 + 1;
    for(size_t j=0;j<n;j++) buf[j]=rand()&0xff;
    if(i%500000==0){fprintf(stderr,"iter %ld\n",i);fflush(stderr);}
    parse_record(buf,n,&r);     /* ASan aborts the process on the first overflow */
  }
  puts("no crash"); return 0;
}
