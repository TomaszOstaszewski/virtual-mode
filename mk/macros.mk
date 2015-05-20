define SETUP_VARS

$(1)_OBJ_DIR:=$(BUILD_ROOT)$(1)/
$(1)_SRC:=$$(addprefix $$($(1)_DIR),$$($(1)_FILES))

$(1)_OBJ+=$$(addprefix $$($(1)_OBJ_DIR),$$(patsubst %.c,%.o,$$(filter %.c,$$($(1)_FILES))))
$(1)_OBJ+=$$(addprefix $$($(1)_OBJ_DIR),$$(patsubst %.s,%.o,$$(filter %.s,$$($(1)_FILES))))

$(1)_DEP:=$$(patsubst %.o,%.d,$$($(1)_OBJ))
ALL_OBJECTS+=$$($(1)_OBJ)
ALL_DEPENDS+=$$($(1)_DEP)
$$($(1)_OBJ_DIR)% : CPPFLAGS+=$$($(1)_INCLUDES)

.PHONY: echo-$(1)
echo-$(1):
	@echo $(1)_SRC $$($(1)_SRC)
	@echo $(1)_OBJ $$($(1)_OBJ)
	@echo $(1)_DEP $$($(1)_DEP)

include mk/$(1).mk

.PHONY : dirs-$(1)
dirs-$(1):
	@echo $$(sort $$(^D))

# Rules for creating object files
$$($(1)_OBJ_DIR)%.o: $$($(1)_DIR)%.c | $$$$(@D)/.
	@$(ECHO_CC) $$@
	$(NOECHO)$(CC)  $$(CFLAGS)   $$(CPPFLAGS) $$(DEPGEN_FLAGS) -o $$@ -c $$<

$$($(1)_OBJ_DIR)%.o: $$($(1)_DIR)%.s | $$$$(@D)/.
	@$(ECHO_AS) $$@
	$(NOECHO)$(CC) -m32 -c $$(AFLAGS) -Wa,-al=$$(@D)/$$(*).lst $$(INCLUDE_PATH) -o $$@ $$<

# Rules for creating preprocessor source files
$$($(1)_OBJ_DIR)%.c: $$($(1)_DIR)%.c | $$$$(@D)/.
	@$(ECHO_CC) $$@
	$(NOECHO) $(CC) $$(CPPFLAGS) $(INCLUDE_PATH) -E -o $$@ $$<

# Rules for creating preprocessor definition files
$$($(1)_OBJ_DIR)%.def: $$($(1)_DIR)%.c | $$$$(@D)/.
	@$(ECHO_CC) $$@
	$(NOECHO)$(ECHO_CC) $$@; $(CC) $$(CPPFLAGS) $(INCLUDE_PATH) -E -dM -o $$@ $$<

# Rules for creating assembly files
$$($(1)_OBJ_DIR)%.s: $$$$(call to-md5,$$($(1)_DIR)%.c $$$$^) | $$$$(@D)/.
	@$(ECHO_CC) $$@
	$(NOECHO)$(CC) $$(CPPFLAGS) $(INCLUDE_PATH) $(DEPGEN_FLAGS) $(CFLAGS) $$($(1)_CFLAGS) -S -o $$@ $$<

endef

