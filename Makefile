

CC = $(LLVM_BIN)/clang
COMMONCC = $(LLVM_BIN)/riscv64-unknown-elf-gcc

CFLAGS = -march=rv64imafdc -mabi=lp64d -mcmodel=medany -Iinc -I$(LLVM_BIN)/../riscv64-unknown-elf/include
LDFLAGS = -T link/link.ld -static -nostdlib -nostartfiles -lgcc
COMMONFLAGS = -mcmodel=medany -l -std=gnu99 -O0 -g -fno-common -fno-builtin-printf -Wall -Iinc -Wno-unused-function -Wno-unused-variable

PROGRAMS=indirectBranchFunction indirectBranchSwitch returnStackBuffer
BINS=$(addprefix bin/,$(addsuffix .riscv,$(PROGRAMS)))


ifeq ($(RETPOLINE),1)
	RETPOLINE_FLAG=-mretpoline
	RSB_PROTECTED_FLAG="-mrsb-protect=frameDump"
else
	RETPOLINE_FLAG=
	RBS_PROTECTED_FLAG=
endif

obj/%.o: src/%.c
	@mkdir -p obj
	$(CC) $(CFLAGS) $(RETPOLINE_FLAG) $(RSB_PROTECTED_FLAG) -c $< -o $@

obj/%.o: src/%.s
	@mkdir -p obj
	$(CC) $(CFLAGS) $(RETPOLINE_FLAG) $(RSB_PROTECTED_FLAG) -c $< -o $@

obj/%.o: common/%.S
	@mkdir -p obj
	$(COMMONCC) $(COMMONFLAGS) -D__ASSEMBLY__=1 -c $< -o $@

obj/%.o: common/%.c
	@mkdir -p obj
	$(COMMONCC) $(COMMONFLAGS) -c $< -o $@
	
	
bin/indirectBranchFunction.riscv: obj/crt.o obj/syscalls.o obj/functions.o obj/indirectBranchFunction.o 
	@mkdir -p bin
	$(CC) $(LDFLAGS) obj/crt.o obj/syscalls.o obj/functions.o obj/indirectBranchFunction.o  -o $@
	
bin/indirectBranchSwitch.riscv: obj/crt.o obj/syscalls.o obj/gadgets.o obj/indirectBranchSwitch.o 
	@mkdir -p bin
	$(CC) $(LDFLAGS) obj/crt.o obj/syscalls.o obj/gadgets.o obj/indirectBranchSwitch.o  -o $@

bin/returnStackBuffer.riscv: obj/crt.o obj/syscalls.o obj/rsb.o obj/returnStackBuffer.o 
	@mkdir -p bin
	$(CC) $(LDFLAGS) obj/crt.o obj/syscalls.o obj/rsb.o obj/returnStackBuffer.o  -o $@
	
	
	
all: $(BINS)


run: $(BINS)
	./simulator-chipyard-SmallBoomConfig bin/indirectBranchFunction.riscv
	./simulator-chipyard-SmallBoomConfig bin/indirectBranchSwitch.riscv
	./simulator-chipyard-SmallBoomConfig bin/returnStackBuffer.riscv
clean:
	rm -rf bin obj
