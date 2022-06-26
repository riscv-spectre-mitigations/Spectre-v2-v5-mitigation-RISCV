# Spectre-v2-v5-mitigation-RISCV

Steps:

1. Download the llvm retpoline project https://github.com/riscv-spectre-mitigations/llvm_retpoline.git.

2. Use the LLVM official site and read about how to build the project. On our side, we've used the following commands. They should be adapted in order to suit your needs. cmake -G Ninja -DCMAKE_BUILD_TYPE="Release" -DBUILD_SHARED_LIBS=True -DLLVM_USE_SPLIT_DWARF=True -DCMAKE_INSTALL_PREFIX="../../_install" -DLLVM_OPTIMIZED_TABLEGEN=True -DLLVM_BUILD_TESTS=False -DDEFAULT_SYSROOT="../../_install/riscv64-unknown-elf" -DLLVM_DEFAULT_TARGET_TRIPLE="riscv64-unknown-elf" -DLLVM_TARGETS_TO_BUILD="RISCV" -DLLVM_ENABLE_PROJECTS="clang;lld" ../llvm

3. cmake --build . --target install

4. Add the RISCV Tools folder in the install folder. The used version can be found here https://bit.ly/3meKh0H, but you need to add execution rights on the executables from bin, riscv64-unknown-elf/bin/ and libexec/gcc/riscv64-unknown-elf/9.2.0/ . Otherwise, it can be build from the sources.

5. Set an environment variable LLVM_BIN (in our case LLVM_BIN=_install/bin).

6. Use make all to obtain the executables or make run to also run them on the simulator.

7. In order to activate the retpoline mitigation, add RETPOLINE=1 as a make parameter(for Spectre v5, the mitigation will be applied for the vulnerable function frameDump).

In this repo, in the src folder, there are 3 sources ended in "Defense". These are the sources that don't need an updated compiler, they reflect the mitigation in the inline asm code.
