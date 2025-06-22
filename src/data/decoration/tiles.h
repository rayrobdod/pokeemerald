#define DecorTilesMaxSize (DECOR_TILES_PER_ITEM * TILE_SIZE_4BPP / sizeof(u32))

#if 8 == NUM_TILES_PER_METATILE
#define DECORMETATILE_COVERED(a,b,c,d)	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, a, b, c, d
#define DECORMETATILE_SPLIT(a,b,c,d)	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, a, b, c, d
#endif
#if 12 == NUM_TILES_PER_METATILE
#define DECORMETATILE_COVERED(a,b,c,d)	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, a, b, c, d, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
#define DECORMETATILE_SPLIT(a,b,c,d)	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, a, b, c, d
#endif


// set to `DecorTilesMaxSize` to fill unused tiles with zeros for prettier tile inspection and bounds checking
// otherwise, set to empty to minimize used rom
#define DecorTilesSize


const u32 DecorTiles_SMALL_DESK[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/small_desk.4bpp");
const u16 DecorMetatiles_SMALL_DESK[] = {
    DECORMETATILE_COVERED(0x5000, BG_TILE_H_FLIP(0x5000), 0x5001, BG_TILE_H_FLIP(0x5001)),
};

const u32 DecorTiles_POKEMON_DESK[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/pokemon_desk.4bpp");
const u16 DecorMetatiles_POKEMON_DESK[] = {
    DECORMETATILE_COVERED(0x2000, BG_TILE_H_FLIP(0x2000), 0x2001, BG_TILE_H_FLIP(0x2001)),
};

const u32 DecorTiles_HEAVY_DESK[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/heavy_desk.4bpp");
const u16 DecorMetatiles_HEAVY_DESK[] = {
    DECORMETATILE_COVERED(0x4000, 0x4001, 0x4002, 0x4003),
    DECORMETATILE_COVERED(0x4401, 0x4001, 0x4403, 0x4003),
    DECORMETATILE_COVERED(0x4401, 0x4400, 0x4403, 0x4402),
    DECORMETATILE_COVERED(0x4004, 0x4005, 0x4006, 0x4007),
    DECORMETATILE_COVERED(0x4405, 0x4005, 0x4407, 0x4007),
    DECORMETATILE_COVERED(0x4405, 0x4404, 0x4407, 0x4406),
};

const u32 DecorTiles_RAGGED_DESK[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/ragged_desk.4bpp");
const u16 DecorMetatiles_RAGGED_DESK[] = {
    DECORMETATILE_COVERED(0x5000, 0x5001, 0x5002, 0x5003),
    DECORMETATILE_COVERED(0x5401, 0x5001, 0x5403, 0x5003),
    DECORMETATILE_COVERED(0x5401, 0x5400, 0x5403, 0x5402),
    DECORMETATILE_COVERED(0x5004, 0x5005, 0x5006, 0x5007),
    DECORMETATILE_COVERED(0x5405, 0x5005, 0x5407, 0x5007),
    DECORMETATILE_COVERED(0x5405, 0x5404, 0x5407, 0x5406),
};

const u32 DecorTiles_COMFORT_DESK[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/comfort_desk.4bpp");
const u16 DecorMetatiles_COMFORT_DESK[] = {
    DECORMETATILE_COVERED(0x5000, 0x5001, 0x5002, 0x5003),
    DECORMETATILE_COVERED(0x5401, 0x5001, 0x5403, 0x5003),
    DECORMETATILE_COVERED(0x5401, 0x5400, 0x5403, 0x5402),
    DECORMETATILE_COVERED(0x5004, 0x5005, 0x5006, 0x5007),
    DECORMETATILE_COVERED(0x5405, 0x5005, 0x5407, 0x5007),
    DECORMETATILE_COVERED(0x5405, 0x5404, 0x5407, 0x5406),
};

const u32 DecorTiles_PRETTY_DESK[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/pretty_desk.4bpp");
const u16 DecorMetatiles_PRETTY_DESK[] = {
    DECORMETATILE_COVERED(0x4000, 0x4001, 0x4002, 0x4003),
    DECORMETATILE_COVERED(0x4001, 0x4001, 0x4003, 0x4003),
    DECORMETATILE_COVERED(0x4001, 0x4400, 0x4003, 0x4402),
    DECORMETATILE_COVERED(0x4002, 0x4003, 0x4002, 0x4003),
    DECORMETATILE_COVERED(0x4003, 0x4003, 0x4003, 0x4003),
    DECORMETATILE_COVERED(0x4003, 0x4402, 0x4403, 0x4402),
    DECORMETATILE_COVERED(0x4004, 0x4005, 0x4006, 0x4007),
    DECORMETATILE_COVERED(0x4005, 0x4005, 0x4007, 0x4007),
    DECORMETATILE_COVERED(0x4005, 0x4404, 0x4007, 0x4406),
};

const u32 DecorTiles_BRICK_DESK[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/brick_desk.4bpp");
const u16 DecorMetatiles_BRICK_DESK[] = {
    DECORMETATILE_COVERED(0x4000, 0x4001, 0x4003, 0x4004),
    DECORMETATILE_COVERED(0x4002, 0x4402, 0x4005, 0x4405),
    DECORMETATILE_COVERED(0x4401, 0x4400, 0x4404, 0x4403),
    DECORMETATILE_COVERED(0x4003, 0x4004, 0x4003, 0x4004),
    DECORMETATILE_COVERED(0x4005, 0x4405, 0x4005, 0x4405),
    DECORMETATILE_COVERED(0x4404, 0x4403, 0x4404, 0x4403),
    DECORMETATILE_COVERED(0x4006, 0x4007, 0x4009, 0x400A),
    DECORMETATILE_COVERED(0x4007, 0x4407, 0x400A, 0x400A),
    DECORMETATILE_COVERED(0x4407, 0x4406, 0x440A, 0x4409),
};

const u32 DecorTiles_CAMP_DESK[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/camp_desk.4bpp");
const u16 DecorMetatiles_CAMP_DESK[] = {
    DECORMETATILE_COVERED(0x5000, 0x5001, 0x5002, 0x5003),
    DECORMETATILE_COVERED(0x5001, 0x5001, 0x5003, 0x5003),
    DECORMETATILE_COVERED(0x5001, 0x5402, 0x5003, 0x5400),
    DECORMETATILE_COVERED(0x5000, 0x5001, 0x5000, 0x5001),
    DECORMETATILE_COVERED(0x5001, 0x5001, 0x5001, 0x5001),
    DECORMETATILE_COVERED(0x5001, 0x5402, 0x5001, 0x5402),
    DECORMETATILE_COVERED(0x5002, 0x5003, 0x5004, 0x5005),
    DECORMETATILE_COVERED(0x5003, 0x5003, 0x5005, 0x5005),
    DECORMETATILE_COVERED(0x5003, 0x5400, 0x5005, 0x5404),
};

const u32 DecorTiles_HARD_DESK[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/hard_desk.4bpp");
const u16 DecorMetatiles_HARD_DESK[] = {
    DECORMETATILE_COVERED(0x4000, 0x4001, 0x4002, 0x4003),
    DECORMETATILE_COVERED(0x4001, 0x4401, 0x4003, 0x4403),
    DECORMETATILE_COVERED(0x4401, 0x4400, 0x4403, 0x4402),
    DECORMETATILE_COVERED(0x4002, 0x4003, 0x4002, 0x4003),
    DECORMETATILE_COVERED(0x4003, 0x4403, 0x4003, 0x4403),
    DECORMETATILE_COVERED(0x4403, 0x4402, 0x4403, 0x4402),
    DECORMETATILE_COVERED(0x4004, 0x4005, 0x4006, 0x4007),
    DECORMETATILE_COVERED(0x4005, 0x4405, 0x4007, 0x4407),
    DECORMETATILE_COVERED(0x4405, 0x4404, 0x4407, 0x4406),
};

const u32 DecorTiles_SMALL_CHAIR[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/small_chair.4bpp");
const u16 DecorMetatiles_SMALL_CHAIR[] = {
    DECORMETATILE_COVERED(0x2000, 0x2400, 0x2001, 0x2401),
};

const u32 DecorTiles_POKEMON_CHAIR[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/pokemon_chair.4bpp");

const u32 DecorTiles_HEAVY_CHAIR[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/heavy_chair.4bpp");
const u16 DecorMetatiles_HEAVY_CHAIR[] = {
    DECORMETATILE_COVERED(0x4000, 0x4400, 0x4001, 0x4401),
};

const u32 DecorTiles_PRETTY_CHAIR[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/pretty_chair.4bpp");
const u16 DecorMetatiles_PRETTY_CHAIR[] = {
    DECORMETATILE_COVERED(0x4000, 0x4400, 0x4001, 0x4401),
};

const u32 DecorTiles_COMFORT_CHAIR[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/comfort_chair.4bpp");
const u16 DecorMetatiles_COMFORT_CHAIR[] = {
    DECORMETATILE_COVERED(0x5000, 0x5400, 0x5001, 0x5401),
};

const u32 DecorTiles_RAGGED_CHAIR[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/ragged_chair.4bpp");
const u16 DecorMetatiles_RAGGED_CHAIR[] = {
    DECORMETATILE_COVERED(0x5000, 0x5400, 0x5001, 0x5401),
};

const u32 DecorTiles_BRICK_CHAIR[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/brick_chair.4bpp");
const u16 DecorMetatiles_BRICK_CHAIR[] = {
    DECORMETATILE_COVERED(0x4000, 0x4400, 0x4001, 0x4401),
};

const u32 DecorTiles_CAMP_CHAIR[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/camp_chair.4bpp");
const u16 DecorMetatiles_CAMP_CHAIR[] = {
    DECORMETATILE_COVERED(0x5000, 0x5400, 0x5001, 0x5401),
};

const u32 DecorTiles_HARD_CHAIR[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/hard_chair.4bpp");
const u16 DecorMetatiles_HARD_CHAIR[] = {
    DECORMETATILE_COVERED(0x4000, 0x4001, 0x4002, 0x4003),
};

const u32 DecorTiles_RED_PLANT[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/red_plant.4bpp");
const u16 DecorMetatiles_RED_PLANT[] = {
    DECORMETATILE_SPLIT(0x2000, 0x2001, 0x2002, 0x2003),
    DECORMETATILE_COVERED(0x2004, 0x2005, 0x2006, 0x2007),
};

const u32 DecorTiles_TROPICAL_PLANT[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/tropical_plant.4bpp");
const u16 DecorMetatiles_TROPICAL_PLANT[] = {
    DECORMETATILE_SPLIT(0x5000, 0x5001, 0x5002, 0x5003),
    DECORMETATILE_COVERED(0x5004, 0x5005, 0x5006, 0x5007),
};

const u32 DecorTiles_PRETTY_FLOWERS[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/pretty_flowers.4bpp");
const u16 DecorMetatiles_PRETTY_FLOWERS[] = {
    DECORMETATILE_SPLIT(0x2000, 0x2001, 0x2002, 0x2003),
    DECORMETATILE_COVERED(0x2004, 0x2005, 0x2006, 0x2007),
};

const u32 DecorTiles_COLORFUL_PLANT[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/colorful_plant.4bpp");
const u16 DecorMetatiles_COLORFUL_PLANT[] = {
    DECORMETATILE_SPLIT(0x5000, 0x5001, 0x5004, 0x5005),
    DECORMETATILE_SPLIT(0x5002, 0x5003, 0x5006, 0x5007),
    DECORMETATILE_COVERED(0x5008, 0x5009, 0x500C, 0x500D),
    DECORMETATILE_COVERED(0x500A, 0x500B, 0x500E, 0x500F),
};

const u32 DecorTiles_BIG_PLANT[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/big_plant.4bpp");
const u32 DecorTiles_GORGEOUS_PLANT[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/gorgeous_plant.4bpp");
const u16 DecorMetatiles_BIG_PLANT[] = {
    DECORMETATILE_SPLIT(0x5000, 0x5001, 0x5002, 0x5003),
    DECORMETATILE_SPLIT(0x5401, 0x5400, 0x5403, 0x5402),
    DECORMETATILE_COVERED(0x5004, 0x5005, 0x5006, 0x5007),
    DECORMETATILE_COVERED(0x5405, 0x5404, 0x5407, 0x5406),
};

const u32 DecorTiles_BRICK[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/brick.4bpp");
const u16 DecorMetatiles_RED_BRICK[] = {
    DECORMETATILE_COVERED(0x2000, 0x2400, 0x2001, 0x2401),
    DECORMETATILE_COVERED(0x2002, 0x2402, 0x2003, 0x2403),
};
const u16 DecorMetatiles_YELLOW_BRICK[] = {
    DECORMETATILE_COVERED(0x3000, 0x3400, 0x3001, 0x3401),
    DECORMETATILE_COVERED(0x3002, 0x3402, 0x3003, 0x3403),
};
const u16 DecorMetatiles_BLUE_BRICK[] = {
    DECORMETATILE_COVERED(0x4000, 0x4400, 0x4001, 0x4401),
    DECORMETATILE_COVERED(0x4002, 0x4402, 0x4003, 0x4403),
};

const u32 DecorTiles_BALLOON[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/balloon.4bpp");
const u16 DecorMetatiles_RED_BALLOON[] = {
    DECORMETATILE_COVERED(0x2000, 0x2001, 0x2002, 0x2003),
    DECORMETATILE_COVERED(0x2004, 0x2005, 0x2006, 0x2007),
    DECORMETATILE_COVERED(0x2008, 0x2009, 0x200A, 0x200B),
    DECORMETATILE_COVERED(0x200C, 0x200D, 0x200E, 0x200F),
};
const u16 DecorMetatiles_YELLOW_BALLOON[] = {
    DECORMETATILE_COVERED(0x3000, 0x3001, 0x3002, 0x3003),
    DECORMETATILE_COVERED(0x3004, 0x3005, 0x3006, 0x3007),
    DECORMETATILE_COVERED(0x3008, 0x3009, 0x300A, 0x300B),
    DECORMETATILE_COVERED(0x300C, 0x300D, 0x300E, 0x300F),
};
const u16 DecorMetatiles_BLUE_BALLOON[] = {
    DECORMETATILE_COVERED(0x4000, 0x4001, 0x4002, 0x4003),
    DECORMETATILE_COVERED(0x4004, 0x4005, 0x4006, 0x4007),
    DECORMETATILE_COVERED(0x4008, 0x4009, 0x400A, 0x400B),
    DECORMETATILE_COVERED(0x400C, 0x400D, 0x400E, 0x400F),
};

const u32 DecorTiles_TENT[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/tent.4bpp");
const u32 DecorTiles_GREEN_TENT[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/green_tent.4bpp");

#if 8 == NUM_TILES_PER_METATILE
const u16 DecorMetatiles_RED_TENT[] = {
    0xFFFF, 0xFFFF, 0xFFFF, 0x4005, 0x2004, 0x2000, 0x200A, 0x2006,
    0x4005, 0x4005, 0x4005, 0x4005, 0x2001, 0x2002, 0x2007, 0x2008,
    0xFFFF, 0xFFFF, 0x4005, 0xFFFF, 0x2400, 0x2404, 0x2009, 0x240A,
    0xFFFF, 0x4005, 0xFFFF, 0x4005, 0x2003, 0x200C, 0x2003, 0x2012,
    0x4005, 0x4005, 0x4005, 0x4005, 0x200D, 0x200E, 0x2013, 0x2014,
    0x4005, 0xFFFF, 0x4005, 0xFFFF, 0x200F, 0x2403, 0x2412, 0x2403,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x2010, 0x2011, 0x2016, 0x2017,
    0x4015, 0x4415, 0x400B, 0x440B, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x2411, 0x2410, 0x2417, 0x2416,
};

const u16 DecorMetatiles_BLUE_TENT[] = {
    0xFFFF, 0xFFFF, 0xFFFF, 0x4005, 0x4004, 0x4000, 0x400A, 0x4006,
    0x4005, 0x4005, 0x4005, 0x4005, 0x4001, 0x4002, 0x4007, 0x4008,
    0xFFFF, 0xFFFF, 0x4005, 0xFFFF, 0x4400, 0x4404, 0x4009, 0x440A,
    0xFFFF, 0x4005, 0xFFFF, 0x4005, 0x4003, 0x400C, 0x4003, 0x4012,
    0x4005, 0x4005, 0x4005, 0x4005, 0x400D, 0x400E, 0x4013, 0x4014,
    0x4005, 0xFFFF, 0x4005, 0xFFFF, 0x400F, 0x4403, 0x4412, 0x4403,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x4010, 0x4011, 0x4016, 0x4017,
    0x4015, 0x4415, 0x400B, 0x440B, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x4411, 0x4410, 0x4417, 0x4416,
};

const u16 DecorMetatiles_GREEN_TENT[] = {
    0xFFFF, 0xFFFF, 0xFFFF, 0x5005, 0x5004, 0x5000, 0x500A, 0x5006,
    0x5005, 0x5005, 0x5005, 0x5005, 0x5001, 0x5002, 0x5007, 0x5008,
    0xFFFF, 0xFFFF, 0x5005, 0xFFFF, 0x5400, 0x5404, 0x5009, 0x540A,
    0xFFFF, 0x5005, 0xFFFF, 0x5005, 0x5003, 0x500C, 0x5003, 0x5012,
    0x5005, 0x5005, 0x5005, 0x5005, 0x500D, 0x500E, 0x5013, 0x5014,
    0x5005, 0xFFFF, 0x5005, 0xFFFF, 0x500F, 0x5403, 0x5412, 0x5403,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x5010, 0x5011, 0x5016, 0x5017,
    0x5015, 0x5415, 0x500B, 0x540B, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x5411, 0x5410, 0x5417, 0x5416,
};
#else
const u16 DecorMetatiles_RED_TENT[] = {
    0xFFFF, 0xFFFF, 0xFFFF, 0x4005, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x2004, 0x2000, 0x200A, 0x2006,
    0x4005, 0x4005, 0x4005, 0x4005, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x2001, 0x2002, 0x2007, 0x2008,
    0xFFFF, 0xFFFF, 0x4005, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x2400, 0x2404, 0x2009, 0x240A,
    0xFFFF, 0x4005, 0xFFFF, 0x4005, 0x2003, 0x200C, 0x2003, 0x2012, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0x4005, 0x4005, 0x4005, 0x4005, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x200D, 0x200E, 0x2013, 0x2014,
    0x4005, 0xFFFF, 0x4005, 0xFFFF, 0x200F, 0x2403, 0x2412, 0x2403, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x2010, 0x2011, 0x2016, 0x2017, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0x4015, 0x4415, 0x400B, 0x440B, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x2411, 0x2410, 0x2417, 0x2416, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
};

const u16 DecorMetatiles_BLUE_TENT[] = {
    0xFFFF, 0xFFFF, 0xFFFF, 0x4005, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x4004, 0x4000, 0x400A, 0x4006,
    0x4005, 0x4005, 0x4005, 0x4005, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x4001, 0x4002, 0x4007, 0x4008,
    0xFFFF, 0xFFFF, 0x4005, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x4400, 0x4404, 0x4009, 0x440A,
    0xFFFF, 0x4005, 0xFFFF, 0x4005, 0x4003, 0x400C, 0x4003, 0x4012, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0x4005, 0x4005, 0x4005, 0x4005, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x400D, 0x400E, 0x4013, 0x4014,
    0x4005, 0xFFFF, 0x4005, 0xFFFF, 0x400F, 0x4403, 0x4412, 0x4403, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x4010, 0x4011, 0x4016, 0x4017, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0x4015, 0x4415, 0x400B, 0x440B, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x4411, 0x4410, 0x4417, 0x4416, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
};

const u16 DecorMetatiles_GREEN_TENT[] = {
    0xFFFF, 0xFFFF, 0xFFFF, 0x5005, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x5004, 0x5000, 0x500A, 0x5006,
    0x5005, 0x5005, 0x5005, 0x5005, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x5001, 0x5002, 0x5007, 0x5008,
    0xFFFF, 0xFFFF, 0x5005, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x5400, 0x5404, 0x5009, 0x540A,
    0xFFFF, 0x5005, 0xFFFF, 0x5005, 0x5003, 0x500C, 0x5003, 0x5012, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0x5005, 0x5005, 0x5005, 0x5005, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x500D, 0x500E, 0x5013, 0x5014,
    0x5005, 0xFFFF, 0x5005, 0xFFFF, 0x500F, 0x5403, 0x5412, 0x5403, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x5010, 0x5011, 0x5016, 0x5017, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0x5015, 0x5415, 0x500B, 0x540B, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x5411, 0x5410, 0x5417, 0x5416, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
};
#endif

const u32 DecorTiles_SOLID_BOARD[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/solid_board.4bpp");
const u16 DecorMetatiles_SOLID_BOARD[] = {
    DECORMETATILE_COVERED(0x5802, 0x5803, 0x5800, 0x5801),
    DECORMETATILE_COVERED(0x5000, 0x5001, 0x5002, 0x5003),
};

const u32 DecorTiles_SLIDE[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/slide.4bpp");
const u16 DecorMetatiles_SLIDE[] = {
    DECORMETATILE_COVERED(0x4000, 0x4001, 0x4002, 0x4003),
    DECORMETATILE_COVERED(0x4401, 0x4400, 0x4405, 0x4404),
    DECORMETATILE_COVERED(0x4006, 0x4406, 0x4006, 0x4406),
    DECORMETATILE_COVERED(0x400C, 0x400D, 0x400C, 0x400D),
    DECORMETATILE_COVERED(0x4008, 0x4009, 0x400A, 0x400B),
    DECORMETATILE_COVERED(0x400C, 0x400D, 0x400E, 0x400F),
    DECORMETATILE_COVERED(0x4010, 0x4011, 0x4012, 0x4013),
    DECORMETATILE_COVERED(0x4411, 0x4410, 0x4413, 0x4412),
};

const u32 DecorTiles_FENCE_LENGTH[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/fence_length.4bpp");
const u32 DecorTiles_FENCE_WIDTH[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/fence_width.4bpp");
const u16 DecorMetatiles_FENCE[] = {
    DECORMETATILE_COVERED(0x5000, 0x5400, 0x5001, 0x5401),
};

const u32 DecorTiles_TIRE[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/tire.4bpp");
const u16 DecorMetatiles_TIRE[] = {
    DECORMETATILE_COVERED(0x4000, 0x4001, 0x4002, 0x4003),
    DECORMETATILE_COVERED(0x4401, 0x4400, 0x4403, 0x4402),
    DECORMETATILE_COVERED(0x4004, 0x4005, 0x4006, 0x4007),
    DECORMETATILE_COVERED(0x4405, 0x4404, 0x4407, 0x4406),
};

const u32 DecorTiles_STAND[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/stand.4bpp");
const u16 DecorMetatiles_STAND[] = {
    DECORMETATILE_COVERED(0x2000, 0x2001, 0x2003, 0x2004),
    DECORMETATILE_COVERED(0x2002, 0x2002, 0x2005, 0x2005),
    DECORMETATILE_COVERED(0x2002, 0x2002, 0x2005, 0x2005),
    DECORMETATILE_COVERED(0x2401, 0x2400, 0x2404, 0x2403),
    DECORMETATILE_COVERED(0x2006, 0x2007, 0x2009, 0x200A),
    DECORMETATILE_COVERED(0x2008, 0x2008, 0x200B, 0x200B),
    DECORMETATILE_COVERED(0x2008, 0x2008, 0x200B, 0x200B),
    DECORMETATILE_COVERED(0x2407, 0x2406, 0x240A, 0x2409),
};

const u32 DecorTiles_MUD_BALL[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/mud_ball.4bpp");
const u16 DecorMetatiles_MUD_BALL[] = {
    DECORMETATILE_COVERED(0x5000, 0x5001, 0x5002, 0x5003),
    DECORMETATILE_COVERED(0x5004, 0x5005, 0x5006, 0x5007),
    DECORMETATILE_COVERED(0x5008, 0x5009, 0x500A, 0x500B),
    DECORMETATILE_COVERED(0x500C, 0x500D, 0x500E, 0x500F),
};

const u32 DecorTiles_BREAKABLE_DOOR[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/breakable_door.4bpp");
const u16 DecorMetatiles_BREAKABLE_DOOR[] = {
    DECORMETATILE_SPLIT(0x5000, 0x5001, 0x5002, 0x5003),
    DECORMETATILE_COVERED(0x5004, 0x5005, 0x5006, 0x5007),
    DECORMETATILE_SPLIT(0x5008, 0x5009, 0x500A, 0x500B),
    DECORMETATILE_COVERED(0x500C, 0x500D, 0x500E, 0x500F),
};

const u32 DecorTiles_SAND_ORNAMENT[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/sand_ornament.4bpp");
const u16 DecorMetatiles_SAND_ORNAMENT[] = {
    DECORMETATILE_SPLIT(0x5000, 0x5001, 0x5002, 0x5003),
    DECORMETATILE_COVERED(0x5004, 0x5005, 0x5006, 0x5007),
    DECORMETATILE_SPLIT(0x5008, 0x5009, 0x500A, 0x500B),
    DECORMETATILE_COVERED(0x500C, 0x500D, 0x500E, 0x500F),
    DECORMETATILE_SPLIT(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF),
    DECORMETATILE_COVERED(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF),
};

const u32 DecorTiles_SILVER_SHIELD[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/silver_shield.4bpp");
const u32 DecorTiles_GOLD_SHIELD[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/gold_shield.4bpp");
const u16 DecorMetatiles_SILVER_SHIELD[] = {
    DECORMETATILE_SPLIT(0xFFFF, 0xFFFF, 0x3000, 0x3001),
    DECORMETATILE_COVERED(0x3002, 0x3003, 0x3004, 0x3005),
};

const u32 DecorTiles_GLASS_ORNAMENT[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/glass_ornament.4bpp");
const u16 DecorMetatiles_GLASS_ORNAMENT[] = {
    DECORMETATILE_SPLIT(0x4000, 0x4001, 0x4002, 0x4003),
    DECORMETATILE_COVERED(0x4004, 0x4005, 0x4006, 0x4007),
};

const u32 DecorTiles_TV[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/tv.4bpp");
const u16 DecorMetatiles_TV[] = {
    DECORMETATILE_COVERED(0x4000, 0x4001, 0x4002, 0x4003),
};

const u32 DecorTiles_ROUND_TV[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/round_tv.4bpp");
const u16 DecorMetatiles_ROUND_TV[] = {
    DECORMETATILE_COVERED(0x5000, 0x5001, 0x5002, 0x5003),
};

const u32 DecorTiles_CUTE_TV[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/cute_tv.4bpp");
const u16 DecorMetatiles_CUTE_TV[] = {
    DECORMETATILE_COVERED(0x1000, 0x1001, 0x1002, 0x1003),
};

const u32 DecorTiles_GLITTER_MAT[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/glitter_mat.4bpp");
const u16 DecorMetatiles_GLITTER_MAT[] = {
    DECORMETATILE_COVERED(0x4000, 0x4001, 0x4002, 0x4003),
};

const u32 DecorTiles_JUMP_MAT[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/jump_mat.4bpp");
const u16 DecorMetatiles_JUMP_MAT[] = {
    DECORMETATILE_COVERED(0x4000, 0x4400, 0x4001, 0x4401),
};

const u32 DecorTiles_SPIN_MAT[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/spin_mat.4bpp");
const u16 DecorMetatiles_SPIN_MAT[] = {
    DECORMETATILE_COVERED(0x3000, 0x3001, 0x3002, 0x3003),
};

const u32 DecorTiles_NOTE_MAT[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/note_mat.4bpp");
const u16 DecorMetatiles_NOTE_MAT[] = {
    DECORMETATILE_COVERED(0x4000, 0x4001, 0x4002, 0x4003),
};

const u32 DecorTiles_SURF_MAT[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/surf_mat.4bpp");
const u32 DecorTiles_THUNDER_MAT[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/thunder_mat.4bpp");
const u32 DecorTiles_FIRE_BLAST_MAT[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/fire_blast_mat.4bpp");
const u32 DecorTiles_POWDER_SNOW_MAT[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/powder_snow_mat.4bpp");
const u32 DecorTiles_ATTRACT_MAT[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/attract_mat.4bpp");
const u32 DecorTiles_FISSURE_MAT[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/fissure_mat.4bpp");
const u32 DecorTiles_SPIKES_MAT[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/spikes_mat.4bpp");

const u16 DecorMetatiles_MAT_3x3_PAL2[] = {
    DECORMETATILE_COVERED(0x2000, 0x2001, 0x2003, 0x2004),
    DECORMETATILE_COVERED(0x2002, 0x2402, 0x2005, 0x2405),
    DECORMETATILE_COVERED(0x2401, 0x2400, 0x2404, 0x2403),
    DECORMETATILE_COVERED(0x2006, 0x2007, 0x2806, 0x2807),
    DECORMETATILE_COVERED(0x2008, 0x2408, 0x2808, 0x2C08),
    DECORMETATILE_COVERED(0x2407, 0x2406, 0x2C07, 0x2C06),
    DECORMETATILE_COVERED(0x2803, 0x2804, 0x2800, 0x2801),
    DECORMETATILE_COVERED(0x2805, 0x2C05, 0x2802, 0x2C02),
    DECORMETATILE_COVERED(0x2C04, 0x2C03, 0x2C01, 0x2C00),
};

const u16 DecorMetatiles_MAT_3x3_PAL3[] = {
    DECORMETATILE_COVERED(0x3000, 0x3001, 0x3003, 0x3004),
    DECORMETATILE_COVERED(0x3002, 0x3402, 0x3005, 0x3405),
    DECORMETATILE_COVERED(0x3401, 0x3400, 0x3404, 0x3403),
    DECORMETATILE_COVERED(0x3006, 0x3007, 0x3806, 0x3807),
    DECORMETATILE_COVERED(0x3008, 0x3408, 0x3808, 0x3C08),
    DECORMETATILE_COVERED(0x3407, 0x3406, 0x3C07, 0x3C06),
    DECORMETATILE_COVERED(0x3803, 0x3804, 0x3800, 0x3801),
    DECORMETATILE_COVERED(0x3805, 0x3C05, 0x3802, 0x3C02),
    DECORMETATILE_COVERED(0x3C04, 0x3C03, 0x3C01, 0x3C00),
};

const u16 DecorMetatiles_MAT_3x3_PAL4[] = {
    DECORMETATILE_COVERED(0x4000, 0x4001, 0x4003, 0x4004),
    DECORMETATILE_COVERED(0x4002, 0x4402, 0x4005, 0x4405),
    DECORMETATILE_COVERED(0x4401, 0x4400, 0x4404, 0x4403),
    DECORMETATILE_COVERED(0x4006, 0x4007, 0x4806, 0x4807),
    DECORMETATILE_COVERED(0x4008, 0x4408, 0x4808, 0x4C08),
    DECORMETATILE_COVERED(0x4407, 0x4406, 0x4C07, 0x4C06),
    DECORMETATILE_COVERED(0x4803, 0x4804, 0x4800, 0x4801),
    DECORMETATILE_COVERED(0x4805, 0x4C05, 0x4802, 0x4C02),
    DECORMETATILE_COVERED(0x4C04, 0x4C03, 0x4C01, 0x4C00),
};

const u16 DecorMetatiles_MAT_3x3_PAL5[] = {
    DECORMETATILE_COVERED(0x5000, 0x5001, 0x5003, 0x5004),
    DECORMETATILE_COVERED(0x5002, 0x5402, 0x5005, 0x5405),
    DECORMETATILE_COVERED(0x5401, 0x5400, 0x5404, 0x5403),
    DECORMETATILE_COVERED(0x5006, 0x5007, 0x5806, 0x5807),
    DECORMETATILE_COVERED(0x5008, 0x5408, 0x5808, 0x5C08),
    DECORMETATILE_COVERED(0x5407, 0x5406, 0x5C07, 0x5C06),
    DECORMETATILE_COVERED(0x5803, 0x5804, 0x5800, 0x5801),
    DECORMETATILE_COVERED(0x5805, 0x5C05, 0x5802, 0x5C02),
    DECORMETATILE_COVERED(0x5C04, 0x5C03, 0x5C01, 0x5C00),
};

const u32 DecorTiles_BALL_POSTER[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/ball_poster.4bpp");
const u32 DecorTiles_GREEN_POSTER[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/green_poster.4bpp");
const u32 DecorTiles_RED_POSTER[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/red_poster.4bpp");
const u32 DecorTiles_BLUE_POSTER[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/blue_poster.4bpp");
const u32 DecorTiles_CUTE_POSTER[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/cute_poster.4bpp");

const u16 DecorMetatiles_POSTER_1x1_PAL1[] = {
    DECORMETATILE_COVERED(0x1000, 0x1001, 0x1002, 0x1003),
};

const u16 DecorMetatiles_POSTER_1x1_PAL4[] = {
    DECORMETATILE_COVERED(0x4000, 0x4001, 0x4002, 0x4003),
};

const u32 DecorTiles_PIKA_POSTER[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/pika_poster.4bpp");
const u32 DecorTiles_LONG_POSTER[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/long_poster.4bpp");
const u32 DecorTiles_SEA_POSTER[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/sea_poster.4bpp");
const u32 DecorTiles_SKY_POSTER[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/sky_poster.4bpp");
const u32 DecorTiles_KISS_POSTER[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/kiss_poster.4bpp");
const u32 DecorTiles_DADS_SCROLL[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/dads_scroll.4bpp");
const u32 DecorTiles_REGION_MAP[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/region_map.4bpp");

const u16 DecorMetatiles_POSTER_2x1_PAL1[] = {
    DECORMETATILE_COVERED(0x1000, 0x1001, 0x1004, 0x1005),
    DECORMETATILE_COVERED(0x1002, 0x1003, 0x1006, 0x1007),
};

const u16 DecorMetatiles_POSTER_2x1_PAL3[] = {
    DECORMETATILE_COVERED(0x3000, 0x3001, 0x3004, 0x3005),
    DECORMETATILE_COVERED(0x3002, 0x3003, 0x3006, 0x3007),
};

const u16 DecorMetatiles_POSTER_2x1_PAL5[] = {
    DECORMETATILE_COVERED(0x5000, 0x5001, 0x5004, 0x5005),
    DECORMETATILE_COVERED(0x5002, 0x5003, 0x5006, 0x5007),
};

const u32 DecorTiles_SMALL_BOOKSHELF[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/small_bookshelf.4bpp");
const u32 DecorTiles_BIG_BOOKSHELF[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/big_bookshelf.4bpp");

const u16 DecorMetatiles_CANDLESTICK[] = {
    DECORMETATILE_SPLIT(0x1000, 0x1001, 0x1004, 0x1005),
    DECORMETATILE_COVERED(0x1008, 0x1009, 0x100C, 0x100D),
    DECORMETATILE_SPLIT(0x1002, 0x1003, 0x1006, 0x1007),
    DECORMETATILE_COVERED(0x100A, 0x100B, 0x100E, 0x100F),
};

const u32 DecorTiles_CANDLESTICK[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/candlestick.4bpp");

const u16 DecorMetatiles_ICY_DESK[] = {
    DECORMETATILE_COVERED(0x4000, 0x4001, 0x4006, 0x4007),
    DECORMETATILE_COVERED(0x4002, 0x4001, 0x4008, 0x4007),
    DECORMETATILE_COVERED(0x4002, 0x4003, 0x4008, 0x4009),
    DECORMETATILE_COVERED(0x4004, 0x4008, 0x400A, 0x400B),
    DECORMETATILE_COVERED(0x4007, 0x4008, 0x4008, 0x4007),
    DECORMETATILE_COVERED(0x4007, 0x4005, 0x4008, 0x4009),
    DECORMETATILE_COVERED(0x400C, 0x400D, 0x4012, 0x4013),
    DECORMETATILE_COVERED(0x400E, 0x400F, 0x4014, 0x4015),
    DECORMETATILE_COVERED(0x4010, 0x4011, 0x4016, 0x4017),
};

const u32 DecorTiles_ICY_DESK[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/icy_desk.4bpp");


const u32 DecorTiles_BONSAI[DecorTilesSize] = INCBIN_U32("graphics/decorations/tiles/bonsai.4bpp");
const u16 DecorMetatiles_BONSAI[] = {
    DECORMETATILE_COVERED(0x5000, 0x5001, 0x5002, 0x5003),
};

#undef DECORMETATILE_COVERED
#undef DECORMETATILE_SPLIT
