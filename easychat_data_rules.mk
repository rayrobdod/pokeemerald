
EASY_CHAT_SUBDIR := $(DATA_SRC_SUBDIR)/easy_chat
EASY_CHAT_SRCS := $(wildcard $(EASY_CHAT_SUBDIR)/*.json)

SPECIES_NAMES := $(DATA_SRC_SUBDIR)/text/species_names.h
POKEDEX_CONSTANTS := include/constants/pokedex.h

define EASY_CHAT_WORD_INFO
AUTO_GEN_TARGETS += $1
$1: $2 $$(EASYCHAT)
	$$(EASYCHAT) wordinfo $$< $$@
$$(C_BUILDDIR)/easy_chat.o: c_dep += $1
endef
$(foreach src, $(EASY_CHAT_SRCS), $(eval $(call EASY_CHAT_WORD_INFO,$(patsubst $(EASY_CHAT_SUBDIR)/%.json,$(EASY_CHAT_SUBDIR)/%.h, $(src)),$(src))))

AUTO_GEN_TARGETS += $(EASY_CHAT_SUBDIR)/easy_chat_group_pokemon.h
$(EASY_CHAT_SUBDIR)/easy_chat_group_pokemon.h: $(SPECIES_NAMES) $(POKEDEX_CONSTANTS) $(EASYCHAT)
	$(EASYCHAT) pokemon hoenn $(SPECIES_NAMES) $(POKEDEX_CONSTANTS) $@

AUTO_GEN_TARGETS += $(EASY_CHAT_SUBDIR)/easy_chat_group_pokemon2.h
$(EASY_CHAT_SUBDIR)/easy_chat_group_pokemon2.h: $(SPECIES_NAMES) $(POKEDEX_CONSTANTS) $(EASYCHAT)
	$(EASYCHAT) pokemon kantojohto $(SPECIES_NAMES) $(POKEDEX_CONSTANTS) $@
