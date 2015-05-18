# An ultimate makefile... 

.SUFFIXES:
.DEFAULT_GOAL:=all

ECHO_DEP:= echo "	DEP	"
ECHO_CC := echo "	CC	"
ECHO_AS := echo "	AS	"
ECHO_CXX:= echo "	CXX	"
ECHO_LD := echo "	LD	"
ECHO_MD := echo "	MD	"
ECHO_ISO:= echo "	ISO	"
ECHO_STRIP:= echo "	STRIP	"
ECHO_CP := echo "	CP	"
ECHO_RM := echo "	RM	"

.SECONDEXPANSION:

BUILD_ROOT:=$(firstword $(subst -, ,$(shell $(CC) -dumpmachine)))/

.PHONY: echo-build-root
echo-build-root:
	@echo $(BUILD_ROOT)

.PHONY: echo-app-name
echo-app-name:
	@echo $(OS_IMG_NAME)

OS_IMG_NAME:=atos.kernel.elf
THIN_IMG_NAME:=atos.kernel
ISO_IMG_NAME:=atos.iso


DEP	?=2
MD5CHK	:=0
AS	:=gcc
CC	:=gcc
CXX	:=g++
CFLAGS	:=-m32 -Wall -Werror -fno-omit-frame-pointer -ffreestanding -nostdlib -fno-stack-protector
AFLAGS  :=

ifeq ($(V),1)
NOECHO=
CP:=cp -v
else
NOECHO=@
CP:=cp
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
include autodir.mk

$(foreach comp,$(COMPONENTS),$(eval $(call SETUP_VARS,$(comp))))

-include $(ALL_DEPENDS)

$(foreach comp,$(COMPONENTS),$(eval $(call MAKE_OBJECT,$(comp))))

$(BUILD_ROOT)$(OS_IMG_NAME): $(ALL_OBJECTS) link.ld
	@$(ECHO_LD) $@
	$(NOECHO)$(CXX) $(CFLAGS) -Wl,-Map=$(@).map -T$(filter link.ld,$(^)) -o $@ $(filter %.o,$(^)) -lgcc


.PHONY: all
all: $(BUILD_ROOT)$(OS_IMG_NAME) ;

.PHONY: iso
iso: $(BUILD_ROOT)$(ISO_IMG_NAME)

$(BUILD_ROOT)$(ISO_IMG_NAME): \
 $(BUILD_ROOT)isodir/boot/$(THIN_IMG_NAME) \
 $(BUILD_ROOT)isodir/boot/grub/grub.cfg  \
 | $$(@D)/.
	@$(ECHO_ISO) $(@)
	$(NOECHO)grub-mkrescue -o $(@) $(BUILD_ROOT)isodir

$(BUILD_ROOT)isodir/boot/grub/grub.cfg: ./grub.conf | $$(@D)/.
	@$(ECHO_CP) $(@)
	$(NOECHO)$(CP) $(<) $(@)

$(BUILD_ROOT)isodir/boot/$(THIN_IMG_NAME): $(BUILD_ROOT)$(OS_IMG_NAME) | $$(@D)/.
	@$(ECHO_STRIP) $(@)
	$(NOECHO)strip -o $(@) -R .comment -R .note.gnu.build-id $(<) 

$(BUILD_ROOT)isodir/boot/$(OS_IMG_NAME): $(BUILD_ROOT)$(OS_IMG_NAME) | $$(@D)/.
	@$(ECHO_CP) $(@)
	$(NOECHO)$(CP) $(<) $(@)

.PHONY: log
log: all 
	@echo $? > $(BUILD_ROOT)$@

$(BUILD_ROOT)cscope.files:
	echo $(filter %.c,$^) > $@

.PHONY: cscope
cscope: $(BUILD_ROOT)cscope.files
	cscope -bkq -i $< -I $(^D)

.PHONY: clean
clean:
	@$(ECHO_RM) $(ALL_OBJECTS) $(BUILD_ROOT)$(OS_IMG_NAME) $(BUILD_ROOT)$(ISO_IMG_NAME)
	-$(NOECHO)$(RM) $(ALL_OBJECTS) $(BUILD_ROOT)$(OS_IMG_NAME) $(BUILD_ROOT)$(ISO_IMG_NAME)

.PHONY: depclean
depclean: 
	-$(NOECHO)$(RM) $(ALL_DEPENDS);

.PHONY: realclean
realclean: 
	-$(NOECHO)$(RM) -r $(BUILD_ROOT)
