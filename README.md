# Fuzzing a binary parser to a memory-safety crash (CWE-121 / CWE-787)

A short, reproducible demonstration of finding and fixing a stack-buffer-overflow in a C binary parser using sanitizer-assisted fuzzing. Built recent (2026-11) with AI assistance; every step below runs on a stock Linux box with gcc, no special tooling.

## The target
`toy_parser.c` is a minimal TLV ("tag-length-value") parser — the shape of code embedded in countless network and file-format handlers. It reads a one-byte length from the input and `memcpy`s that many bytes into a fixed 64-byte stack buffer, trusting the attacker-controlled length. That is the classic parser overflow.

## Finding it
`fuzz_main.c` feeds randomized inputs into `parse_record` under AddressSanitizer. Within seconds ASan aborts on the first out-of-bounds write.

```
gcc -g -O1 -fsanitize=address,undefined toy_parser.c fuzz_main.c -o fuzzer
./fuzzer
```

## Reproducing from bytes
The crashing input is saved as `crash-001.bin` (184 bytes; declares a name length of 105 into a 64-byte buffer). It reproduces deterministically:

```
gcc -g -O1 -fsanitize=address toy_parser.c repro.c -o repro
./repro crash-001.bin
```

ASan trace (`asan_trace.txt`):

    ==1985438==ERROR: AddressSanitizer: stack-buffer-overflow on address 0x6e0634df0078 at pc 0x720636d28d2d bp 0x7fff5291f400 sp 0x7fff5291eba8
    WRITE of size 105 at 0x6e0634df0078 thread T0
        #0 0x720636d28d2c in memcpy ../../../../src/libsanitizer/sanitizer_common/sanitizer_common_interceptors_memintrinsics.inc:117
        #1 0x5c4d1e94f395 in memcpy /usr/include/x86_64-linux-gnu/bits/string_fortified.h:29
        #2 0x5c4d1e94f395 in parse_record /home/marcus/build/fuzz-demo/toy_parser.c:13
        #3 0x5c4d1e94f539 in main /home/marcus/build/fuzz-demo/repro.c:7
        #4 0x72063682a600  (/usr/lib/x86_64-linux-gnu/libc.so.6+0x2a600) (BuildId: 066527e430a32768d82741e00b81eebb1a872294)
        #5 0x72063682a717 in __libc_start_main (/usr/lib/x86_64-linux-gnu/libc.so.6+0x2a717) (BuildId: 066527e430a32768d82741e00b81eebb1a872294)
        #6 0x5c4d1e94f244 in _start (/home/marcus/build/fuzz-demo/repro+0x1244) (BuildId: 69efd675d67a644f16db74fb8768764bd9c0cd7b)
    
    Address 0x6e0634df0078 is located in stack of thread T0 at offset 120 in frame
        #0 0x5c4d1e94f419 in main /home/marcus/build/fuzz-demo/repro.c:6
    
      This frame has 2 object(s):
        [48, 120) 'r' (line 7)

## Root cause
`parse_record` bounds the copy by neither the destination size nor the remaining input length. A `nlen` above 63 overflows `out->name`; a `nlen` that exceeds the bytes present also reads past the input.

## The fix
`patch.diff` rejects any record whose declared length runs past the input or overflows the destination, before the copy. After the patch, `./repro crash-001.bin` prints "parsed without crash" and the fuzzer runs to completion with no ASan abort.

## What stays human
Deciding that an over-length record should be rejected (vs. truncated) is a protocol decision, not something a fuzzer answers. The fuzzer finds the memory-safety violation; a person decides the correct contract.

Author: Marcus Still, Still OS Digital Holdings LLC. MIT.
