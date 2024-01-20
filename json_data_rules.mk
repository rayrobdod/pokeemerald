# JSON files are run through jsonproc, which is a tool that converts JSON data to an output file
# based on an Inja template. https://github.com/pantor/inja

EASY_CHAT_SUBDIR := $(DATA_SRC_SUBDIR)/easy_chat
EASY_CHAT_SRCS := $(wildcard $(EASY_CHAT_SUBDIR)/*.json)

define EASY_CHAT_JSONPROC
AUTO_GEN_TARGETS += $1
$1: $2 $$(EASY_CHAT_SUBDIR)/easy_chat_group.inja
	$$(JSONPROC) $$^ $$@
$$(C_BUILDDIR)/easy_chat.o: c_dep += $1
endef
$(foreach src, $(EASY_CHAT_SRCS), $(eval $(call EASY_CHAT_JSONPROC,$(patsubst $(EASY_CHAT_SUBDIR)/%.json,$(EASY_CHAT_SUBDIR)/%.h, $(src)),$(src))))

AUTO_GEN_TARGETS += $(DATA_SRC_SUBDIR)/wild_encounters.h
$(DATA_SRC_SUBDIR)/wild_encounters.h: $(DATA_SRC_SUBDIR)/wild_encounters.json $(DATA_SRC_SUBDIR)/wild_encounters.json.txt
	$(JSONPROC) $^ $@

$(C_BUILDDIR)/wild_encounter.o: c_dep += $(DATA_SRC_SUBDIR)/wild_encounters.h

AUTO_GEN_TARGETS += $(DATA_SRC_SUBDIR)/region_map/region_map_entries.h
$(DATA_SRC_SUBDIR)/region_map/region_map_entries.h: $(DATA_SRC_SUBDIR)/region_map/region_map_sections.json $(DATA_SRC_SUBDIR)/region_map/region_map_sections.json.txt
	$(JSONPROC) $^ $@

$(C_BUILDDIR)/region_map.o: c_dep += $(DATA_SRC_SUBDIR)/region_map/region_map_entries.h
