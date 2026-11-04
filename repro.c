#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct { uint8_t type; uint8_t name[64]; uint32_t vlen; } record_t;
int parse_record(const uint8_t*,size_t,record_t*);
int main(int c,char**v){ FILE*f=fopen(v[1],"rb"); uint8_t b[4096]; size_t n=fread(b,1,sizeof b,f); fclose(f);
  record_t r; parse_record(b,n,&r); puts("parsed without crash"); return 0; }
