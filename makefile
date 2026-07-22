
override MAKEFLAGS += -rR

ARCH := x86
override BIN := bin
override SRC := src
override KERNEL := $(SRC)/kernel
override LIBK := $(SRC)/libk
override OUTPUT := junix

override CFILES := $(shell find $(KERNEL)/ -type f -name '*.c' | sed 's|^$(SRC)/||' | LC_ALL=C sort)
override CFILES += $(shell find $(LIBK)/ -type f -name '*.c' | sed 's|^$(SRC)/||' | LC_ALL=C sort)
override CFILES += $(shell find $(SRC)/arch/$(ARCH) -type f -name '*.c' | sed 's|^$(SRC)/||' | LC_ALL=C sort)
override ASFILES := $(shell find $(KERNEL)/ -type f -name '*.s' | sed 's|^$(SRC)/||' | LC_ALL=C sort)
override ASFILES += $(shell find $(SRC)/arch/$(ARCH) -type f -name '*.s' | sed 's|^$(SRC)/||' | LC_ALL=C sort)

override KCC := clang
override KCPPC := clang++
override KAS := clang
override KLD := ld.lld
override KCFLAGS := -pipe -Wall -Wextra -c -O0 -Wpedantic -Werror -Wno-unused-variable -Wno-unused-but-set-variable -g
override KCFLAGS += 
override ASFLAGS += 
override KLDFLAGS :=

override ARCH_CFLAGS :=
override ARCH_ASFLAGS :=
override ARCH_KLDFLAGS :=
override ARCH_LINKER_SCRIPT := $(SRC)/$(ARCH)/linker.ld
override ARCH_OUTPUT_SUFFIX :=
override ARCH_RUN_SCRIPT :=

include $(SRC)/arch/$(ARCH)/flags.mk

override KCFLAGS += \
    -c \
    -ffreestanding \
    -fno-builtin \
    $(ARCH_CFLAGS) \
    #-Werror

override ASFLAGS += \
    -c \
    $(ARCH_ASFLAGS)

override KLDFLAGS += \
    -nostdlib \
    -z max-page-size=0x1000 \
    $(ARCH_KLDFLAGS)

override OUTPUT := $(OUTPUT)$(ARCH_OUTPUT_SUFFIX)
override COBJ := $(addprefix obj/boot/,$(CFILES:.c=.c.o))
override ASOBJ := $(addprefix obj/boot/,$(ASFILES:.s=.s.o))

OBJ := $(ASOBJ) $(COBJ)

override HEADER_DEPS := $(addprefix obj/boot/,$(CFILES:.c=.c.d))
override HEADER_DEPS += $(addprefix obj/boot/,$(CFILES:.C=.C.d))

.PHONY: all
all: bin/boot/$(OUTPUT)

bin/boot/$(OUTPUT): $(OBJ)
	mkdir -p "$$(dirname $@)"
	$(KLD) $(OBJ) $(KLDFLAGS) -o $@ -T $(ARCH_LINKER_SCRIPT) 
	@echo " [INFO] Built $(OUTPUT) for architecture $(ARCH)"

-include $(HEADER_DEPS)

.PHONY: buildnum jwm libc
buildnum:
	@if [ ! -f buildnum ]; then echo 0 > buildnum; fi
	@echo $$(($$(cat buildnum) + 1)) > buildnum
	@echo "#define JUNIX_BUILD $$(cat buildnum)" > src/kernel/version.h

libc:
	mkdir -p "bin/bin"
	$(KCC) $(KCFLAGS) $(KCPPFLAGS) -c $(SRC)/libc/stdio.c -o $(BIN)/bin/libc.o -I $(SRC)/libc

jwm: libc
	mkdir -p "bin/bin"
	$(KCPPC) $(KCFLAGS) $(KCPPFLAGS) -c $(SRC)/jwm/main.cpp -o $(BIN)/bin/jwm.to -I $(SRC)/libc -I $(SRC)/jwm  -nostdlib -fno-exceptions -fno-rtti
	$(KLD) $(BIN)/bin/jwm.to $(BIN)/bin/libc.o -o $(BIN)/bin/jwm.o -nostdlib $(KLDFLAGS)
	rm $(BIN)/bin/jwm.to

# Kernel shit

obj/boot/%.c.o: $(SRC)/%.c
	mkdir -p "$$(dirname $@)"
	$(KCC) $(KCFLAGS) $(KCPPFLAGS) -c $< -o $@ -I $(KERNEL) -I $(SRC)/arch/$(ARCH) -I $(LIBK)

obj/boot/%.s.o: $(SRC)/%.s
	mkdir -p "$$(dirname $@)"
	$(KAS) $(ASFLAGS) -c $< -o $@

obj/boot/%.s.o: $(SRC)/%.s
	mkdir -p "$$(dirname $@)"
	$(KAS) $(ASFLAGS) -c $< -o $@

include $(SRC)/arch/$(ARCH)/addendum.mk

.PHONY: clean
clean:
	@rm -rf bin obj

.PHONY: run
run:
	@bash $(ARCH_RUN_SCRIPT)
