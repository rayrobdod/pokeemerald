static const u8 sText_UndiscoveredMove[] = _("????????");
static const u8 sText_CloseMenuItem[] = _("Close Manual");


static const u8 sText_PromptTeach[] = _("TEACH");
static const u8 sText_PromptClose[] = _("CLOSE");


static const u8 * const sCounterDesc[TM_COUNTER_COUNT] =
{
    [TM_COUNTER_NONE] = COMPOUND_STRING(""),
    [TM_COUNTER_BE_TUTORED] = COMPOUND_STRING("Be taught"),
    [TM_COUNTER_SEEN] = COMPOUND_STRING("See the move used"),
    [TM_COUNTER_SEEN_RAIN] = COMPOUND_STRING("See the move used\nin rain"),
    [TM_COUNTER_SEEN_SUN] = COMPOUND_STRING("See the move used\nin harsh sunlight"),
    [TM_COUNTER_WATER_USING_ICE] = COMPOUND_STRING("See a Water Pokémon\nuse an Ice move"),
};


static const u8 * const sSpeciesDesc[TM_SPECIESCOUNTER_COUNT] =
{
    [TM_SPECIESCOUNTER_NONE] = COMPOUND_STRING(""),
    [TM_SPECIESCOUNTER_SEEN] = COMPOUND_STRING("See the move used by\ndifferent species"),
};
