/********************************************************************************
 * dm320/dm320_decodeirq.c
 *
 *   Copyright (C) 2007 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <spudmonkey@racsa.co.cr>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name Gregory Nutt nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ********************************************************************************/

/********************************************************************************
 * Included Files
 ********************************************************************************/

#include <nuttx/config.h>
#include <sys/types.h>
#include <nuttx/irq.h>
#include <nuttx/arch.h>
#include <assert.h>
#include <debug.h>
#include "up_arch.h"
#include "os_internal.h"
#include "up_internal.h"

/********************************************************************************
 * Definitions
 ********************************************************************************/

/********************************************************************************
 * Public Data
 ********************************************************************************/

/********************************************************************************
 * Private Data
 ********************************************************************************/

/********************************************************************************
 * Private Functions
 ********************************************************************************/

/********************************************************************************
 * Public Funtions
 ********************************************************************************/

void up_decodeirq(uint32* regs)
{
#ifdef CONFIG_SUPPRESS_INTERRUPTS
  lib_lowprintf("Unexpected IRQ\n");
  current_regs = regs;
  PANIC(OSERR_ERREXCEPTION);
#else
  /* Decode the interrupt.  First, fetch the interrupt id register. */

  uint16 irqentry = getreg16(DM320_INTC_IRQENTRY0);

  /* The irqentry value is an offset into a table.  Zero means no interrupt. */

  if (irqentry != 0)
    {
      /* If non-zero, then we can map the table offset into an IRQ number */

      int irq = (irqentry >> 2) - 1;

      /* Verify that the resulting IRQ number is valie */

      if ((unsigned)irq < NR_IRQS)
        {
          /* Mask and acknowledge the interrupt */

          up_maskack_irq(irq);

          /* Current regs non-zero indicates that we are processing an interrupt;
           * current_regs is also used to manage interrupt level context switches.
           */

          current_regs = regs;

          /* Deliver the IRQ */

          irq_dispatch(irq, regs);

          /* Indicate that we are no long in an interrupt handler */

          current_regs = NULL;

          /* Unmask the last interrupt (global interrupts are still
           * disabled.
           */

          up_enable_irq(irq);
        }
    }
#endif
}