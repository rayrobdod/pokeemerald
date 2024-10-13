static const u8 sText_UndiscoveredMove[] = _("????????");
static const u8 sText_CloseMenuItem[] = _("Close Manual");


static const u8 sText_PromptTeach[] = _("TEACH");
static const u8 sText_PromptClose[] = _("CLOSE");


static const u8 * const sTaskDefaultDesc[TM_TASK_COUNT] =
{
    [TM_TASK_NONE] = COMPOUND_STRING(""),
    [TM_TASK_SCRIPT_FLAG] = COMPOUND_STRING("Be taught"),
    [TM_TASK_BATTLE_SPECIAL] = COMPOUND_STRING("Special"),
    [TM_TASK_SEEN] = COMPOUND_STRING("See the move used"),
    [TM_TASK_SEEN_DIFFERENT_SPECIES] = COMPOUND_STRING("See the move used by\ndifferent species"),
    [TM_TASK_SEEN_RAIN] = COMPOUND_STRING("See the move used\nin rain"),
    [TM_TASK_SEEN_SUN] = COMPOUND_STRING("See the move used\nin harsh sunlight"),
};
