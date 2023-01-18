cmake_minimum_required(VERSION 3.17)
project(VM VERSION 0.1 LANGUAGES CXX)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)
target_compile_options(project_options INTERFACE
	-fcheck-new -fsized-deallocation -fstack-check -fstack-protector
	-fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer
	-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,leak,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr)

target_link_options(project_options INTERFACE
	-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,leak,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr)

add_library(project_warnings INTERFACE)
target_compile_options(project_warnings INTERFACE
	-Wall
	-Wextra -Weffc++ -Waggressive-loop-optimizations
	-Wc++14-compat -Wmissing-declarations -Wcast-align
	-Wcast-qual -Wchar-subscripts -Wconditionally-supported
	-Wconversion -Wctor-dtor-privacy -Wempty-body
	-Wfloat-equal -Wformat-nonliteral -Wformat-security
	-Wformat-signedness -Wformat=2 -Winline -Wlogical-op
	-Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual
	-Wpacked -Wpointer-arith -Winit-self -Wredundant-decls
	-Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel
	-Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods
	-Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum
	-Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast
	-Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers
	-Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector)

add_subdirectory(vm)

add_subdirectory(asm)

add_subdirectory(tests)
