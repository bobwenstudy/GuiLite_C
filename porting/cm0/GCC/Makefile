#
# 'make'        build executable file 'main'
# 'make clean'  removes all .o and executable files
#
.DEFAULT_GOAL := all

ifeq ($(OS),Windows_NT)
TOOLCHAIN_DIR 	= D:\Program Files (x86)\Arm GNU Toolchain arm-none-eabi\12.2 mpacbti-rel1
else
TOOLCHAIN_DIR = /usr/gcc-arm-none-eabi-7-2018-q2-update
endif

# define the target
TARGET		  	?= main





COMPILE_PREFIX 	+= $(TOOLCHAIN_DIR)/bin/arm-none-eabi-

# define the C compiler to use
CC      := $(COMPILE_PREFIX)gcc
CXX     := $(COMPILE_PREFIX)g++
OBJDUMP := $(COMPILE_PREFIX)objdump
OBJCOPY := $(COMPILE_PREFIX)objcopy
GDB     := $(COMPILE_PREFIX)gdb
AR      := $(COMPILE_PREFIX)ar
SIZE    := $(COMPILE_PREFIX)size

## cpu setting ##
CPU_ARCH := cortex-m0

LINKER_SCRIPT ?= gcc_arm.ld



# define any compile-time flags
CFLAGS  := -Os
CFLAGS  += -g

CFLAGS += -mthumb -mcpu=$(CPU_ARCH)

# warning param setting
CFLAGS	+= -Wall
# think use -Os.
CFLAGS	+= -Wno-unused-function
CFLAGS	+= -Wno-unused-variable

CFLAGS	+= -Wstrict-prototypes
CFLAGS	+= -Wshadow
# CFLAGS	+= -Werror

# spec c version
CFLAGS  += -std=c99

# for makefile depend tree create
CFLAGS  += -MMD -MP

# for weak.
CFLAGS  += -fno-common

## MAKEFILE COMPILE MESSAGE CONTROL ##
ifeq ($(V),1)
	Q=
else
	Q=@
endif

# Put functions and data in their own binary sections so that ld can
# garbage collect them
ifeq ($(NOGC),1)
GC_CFLAGS =
GC_LDFLAGS =
else
GC_CFLAGS = -ffunction-sections -fdata-sections
GC_LDFLAGS = -Wl,--gc-sections -Wl,--check-sections
endif


CFLAGS	+= $(GC_CFLAGS)

# define ld params
LDFLAGS	:=
LDFLAGS	+= $(GC_LDFLAGS)
# LDFLAGS += --specs=nosys.specs
LDFLAGS += -lc_nano -lgcc
# LDFLAGS += --specs=nano.specs
LDFLAGS += -T $(LINKER_SCRIPT)

LDFLAGS += -Wl,--no-warn-rwx-segments

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS :=

# define output directory
OUTPUT_PATH	:= output

# define source directory
SRC		:= 

# define include directory
INCLUDE	:=
INCLUDE	+= output

# define lib directory
LIB		:= 



#set report setting. becareful, this only support elf. not work on windows.
DEBUG_REPORT :=

# load .conf setting.
#include $(DOTCONFIG_PATH)

ROOT_PATH := ../../..

# include guilite src
GUILITE_PATH := $(ROOT_PATH)/src
include $(GUILITE_PATH)/build.mk

# include port info
PORT ?= cm0

PORT_ROOT_PATH = $(ROOT_PATH)/porting
PORT_PATH = $(PORT_ROOT_PATH)/$(PORT)
include $(PORT_PATH)/build.mk

# include app info
APP_ROOT_PATH = $(ROOT_PATH)/example
APP_PATH = $(APP_ROOT_PATH)/$(APP)
include $(APP_PATH)/build.mk



LIB_DIR = ../Libraries

CMSIS_DIR = $(LIB_DIR)/CMSIS
CMSIS_DEVICE_DIR = $(CMSIS_DIR)/Device/ARM/ARMCM0

SRC += $(CMSIS_DEVICE_DIR)/Source
INCLUDE += $(CMSIS_DEVICE_DIR)/Include $(CMSIS_DIR)/Include








ifeq ($(OS),Windows_NT)
ifdef ComSpec
	WINCMD:=$(ComSpec)
endif
ifdef COMSPEC
	WINCMD:=$(COMSPEC)
endif

SHELL:=$(WINCMD)

MAIN	:= $(TARGET).elf
ECHO=echo
SOURCEDIRS	:= $(SRC)
INCLUDEDIRS	:= $(INCLUDE)
LIBDIRS		:= $(LIB)
FIXPATH = $(subst /,\,$1)
RM			:= del /q /s
MD			:= mkdir
else
MAIN	:= $(TARGET).elf
ECHO=echo
SOURCEDIRS	:= $(shell find $(SRC) -type d)
INCLUDEDIRS	:= $(shell find $(INCLUDE) -type d)
LIBDIRS		:= $(shell find $(LIB) -type d)
FIXPATH = $1
RM = rm -rf
MD	:= mkdir -p
endif

# define any directories containing header files other than /usr/include
INCLUDES	:= $(patsubst %,-I%, $(INCLUDEDIRS:%/=%))
@echo INCLUDES: $(INCLUDES)

# define the C libs
LIBS		:= $(patsubst %,-L%, $(LIBDIRS:%/=%))

# define the C source files
SOURCES		:= $(wildcard $(patsubst %,%/*.c, $(SOURCEDIRS)))

# define the C object files 
OBJDIR = $(OUTPUT_PATH)/obj/1/2/3/4/5
OBJECTS			:= $(patsubst %, $(OBJDIR)/%, $(SOURCES:.c=.o))
OBJ_MD			:= $(addprefix $(OBJDIR)/, $(SOURCEDIRS))





ALL_DEPS := $(OBJECTS:.o=.d)

# include dependency files of application
ifneq ($(MAKECMDGOALS),clean)
-include $(ALL_DEPS)
endif

#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

OUTPUT_MAIN		:= $(OUTPUT_PATH)/$(MAIN)
OUTPUT_TARGET	:= $(OUTPUT_PATH)/$(TARGET)

# Fix path error.
#OUTPUT_MAIN := $(call FIXPATH,$(OUTPUT_MAIN))

.PHONY: clean help info

info:
	@$(ECHO) Current Configuration: APP=$(APP) PORT=$(PORT)

dasm:
	$(OBJCOPY) -v -O binary $(OUTPUT_MAIN) $(OUTPUT_TARGET).bin
	"$(OBJDUMP)" --source --all-headers --demangle --line-numbers --wide $(OUTPUT_MAIN) > $(OUTPUT_TARGET).lst
	"$(OBJDUMP)" -S -d $(OUTPUT_MAIN) > $(OUTPUT_TARGET).dump

all: | info $(MAIN) $(DEBUG_REPORT)
	@$(ECHO) Executing 'all' complete!
	@$(ECHO) Print Size
	$(Q)@$(SIZE) --format=berkeley $(OUTPUT_MAIN)

# mk path for object.
$(OBJ_MD):
	$(Q)if not exist "$@" $(Q)$(MD) $(call FIXPATH, $@)

# mk output path.
$(OUTPUT_PATH):
	$(Q)if not exist "$@" $(Q)$(MD) $(call FIXPATH, $@)

$(OBJDIR):
	$(Q)if not exist "$@" $(Q)$(MD) $(call FIXPATH, $@)

$(MAIN): | $(OUTPUT_PATH) $(OBJDIR) $(OBJ_MD) $(OBJECTS) 
	@$(ECHO) Linking    : "$@"
	$(Q)$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) -Wl,-Map,$(OUTPUT_TARGET).map -o $(OUTPUT_MAIN) $(OBJECTS) $(LFLAGS) $(LIBS)

# Static Pattern Rules [targets ...: target-pattern: prereq-patterns ...]
# $(OBJECTS): $(OBJDIR)/%.o : %.c $(AUTOCONFIG_H)
$(OBJECTS): $(OBJDIR)/%.o : %.c
	@$(ECHO) Compiling  : "$<"
	$(Q)$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@


# $(AUTOCONFIG_H): $(DOTCONFIG_PATH)
# 	python scripts/kconfig/kconfig.py $(KCONFIG_ROOT_PATH) $(DOTCONFIG_PATH) $(AUTOCONFIG_H) $(OUTPUT_PATH)/autoconfig_log.txt $(DOTCONFIG_PATH)

# $(USER_RECORD_CONFIG_PATH): $(USER_CONFIG_SET)
# 	@echo Using user config.
# #	create user_record.conf to record current setting.
# 	@copy $(call FIXPATH, $(USER_CONFIG_SET)) $(call FIXPATH, $(USER_RECORD_CONFIG_PATH))
# #	create .config by user config setting.
# 	python scripts/kconfig/kconfig.py --handwritten-input-configs $(KCONFIG_ROOT_PATH) $(DOTCONFIG_PATH) $(AUTOCONFIG_H) $(OUTPUT_PATH)/autoconfig_log.txt $(USER_CONFIG_SET)

# export KCONFIG_CONFIG=$(DOTCONFIG_PATH)
# $(DOTCONFIG_PATH):$(USER_RECORD_CONFIG_PATH)
# 	@echo .config updated

# menuconfig:$(DOTCONFIG_PATH)
# #	set KCONFIG_CONFIG=$(DOTCONFIG_PATH)
# 	menuconfig $(KCONFIG_ROOT_PATH)

# guiconfig:
# 	guiconfig $(KCONFIG_ROOT_PATH)

clean:
#	$(RM) $(OUTPUT_MAIN)
#	$(RM) $(OBJECTS)
#	$(RM) $(OBJDIR)
	$(Q)$(RM) $(call FIXPATH, $(OUTPUT_PATH))
	@$(ECHO) Cleanup complete!

run: all
	./$(OUTPUT_MAIN)
	@$(ECHO) Executing 'run: all' complete!

ram_report:
	python $(ROOT_PATH)/scripts/footprint/size_report -k $(OUTPUT_MAIN) -z $(OUTPUT_PATH) -o $(OUTPUT_PATH) --workspace=$(OUTPUT_PATH) -d 99 ram

rom_report:
	python $(ROOT_PATH)/scripts/footprint/size_report -k $(OUTPUT_MAIN) -z $(OUTPUT_PATH) -o $(OUTPUT_PATH) --workspace=$(OUTPUT_PATH) -d 99 rom

all_report:
	python $(ROOT_PATH)/scripts/footprint/size_report -k $(OUTPUT_MAIN) -z $(OUTPUT_PATH) -o $(OUTPUT_PATH) --workspace=$(OUTPUT_PATH) -d 99 all

code_format:
	python code_format.py


help:
	@$(ECHO) "zephyr_polling Software Development Kit"
	@$(ECHO) "== For detailed user guide, please check xxxx"
	@$(ECHO) "== Make variables used in SDK =="
	@$(ECHO) "APP:         Select APP Demo built in SDK, will select <beacon> by default"
	@$(ECHO) "PORT:        Select Porting info built in SDK, will select <windows_libusb_win32> by default"
	@$(ECHO) "CHIPSET:     Select Chipset built in SDK, will select <csr8510> by default"
	@$(ECHO) "NOGC:        NOGC=1 diable gc sections, default is 0"
	@$(ECHO) "V:           V=1 verbose make, will print more information, by default V=0"
	@$(ECHO) "== How to Use with Make =="
	@$(ECHO) "1. Build Application:"
	@$(ECHO) "all [APP=beacon] [PORT=windows_libusb_win32] [CHIPSET=csr8510]"
	@$(ECHO) "   Build a software program."
	@$(ECHO) "2. Show menuconfig:"
	@$(ECHO) "menuconfig [APP=beacon] [PORT=windows_libusb_win32] [CHIPSET=csr8510]"
	@$(ECHO) "   Use menuconfig to show the Kconfig setting."
	@$(ECHO) "3. Show guiconfig:"
	@$(ECHO) "guiconfig [APP=beacon] [PORT=windows_libusb_win32] [CHIPSET=csr8510]"
	@$(ECHO) "   Use guiconfig to show the Kconfig setting."
	@$(ECHO) "4: Format all code style by .clang-format"
	@$(ECHO) "code_format"
	@$(ECHO) "   It is recommended to modify the code format before submitting."
	@$(ECHO) ""
	@$(ECHO) "== Example Usage =="
	@$(ECHO) "1. Build for default application: make all"
	@$(ECHO) "2. Run app(Windows), output\main.exe"
	@$(ECHO) ""
