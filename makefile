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

DEP	?=2
MD5CHK	:=0
AS	:=gcc
CC	:=gcc
CXX	:=g++
CFLAGS	:=-m32 -Wall -Werror -fno-omit-frame-pointer -ffreestanding -nostdlib -fno-stack-protector
AFLAGS  :=

ifeq ($(V),1)
NOECHO=
else
NOECHO=@
endif

kernel_DIR:=./
kernel_FILES:= \
 boot.s \
 main.c


COMPONENTS:=\
 kernel

EXTRA_DEP='$$(@D)/$$*.o $$(@D)/$$*.d $$(@D)/$$*.i $$(@D)/$$*.S $$(@D)/$$*.def dirs-$(1) doxy-$(1) $(BUILD_ROOT)cscope.files bundle log'

INCLUDE_PATH=
DEPGEN_FLAGS=-MP -MMD -MT $(EXTRA_DEP)

include macros.mk

$(foreach comp,$(COMPONENTS),$(eval $(call SETUP_VARS,$(comp))))

-include $(ALL_DEPENDS)

$(foreach comp,$(COMPONENTS),$(eval $(call MAKE_OBJECT,$(comp))))

$(BUILD_ROOT)$(APP_NAME): $(ALL_OBJECTS) link.ld
	@$(ECHO_LD) $@
	$(NOECHO)$(CXX) $(CFLAGS) -Wl,-Map=$(@).map -T$(filter link.ld,$(^)) -o $@ $(filter %.o,$^) -lgcc

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
