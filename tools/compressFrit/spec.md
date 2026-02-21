Many tilemaps consist of one or two tiles used many times and many tiles used once.
In addition, automatically generated tileset/tilemap pairs,
such as those made by "Tilemap Studio"'s `Image to Tiles` tool or by "SuperFamiconV",
order tiles in order of first use.
In its purest form, this results in a tilemap whose sequence consists of
a counting sequence alternating with a constant sequence.
A delta-and-sliding-window compression is good at compressing a counting sequence part
and is good at compressing a constant sequence part,
but is bad at compressing the transition since the delta at each transition will be a different number.

This specification defines a lossless compressed data format 
which is tuned to the types of tilemaps described above.

# Format

In the byte diagrams below, a single `|` represents a field boundary while a double `|` represents a byte boundary.
A field is written with a name, followed by a `:`, followed by the number of bits.
Within a byte, the most-significant bits are written to the left and the least-significant bits are written to the right.
Multi-byte numbers are stored with the least significant byte first.

The uncompressed data is either a sequence of 16-bit words or a sequence of 8-bit words.

The compressed format consists of a four-byte-long header followed by any number of instructions.

The worst-case expansion is four bytes per byte.

## Header

The header is four bytes long.
The first byte is a magic number, and the next three bytes is the length of the uncompressed data in bytes.
This is the same header as the GBA BIOS compression formats.
The magic number determines whether the format is the 8-bit-word variant or the 16-bit-word variant.

    || magic : 8 || low_size : 8 || middle_size : 8 || hi_size : 8 ||

## Instructions

An array of four word-sized registers is maintained by the decoder. 
These registers are zero-initialized.

Each instruction consists of one-or-more bytes.
The two most-significant bits of the first byte is the opcode.

If the compressed data size is not naturally a multiple of four bytes,
the compressed data should be padded with `0b0000_0000` bytes.

### XOR
    || 0 0 | from: 2 | to: 2 | has_hi: 1 | has_low: 1 || (if has_hi) ⊻hi : 8 || (if has_low) ⊻low : 8 ||

`to <- from XOR (⊻hi << 8 | ⊻low)`

If `has_hi` is `0`, the `⊻hi` byte is not present in the byte stream, and `⊻hi` is `0`
If `has_low` is `0`, the `⊻low` byte is not present in the byte stream, and `⊻low` is `0`

In the eight-bit variant, `has_hi` SHOULD be `0`.
However, if `has_hi` is `1`, then the `⊻hi` byte is still present in the byte stream. 
Since the `⊻hi` value affect bits out of range of the 8-bit register, `⊻hi` is effectively ignored.

The XOR operations in which `from` is equal to `to` and in which `has_hi` and `has_low` are both `0` are no-ops.
The no-op in which `from` = `to` = `0` is encoded as `0x00` and is an intentional no-op and used as a padding byte.
The other no-ops SHOULD NOT appear in compressed data streams.

### RUN
    || 1 0 | reg : 2 | lengthM1 : 4 || (if lengthM1 == 0b1111) lengthM31 : 8 ||

`length` times, write the value in register `reg` to output

If `lengthM1` is not `0b1111`, `length` is `lengthM1 + 1`, and `lengthM31` is not serialized.
Otherwise, `length` is `lengthM31 + 31`.

Use two consecutive `RUN`s to represent a run with length between 16 and 30 inclusive

### SLIDE UP
    || 1 1 | ... (same as RUN)
    
`length` times, write the value in register `reg` to output and post-increment the value in `reg`

The increment wraps around.

### SLIDE DOWN
    || 0 1 | ... (same as RUN)

`length` times, write the value in register `reg` to output and post-decrement the value in `reg`

The decrement wraps around.
