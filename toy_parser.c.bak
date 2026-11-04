/* A tiny TLV ("tag-length-value") binary parser, the kind embedded in many network and
 * file formats. It contains a deliberately realistic bug: it trusts the length field in
 * the input and copies that many bytes into a fixed 64-byte stack buffer, without checking
 * it against the buffer size OR the actual remaining input. Classic CWE-121 / CWE-787. */
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
typedef struct { uint8_t type; uint8_t name[64]; uint32_t vlen; } record_t;
int parse_record(const uint8_t *buf, size_t len, record_t *out) {
    if (len < 2) return -1;
    out->type = buf[0];
    uint8_t nlen = buf[1];                 /* attacker-controlled length */
    memcpy(out->name, buf + 2, nlen);      /* BUG: no bound on nlen vs sizeof(out->name) or len */
    out->name[nlen] = 0;
    return 0;
}
