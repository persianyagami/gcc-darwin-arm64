/* Target definitions for Arm64/Aarch64 running on macOS/iOS.
   Copyright (C) 2020 Free Software Foundation, Inc.
   Contributed by Iain Sandoe.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

/* Enable Mach-O bits in generic Aarch64 code.  */
#undef TARGET_MACHO
#define TARGET_MACHO 1

#undef DARWIN_ARM64
#define DARWIN_ARM64 1

/* FIXME FIXME FIXME - these are mostly guesses right now.  */

/* FIXME: this is only used in generic code in darwin.c.  */
#undef TARGET_64BIT
#define TARGET_64BIT 1

#undef  PTRDIFF_TYPE
#define PTRDIFF_TYPE "long int"

/* NOTE that arm64_32 is a valid thing and corresponds to darwinpcs
   and TARGET_ILP32, but we are not implementing that for now.  */
#define TARGET_OS_CPP_BUILTINS()		\
  do {						\
    builtin_define ("__LITTLE_ENDIAN__");	\
    builtin_define ("__arm64");			\
    builtin_define ("__arm64__");		\
    darwin_cpp_builtins (pfile);		\
  } while (0)

/* In Darwin's arm64 ABI, chars are signed, for consistency with other Darwin
   architectures.  */

#undef DEFAULT_SIGNED_CHAR
#define DEFAULT_SIGNED_CHAR 1

#undef LONG_DOUBLE_TYPE_SIZE
#define LONG_DOUBLE_TYPE_SIZE	64

/* Hack alert - we want the exported cas etc.  */
#undef LIB_SPEC
#define LIB_SPEC "%{!static:-lSystem} -lgcc"

/* Force the default endianness and ABI flags onto the command line
   in order to make the other specs easier to write.  */
#undef DRIVER_SELF_SPECS
#define DRIVER_SELF_SPECS \
"%{mbig-endian:%eDarwin platforms do not support big-endian arm64}" \
"%{!mlittle-endian:-mlittle-endian} " \
"%{mabi=ilp32:%eSorry, support for Darwin ilp32 arm64 is not implemented} " \
"%{!mabi=*:-mabi=lp64} " \
  MCPU_MTUNE_NATIVE_SPECS \
  SUBTARGET_DRIVER_SELF_SPECS

/* We want -fPIC by default, unless we're using -static to compile for
   the kernel or some such.  */

#undef CC1_SPEC
#define CC1_SPEC \
"%{!mkernel:%{!static:-fPIC}} " DARWIN_CC1_SPEC

#undef ASM_SPEC
#define ASM_SPEC "-arch %(darwin_arch) "\
  ASM_OPTIONS " %{static} " ASM_MMACOSX_VERSION_MIN_SPEC

#undef ENDFILE_SPEC
#define ENDFILE_SPEC \
  " " TM_DESTRUCTOR

/* The arch is known as 'arm64' by the system tools.  */
#define DARWIN_ARCH_SPEC "arm64"

#undef SUBTARGET_EXTRA_SPECS
#define SUBTARGET_EXTRA_SPECS					\
  DARWIN_EXTRA_SPECS						\
  { "darwin_arch", DARWIN_ARCH_SPEC },				\
  { "darwin_crt2", "" },					\
  { "darwin_subarch", DARWIN_ARCH_SPEC },

#undef TARGET_ASM_FILE_END
#define TARGET_ASM_FILE_END darwin_file_end

/* Define the syntax of pseudo-ops, labels and comments.  */

#ifdef HAVE_GAS_MAX_SKIP_P2ALIGN
/* Support for -falign-* switches.  Use .p2align to ensure that code
   sections are padded with NOP instructions, rather than zeros.  */
#define ASM_OUTPUT_MAX_SKIP_ALIGN(FILE, LOG, MAX_SKIP)		\
  do								\
    {								\
      if ((LOG) != 0)						\
	{							\
	  if ((MAX_SKIP) == 0)					\
	    fprintf ((FILE), "\t.p2align %d\n", (int) (LOG));	\
	  else							\
	    fprintf ((FILE), "\t.p2align %d,,%d\n",		\
		     (int) (LOG), (int) (MAX_SKIP));		\
	}							\
    } while (0)

#endif /* HAVE_GAS_MAX_SKIP_P2ALIGN */

/* String containing the assembler's comment-starter.  */

#define ASM_COMMENT_START ";"

/* Define the syntax of pseudo-ops, labels and comments.  */

#define LPREFIX "L"

/* Assembler pseudos to introduce constants of various size.  */

#define ASM_BYTE "\t.byte\t"
#define ASM_SHORT "\t.word\t"
#define ASM_LONG "\t.long\t"
#define ASM_QUAD "\t.quad\t"

/* darwinpcs reserves X18.  */

#undef FIXED_REGISTERS
#define FIXED_REGISTERS					\
  {							\
    0, 0, 0, 0,   0, 0, 0, 0,	/* R0 - R7 */		\
    0, 0, 0, 0,   0, 0, 0, 0,	/* R8 - R15 */		\
    0, 0, 1, 0,   0, 0, 0, 0,	/* R16 - R23 */		\
    0, 0, 0, 0,   0, 1, 0, 1,	/* R24 - R30, SP */	\
    0, 0, 0, 0,   0, 0, 0, 0,   /* V0 - V7 */           \
    0, 0, 0, 0,   0, 0, 0, 0,   /* V8 - V15 */		\
    0, 0, 0, 0,   0, 0, 0, 0,   /* V16 - V23 */         \
    0, 0, 0, 0,   0, 0, 0, 0,   /* V24 - V31 */         \
    1, 1, 1, 1,			/* SFP, AP, CC, VG */	\
    0, 0, 0, 0,   0, 0, 0, 0,   /* P0 - P7 */           \
    0, 0, 0, 0,   0, 0, 0, 0,   /* P8 - P15 */          \
    1, 1			/* FFR and FFRT */	\
  }

/* Although we cannot use executable stack, we still need to assign
   a static chain regnum.  At the moment using R16 (IP0) is available.  */
#undef STATIC_CHAIN_REGNUM
#define STATIC_CHAIN_REGNUM	R16_REGNUM

#define SUBTARGET_ENCODE_SECTION_INFO  darwin_encode_section_info

#undef ASM_MAYBE_OUTPUT_ENCODED_ADDR_RTX
#define ASM_MAYBE_OUTPUT_ENCODED_ADDR_RTX(FILE, ENCODING, SIZE, ADDR, DONE) \
  if (TARGET_64BIT)							\
    {									\
      if ((SIZE) == 4 && ((ENCODING) & 0x70) == DW_EH_PE_pcrel)		\
	{								\
	  fputs (ASM_LONG, FILE);					\
	  assemble_name (FILE, XSTR (ADDR, 0));				\
	  fputs ("@GOT-.", FILE);					\
	  goto DONE;							\
	}								\
    }									\
  else									\
    {									\
      if (ENCODING == ASM_PREFERRED_EH_DATA_FORMAT (2, 1))		\
	{								\
	  gcc_unreachable (); /* no 32b support yet.*/			\
	  /*darwin_non_lazy_pcrel (FILE, ADDR);*/			\
	  goto DONE;							\
	}								\
    }

/* Darwin x86 assemblers support the .ident directive.  */

#undef TARGET_ASM_OUTPUT_IDENT
#define TARGET_ASM_OUTPUT_IDENT default_asm_output_ident_directive

/* Pull in the stuff common to all Darwin-based platforms.  */
#define C_COMMON_OVERRIDE_OPTIONS				\
  do {								\
    SUBTARGET_C_COMMON_OVERRIDE_OPTIONS;			\
  } while (0)

#undef SUBTARGET_OVERRIDE_OPTIONS
#define SUBTARGET_OVERRIDE_OPTIONS					\
  do {									\
    if (global_options.x_aarch64_cmodel_var == AARCH64_CMODEL_TINY)	\
      error ("code model %qs is not supported on Darwin platforms",	\
	     "tiny");							\
    if (!global_options_set.x_flag_section_anchors)			\
      flag_section_anchors = 0;						\
  } while (0); 								\
  SUBSUBTARGET_OVERRIDE_OPTIONS

#undef  SUBTARGET_INIT_BUILTINS
#define SUBTARGET_INIT_BUILTINS						\
  do {									\
    aarch64_builtin_decls[AARCH64_BUILTIN_CFSTRING]			\
      = darwin_init_cfstring_builtins ((AARCH64_BUILTIN_CFSTRING << AARCH64_BUILTIN_SHIFT) | AARCH64_BUILTIN_GENERAL); \
  } while(0)

/* Darwin on Arm64 uses dwarf-2.  */
#ifndef DARWIN_PREFER_DWARF
# undef PREFERRED_DEBUGGING_TYPE
# define PREFERRED_DEBUGGING_TYPE DWARF2_DEBUG
#endif

#undef REGISTER_SUBTARGET_PRAGMAS
#define REGISTER_SUBTARGET_PRAGMAS() DARWIN_REGISTER_TARGET_PRAGMAS()

#undef TARGET_SET_DEFAULT_TYPE_ATTRIBUTES
#define TARGET_SET_DEFAULT_TYPE_ATTRIBUTES darwin_set_default_type_attributes

/* FIXME:  CHECK Define the shadow offset for asan.  */
#undef SUBTARGET_SHADOW_OFFSET
#define SUBTARGET_SHADOW_OFFSET (HOST_WIDE_INT_1 << 44)

/* First available SYMBOL flag bit for use by subtargets.  */
#define SYMBOL_FLAG_SUBT_DEP (SYMBOL_FLAG_MACH_DEP)

#undef ASM_OUTPUT_DEF_FROM_DECLS

#undef CLEAR_INSN_CACHE
#define CLEAR_INSN_CACHE(beg, end)				\
  extern void sys_icache_invalidate(void *start, size_t len);	\
  sys_icache_invalidate ((beg), (size_t)((end)-(beg)))

