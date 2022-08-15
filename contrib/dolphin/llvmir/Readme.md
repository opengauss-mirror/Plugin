## 目录结构
./
├── openGauss_expr_dolphin_aarch64.ir
├── openGauss_expr_dolphin_aarch64.ll
├── openGauss_expr_dolphin.cpp
├── openGauss_expr_dolphin_x86_64.ir
├── openGauss_expr_dolphin_x86_64.ll
└── Readme.md

openGauss_expr_dolphin.cpp为对应llvm代码的CPP代码，作为输入文件。两个 ir 和 ll 文件为 CPP 文件经过clang llvm编译过后的文件，分别在x86_64以及aarch64平台上使用。如果需要修改 ir 文件的内容，先修改 openGauss_expr_dolphin.cpp 中对应的C++代码，然后利用下面的Makefile内容即可重新生成 ir 文件。注意需要在x86_64以及aarch64平台上都需要生成对应的 ir 文件。

在编译安装插件时，将根据cpu类型，选择本文件夹下对应的 ir 文件，安装到指定安装目录的 share/llvmir/ 文件夹下。内核在启动插件的场景下，将使用对应的 ir 文件生成llvm代码。（参考内核代码 src\gausskernel\runtime\codegen\gscodegen.cpp:loadIRFile）

## Makefile内容
```Makefile
top_builddir=../../../
include $(top_builddir)/src/Makefile.global

GCC_TOOLCHAIN=$(with_3rd)/$(BUILD_TOOLS_PATH)/gcc7.3/gcc

ifeq ("$(host_cpu)", "aarch64")
    arch_opt = -march=armv8-a -D__USE_NUMA
endif

CLANGFLAGS = ${CPPFLAGS}

CLANGFLAGS += -pipe -pthread -Wall -Wpointer-arith -Wno-write-strings -fnon-call-exceptions -fno-common -Wendif-labels -Wmissing-format-attribute -Wformat-security -fno-strict-aliasing -fwrapv
CLANGFLAGS += -Wno-attributes -fno-omit-frame-pointer -fsigned-char -fstrict-aliasing -fdiscard-value-names -std=c++14 $(arch_opt)  -DENABLE_GSTRACE -D_GNU_SOURCE -D_REENTRANT -D_THREAD_SAFE -D_POSIX_PTHREAD_SEMANTICS
CLANGFLAGS += -D_GLIBCXX_USE_CXX11_ABI=0 -DSTREAMPLAN -DPGXC
CLANGFLAGS += --gcc-toolchain=$(GCC_TOOLCHAIN)

source_files = openGauss_expr_dolphin.cpp
target_files = $(addsuffix _$(host_cpu).ll,$(basename $(source_files))) $(addsuffix _$(host_cpu).ir,$(basename $(source_files)))
all: $(target_files)

clean:
	rm -f $(target_files)

%_$(host_cpu).ll: %.cpp
	clang -c -O2 -emit-llvm $(CLANGFLAGS) -S -o $@ $<

%_$(host_cpu).ir: %.cpp
	clang -c -O2 -emit-llvm $(CLANGFLAGS)    -o $@ $<
```