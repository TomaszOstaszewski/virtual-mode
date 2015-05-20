.SUFFIXES:
.DEFAULT_GOAL:=all

ECHO_DEP        := echo "       DEP     "
ECHO_CC         := echo "       CC      "
ECHO_AS         := echo "       AS      "
ECHO_CXX        := echo "       CXX     "
ECHO_LD         := echo "       LD      "
ECHO_MD         := echo "       MD      "
ECHO_ISO        := echo "       ISO     "
ECHO_STRIP      := echo "       STRIP   "
ECHO_CP         := echo "       CP      "
ECHO_RM         := echo "       RM      "

.SECONDEXPANSION:

CROSS_COMPILE   :=i686-unknown-elf-
PATH    	:=/home/tomek/x-tools/${CROSS_COMPILE:%-=%}/bin/:${PATH}
CC              :=/home/tomek/x-tools/${CROSS_COMPILE:%-=%}/bin/${CROSS_COMPILE}gcc
BUILD_ROOT      :=$(firstword $(subst -, ,$(shell $(CC) -dumpmachine)))/
OS_IMG_NAME     :=atos.kernel.elf
THIN_IMG_NAME   :=atos.kernel
ISO_IMG_NAME    :=atos.iso

DEP     ?=2
MD5CHK  :=0
AS      :=gcc
CC      :=gcc
CXX     :=g++
CFLAGS  :=-O0 -ggdb -m32 -Wall -Werror -nostdlib \
 -fno-omit-frame-pointer -ffreestanding -fno-stack-protector \
 -ffunction-sections -fdata-sections
AFLAGS  :=
LDFLAGS :=-Wl,-gc-sections

ifeq ($(V),1)
NOECHO  :=
CP      :=cp -v
else
NOECHO  :=@
CP      :=cp
endif

boot_DIR        :=./arch/i386/
boot_FILES      :=\
 boot.s   \
 crti.s   \
 crtn.s   \

kernel_DIR      :=./
kernel_FILES    :=\
 main.c \

vga_DIR         :=./drivers/vga/
vga_FILES       :=\
 monitor.c

COMPONENTS      :=\
 boot   \
 kernel \
 vga    \

DEPGEN_FLAGS=-MP -MMD \
 -MT '$(@D)/$(*).o $(@D)/$(*).d $(@D)/$(*).i $(@D)/$(*).S $(@D)/$(*).def dirs-$$(1) doxy-$$(1) $$(BUILD_ROOT)cscope.files bundle log'

include mk/macros.mk
include mk/autodir.mk

$(foreach comp,$(COMPONENTS),$(eval $(call SETUP_VARS,$(comp))))

$(kernel_OBJ_DIR)% : CPPFLAGS+=\
 -Idrivers/vga/ \
 -Iarch/i386/

$(vga_OBJ_DIR)% : CPPFLAGS+= \
 -Iarch/i386/

-include $(ALL_DEPENDS)

.PHONY: echo-build-root
echo-build-root:
	@echo $(BUILD_ROOT)

.PHONY: echo-app-name
echo-app-name:
	@echo $(OS_IMG_NAME)

.PHONY: echo
echo:
	@echo COMPONENTS $(COMPONENTS)
	@echo ALL_OBJECTS $(ALL_OBJECTS)
	@echo vga_OBJ $(vga_OBJ)
	@echo vga_SRC $(vga_SRC)
	@echo boot_OBJ $(boot_OBJ)
	@echo kernel_OBJ $(kernel_OBJ)

.PHONY: all iso
iso all: $(BUILD_ROOT)$(ISO_IMG_NAME)

$(BUILD_ROOT)$(ISO_IMG_NAME): \
 $(BUILD_ROOT)isodir/boot/$(THIN_IMG_NAME) \
 $(BUILD_ROOT)isodir/boot/grub/grub.cfg  \
 | $$(@D)/.
	@$(ECHO_ISO) $(@)
	$(NOECHO)grub-mkrescue -o $(@) $(BUILD_ROOT)isodir

$(BUILD_ROOT)$(OS_IMG_NAME): $(ALL_OBJECTS) link.ld
	@$(ECHO_LD) $@
	$(NOECHO)$(CC) $(CFLAGS) $(LDFLAGS) -Wl,-Map=$(@).map -T$(filter link.ld,$(^)) -o $(@) $(filter %.o,$(^)) -lgcc

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

.PHONY: test
test: $(BUILD_ROOT)$(ISO_IMG_NAME)
	qemu-system-i386 -cdrom $(<)
