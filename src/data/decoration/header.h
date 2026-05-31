const struct Decoration gDecorations[] =
{
    [DECOR_NONE] =
    {
        .id = DECOR_NONE,
        .name = _("SMALL DESK"),
        .permission = DECORPERM_SOLID_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DESK,
        .price = 0,
        .description = COMPOUND_STRING(
            "A small desk built\n"
            "for one."),
        .tiles = DecorTiles_SMALL_DESK,
        .icon = {gItemIcon_QuestionMark, gItemIconPalette_QuestionMark},
        .metatiles = DecorMetatiles_SMALL_DESK,
        .attributes = DecorAttributes_Desk_1x1,
    },

    [DECOR_SMALL_DESK] =
    {
        .id = DECOR_SMALL_DESK,
        .name = _("SMALL DESK"),
        .permission = DECORPERM_SOLID_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DESK,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A small desk built\n"
            "for one."),
        .tiles = DecorTiles_SMALL_DESK,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_SMALL_DESK,
        .attributes = DecorAttributes_Desk_1x1,
    },

    [DECOR_POKEMON_DESK] =
    {
        .id = DECOR_POKEMON_DESK,
        .name = _("POKéMON DESK"),
        .permission = DECORPERM_SOLID_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DESK,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A small desk built in\n"
            "the shape of a POKé\n"
            "BALL."),
        .tiles = DecorTiles_POKEMON_DESK,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_POKEMON_DESK,
        .attributes = DecorAttributes_Desk_1x1,
    },

    [DECOR_HEAVY_DESK] =
    {
        .id = DECOR_HEAVY_DESK,
        .name = _("HEAVY DESK"),
        .permission = DECORPERM_SOLID_FLOOR,
        .shape = DECORSHAPE_3x2,
        .category = DECORCAT_DESK,
        .price = 6000,
        .description = COMPOUND_STRING(
            "A large desk made\n"
            "of steel. Put some\n"
            "decorations on it."),
        .tiles = DecorTiles_HEAVY_DESK,
        .icon = {gDecorIcon_HeavyDesk, gDecorIconPalette_HeavyDesk},
        .metatiles = DecorMetatiles_HEAVY_DESK,
        .attributes = DecorAttributes_Desk_3x2,
    },

    [DECOR_RAGGED_DESK] =
    {
        .id = DECOR_RAGGED_DESK,
        .name = _("RAGGED DESK"),
        .permission = DECORPERM_SOLID_FLOOR,
        .shape = DECORSHAPE_3x2,
        .category = DECORCAT_DESK,
        .price = 6000,
        .description = COMPOUND_STRING(
            "A large desk made\n"
            "of wood. Put some\n"
            "decorations on it."),
        .tiles = DecorTiles_RAGGED_DESK,
        .icon = {gDecorIcon_RaggedDesk, gDecorIconPalette_RaggedDesk},
        .metatiles = DecorMetatiles_RAGGED_DESK,
        .attributes = DecorAttributes_Desk_3x2,
    },

    [DECOR_COMFORT_DESK] =
    {
        .id = DECOR_COMFORT_DESK,
        .name = _("COMFORT DESK"),
        .permission = DECORPERM_SOLID_FLOOR,
        .shape = DECORSHAPE_3x2,
        .category = DECORCAT_DESK,
        .price = 6000,
        .description = COMPOUND_STRING(
            "A large desk made\n"
            "of leaves. Put some\n"
            "decorations on it."),
        .tiles = DecorTiles_COMFORT_DESK,
        .icon = {gDecorIcon_ComfortDesk, gDecorIconPalette_ComfortDesk},
        .metatiles = DecorMetatiles_COMFORT_DESK,
        .attributes = DecorAttributes_Desk_3x2,
    },

    [DECOR_PRETTY_DESK] =
    {
        .id = DECOR_PRETTY_DESK,
        .name = _("PRETTY DESK"),
        .permission = DECORPERM_SOLID_FLOOR,
        .shape = DECORSHAPE_3x3,
        .category = DECORCAT_DESK,
        .price = 9000,
        .description = COMPOUND_STRING(
            "A huge desk made\n"
            "of glass. Holds lots\n"
            "of decorations."),
        .tiles = DecorTiles_PRETTY_DESK,
        .icon = {gDecorIcon_PrettyDesk, gDecorIconPalette_PrettyDesk},
        .metatiles = DecorMetatiles_PRETTY_DESK,
        .attributes = DecorAttributes_Desk_3x3,
    },

    [DECOR_BRICK_DESK] =
    {
        .id = DECOR_BRICK_DESK,
        .name = _("BRICK DESK"),
        .permission = DECORPERM_SOLID_FLOOR,
        .shape = DECORSHAPE_3x3,
        .category = DECORCAT_DESK,
        .price = 9000,
        .description = COMPOUND_STRING(
            "A huge desk made\n"
            "of brick. Holds lots\n"
            "of decorations."),
        .tiles = DecorTiles_BRICK_DESK,
        .icon = {gDecorIcon_BrickDesk, gDecorIconPalette_BrickDesk},
        .metatiles = DecorMetatiles_BRICK_DESK,
        .attributes = DecorAttributes_Desk_3x3,
    },

    [DECOR_CAMP_DESK] =
    {
        .id = DECOR_CAMP_DESK,
        .name = _("CAMP DESK"),
        .permission = DECORPERM_SOLID_FLOOR,
        .shape = DECORSHAPE_3x3,
        .category = DECORCAT_DESK,
        .price = 9000,
        .description = COMPOUND_STRING(
            "A huge desk made\n"
            "of logs. Put lots of\n"
            "decorations on it."),
        .tiles = DecorTiles_CAMP_DESK,
        .icon = {gDecorIcon_CampDesk, gDecorIconPalette_CampDesk},
        .metatiles = DecorMetatiles_CAMP_DESK,
        .attributes = DecorAttributes_Desk_3x3,
    },

    [DECOR_HARD_DESK] =
    {
        .id = DECOR_HARD_DESK,
        .name = _("HARD DESK"),
        .permission = DECORPERM_SOLID_FLOOR,
        .shape = DECORSHAPE_3x3,
        .category = DECORCAT_DESK,
        .price = 9000,
        .description = COMPOUND_STRING(
            "A huge desk made\n"
            "of rocks. Holds\n"
            "many decorations."),
        .tiles = DecorTiles_HARD_DESK,
        .icon = {gDecorIcon_HardDesk, gDecorIconPalette_HardDesk},
        .metatiles = DecorMetatiles_HARD_DESK,
        .attributes = DecorAttributes_Desk_3x3,
    },

    [DECOR_SMALL_CHAIR] =
    {
        .id = DECOR_SMALL_CHAIR,
        .name = _("SMALL CHAIR"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_CHAIR,
        .price = 2000,
        .description = COMPOUND_STRING(
            "A small chair made\n"
            "for one."),
        .tiles = DecorTiles_SMALL_CHAIR,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_SMALL_CHAIR,
        .attributes = DecorAttributes_Chair,
    },

    [DECOR_POKEMON_CHAIR] =
    {
        .id = DECOR_POKEMON_CHAIR,
        .name = _("POKéMON CHAIR"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_CHAIR,
        .price = 2000,
        .description = COMPOUND_STRING(
            "A small chair built\n"
            "in the shape of a\n"
            "POKé BALL."),
        .tiles = DecorTiles_POKEMON_CHAIR,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_SMALL_CHAIR,
        .attributes = DecorAttributes_Chair,
    },

    [DECOR_HEAVY_CHAIR] =
    {
        .id = DECOR_HEAVY_CHAIR,
        .name = _("HEAVY CHAIR"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_CHAIR,
        .price = 2000,
        .description = COMPOUND_STRING(
            "A small chair made\n"
            "of steel."),
        .tiles = DecorTiles_HEAVY_CHAIR,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_HEAVY_CHAIR,
        .attributes = DecorAttributes_Chair,
    },

    [DECOR_PRETTY_CHAIR] =
    {
        .id = DECOR_PRETTY_CHAIR,
        .name = _("PRETTY CHAIR"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_CHAIR,
        .price = 2000,
        .description = COMPOUND_STRING(
            "A small chair made\n"
            "of glass."),
        .tiles = DecorTiles_PRETTY_CHAIR,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_PRETTY_CHAIR,
        .attributes = DecorAttributes_Chair,
    },

    [DECOR_COMFORT_CHAIR] =
    {
        .id = DECOR_COMFORT_CHAIR,
        .name = _("COMFORT CHAIR"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_CHAIR,
        .price = 2000,
        .description = COMPOUND_STRING(
            "A small chair made\n"
            "of leaves."),
        .tiles = DecorTiles_COMFORT_CHAIR,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_COMFORT_CHAIR,
        .attributes = DecorAttributes_Chair,
    },

    [DECOR_RAGGED_CHAIR] =
    {
        .id = DECOR_RAGGED_CHAIR,
        .name = _("RAGGED CHAIR"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_CHAIR,
        .price = 2000,
        .description = COMPOUND_STRING(
            "A small chair made\n"
            "of wood."),
        .tiles = DecorTiles_RAGGED_CHAIR,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_RAGGED_CHAIR,
        .attributes = DecorAttributes_Chair,
    },

    [DECOR_BRICK_CHAIR] =
    {
        .id = DECOR_BRICK_CHAIR,
        .name = _("BRICK CHAIR"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_CHAIR,
        .price = 2000,
        .description = COMPOUND_STRING(
            "A small chair made\n"
            "of brick."),
        .tiles = DecorTiles_BRICK_CHAIR,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_BRICK_CHAIR,
        .attributes = DecorAttributes_Chair,
    },

    [DECOR_CAMP_CHAIR] =
    {
        .id = DECOR_CAMP_CHAIR,
        .name = _("CAMP CHAIR"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_CHAIR,
        .price = 2000,
        .description = COMPOUND_STRING(
            "A small chair made\n"
            "of logs."),
        .tiles = DecorTiles_CAMP_CHAIR,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_CAMP_CHAIR,
        .attributes = DecorAttributes_Chair,
    },

    [DECOR_HARD_CHAIR] =
    {
        .id = DECOR_HARD_CHAIR,
        .name = _("HARD CHAIR"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_CHAIR,
        .price = 2000,
        .description = COMPOUND_STRING(
            "A small chair made\n"
            "of rock."),
        .tiles = DecorTiles_HARD_CHAIR,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_HARD_CHAIR,
        .attributes = DecorAttributes_Chair,
    },

    [DECOR_RED_PLANT] =
    {
        .id = DECOR_RED_PLANT,
        .name = _("RED PLANT"),
        .permission = DECORPERM_BEHIND_FLOOR,
        .shape = DECORSHAPE_1x2,
        .category = DECORCAT_PLANT,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A vivid red potted\n"
            "plant."),
        .tiles = DecorTiles_RED_PLANT,
        .icon = {gDecorIcon_RedPlant, gDecorIconPalette_RedPlant},
        .metatiles = DecorMetatiles_RED_PLANT,
        .attributes = DecorAttributes_Plant_1x2,
    },

    [DECOR_TROPICAL_PLANT] =
    {
        .id = DECOR_TROPICAL_PLANT,
        .name = _("TROPICAL PLANT"),
        .permission = DECORPERM_BEHIND_FLOOR,
        .shape = DECORSHAPE_1x2,
        .category = DECORCAT_PLANT,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A flowering tropical\n"
            "plant in a pot."),
        .tiles = DecorTiles_TROPICAL_PLANT,
        .icon = {gDecorIcon_TropicalPlant, gDecorIconPalette_TropicalPlant},
        .metatiles = DecorMetatiles_TROPICAL_PLANT,
        .attributes = DecorAttributes_Plant_1x2,
    },

    [DECOR_PRETTY_FLOWERS] =
    {
        .id = DECOR_PRETTY_FLOWERS,
        .name = _("PRETTY FLOWERS"),
        .permission = DECORPERM_BEHIND_FLOOR,
        .shape = DECORSHAPE_1x2,
        .category = DECORCAT_PLANT,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A pot of cute\n"
            "flowers."),
        .tiles = DecorTiles_PRETTY_FLOWERS,
        .icon = {gDecorIcon_PrettyFlowers, gDecorIconPalette_PrettyFlowers},
        .metatiles = DecorMetatiles_PRETTY_FLOWERS,
        .attributes = DecorAttributes_Plant_1x2,
    },

    [DECOR_COLORFUL_PLANT] =
    {
        .id = DECOR_COLORFUL_PLANT,
        .name = _("COLORFUL PLANT"),
        .permission = DECORPERM_BEHIND_FLOOR,
        .shape = DECORSHAPE_2x2,
        .category = DECORCAT_PLANT,
        .price = 5000,
        .description = COMPOUND_STRING(
            "A large pot with\n"
            "many colorful\n"
            "flowers."),
        .tiles = DecorTiles_COLORFUL_PLANT,
        .icon = {gDecorIcon_ColorfulPlant, gDecorIconPalette_ColorfulPlant},
        .metatiles = DecorMetatiles_COLORFUL_PLANT,
        .attributes = DecorAttributes_Plant_2x2,
    },

    [DECOR_BIG_PLANT] =
    {
        .id = DECOR_BIG_PLANT,
        .name = _("BIG PLANT"),
        .permission = DECORPERM_BEHIND_FLOOR,
        .shape = DECORSHAPE_2x2,
        .category = DECORCAT_PLANT,
        .price = 5000,
        .description = COMPOUND_STRING(
            "A large, umbrella-\n"
            "shaped plant in a\n"
            "big pot."),
        .tiles = DecorTiles_BIG_PLANT,
        .icon = {gDecorIcon_BigPlant, gDecorIconPalette_BigPlant},
        .metatiles = DecorMetatiles_BIG_PLANT,
        .attributes = DecorAttributes_Plant_2x2,
    },

    [DECOR_GORGEOUS_PLANT] =
    {
        .id = DECOR_GORGEOUS_PLANT,
        .name = _("GORGEOUS PLANT"),
        .permission = DECORPERM_BEHIND_FLOOR,
        .shape = DECORSHAPE_2x2,
        .category = DECORCAT_PLANT,
        .price = 5000,
        .description = COMPOUND_STRING(
            "A large, impressive\n"
            "plant in a big pot."),
        .tiles = DecorTiles_GORGEOUS_PLANT,
        .icon = {gDecorIcon_GorgeousPlant, gDecorIconPalette_GorgeousPlant},
        .metatiles = DecorMetatiles_BIG_PLANT,
        .attributes = DecorAttributes_Plant_2x2,
    },

    [DECOR_RED_BRICK] =
    {
        .id = DECOR_RED_BRICK,
        .name = _("RED BRICK"),
        .permission = DECORPERM_SOLID_FLOOR,
        .shape = DECORSHAPE_1x2,
        .category = DECORCAT_ORNAMENT,
        .price = 500,
        .description = COMPOUND_STRING(
            "A red-colored brick.\n"
            "Decorations can be\n"
            "placed on top."),
        .tiles = DecorTiles_BRICK,
        .icon = {gDecorIcon_RedBrick, gDecorIconPalette_RedBrick},
        .metatiles = DecorMetatiles_RED_BRICK,
        .attributes = DecorAttributes_Brick,
    },

    [DECOR_YELLOW_BRICK] =
    {
        .id = DECOR_YELLOW_BRICK,
        .name = _("YELLOW BRICK"),
        .permission = DECORPERM_SOLID_FLOOR,
        .shape = DECORSHAPE_1x2,
        .category = DECORCAT_ORNAMENT,
        .price = 500,
        .description = COMPOUND_STRING(
            "A yellow-colored\n"
            "brick. Put some\n"
            "decorations on top."),
        .tiles = DecorTiles_BRICK,
        .icon = {gDecorIcon_YellowBrick, gDecorIconPalette_YellowBrick},
        .metatiles = DecorMetatiles_YELLOW_BRICK,
        .attributes = DecorAttributes_Brick,
    },

    [DECOR_BLUE_BRICK] =
    {
        .id = DECOR_BLUE_BRICK,
        .name = _("BLUE BRICK"),
        .permission = DECORPERM_SOLID_FLOOR,
        .shape = DECORSHAPE_1x2,
        .category = DECORCAT_ORNAMENT,
        .price = 500,
        .description = COMPOUND_STRING(
            "A blue-colored\n"
            "brick. Put some\n"
            "decorations on top."),
        .tiles = DecorTiles_BRICK,
        .icon = {gDecorIcon_BlueBrick, gDecorIconPalette_BlueBrick},
        .metatiles = DecorMetatiles_BLUE_BRICK,
        .attributes = DecorAttributes_Brick,
    },

    [DECOR_RED_BALLOON] =
    {
        .id = DECOR_RED_BALLOON,
        .name = _("RED BALLOON"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_ORNAMENT,
        .price = 500,
        .description = COMPOUND_STRING(
            "A red balloon filled\n"
            "with water. Bursts\n"
            "if stepped on."),
        .tiles = DecorTiles_BALLOON,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_RED_BALLOON,
        .attributes = DecorAttributes_Balloon,
    },

    [DECOR_BLUE_BALLOON] =
    {
        .id = DECOR_BLUE_BALLOON,
        .name = _("BLUE BALLOON"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_ORNAMENT,
        .price = 500,
        .description = COMPOUND_STRING(
            "A blue balloon filled\n"
            "with water. Bursts\n"
            "if stepped on."),
        .tiles = DecorTiles_BALLOON,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_BLUE_BALLOON,
        .attributes = DecorAttributes_Balloon,
    },

    [DECOR_YELLOW_BALLOON] =
    {
        .id = DECOR_YELLOW_BALLOON,
        .name = _("YELLOW BALLOON"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_ORNAMENT,
        .price = 500,
        .description = COMPOUND_STRING(
            "A yellow balloon\n"
            "filled with water.\n"
            "Pops if stepped on."),
        .tiles = DecorTiles_BALLOON,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_YELLOW_BALLOON,
        .attributes = DecorAttributes_Balloon,
    },

    [DECOR_RED_TENT] =
    {
        .id = DECOR_RED_TENT,
        .name = _("RED TENT"),
        .permission = DECORPERM_BEHIND_FLOOR,
        .shape = DECORSHAPE_3x3,
        .category = DECORCAT_ORNAMENT,
        .price = 10000,
        .description = COMPOUND_STRING(
            "A large red tent.\n"
            "You can hide inside\n"
            "it."),
        .tiles = DecorTiles_TENT,
        .icon = {gDecorIcon_RedTent, gDecorIconPalette_RedTent},
        .metatiles = DecorMetatiles_RED_TENT,
        .attributes = DecorAttributes_Tent,
    },

    [DECOR_BLUE_TENT] =
    {
        .id = DECOR_BLUE_TENT,
        .name = _("BLUE TENT"),
        .permission = DECORPERM_BEHIND_FLOOR,
        .shape = DECORSHAPE_3x3,
        .category = DECORCAT_ORNAMENT,
        .price = 10000,
        .description = COMPOUND_STRING(
            "A large blue tent.\n"
            "You can hide inside\n"
            "it."),
        .tiles = DecorTiles_TENT,
        .icon = {gDecorIcon_BlueTent, gDecorIconPalette_BlueTent},
        .metatiles = DecorMetatiles_BLUE_TENT,
        .attributes = DecorAttributes_Tent,
    },

    [DECOR_SOLID_BOARD] =
    {
        .id = DECOR_SOLID_BOARD,
        .name = _("SOLID BOARD"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x2,
        .category = DECORCAT_ORNAMENT,
        .price = 3000,
        .description = COMPOUND_STRING(
            "Place over a hole to\n"
            "cross to the other\n"
            "side."),
        .tiles = DecorTiles_SOLID_BOARD,
        .icon = {gDecorIcon_SolidBoard, gDecorIconPalette_SolidBoard},
        .metatiles = DecorMetatiles_SOLID_BOARD,
        .attributes = DecorAttributes_SolidBoard,
    },

    [DECOR_SLIDE] =
    {
        .id = DECOR_SLIDE,
        .name = _("SLIDE"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_2x4,
        .category = DECORCAT_ORNAMENT,
        .price = 8000,
        .description = COMPOUND_STRING(
            "Use to slide down\n"
            "from the platform."),
        .tiles = DecorTiles_SLIDE,
        .icon = {gDecorIcon_Slide, gDecorIconPalette_Slide},
        .metatiles = DecorMetatiles_SLIDE,
        .attributes = DecorAttributes_Slide,
    },

    [DECOR_FENCE_LENGTH] =
    {
        .id = DECOR_FENCE_LENGTH,
        .name = _("FENCE LENGTH"),
        .permission = DECORPERM_SOLID_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_ORNAMENT,
        .price = 500,
        .description = COMPOUND_STRING(
            "A small fence that\n"
            "blocks passage."),
        .tiles = DecorTiles_FENCE_LENGTH,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_FENCE,
        .attributes = DecorAttributes_Fence,
    },

    [DECOR_FENCE_WIDTH] =
    {
        .id = DECOR_FENCE_WIDTH,
        .name = _("FENCE WIDTH"),
        .permission = DECORPERM_SOLID_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_ORNAMENT,
        .price = 500,
        .description = COMPOUND_STRING(
            "A small fence that\n"
            "blocks passage."),
        .tiles = DecorTiles_FENCE_WIDTH,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_FENCE,
        .attributes = DecorAttributes_Fence,
    },

    [DECOR_TIRE] =
    {
        .id = DECOR_TIRE,
        .name = _("TIRE"),
        .permission = DECORPERM_SOLID_FLOOR,
        .shape = DECORSHAPE_2x2,
        .category = DECORCAT_ORNAMENT,
        .price = 800,
        .description = COMPOUND_STRING(
            "An old large tire.\n"
            "Decorations can be\n"
            "placed on top."),
        .tiles = DecorTiles_TIRE,
        .icon = {gDecorIcon_Tire, gDecorIconPalette_Tire},
        .metatiles = DecorMetatiles_TIRE,
        .attributes = DecorAttributes_Tire,
    },

    [DECOR_STAND] =
    {
        .id = DECOR_STAND,
        .name = _("STAND"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_4x2,
        .category = DECORCAT_ORNAMENT,
        .price = 7000,
        .description = COMPOUND_STRING(
            "A large pedestal\n"
            "with steps."),
        .tiles = DecorTiles_STAND,
        .icon = {gDecorIcon_Stand, gDecorIconPalette_Stand},
        .metatiles = DecorMetatiles_STAND,
        .attributes = DecorAttributes_Stand,
    },

    [DECOR_MUD_BALL] =
    {
        .id = DECOR_MUD_BALL,
        .name = _("MUD BALL"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_ORNAMENT,
        .price = 200,
        .description = COMPOUND_STRING(
            "A large ball of mud.\n"
            "Crumbles if stepped\n"
            "on."),
        .tiles = DecorTiles_MUD_BALL,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_MUD_BALL,
        .attributes = DecorAttributes_Balloon,
    },

    [DECOR_BREAKABLE_DOOR] =
    {
        .id = DECOR_BREAKABLE_DOOR,
        .name = _("BREAKABLE DOOR"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x2,
        .category = DECORCAT_ORNAMENT,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A weird door that\n"
            "people can walk\n"
            "right through."),
        .tiles = DecorTiles_BREAKABLE_DOOR,
        .icon = {gDecorIcon_BreakableDoor, gDecorIconPalette_BreakableDoor},
        .metatiles = DecorMetatiles_BREAKABLE_DOOR,
        .attributes = DecorAttributes_BreakableDoor,
    },

    [DECOR_SAND_ORNAMENT] =
    {
        .id = DECOR_SAND_ORNAMENT,
        .name = _("SAND ORNAMENT"),
        .permission = DECORPERM_BEHIND_FLOOR,
        .shape = DECORSHAPE_1x2,
        .category = DECORCAT_ORNAMENT,
        .price = 3000,
        .description = COMPOUND_STRING(
            "An ornament made\n"
            "of sand. Crumbles if\n"
            "touched."),
        .tiles = DecorTiles_SAND_ORNAMENT,
        .icon = {gDecorIcon_SandOrnament, gDecorIconPalette_SandOrnament},
        .metatiles = DecorMetatiles_SAND_ORNAMENT,
        .attributes = DecorAttributes_SandOrnament,
    },

    [DECOR_SILVER_SHIELD] =
    {
        .id = DECOR_SILVER_SHIELD,
        .name = _("SILVER SHIELD"),
        .permission = DECORPERM_BEHIND_FLOOR,
        .shape = DECORSHAPE_1x2,
        .category = DECORCAT_ORNAMENT,
        .price = 0,
        .description = COMPOUND_STRING(
            "Awarded for 50\n"
            "straight wins at\n"
            "the BATTLE TOWER."),
        .tiles = DecorTiles_SILVER_SHIELD,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_SILVER_SHIELD,
        .attributes = DecorAttributes_Shield,
    },

    [DECOR_GOLD_SHIELD] =
    {
        .id = DECOR_GOLD_SHIELD,
        .name = _("GOLD SHIELD"),
        .permission = DECORPERM_BEHIND_FLOOR,
        .shape = DECORSHAPE_1x2,
        .category = DECORCAT_ORNAMENT,
        .price = 0,
        .description = COMPOUND_STRING(
            "Awarded for 100\n"
            "straight wins at\n"
            "the BATTLE TOWER."),
        .tiles = DecorTiles_GOLD_SHIELD,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_SILVER_SHIELD,
        .attributes = DecorAttributes_Shield,
    },

    [DECOR_GLASS_ORNAMENT] =
    {
        .id = DECOR_GLASS_ORNAMENT,
        .name = _("GLASS ORNAMENT"),
        .permission = DECORPERM_BEHIND_FLOOR,
        .shape = DECORSHAPE_1x2,
        .category = DECORCAT_ORNAMENT,
        .price = 0,
        .description = COMPOUND_STRING(
            "A glass replica of\n"
            "a famous sculpture\n"
            "at the ART MUSEUM."),
        .tiles = DecorTiles_GLASS_ORNAMENT,
        .icon = {gDecorIcon_GlassOrnament, gDecorIconPalette_GlassOrnament},
        .metatiles = DecorMetatiles_GLASS_ORNAMENT,
        .attributes = DecorAttributes_GlassOrnament,
    },

    [DECOR_TV] =
    {
        .id = DECOR_TV,
        .name = _("TV"),
        .permission = DECORPERM_SOLID_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_ORNAMENT,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A small, gray-\n"
            "colored toy TV."),
        .tiles = DecorTiles_TV,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_TV,
        .attributes = DecorAttributes_Tv,
    },

    [DECOR_ROUND_TV] =
    {
        .id = DECOR_ROUND_TV,
        .name = _("ROUND TV"),
        .permission = DECORPERM_SOLID_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_ORNAMENT,
        .price = 4000,
        .description = COMPOUND_STRING(
            "A toy TV modeled\n"
            "in the image of a\n"
            "SEEDOT."),
        .tiles = DecorTiles_ROUND_TV,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_ROUND_TV,
        .attributes = DecorAttributes_Tv,
    },

    [DECOR_CUTE_TV] =
    {
        .id = DECOR_CUTE_TV,
        .name = _("CUTE TV"),
        .permission = DECORPERM_SOLID_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_ORNAMENT,
        .price = 4000,
        .description = COMPOUND_STRING(
            "A toy TV modeled\n"
            "in the image of a\n"
            "SKITTY."),
        .tiles = DecorTiles_CUTE_TV,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_CUTE_TV,
        .attributes = DecorAttributes_Tv,
    },

    [DECOR_GLITTER_MAT] =
    {
        .id = DECOR_GLITTER_MAT,
        .name = _("GLITTER MAT"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_MAT,
        .price = 2000,
        .description = COMPOUND_STRING(
            "An odd mat that\n"
            "glitters if stepped\n"
            "on."),
        .tiles = DecorTiles_GLITTER_MAT,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_GLITTER_MAT,
        .attributes = DecorAttributes_GlitterMat,
    },

    [DECOR_JUMP_MAT] =
    {
        .id = DECOR_JUMP_MAT,
        .name = _("JUMP MAT"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_MAT,
        .price = 2000,
        .description = COMPOUND_STRING(
            "A trick mat that\n"
            "jumps when it is\n"
            "stepped on."),
        .tiles = DecorTiles_JUMP_MAT,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_JUMP_MAT,
        .attributes = DecorAttributes_JumpMat,
    },

    [DECOR_SPIN_MAT] =
    {
        .id = DECOR_SPIN_MAT,
        .name = _("SPIN MAT"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_MAT,
        .price = 2000,
        .description = COMPOUND_STRING(
            "A trick mat that\n"
            "spins around when\n"
            "stepped on."),
        .tiles = DecorTiles_SPIN_MAT,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_SPIN_MAT,
        .attributes = DecorAttributes_SpinMat,
    },

    [DECOR_C_LOW_NOTE_MAT] =
    {
        .id = DECOR_C_LOW_NOTE_MAT,
        .name = _("C Low NOTE MAT"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_MAT,
        .price = 500,
        .description = COMPOUND_STRING(
            "A mat that plays\n"
            "a low C note when\n"
            "stepped on."),
        .tiles = DecorTiles_NOTE_MAT,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_NOTE_MAT,
        .attributes = DecorAttributes_NoteMat,
    },

    [DECOR_D_NOTE_MAT] =
    {
        .id = DECOR_D_NOTE_MAT,
        .name = _("D NOTE MAT"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_MAT,
        .price = 500,
        .description = COMPOUND_STRING(
            "A mat that plays\n"
            "a D note when\n"
            "stepped on."),
        .tiles = DecorTiles_NOTE_MAT,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_NOTE_MAT,
        .attributes = DecorAttributes_NoteMat,
    },

    [DECOR_E_NOTE_MAT] =
    {
        .id = DECOR_E_NOTE_MAT,
        .name = _("E NOTE MAT"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_MAT,
        .price = 500,
        .description = COMPOUND_STRING(
            "A mat that plays\n"
            "an E note when\n"
            "stepped on."),
        .tiles = DecorTiles_NOTE_MAT,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_NOTE_MAT,
        .attributes = DecorAttributes_NoteMat,
    },

    [DECOR_F_NOTE_MAT] =
    {
        .id = DECOR_F_NOTE_MAT,
        .name = _("F NOTE MAT"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_MAT,
        .price = 500,
        .description = COMPOUND_STRING(
            "A mat that plays\n"
            "an F note when\n"
            "stepped on."),
        .tiles = DecorTiles_NOTE_MAT,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_NOTE_MAT,
        .attributes = DecorAttributes_NoteMat,
    },

    [DECOR_G_NOTE_MAT] =
    {
        .id = DECOR_G_NOTE_MAT,
        .name = _("G NOTE MAT"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_MAT,
        .price = 500,
        .description = COMPOUND_STRING(
            "A mat that plays\n"
            "a G note when\n"
            "stepped on."),
        .tiles = DecorTiles_NOTE_MAT,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_NOTE_MAT,
        .attributes = DecorAttributes_NoteMat,
    },

    [DECOR_A_NOTE_MAT] =
    {
        .id = DECOR_A_NOTE_MAT,
        .name = _("A NOTE MAT"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_MAT,
        .price = 500,
        .description = COMPOUND_STRING(
            "A mat that plays\n"
            "an A note when\n"
            "stepped on."),
        .tiles = DecorTiles_NOTE_MAT,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_NOTE_MAT,
        .attributes = DecorAttributes_NoteMat,
    },

    [DECOR_B_NOTE_MAT] =
    {
        .id = DECOR_B_NOTE_MAT,
        .name = _("B NOTE MAT"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_MAT,
        .price = 500,
        .description = COMPOUND_STRING(
            "A mat that plays\n"
            "a B note when\n"
            "stepped on."),
        .tiles = DecorTiles_NOTE_MAT,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_NOTE_MAT,
        .attributes = DecorAttributes_NoteMat,
    },

    [DECOR_C_HIGH_NOTE_MAT] =
    {
        .id = DECOR_C_HIGH_NOTE_MAT,
        .name = _("C High NOTE MAT"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_MAT,
        .price = 500,
        .description = COMPOUND_STRING(
            "A mat that plays\n"
            "a high C note when\n"
            "stepped on."),
        .tiles = DecorTiles_NOTE_MAT,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_NOTE_MAT,
        .attributes = DecorAttributes_NoteMat,
    },

    [DECOR_SURF_MAT] =
    {
        .id = DECOR_SURF_MAT,
        .name = _("SURF MAT"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_3x3,
        .category = DECORCAT_MAT,
        .price = 4000,
        .description = COMPOUND_STRING(
            "A mat designed with\n"
            "a SURF image.\n"
            "Put items on top."),
        .tiles = DecorTiles_SURF_MAT,
        .icon = {gDecorIcon_SurfMat, gDecorIconPalette_SurfMat},
        .metatiles = DecorMetatiles_MAT_3x3_PAL4,
        .attributes = DecorAttributes_Mat_3x3,
    },

    [DECOR_THUNDER_MAT] =
    {
        .id = DECOR_THUNDER_MAT,
        .name = _("THUNDER MAT"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_3x3,
        .category = DECORCAT_MAT,
        .price = 4000,
        .description = COMPOUND_STRING(
            "A mat designed with\n"
            "a THUNDER image.\n"
            "Put items on top."),
        .tiles = DecorTiles_THUNDER_MAT,
        .icon = {gDecorIcon_ThunderMat, gDecorIconPalette_ThunderMat},
        .metatiles = DecorMetatiles_MAT_3x3_PAL3,
        .attributes = DecorAttributes_Mat_3x3,
    },

    [DECOR_FIRE_BLAST_MAT] =
    {
        .id = DECOR_FIRE_BLAST_MAT,
        .name = _("FIRE BLAST MAT"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_3x3,
        .category = DECORCAT_MAT,
        .price = 4000,
        .description = COMPOUND_STRING(
            "A mat designed with\n"
            "a FIRE BLAST image.\n"
            "Put items on top."),
        .tiles = DecorTiles_FIRE_BLAST_MAT,
        .icon = {gDecorIcon_FireBlastMat, gDecorIconPalette_FireBlastMat},
        .metatiles = DecorMetatiles_MAT_3x3_PAL4,
        .attributes = DecorAttributes_Mat_3x3,
    },

    [DECOR_POWDER_SNOW_MAT] =
    {
        .id = DECOR_POWDER_SNOW_MAT,
        .name = _("POWDER SNOW MAT"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_3x3,
        .category = DECORCAT_MAT,
        .price = 4000,
        .description = COMPOUND_STRING(
            "A mat with a POWDER\n"
            "SNOW image design.\n"
            "Put items on top."),
        .tiles = DecorTiles_POWDER_SNOW_MAT,
        .icon = {gDecorIcon_PowderSnowMat, gDecorIconPalette_PowderSnowMat},
        .metatiles = DecorMetatiles_MAT_3x3_PAL3,
        .attributes = DecorAttributes_Mat_3x3,
    },

    [DECOR_ATTRACT_MAT] =
    {
        .id = DECOR_ATTRACT_MAT,
        .name = _("ATTRACT MAT"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_3x3,
        .category = DECORCAT_MAT,
        .price = 4000,
        .description = COMPOUND_STRING(
            "A mat designed with\n"
            "an ATTRACT image.\n"
            "Put items on top."),
        .tiles = DecorTiles_ATTRACT_MAT,
        .icon = {gDecorIcon_AttractMat, gDecorIconPalette_AttractMat},
        .metatiles = DecorMetatiles_MAT_3x3_PAL2,
        .attributes = DecorAttributes_Mat_3x3,
    },

    [DECOR_FISSURE_MAT] =
    {
        .id = DECOR_FISSURE_MAT,
        .name = _("FISSURE MAT"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_3x3,
        .category = DECORCAT_MAT,
        .price = 4000,
        .description = COMPOUND_STRING(
            "A mat designed with\n"
            "a FISSURE image.\n"
            "Put items on top."),
        .tiles = DecorTiles_FISSURE_MAT,
        .icon = {gDecorIcon_FissureMat, gDecorIconPalette_FissureMat},
        .metatiles = DecorMetatiles_MAT_3x3_PAL5,
        .attributes = DecorAttributes_Mat_3x3,
    },

    [DECOR_SPIKES_MAT] =
    {
        .id = DECOR_SPIKES_MAT,
        .name = _("SPIKES MAT"),
        .permission = DECORPERM_PASS_FLOOR,
        .shape = DECORSHAPE_3x3,
        .category = DECORCAT_MAT,
        .price = 4000,
        .description = COMPOUND_STRING(
            "A mat designed with\n"
            "a SPIKES image.\n"
            "Put items on top."),
        .tiles = DecorTiles_SPIKES_MAT,
        .icon = {gDecorIcon_SpikesMat, gDecorIconPalette_SpikesMat},
        .metatiles = DecorMetatiles_MAT_3x3_PAL5,
        .attributes = DecorAttributes_Mat_3x3,
    },

    [DECOR_BALL_POSTER] =
    {
        .id = DECOR_BALL_POSTER,
        .name = _("BALL POSTER"),
        .permission = DECORPERM_NA_WALL,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_POSTER,
        .price = 1000,
        .description = COMPOUND_STRING(
            "A small poster\n"
            "printed with POKé\n"
            "BALLS."),
        .tiles = DecorTiles_BALL_POSTER,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_POSTER_1x1_PAL1,
        .attributes = DecorAttributes_Poster_1x1,
    },

    [DECOR_GREEN_POSTER] =
    {
        .id = DECOR_GREEN_POSTER,
        .name = _("GREEN POSTER"),
        .permission = DECORPERM_NA_WALL,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_POSTER,
        .price = 1000,
        .description = COMPOUND_STRING(
            "A small poster with\n"
            "a TREECKO print."),
        .tiles = DecorTiles_GREEN_POSTER,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_POSTER_1x1_PAL1,
        .attributes = DecorAttributes_Poster_1x1,
    },

    [DECOR_RED_POSTER] =
    {
        .id = DECOR_RED_POSTER,
        .name = _("RED POSTER"),
        .permission = DECORPERM_NA_WALL,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_POSTER,
        .price = 1000,
        .description = COMPOUND_STRING(
            "A small poster with\n"
            "a TORCHIC print."),
        .tiles = DecorTiles_RED_POSTER,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_POSTER_1x1_PAL1,
        .attributes = DecorAttributes_Poster_1x1,
    },

    [DECOR_BLUE_POSTER] =
    {
        .id = DECOR_BLUE_POSTER,
        .name = _("BLUE POSTER"),
        .permission = DECORPERM_NA_WALL,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_POSTER,
        .price = 1000,
        .description = COMPOUND_STRING(
            "A small poster with\n"
            "a MUDKIP print."),
        .tiles = DecorTiles_BLUE_POSTER,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_POSTER_1x1_PAL1,
        .attributes = DecorAttributes_Poster_1x1,
    },

    [DECOR_CUTE_POSTER] =
    {
        .id = DECOR_CUTE_POSTER,
        .name = _("CUTE POSTER"),
        .permission = DECORPERM_NA_WALL,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_POSTER,
        .price = 1000,
        .description = COMPOUND_STRING(
            "A small poster with\n"
            "an AZURILL print."),
        .tiles = DecorTiles_CUTE_POSTER,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_POSTER_1x1_PAL4,
        .attributes = DecorAttributes_Poster_1x1,
    },

    [DECOR_PIKA_POSTER] =
    {
        .id = DECOR_PIKA_POSTER,
        .name = _("PIKA POSTER"),
        .permission = DECORPERM_NA_WALL,
        .shape = DECORSHAPE_2x1,
        .category = DECORCAT_POSTER,
        .price = 1500,
        .description = COMPOUND_STRING(
            "A large poster with\n"
            "a PIKACHU and\n"
            "PICHU print."),
        .tiles = DecorTiles_PIKA_POSTER,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_POSTER_2x1_PAL1,
        .attributes = DecorAttributes_Poster_2x1,
    },

    [DECOR_LONG_POSTER] =
    {
        .id = DECOR_LONG_POSTER,
        .name = _("LONG POSTER"),
        .permission = DECORPERM_NA_WALL,
        .shape = DECORSHAPE_2x1,
        .category = DECORCAT_POSTER,
        .price = 1500,
        .description = COMPOUND_STRING(
            "A large poster with\n"
            "a SEVIPER print."),
        .tiles = DecorTiles_LONG_POSTER,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_POSTER_2x1_PAL1,
        .attributes = DecorAttributes_Poster_2x1,
    },

    [DECOR_SEA_POSTER] =
    {
        .id = DECOR_SEA_POSTER,
        .name = _("SEA POSTER"),
        .permission = DECORPERM_NA_WALL,
        .shape = DECORSHAPE_2x1,
        .category = DECORCAT_POSTER,
        .price = 1500,
        .description = COMPOUND_STRING(
            "A large poster with\n"
            "a RELICANTH print."),
        .tiles = DecorTiles_SEA_POSTER,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_POSTER_2x1_PAL3,
        .attributes = DecorAttributes_Poster_2x1,
    },

    [DECOR_SKY_POSTER] =
    {
        .id = DECOR_SKY_POSTER,
        .name = _("SKY POSTER"),
        .permission = DECORPERM_NA_WALL,
        .shape = DECORSHAPE_2x1,
        .category = DECORCAT_POSTER,
        .price = 1500,
        .description = COMPOUND_STRING(
            "A large poster with\n"
            "a WINGULL print."),
        .tiles = DecorTiles_SKY_POSTER,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_POSTER_2x1_PAL3,
        .attributes = DecorAttributes_Poster_2x1,
    },

    [DECOR_KISS_POSTER] =
    {
        .id = DECOR_KISS_POSTER,
        .name = _("KISS POSTER"),
        .permission = DECORPERM_NA_WALL,
        .shape = DECORSHAPE_2x1,
        .category = DECORCAT_POSTER,
        .price = 1500,
        .description = COMPOUND_STRING(
            "A large poster with\n"
            "a SMOOCHUM print."),
        .tiles = DecorTiles_KISS_POSTER,
        .icon = {NULL, NULL},
        .metatiles = DecorMetatiles_POSTER_2x1_PAL1,
        .attributes = DecorAttributes_Poster_2x1,
    },

    [DECOR_PICHU_DOLL] =
    {
        .id = DECOR_PICHU_DOLL,
        .name = _("PICHU DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DOLL,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A PICHU doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_PICHU_DOLL,
        .icon = {NULL, NULL},
    },

    [DECOR_PIKACHU_DOLL] =
    {
        .id = DECOR_PIKACHU_DOLL,
        .name = _("PIKACHU DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DOLL,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A PIKACHU doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_PIKACHU_DOLL,
        .icon = {NULL, NULL},
    },

    [DECOR_MARILL_DOLL] =
    {
        .id = DECOR_MARILL_DOLL,
        .name = _("MARILL DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DOLL,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A MARILL doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_MARILL_DOLL,
        .icon = {NULL, NULL},
    },

    [DECOR_TOGEPI_DOLL] =
    {
        .id = DECOR_TOGEPI_DOLL,
        .name = _("TOGEPI DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DOLL,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A TOGEPI doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_TOGEPI_DOLL,
        .icon = {NULL, NULL},
    },

    [DECOR_CYNDAQUIL_DOLL] =
    {
        .id = DECOR_CYNDAQUIL_DOLL,
        .name = _("CYNDAQUIL DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DOLL,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A CYNDAQUIL doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_CYNDAQUIL_DOLL,
        .icon = {NULL, NULL},
    },

    [DECOR_CHIKORITA_DOLL] =
    {
        .id = DECOR_CHIKORITA_DOLL,
        .name = _("CHIKORITA DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DOLL,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A CHIKORITA doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_CHIKORITA_DOLL,
        .icon = {NULL, NULL},
    },

    [DECOR_TOTODILE_DOLL] =
    {
        .id = DECOR_TOTODILE_DOLL,
        .name = _("TOTODILE DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DOLL,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A TOTODILE doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_TOTODILE_DOLL,
        .icon = {NULL, NULL},
    },

    [DECOR_JIGGLYPUFF_DOLL] =
    {
        .id = DECOR_JIGGLYPUFF_DOLL,
        .name = _("JIGGLYPUFF DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DOLL,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A JIGGLYPUFF doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_JIGGLYPUFF_DOLL,
        .icon = {NULL, NULL},
    },

    [DECOR_MEOWTH_DOLL] =
    {
        .id = DECOR_MEOWTH_DOLL,
        .name = _("MEOWTH DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DOLL,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A MEOWTH doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_MEOWTH_DOLL,
        .icon = {NULL, NULL},
    },

    [DECOR_CLEFAIRY_DOLL] =
    {
        .id = DECOR_CLEFAIRY_DOLL,
        .name = _("CLEFAIRY DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DOLL,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A CLEFAIRY doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_CLEFAIRY_DOLL,
        .icon = {NULL, NULL},
    },

    [DECOR_DITTO_DOLL] =
    {
        .id = DECOR_DITTO_DOLL,
        .name = _("DITTO DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DOLL,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A DITTO doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_DITTO_DOLL,
        .icon = {NULL, NULL},
    },

    [DECOR_SMOOCHUM_DOLL] =
    {
        .id = DECOR_SMOOCHUM_DOLL,
        .name = _("SMOOCHUM DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DOLL,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A SMOOCHUM doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_SMOOCHUM_DOLL,
        .icon = {NULL, NULL},
    },

    [DECOR_TREECKO_DOLL] =
    {
        .id = DECOR_TREECKO_DOLL,
        .name = _("TREECKO DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DOLL,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A TREECKO doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_TREECKO_DOLL,
        .icon = {NULL, NULL},
    },

    [DECOR_TORCHIC_DOLL] =
    {
        .id = DECOR_TORCHIC_DOLL,
        .name = _("TORCHIC DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DOLL,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A TORCHIC doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_TORCHIC_DOLL,
        .icon = {NULL, NULL},
    },

    [DECOR_MUDKIP_DOLL] =
    {
        .id = DECOR_MUDKIP_DOLL,
        .name = _("MUDKIP DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DOLL,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A MUDKIP doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_MUDKIP_DOLL,
        .icon = {NULL, NULL},
    },

    [DECOR_DUSKULL_DOLL] =
    {
        .id = DECOR_DUSKULL_DOLL,
        .name = _("DUSKULL DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DOLL,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A DUSKULL doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_DUSKULL_DOLL,
        .icon = {NULL, NULL},
    },

    [DECOR_WYNAUT_DOLL] =
    {
        .id = DECOR_WYNAUT_DOLL,
        .name = _("WYNAUT DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DOLL,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A WYNAUT doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_WYNAUT_DOLL,
        .icon = {NULL, NULL},
    },

    [DECOR_BALTOY_DOLL] =
    {
        .id = DECOR_BALTOY_DOLL,
        .name = _("BALTOY DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DOLL,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A BALTOY doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_BALTOY_DOLL,
        .icon = {NULL, NULL},
    },

    [DECOR_KECLEON_DOLL] =
    {
        .id = DECOR_KECLEON_DOLL,
        .name = _("KECLEON DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DOLL,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A KECLEON doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_KECLEON_DOLL,
        .icon = {NULL, NULL},
    },

    [DECOR_AZURILL_DOLL] =
    {
        .id = DECOR_AZURILL_DOLL,
        .name = _("AZURILL DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DOLL,
        .price = 3000,
        .description = COMPOUND_STRING(
            "An AZURILL doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_AZURILL_DOLL,
        .icon = {NULL, NULL},
    },

    [DECOR_SKITTY_DOLL] =
    {
        .id = DECOR_SKITTY_DOLL,
        .name = _("SKITTY DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DOLL,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A SKITTY doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_SKITTY_DOLL,
        .icon = {NULL, NULL},
    },

    [DECOR_SWABLU_DOLL] =
    {
        .id = DECOR_SWABLU_DOLL,
        .name = _("SWABLU DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DOLL,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A SWABLU doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_SWABLU_DOLL,
        .icon = {NULL, NULL},
    },

    [DECOR_GULPIN_DOLL] =
    {
        .id = DECOR_GULPIN_DOLL,
        .name = _("GULPIN DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DOLL,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A GULPIN doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_GULPIN_DOLL,
        .icon = {NULL, NULL},
    },

    [DECOR_LOTAD_DOLL] =
    {
        .id = DECOR_LOTAD_DOLL,
        .name = _("LOTAD DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DOLL,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A LOTAD doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_LOTAD_DOLL,
        .icon = {NULL, NULL},
    },

    [DECOR_SEEDOT_DOLL] =
    {
        .id = DECOR_SEEDOT_DOLL,
        .name = _("SEEDOT DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_DOLL,
        .price = 3000,
        .description = COMPOUND_STRING(
            "A SEEDOT doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_SEEDOT_DOLL,
        .icon = {NULL, NULL},
    },

    [DECOR_PIKA_CUSHION] =
    {
        .id = DECOR_PIKA_CUSHION,
        .name = _("PIKA CUSHION"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_CUSHION,
        .price = 2000,
        .description = COMPOUND_STRING(
            "A PIKACHU cushion.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_PIKA_CUSHION,
        .icon = {NULL, NULL},
    },

    [DECOR_ROUND_CUSHION] =
    {
        .id = DECOR_ROUND_CUSHION,
        .name = _("ROUND CUSHION"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_CUSHION,
        .price = 2000,
        .description = COMPOUND_STRING(
            "A MARILL cushion.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_ROUND_CUSHION,
        .icon = {NULL, NULL},
    },

    [DECOR_KISS_CUSHION] =
    {
        .id = DECOR_KISS_CUSHION,
        .name = _("KISS CUSHION"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_CUSHION,
        .price = 2000,
        .description = COMPOUND_STRING(
            "A SMOOCHUM\n"
            "cushion. Place it on\n"
            "a mat or a desk."),
        .objEvent = OBJ_EVENT_GFX_KISS_CUSHION,
        .icon = {NULL, NULL},
    },

    [DECOR_ZIGZAG_CUSHION] =
    {
        .id = DECOR_ZIGZAG_CUSHION,
        .name = _("ZIGZAG CUSHION"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_CUSHION,
        .price = 2000,
        .description = COMPOUND_STRING(
            "A ZIGZAGOON\n"
            "cushion. Place it on\n"
            "a mat or a desk."),
        .objEvent = OBJ_EVENT_GFX_ZIGZAG_CUSHION,
        .icon = {NULL, NULL},
    },

    [DECOR_SPIN_CUSHION] =
    {
        .id = DECOR_SPIN_CUSHION,
        .name = _("SPIN CUSHION"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_CUSHION,
        .price = 2000,
        .description = COMPOUND_STRING(
            "A SPINDA cushion.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_SPIN_CUSHION,
        .icon = {NULL, NULL},
    },

    [DECOR_DIAMOND_CUSHION] =
    {
        .id = DECOR_DIAMOND_CUSHION,
        .name = _("DIAMOND CUSHION"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_CUSHION,
        .price = 2000,
        .description = COMPOUND_STRING(
            "A SABLEYE cushion.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_DIAMOND_CUSHION,
        .icon = {NULL, NULL},
    },

    [DECOR_BALL_CUSHION] =
    {
        .id = DECOR_BALL_CUSHION,
        .name = _("BALL CUSHION"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_CUSHION,
        .price = 2000,
        .description = COMPOUND_STRING(
            "A BALL cushion.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_BALL_CUSHION,
        .icon = {NULL, NULL},
    },

    [DECOR_GRASS_CUSHION] =
    {
        .id = DECOR_GRASS_CUSHION,
        .name = _("GRASS CUSHION"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_CUSHION,
        .price = 2000,
        .description = COMPOUND_STRING(
            "A grass-mark\n"
            "cushion. Place it on\n"
            "a mat or a desk."),
        .objEvent = OBJ_EVENT_GFX_GRASS_CUSHION,
        .icon = {NULL, NULL},
    },

    [DECOR_FIRE_CUSHION] =
    {
        .id = DECOR_FIRE_CUSHION,
        .name = _("FIRE CUSHION"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_CUSHION,
        .price = 2000,
        .description = COMPOUND_STRING(
            "A fire-mark\n"
            "cushion. Place it on\n"
            "a mat or a desk."),
        .objEvent = OBJ_EVENT_GFX_FIRE_CUSHION,
        .icon = {NULL, NULL},
    },

    [DECOR_WATER_CUSHION] =
    {
        .id = DECOR_WATER_CUSHION,
        .name = _("WATER CUSHION"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x1,
        .category = DECORCAT_CUSHION,
        .price = 2000,
        .description = COMPOUND_STRING(
            "A water-mark\n"
            "cushion. Place it on\n"
            "a mat or a desk."),
        .objEvent = OBJ_EVENT_GFX_WATER_CUSHION,
        .icon = {NULL, NULL},
    },

    [DECOR_SNORLAX_DOLL] =
    {
        .id = DECOR_SNORLAX_DOLL,
        .name = _("SNORLAX DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x2,
        .category = DECORCAT_DOLL,
        .price = 10000,
        .description = COMPOUND_STRING(
            "A large doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_BIG_SNORLAX_DOLL,
        .icon = {gDecorIcon_SnorlaxDoll, gDecorIconPalette_SnorlaxDoll},
    },

    [DECOR_RHYDON_DOLL] =
    {
        .id = DECOR_RHYDON_DOLL,
        .name = _("RHYDON DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x2,
        .category = DECORCAT_DOLL,
        .price = 10000,
        .description = COMPOUND_STRING(
            "A large doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_BIG_RHYDON_DOLL,
        .icon = {gDecorIcon_RhydonDoll, gDecorIconPalette_RhydonDoll},
    },

    [DECOR_LAPRAS_DOLL] =
    {
        .id = DECOR_LAPRAS_DOLL,
        .name = _("LAPRAS DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x2,
        .category = DECORCAT_DOLL,
        .price = 10000,
        .description = COMPOUND_STRING(
            "A large doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_BIG_LAPRAS_DOLL,
        .icon = {gDecorIcon_LaprasDoll, gDecorIconPalette_LaprasDoll},
    },

    [DECOR_VENUSAUR_DOLL] =
    {
        .id = DECOR_VENUSAUR_DOLL,
        .name = _("VENUSAUR DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x2,
        .category = DECORCAT_DOLL,
        .price = 10000,
        .description = COMPOUND_STRING(
            "A large doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_BIG_VENUSAUR_DOLL,
        .icon = {gDecorIcon_VenusaurDoll, gDecorIconPalette_VenusaurDoll},
    },

    [DECOR_CHARIZARD_DOLL] =
    {
        .id = DECOR_CHARIZARD_DOLL,
        .name = _("CHARIZARD DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x2,
        .category = DECORCAT_DOLL,
        .price = 10000,
        .description = COMPOUND_STRING(
            "A large doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_BIG_CHARIZARD_DOLL,
        .icon = {gDecorIcon_CharizardDoll, gDecorIconPalette_CharizardDoll},
    },

    [DECOR_BLASTOISE_DOLL] =
    {
        .id = DECOR_BLASTOISE_DOLL,
        .name = _("BLASTOISE DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x2,
        .category = DECORCAT_DOLL,
        .price = 10000,
        .description = COMPOUND_STRING(
            "A large doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_BIG_BLASTOISE_DOLL,
        .icon = {gDecorIcon_BlastoiseDoll, gDecorIconPalette_BlastoiseDoll},
    },

    [DECOR_WAILMER_DOLL] =
    {
        .id = DECOR_WAILMER_DOLL,
        .name = _("WAILMER DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x2,
        .category = DECORCAT_DOLL,
        .price = 10000,
        .description = COMPOUND_STRING(
            "A large doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_BIG_WAILMER_DOLL,
        .icon = {gDecorIcon_WailmerDoll, gDecorIconPalette_WailmerDoll},
    },

    [DECOR_REGIROCK_DOLL] =
    {
        .id = DECOR_REGIROCK_DOLL,
        .name = _("REGIROCK DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x2,
        .category = DECORCAT_DOLL,
        .price = 10000,
        .description = COMPOUND_STRING(
            "A large doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_BIG_REGIROCK_DOLL,
        .icon = {gDecorIcon_RegirockDoll, gDecorIconPalette_RegirockDoll},
    },

    [DECOR_REGICE_DOLL] =
    {
        .id = DECOR_REGICE_DOLL,
        .name = _("REGICE DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x2,
        .category = DECORCAT_DOLL,
        .price = 10000,
        .description = COMPOUND_STRING(
            "A large doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_BIG_REGICE_DOLL,
        .icon = {gDecorIcon_RegiceDoll, gDecorIconPalette_RegiceDoll},
    },

    [DECOR_REGISTEEL_DOLL] =
    {
        .id = DECOR_REGISTEEL_DOLL,
        .name = _("REGISTEEL DOLL"),
        .permission = DECORPERM_SPRITE,
        .shape = DECORSHAPE_1x2,
        .category = DECORCAT_DOLL,
        .price = 10000,
        .description = COMPOUND_STRING(
            "A large doll.\n"
            "Place it on a mat\n"
            "or a desk."),
        .objEvent = OBJ_EVENT_GFX_BIG_REGISTEEL_DOLL,
        .icon = {gDecorIcon_RegisteelDoll, gDecorIconPalette_RegisteelDoll},
    }
};
