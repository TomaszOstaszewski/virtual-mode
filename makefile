# An ultimate makefile... 

.SUFFIXES:
.DEFAULT_GOAL:=all
.PHONY : clean depclean realclean FORCE log echo-build-root echo-app-name cscope

ECHO_DEP:= echo   DEP  
ECHO_CC := echo   CC   
ECHO_AS := echo   AS   
ECHO_CXX:= echo   CXX  
ECHO_LD := echo   LD   
ECHO_MD := echo   MD   

.SECONDEXPANSION:

BUILD_ROOT:=$(firstword $(subst -, ,$(shell $(CC) -dumpmachine)))/

echo-build-root:
	@echo $(BUILD_ROOT)

APP_NAME:=image
echo-app-name:
	@echo $(APP_NAME)

DEP?=2
MD5CHK:=0
AS:=nasm
CFLAGS:=-Wall -Werror -fno-omit-frame-pointer -nostdlib -fno-stack-protector
LDFLAGS:=-Tlink.ld -Wl,-Map=$(BUILD_ROOT)$(APP_NAME).map
ifeq ($(V),1)
NOECHO=
else
NOECHO=@
endif

kernel_DIR:=./
kernel_FILES:= \
 common.c \
 descriptor-tables.c \
 isr.c \
 main.c \
 monitor.c \
 pic-8259a.c \
 timer.c \
 boot.s \
 gdt.s \
 interrupt.s
kernel_ASMFLAGS:=-felf

COMPONENTS:=\
 kernel

# This is MD5 definitions part.

# MD5 checksums are generated and checked for every dependency on the target
# dependency list. Specify MD5CHK=1 at make invocation line to enable it.
# This can speed up one-time build, but there's no guarantee that incremental
# build will be reliable.

# Where to store md5 sums.
MD5DIR := $(BUILD_ROOT)md5/

# Function for requesting dependency on md5 checksum in addition to standard
# file dependency.
define to-md5
$(1) $(if $(findstring $(MD5CHK),1),$(addprefix $(MD5DIR),$(addsuffix .md5,$(patsubst ../%,%,$(1)))),)
endef

# This is a variant of rule above. It makes target dependent on already
# generated md5 checksums only. This is used when generating dependency files.
define to-existing-md5
$(1) $(if $(findstring $(MD5CHK),1),$(realpath $(addprefix $(MD5DIR),$(addsuffix .md5,$(patsubst ../%,%,$(1))))),)
endef

# Calculation of md5 checksum.
# Based on http://www.cmcrossroads.com/ask-mr-make/6795-rebuilding-when-a-files-checksum-changes
define calculate-md5
$(if $(filter-out $(shell cat $@ 2>/dev/null),$(shell md5sum $<)),md5sum $< > $@)
endef

.PRECIOUS: $(MD5DIR)%.md5

$(MD5DIR)%.md5: % FORCE | $$(@D)/.
	$(NOECHO)$(calculate-md5)

$(MD5DIR)%.md5: ../% FORCE | $$(@D)/.
	$(NOECHO)$(calculate-md5)

# This one here allows automatic directory creation
# All that is required is the secondary expansion of the prerequisities.
# And all the object files shall also be dependant, 'order only', on their
# containting directories + '/.' suffix.
.PRECIOUS: %/.
%/.:
	@$(ECHO_MD) $*
	$(NOECHO)mkdir -p $@

EXTRA_DEP='$$(@D)/$$*.o $$(@D)/$$*.d $$(@D)/$$*$$(suffix $$<) $$(@D)/$$*.S $$(@D)/$$*.def dirs-$(1) doxy-$(1) $(BUILD_ROOT)cscope.files bundle log'
# This variable says whether or not
# paths for the standard library files shall be
# present in dependencies.
STDLIB_PATH_IN_DEP:=0

# if DEP is set to value 0
# then no dependencies will be generated.
ifeq ($(DEP),0)
INCLUDE_PATH=$$($(1)_INCLUDES)
DEPGEN_FLAGS   =
endif

# if DEP is set to value 1
# then dependencies will be generated as components needed
# to perform actual compilation.
ifeq ($(DEP),1)
INCLUDE_PATH=$$(addprefix -I,$$(sort $$(^D))) 
DEPGEN_GENERIC_FLAGS=-MP -MG -MM -MF $$(@D)/$$*.d -MT $(EXTRA_DEP)
DEPGEN_FLAGS=
# For regular dependency generation, the flag has to be -MM
# This is because the include dirs for object file generation
# are taken via '$$(^D)' rule. Hence, with -M, the include
# dirs string would be cluttered with standard library
# directories. Placing explicit include directories causes
# a compilation to fail in this case.
endif

# if DEP is set to value 2
# then dependencies will be generated as a byproduct
# of regular compilation.
ifeq ($(DEP),2)
INCLUDE_PATH=$$($(1)_INCLUDES)
DEPGEN_FLAGS=-MP -MMD -MT $(EXTRA_DEP)
endif


# If a rule has no prerequisites or commands, and the target of the rule is a nonexistent file, 
# then `make' imagines this target to have been updated whenever its rule is run. This implies 
# that all targets depending on this one will always have their commands run.
# In short - this will trigger regenaration of MD5 checksum for all the source and header
# files, regardless of their timestamp.
FORCE: ;

define SETUP_VARS

$(1)_OBJ_DIR:=$(BUILD_ROOT)$(1)/
$(1)_SRC:=$$(addprefix $$($(1)_DIR),$$($(1)_FILES))

$(1)_OBJ:=$$(addprefix $$($(1)_OBJ_DIR),$$(patsubst %.c,%.o,$$(filter %.c,$$($(1)_FILES))))
$(1)_OBJ+=$$(addprefix $$($(1)_OBJ_DIR),$$(patsubst %.cpp,%.o,$$(filter %.cpp,$$($(1)_FILES))))
$(1)_OBJ+=$$(addprefix $$($(1)_OBJ_DIR),$$(patsubst %.cxx,%.o,$$(filter %.cxx,$$($(1)_FILES))))
$(1)_OBJ+=$$(addprefix $$($(1)_OBJ_DIR),$$(patsubst %.s,%.o,$$(filter %.s,$$($(1)_FILES))))

$(1)_DEP:=$$(patsubst %.o,%.d,$$($(1)_OBJ))
ALL_OBJECTS+=$$($(1)_OBJ)
ALL_DEPENDS+=$$($(1)_DEP)

.PHONY : dirs-$(1)
dirs-$(1):
	@echo $$(sort $$(^D))

endef

define MAKE_DEPEND

$$($(1)_OBJ_DIR)%.d: $$$$(call to-existing-md5,$$($(1)_DIR)%.c $$$$^) | $$$$(@D)/.
	@$(ECHO_DEP) $$@
	$(NOECHO)$(CC)  $(CPPFLAGS) $$($(1)_INCLUDES) $(DEPGEN_GENERIC_FLAGS) -o $$@ $$<
$$($(1)_OBJ_DIR)%.d: $$$$(call to-existing-md5,$$($(1)_DIR)%.cpp $$$$^) | $$$$(@D)/.
	@$(ECHO_DEP) $$@
	$(NOECHO)$(CXX) $(CPPFLAGS) $$($(1)_INCLUDES) $(DEPGEN_GENERIC_FLAGS) -o $$@ $$<
$$($(1)_OBJ_DIR)%.d: $$$$(call to-existing-md5,$$($(1)_DIR)%.cc $$$$^) | $$$$(@D)/.
	@$(ECHO_DEP) $$@
	$(NOECHO)$(CXX) $(CPPFLAGS) $$($(1)_INCLUDES) $(DEPGEN_GENERIC_FLAGS) -o $$@ $$<
$$($(1)_OBJ_DIR)%.d: $$$$(call to-existing-md5,$$($(1)_DIR)%.s $$$$^) | $$$$(@D)/.
	@$(ECHO_DEP) $$@
	$(NOECHO)$(AS) $(CPPFLAGS) $$($(1)_INCLUDES) -M -MT $$@ $$< > $$@


endef

define 	MAKE_OBJECT

# Rules for creating object files
$$($(1)_OBJ_DIR)%.o: $$$$(call to-md5,$$($(1)_DIR)%.c $$$$^) | $$$$(@D)/.
	@$(ECHO_CC) $$@
	$(NOECHO)$(CC)  $(CFLAGS) $$($(1)_CFLAGS) $(CPPFLAGS) $(INCLUDE_PATH) $(DEPGEN_FLAGS) -o $$@ -c $$<
$$($(1)_OBJ_DIR)%.o: $$$$(call to-md5,$$($(1)_DIR)%.cpp $$$$^) | $$$$(@D)/.
	@$(ECHO_CC) $$@
	$(NOECHO)$(CXX) $(CFLAGS) $$($(1)_CFLAGS) $(CPPFLAGS) $(INCLUDE_PATH) $(DEPGEN_FLAGS) -o $$@ -c $$<
$$($(1)_OBJ_DIR)%.o: $$$$(call to-md5,$$($(1)_DIR)%.cc $$$$^) | $$$$(@D)/.
	@$(ECHO_CC) $$@
	$(NOECHO)$(CXX) $(CFLAGS) $$($(1)_CFLAGS) $(CPPFLAGS) $(INCLUDE_PATH) $(DEPGEN_FLAGS) -o $$@ -c $$<
$$($(1)_OBJ_DIR)%.o: $$$$(call to-md5,$$($(1)_DIR)%.s $$$$^) | $$$$(@D)/.
	@$(ECHO_AS) $$@
	$(NOECHO)$(AS) $(ASMFLAGS) $$($(1)_ASMFLAGS) $(INCLUDE_PATH) -l $$(@D)/$$*.lst -o $$@ $$<


# Rules for creating preprocessor source files
$$($(1)_OBJ_DIR)%.c: $$$$(call to-md5,$$($(1)_DIR)%.c $$$$^) | $$$$(@D)/.
	@$(ECHO_CC) $$@
	$(NOECHO) $(CC) $(CPPFLAGS) $(INCLUDE_PATH) -E -o $$@ $$<
$$($(1)_OBJ_DIR)%.cpp: $$$$(call to-md5,$$($(1)_DIR)%.cpp $$$$^) | $$$$(@D)/.
	@$(ECHO_CC) $$@
	$(NOECHO) $(CC) $(CPPFLAGS) $(INCLUDE_PATH) -E -o $$@ $$<
$$($(1)_OBJ_DIR)%.cc: $$$$(call to-md5,$$($(1)_DIR)%.cc $$$$^) | $$$$(@D)/.
	@$(ECHO_CC) $$@
	$(NOECHO) $(CC) $(CPPFLAGS) $(INCLUDE_PATH) -E -o $$@ $$<

# Rules for creating preprocessor definition files
$$($(1)_OBJ_DIR)%.def: $$$$(call to-md5,$$($(1)_DIR)%.c $$$$^) | $$$$(@D)/.
	@$(ECHO_CC) $$@
	$(NOECHO)$(ECHO_CC) $$@; $(CC) $(CPPFLAGS) $(INCLUDE_PATH) -E -dM -o $$@ $$<
$$($(1)_OBJ_DIR)%.def: $$$$(call to-md5,$$($(1)_DIR)%.cpp $$$$^) | $$$$(@D)/.
	@$(ECHO_CC) $$@
	$(NOECHO)$(ECHO_CC) $$@; $(CC) $(CPPFLAGS) $(INCLUDE_PATH) -E -dM -o $$@ $$<
$$($(1)_OBJ_DIR)%.def: $$$$(call to-md5,$$($(1)_DIR)%.cc $$$$^) | $$$$(@D)/.
	@$(ECHO_CC) $$@
	$(NOECHO)$(ECHO_CC) $$@; $(CC) $(CPPFLAGS) $(INCLUDE_PATH) -E -dM -o $$@ $$<

# Rules for creating assembly files
$$($(1)_OBJ_DIR)%.s: $$$$(call to-md5,$$($(1)_DIR)%.c $$$$^) | $$$$(@D)/.
	@$(ECHO_CC) $$@
	$(NOECHO)$(CC)  $(CPPFLAGS) $(INCLUDE_PATH) $(DEPGEN_FLAGS) $(CFLAGS) $$($(1)_CFLAGS) -S -o $$@ $$<
$$($(1)_OBJ_DIR)%.s: $$$$(call to-md5,$$($(1)_DIR)%.cpp $$$$^) | $$$$(@D)/.
	@$(ECHO_CC) $$@
	$(NOECHO)$(CXX) $(CPPFLAGS) $(INCLUDE_PATH) $(DEPGEN_FLAGS) $(CFLAGS) $$($(1)_CFLAGS) -S -o $$@ $$<
$$($(1)_OBJ_DIR)%.s: $$$$(call to-md5,$$($(1)_DIR)%.cc $$$$^) | $$$$(@D)/.
	@$(ECHO_CC) $$@
	$(NOECHO)$(CXX) $(CPPFLAGS) $(INCLUDE_PATH) $(DEPGEN_FLAGS) $(CFLAGS) $$($(1)_CFLAGS) -S -o $$@ $$<

endef

$(foreach comp,$(COMPONENTS),$(eval $(call SETUP_VARS,$(comp))))

ifeq ($(DEP),0)
else
ifeq ($(DEP),1)
$(foreach comp,$(COMPONENTS),$(eval $(call MAKE_DEPEND,$(comp))))
ifeq ($(or $(filter echo-build-root,$(MAKECMDGOALS)),$(filter %clean,$(MAKECMDGOALS))),)
include $(ALL_DEPENDS)
endif
else
ifeq ($(or $(filter echo-%,$(MAKECMDGOALS)),$(filter %clean,$(MAKECMDGOALS))),)
-include $(ALL_DEPENDS)
endif
endif
endif

$(foreach comp,$(COMPONENTS),$(eval $(call MAKE_OBJECT,$(comp))))

$(BUILD_ROOT)$(APP_NAME): $(ALL_OBJECTS) link.ld
	@$(ECHO_LD) $@
	$(NOECHO)$(CXX) $(CFLAGS) $(LDFLAGS) -o $@ $(filter %.o,$^)

all: $(BUILD_ROOT)$(APP_NAME) ;

log: all 
	@echo $? > $(BUILD_ROOT)$@
$(BUILD_ROOT)cscope.files:
	echo $(filter %.c,$^) > $@	
cscope: $(BUILD_ROOT)cscope.files
	cscope -bkq -i $< -I $(^D)
clean: $(if $(findstring $(DEP),2),depclean,)
	-$(RM) $(ALL_OBJECTS) $(BUILD_ROOT)$(APP_NAME);
depclean: 
	-$(RM) $(ALL_DEPENDS);
realclean: 
	-$(RM) -r $(BUILD_ROOT)
