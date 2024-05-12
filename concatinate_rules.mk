INCLUDE_CONSTANTS=include/constants


AUTO_GEN_TARGETS += $(INCLUDE_CONSTANTS)/battle_transition.h
$(INCLUDE_CONSTANTS)/battle_transition.h: $(INCLUDE_CONSTANTS)/battle_transition/_prefix.txt $(wildcard $(INCLUDE_CONSTANTS)/battle_transition/*.part) $(INCLUDE_CONSTANTS)/battle_transition/_suffix.txt
	cat $^ >$@

$(C_BUILDDIR)/battle_transition.o: c_dep += $(INCLUDE_CONSTANTS)/battle_transition.h


AUTO_GEN_TARGETS += src/battle_transition_tasks.inc
src/battle_transition_tasks.inc: $(wildcard src/battle_transition_tasks/*.part)
	cat $^ >$@

$(C_BUILDDIR)/battle_transition.o: c_dep += src/battle_transition_tasks.inc


AUTO_GEN_TARGETS += include/battle_transition_tasks.inc
include/battle_transition_tasks.inc: $(wildcard include/battle_transition_tasks/*.h)
	cat $^ >$@

$(C_BUILDDIR)/battle_transition.o: c_dep += include/battle_transition_tasks.inc
