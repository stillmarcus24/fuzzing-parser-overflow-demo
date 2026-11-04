#include <stdint.h>
#include <stddef.h>
typedef struct { uint8_t type; uint8_t name[64]; uint32_t vlen; } record_t;
int parse_record(const uint8_t*,size_t,record_t*);
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size){ record_t r; parse_record(data,size,&r); return 0; }
