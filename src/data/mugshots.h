const struct Mugshot gMugshots[] = {
    [MUGSHOT_PLACEHOLDER - 1] = {
        .tiles = (const u32[]) INCBIN_U32("graphics/mugshots/placeholder.4bpp.lz"),
        .palette = (const u16[]) INCBIN_U16("graphics/mugshots/placeholder.gbapal"),
    },
};
