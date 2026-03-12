#
# Makefile
#
#命令行执行下面三行
#export PATH=$PATH:/home/book/100ask/T113s4_Lvgl9-Class_TinaSDK-V1/prebuilt/rootfsbuilt/arm/toolchain-sunxi-glibc-gcc-830/toolchain/bin
#export CROSS_COMPILE=arm-openwrt-linux-gnueabi
#export STAGING_DIR=/home/book/100ask/T113s4_Lvgl9-Class_TinaSDK-V1/out/t113_s4/100ask_lvgl9/openwrt/staging_dir/target

#STAGING_DIR ?= $(shell echo $STAGING_DIR)
#CROSS_COMPILE ?= $(shell echo $CROSS_COMPILE)
#PATH := $(shell echo $PATH):$(PATH)

$(info ====== DEBUG: STAGING_DIR = $(STAGING_DIR) ======)
$(info ====== DEBUG: PATH = $(PATH) ======)

# my code
MY_CODE 	=  ./lv_port_demos/my_code/1_test/my_test.c	 \
							./lv_port_demos/my_code/MFD/mfd.c			 \
							./lv_port_demos/my_code/MFD/password.c		 \
							./lv_port_demos/my_code/MFD/menue.c		 	 \
							./lv_port_demos/my_code/MFD/calculate.c		 \
							./lv_port_demos/my_code/MFD/mqtt_led.c		 \
							./lv_port_demos/my_code/MFD/time_show.c		 \
							./lv_port_demos/my_code/MFD/led.c		 \
							./lv_port_demos/my_code/MFD/serial.c			\
							./lv_port_demos/assets/img_calculate50_50.c	 \
							./lv_port_demos/assets/img_led50_50.c	 \
							./lv_port_demos/my_code/VCS/vcs.c			 \
							./lv_port_demos/my_code/VCS/vcs_led.c			 \
							./lv_port_demos/my_code/VCS/vcs_serial.c			 \
							./lv_port_demos/my_code/VCS/vcs_servo.c			 \
							./lv_port_demos/my_code/VCS/vcs_mqtt.c			 \
							./lv_port_demos/my_code/VCS/vcs_temp.c			 \
							./lv_port_demos/assets/car.c	 

#CC 			?= gcc
#CXX 			?= g++
CC 				= arm-openwrt-linux-gnueabi-gcc
CXX 			= arm-openwrt-linux-gnueabi-g++
LVGL_DIR_NAME 	?= lvgl
LVGL_DIR 		?= .

PAHO_MQTT_DIR = ./lv_port_demos/my_code/paho.mqtt.c# MQTT
CFLAGS_MQTT = -I${PAHO_MQTT_DIR}/build/src -O# MQTT


LDFLAGS_FFMPEG = -lavformat -lavcodec -lavutil -lswscale -lm -lz  # FFMPEG
LDFLAGS_MQTT = -L${PAHO_MQTT_DIR}/build/src -lpaho-mqtt3c  # MQTT

CFLAGS=

WARNINGS		:= -Wall -Wshadow -Wundef -Wmissing-prototypes -Wno-discarded-qualifiers -Wextra -Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith \
					-fno-strict-aliasing -Wno-error=cpp -Wuninitialized -Wmaybe-uninitialized -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits \
					-Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wformat-security \
					-Wno-ignored-qualifiers -Wno-error=pedantic -Wno-sign-compare -Wno-error=missing-prototypes -Wdouble-promotion -Wclobbered -Wdeprecated -Wempty-body \
					-Wshift-negative-value -Wstack-usage=2048 -Wno-unused-value -std=gnu99
CFLAGS 			?= -O3 -g0 -I$(LVGL_DIR)/ $(WARNINGS)	\
						-I$(STAGING_DIR)/usr/include\
				   		$(CFLAGS_MQTT)\

LDFLAGS 		?= -lm -lstdc++		\
					-L$(STAGING_DIR)/usr/lib \
					-lrt -ldl -lanl \
					-lpthread		\
					$(LDFLAGS_FFMPEG)\
				   $(LDFLAGS_MQTT)\
					-Wl,--sysroot=$(STAGING_DIR)

				   
BIN 			= main
BUILD_DIR 		= ./build
BUILD_OBJ_DIR 	= $(BUILD_DIR)/obj
BUILD_BIN_DIR 	= $(BUILD_DIR)/bin

prefix 			?= /usr
bindir 			?= $(prefix)/bin

#Collect the files to compile
MAINSRC          = ./main.c \
		   $(MY_CODE)

include $(LVGL_DIR)/lvgl/lvgl.mk

CSRCS 			+=$(LVGL_DIR)/mouse_cursor_icon.c 

#CSRCS                  += $(LVGL_DIR)/lv_port_demos/mycode/1_test/my_test.c
#CSRCS 			+= ./lv_port_indev.c

OBJEXT 			?= .o

AOBJS 			= $(ASRCS:.S=$(OBJEXT))
COBJS 			= $(CSRCS:.c=$(OBJEXT))
CXXOBJS 		= $(CXXSRCS:.cpp=$(OBJEXT))

MAINOBJ 		= $(MAINSRC:.c=$(OBJEXT))

SRCS 			= $(ASRCS) $(CSRCS) $(CXXSRCS) $(MAINSRC)
OBJS 			= $(AOBJS) $(COBJS) $(CXXOBJS) $(MAINOBJ)
TARGET 			= $(addprefix $(BUILD_OBJ_DIR)/, $(patsubst ./%, %, $(OBJS)))

## MAINOBJ -> OBJFILES


all: default

$(BUILD_OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX)  $(CXXFLAGS) -c $< -o $@
	@echo "CXX $<"

$(BUILD_OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC)  $(CFLAGS) -c $< -o $@
	@echo "CC $<"

$(BUILD_OBJ_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	@$(CC)  $(CFLAGS) -c $< -o $@
	@echo "CC $<"


default: $(TARGET)
	@mkdir -p $(dir $(BUILD_BIN_DIR)/)
	$(CC) -o $(BUILD_BIN_DIR)/$(BIN) $(TARGET) $(LDFLAGS)

clean: 
	rm -rf $(BUILD_DIR)

install:
	install -d $(DESTDIR)$(bindir)
	install $(BUILD_BIN_DIR)/$(BIN) $(DESTDIR)$(bindir)

uninstall:
	$(RM) -r $(addprefix $(DESTDIR)$(bindir)/,$(BIN))
