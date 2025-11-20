
EASY_CHAT_SUBDIR := $(DATA_SRC_SUBDIR)/easy_chat
EASY_CHAT_SRCS := $(wildcard $(EASY_CHAT_SUBDIR)/*.json)

define EASY_CHAT_WORD_INFO
AUTO_GEN_TARGETS += $1
$1: $2 $$(EASYCHAT)
	$$(EASYCHAT) wordinfo $$< $$@
$$(C_BUILDDIR)/easy_chat.o: c_dep += $1
endef
$(foreach src, $(EASY_CHAT_SRCS), $(eval $(call EASY_CHAT_WORD_INFO,$(patsubst $(EASY_CHAT_SUBDIR)/%.json,$(EASY_CHAT_SUBDIR)/%.h, $(src)),$(src))))

