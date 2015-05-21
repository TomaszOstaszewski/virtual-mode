.SUFFIXES:
.DEFAULT_GOAL:=all
.SECONDEXPANSION:


#CROSS_COMPILE   :=i686-unknown-elf-
PATH    	:=/home/tomek/x-tools/${CROSS_COMPILE:%-=%}/bin/:${PATH}
CC              :=/home/tomek/x-tools/${CROSS_COMPILE:%-=%}/bin/${CROSS_COMPILE}gcc
OS_IMG_NAME     :=atos.kernel.elf
THIN_IMG_NAME   :=atos.kernel
ISO_IMG_NAME    :=atos.iso

DEP     ?=2
MD5CHK  :=0
AS      :=$(CROSS_COMPILE)gcc
CC      :=$(CROSS_COMPILE)gcc
GCCINC  :=$(shell $(CC) -print-file-name=include)
BUILD_ROOT      :=$(firstword $(subst -, ,$(shell $(CC) -dumpmachine)))/
NASM    :=nasm
CFLAGS  :=-O1 -fno-schedule-insns -ggdb -m32 -Wall -Werror \
 -nostdlib -nostdinc -I$(shell $(CC) -print-file-name=include)  \
 -fno-omit-frame-pointer -ffreestanding -fno-stack-protector \
 -ffunction-sections -fdata-sections
AFLAGS  :=
LDFLAGS :=-Wl,-gc-sections
DEPGEN_FLAGS =-MP -MMD \
 -MT '$(@D)/$(*).o $(@D)/$(*).d $(@D)/$(*).i $(@D)/$(*).S $(@D)/$(*).def dirs-$$(1) doxy-$$(1) $$(BUILD_ROOT)cscope.files bundle log'
LDSCRIPT:=./arch/i386/link.ld

include mk/eyecandy.mk
include mk/macros.mk
include mk/autodir.mk

COMPONENTS      :=\
 boot   \
 kernel \
 vga    \
 klibc_stdio \
 klibc_string \

$(foreach comp,$(COMPONENTS),$(eval $(call ADD_COMPONENT,$(comp))))

-include $(ALL_DEP)

.PHONY: echo-build-root
echo-build-root:
	@echo $(BUILD_ROOT)

.PHONY: echo-app-name
echo-app-name:
	@echo $(OS_IMG_NAME)

.PHONY: echo
echo:
	@echo COMPONENTS $(COMPONENTS)
	@echo ALL_DEP $(ALL_DEP)
	@echo ALL_OBJ $(ALL_OBJ)

.PHONY: all iso
iso all: $(BUILD_ROOT)$(ISO_IMG_NAME)

$(BUILD_ROOT)$(ISO_IMG_NAME): \
 $(BUILD_ROOT)isodir/boot/$(THIN_IMG_NAME) \
 $(BUILD_ROOT)isodir/boot/grub/grub.cfg  \
 | $$(@D)/.
	@$(ECHO_ISO) $(@)
	$(NOECHO)grub-mkrescue -o $(@) $(BUILD_ROOT)isodir

$(BUILD_ROOT)$(OS_IMG_NAME): $(ALL_OBJ) $(LDSCRIPT)
	@$(ECHO_LD) $@
	$(NOECHO)$(CC) $(CFLAGS) $(LDFLAGS) -Wl,-Map=$(@).map -T$(filter %.ld,$(^)) -o $(@) $(filter %.o,$(^)) -lgcc

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
	@$(ECHO_RM) $(ALL_OBJ) $(BUILD_ROOT)$(OS_IMG_NAME) $(BUILD_ROOT)$(ISO_IMG_NAME)
	-$(NOECHO)$(RM) $(ALL_OBJ) $(BUILD_ROOT)$(OS_IMG_NAME) $(BUILD_ROOT)$(ISO_IMG_NAME)

.PHONY: depclean
depclean: 
	-$(NOECHO)$(RM) $(ALL_DEPENDS);

.PHONY: realclean
realclean: 
	-$(NOECHO)$(RM) -r $(BUILD_ROOT)

.PHONY: test
test: $(BUILD_ROOT)$(ISO_IMG_NAME)
	qemu-system-i386 -cdrom $(<)
