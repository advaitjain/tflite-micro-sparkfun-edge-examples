# Copyright 2021 The TensorFlow Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================

$(shell mkdir -p downloads)
DOWNLOAD_RESULT := $(shell ./scripts/arm_gcc_download.sh)
ifneq ($(DOWNLOAD_RESULT), SUCCESS)
  $(error Something went wrong with the GCC download: $(DOWNLOAD_RESULT))
endif

TOOLCHAIN_ROOT := downloads/gcc_embedded/
TOOLCHAIN_PREFIX := $(TOOLCHAIN_ROOT)/bin/arm-none-eabi-
CXX := $(TOOLCHAIN_PREFIX)g++
CC := $(TOOLCHAIN_PREFIX)gcc
AR := $(TOOLCHAIN_PREFIX)ar
ARFLAGS := -r

GENDIR := gen
OBJDIR := $(GENDIR)/obj
BINDIR := $(GENDIR)/bin

LIBTFLM := $(GENDIR)/libtflm.a

APOLLO3_SDK := third_party/AmbiqSuite-Rel2.2.0

EXTRA_LIBS := \
  $(APOLLO3_SDK)/boards_sfe/edge/bsp/gcc/bin/libam_bsp.a \
  $(APOLLO3_SDK)/mcu/apollo3/hal/gcc/bin/libam_hal.a \
  $(TOOLCHAIN_ROOT)/lib/gcc/arm-none-eabi/10.2.1/thumb/v7e-m+fp/hard/crtbegin.o \
  -lm

# The startup_gcc.c file is an altered version of the examples/hello_world/gcc/startup_gcc.c
# file from Ambiq:
#   - Increase the stack size from 1k to 20k
#   - Change the application entry call from main() to _main()
# The am_*.c files should be copied from the Ambiq Apollo3 SDK
# _main.c contains application and target specific initialization, like
# setting clock speed, default uart setups, etc. and an implementation
# of the DebugLog interfaces.

CMSIS_DSP_DIR := third_party/cmsis/CMSIS/DSP/Source

THIRD_PARTY_CC_SRCS := \
  $(APOLLO3_SDK)/boards/apollo3_evb/examples/hello_world/gcc_patched/startup_gcc.c \
  $(APOLLO3_SDK)/utils/am_util_delay.c \
  $(APOLLO3_SDK)/utils/am_util_faultisr.c \
  $(APOLLO3_SDK)/utils/am_util_id.c \
  $(APOLLO3_SDK)/utils/am_util_stdio.c \
  $(APOLLO3_SDK)/devices/am_devices_led.c \
  $(CMSIS_DSP_DIR)/BasicMathFunctions/arm_dot_prod_q15.c \
  $(CMSIS_DSP_DIR)/BasicMathFunctions/arm_mult_q15.c \
  $(CMSIS_DSP_DIR)/TransformFunctions/arm_rfft_init_q15.c \
  $(CMSIS_DSP_DIR)/TransformFunctions/arm_rfft_q15.c \
  $(CMSIS_DSP_DIR)/TransformFunctions/arm_bitreversal2.c \
  $(CMSIS_DSP_DIR)/TransformFunctions/arm_cfft_q15.c \
  $(CMSIS_DSP_DIR)/TransformFunctions/arm_cfft_radix4_q15.c \
  $(CMSIS_DSP_DIR)/CommonTables/arm_const_structs.c \
  $(CMSIS_DSP_DIR)/CommonTables/arm_common_tables.c \
  $(CMSIS_DSP_DIR)/StatisticsFunctions/arm_mean_q15.c \
  $(CMSIS_DSP_DIR)/StatisticsFunctions/arm_max_q7.c

COMMON_FLAGS = \
  -DPART_apollo3 \
  -DAM_PACKAGE_BGA \
  -DAM_PART_APOLLO3 \
  -DGEMMLOWP_ALLOW_SLOW_SCALAR_FALLBACK \
	-DCMSIS_NN \
  -DTF_LITE_STATIC_MEMORY \
  -DNDEBUG \
  -DTF_LITE_MCU_DEBUG_LOG \
  -D __FPU_PRESENT=1 \
  -DARM_MATH_CM4 \
  -fmessage-length=0 \
  -fno-unwind-tables \
  -ffunction-sections \
  -fdata-sections \
  -funsigned-char \
  -MMD \
  -mcpu=cortex-m4 \
  -mthumb \
  -mfpu=fpv4-sp-d16 \
  -mfloat-abi=hard \
  -Wvla \
  -Wall \
  -Wextra \
  -Wno-missing-field-initializers \
  -Wno-return-type \
  -Wno-sign-compare \
  -Wno-strict-aliasing \
  -Wno-type-limits \
  -Wno-unused-function \
  -Wno-unused-parameter \
  -fno-delete-null-pointer-checks \
  -fomit-frame-pointer \
  -nostdlib \
  -ggdb \
  -O3

CXXFLAGS := \
  -std=c++11 \
  -fno-rtti \
  -fno-use-cxa-atexit \
  -fno-threadsafe-statics \
  -fno-exceptions \
  $(COMMON_FLAGS)

CCFLAGS := \
	-std=c11 \
  $(COMMON_FLAGS)

LDFLAGS += \
  -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard \
  -nostartfiles -static \
  -Wl,--gc-sections -Wl,--entry,Reset_Handler \
  -Wl,--start-group -lm -lc -lgcc -Wl,--end-group \
  -fno-exceptions \
  -nostdlib --specs=nano.specs -t -lstdc++ -lc -lnosys -lm \
  -Wl,-T,$(APOLLO3_SDK)/boards/apollo3_evb/examples/hello_world/gcc_patched/apollo3evb.ld \
  -Wl,-Map=gen/sparkfun_edge.map,--cref

INCLUDES := \
  -I. \
  -Ithird_party/cmsis/ \
  -I./third_party/cmsis/CMSIS/Core/Include/ \
  -I./third_party/cmsis/CMSIS/DSP/Include/ \
  -Ithird_party/cmsis/CMSIS/NN/Include/ \
  -I./third_party/flatbuffers/include \
  -I./third_party/gemmlowp \
  -I./third_party/kissfft \
  -I./third_party/ruy \
  -I$(TOOLCHAIN_ROOT)/arm-none-eabi/ \
  -I$(APOLLO3_SDK)/mcu/apollo3/ \
  -I$(APOLLO3_SDK)/mcu/apollo3/regs \
  -I$(APOLLO3_SDK)/mcu/apollo3/hal \
  -I$(APOLLO3_SDK)/CMSIS/ARM/Include/ \
  -I$(APOLLO3_SDK)/CMSIS/AmbiqMicro/Include/ \
  -I$(APOLLO3_SDK)/boards_sfe/edge/bsp \
  -I$(APOLLO3_SDK)/boards_sfe/common/third_party/hm01b0 \
  -I$(APOLLO3_SDK)/boards_sfe/common/third_party/lis2dh12 \
  -I$(APOLLO3_SDK)/devices/ \
  -I$(APOLLO3_SDK)/utils/

TFLM_CC_SRCS := $(shell find tensorflow -name "*.cc" -o -name "*.c")
CMSIS_NN_SRCS := $(shell find third_party/cmsis/CMSIS/NN/Source -name "*.cc" -o -name "*.c")

# Note that we are adding some of the example specific sources (such as
# himax_driver) into ALL_SRCS. This is a shortcut to reuse the rules to
# build .c files without having to explicitly list them out for the examples.
ALL_SRCS := \
	$(CMSIS_NN_SRCS) \
	$(TFLM_CC_SRCS) \
  $(THIRD_PARTY_CC_SRCS) \
  $(wildcard $(APOLLO3_SDK)/boards_sfe/common/third_party/lis2dh12/*.c) \
  $(wildcard examples/person_detection/himax_driver/*.c) \
  $(wildcard third_party/kissfft/*.c) \
  $(wildcard third_party/kissfft/*/*.c)

OBJS := $(addprefix $(OBJDIR)/, $(patsubst %.c,%.o,$(patsubst %.cc,%.o,$(ALL_SRCS))))

$(OBJDIR)/%.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) $(INCLUDES) -c $< -o $@

$(LIBTFLM): $(OBJS)
	@mkdir -p $(dir $@)
	$(AR) $(ARFLAGS) $(LIBTFLM) $(OBJS)

clean:
	rm -rf $(GENDIR)

libtflm: $(LIBTFLM)

hello_world: libtflm
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $(wildcard examples/hello_world/*.cc) $(INCLUDES) $(LIBTFLM) $(LDFLAGS) $(EXTRA_LIBS) -o $(BINDIR)/$@


MAGIC_WAND_SRCS := $(wildcard examples/magic_wand/*.cc)
magic_wand: libtflm
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $(MAGIC_WAND_SRCS) $(MAGIC_WAND_THIRD_PARTY_SRCS) $(INCLUDES) $(LIBTFLM) $(LDFLAGS) $(EXTRA_LIBS) -o $(BINDIR)/$@

MICRO_SPEECH_SRCS := $(wildcard examples/micro_speech/*.cc)
MICRO_SPEECH_SRCS += $(wildcard examples/micro_speech/micro_features/*.cc)
MICRO_SPEECH_INCLUDES := $(INCLUDES) -I./examples/micro_speech

micro_speech: libtflm
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $(MICRO_SPEECH_SRCS) $(MICRO_SPEECH_THIRD_PARTY_SRCS) $(MICRO_SPEECH_INCLUDES) $(LIBTFLM) $(LDFLAGS) $(EXTRA_LIBS) -o $(BINDIR)/$@

PERSON_DETECTION_SRCS := $(wildcard examples/person_detection/*.cc)
PERSON_DETECTION_INCLUDES := $(INCLUDES) -I./examples/person_detection

person_detection: libtflm
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $(PERSON_DETECTION_SRCS) $(PERSON_DETECTION_INCLUDES) $(LIBTFLM) $(LDFLAGS) $(EXTRA_LIBS) -o $(BINDIR)/$@

examples: hello_world magic_wand micro_speech person_detection

