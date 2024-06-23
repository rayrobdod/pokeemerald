# JSON files are run through jsonproc, which is a tool that converts JSON data to an output file
# based on an Inja template. https://github.com/pantor/inja

AUTO_GEN_TARGETS += $(DATA_SRC_SUBDIR)/wild_encounters.h
$(DATA_SRC_SUBDIR)/wild_encounters.h: $(DATA_SRC_SUBDIR)/wild_encounters.json $(DATA_SRC_SUBDIR)/wild_encounters.json.txt
	$(JSONPROC) $^ $@

$(C_BUILDDIR)/wild_encounter.o: c_dep += $(DATA_SRC_SUBDIR)/wild_encounters.h

AUTO_GEN_TARGETS += $(DATA_SRC_SUBDIR)/region_map/region_map_entries.h
$(DATA_SRC_SUBDIR)/region_map/region_map_entries.h: $(DATA_SRC_SUBDIR)/region_map/region_map_sections.json $(DATA_SRC_SUBDIR)/region_map/region_map_sections.json.txt
	$(JSONPROC) $^ $@

$(C_BUILDDIR)/region_map.o: c_dep += $(DATA_SRC_SUBDIR)/region_map/region_map_entries.h

AUTO_GEN_TARGETS += $(DATA_SRC_SUBDIR)/mugshots.h
$(DATA_SRC_SUBDIR)/mugshots.h: $(DATA_SRC_SUBDIR)/mugshots.json $(DATA_SRC_SUBDIR)/mugshots.inja
	$(JSONPROC) $^ $@

$(C_BUILDDIR)/mugshots.o: c_dep += $(DATA_SRC_SUBDIR)/mugshots.h

# scaninc will not find these if data/mugshots.h has not been built yet
# Cannot use the generated-include method since that requires jsonproc to be built first, and make
#    will not attempt to build tools if it cannot read all included files.
# Requires mugshots to be in the expected folder
$(C_BUILDDIR)/mugshots.o: $(patsubst %.png,%.4bpp.lz,$(wildcard graphics/mugshots/*.png))
$(C_BUILDDIR)/mugshots.o: $(patsubst %.png,%.gbapal,$(wildcard graphics/mugshots/*.png))
