/****************************************************************************
 * arch/risc-v/include/arch.h
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/* This file should never be included directly but, rather,
 * only indirectly through nuttx/arch.h
 */

#ifndef __ARCH_RISCV_INCLUDE_ARCH_H
#define __ARCH_RISCV_INCLUDE_ARCH_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#ifndef __ASSEMBLY__
#  include <stdint.h>
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifdef __ASSEMBLY__
#  define __STR(s)  s
#else
#  define __STR(s)  #s
#endif
#define __XSTR(s)   __STR(s)

#if defined(CONFIG_ARCH_QPFPU)
#  define FLOAD     __STR(flq)
#  define FSTORE    __STR(fsq)
#elif defined(CONFIG_ARCH_DPFPU)
#  define FLOAD     __STR(fld)
#  define FSTORE    __STR(fsd)
#else
#  define FLOAD     __STR(flw)
#  define FSTORE    __STR(fsw)
#endif

#ifdef CONFIG_ARCH_RV32
#  define REGLOAD   __STR(lw)
#  define REGSTORE  __STR(sw)
#else
#  define REGLOAD   __STR(ld)
#  define REGSTORE  __STR(sd)
#endif

/****************************************************************************
 * Inline functions
 ****************************************************************************/

#ifndef __ASSEMBLY__

/****************************************************************************
 * Name: up_getsp
 ****************************************************************************/

static inline uintptr_t up_getsp(void)
{
  register uintptr_t sp;
  __asm__
  (
    "\tadd  %0, x0, x2\n"
    : "=r"(sp)
  );
  return sp;
}

#endif

/****************************************************************************
 * Public Types
 ****************************************************************************/

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __ARCH_RISCV_INCLUDE_ARCH_H */
