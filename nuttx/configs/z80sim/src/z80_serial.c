/****************************************************************************
 * board/z80_serial.c
 *
 *   Copyright (C) 2007, 2008 Gregory Nutt. All rights reserved.
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
 * 3. Neither the name NuttX nor the names of its contributors may be
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
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/irq.h>
#include <nuttx/arch.h>
#include <nuttx/serial.h>
#include <arch/serial.h>

#include "up_arch.h"
#include "os_internal.h"
#include "up_internal.h"

#if CONFIG_NFILE_DESCRIPTORS > 0

/****************************************************************************
 * Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int     up_setup(struct uart_dev_s *dev);
static void    up_shutdown(struct uart_dev_s *dev);
static int     up_interrupt(int irq, void *context);
static int     up_ioctl(struct file *filep, int cmd, unsigned long arg);
static int     up_receive(struct uart_dev_s *dev, uint32 *status);
static void    up_rxint(struct uart_dev_s *dev, boolean enable);
static boolean up_rxfifonotempty(struct uart_dev_s *dev);
static void    up_send(struct uart_dev_s *dev, int ch);
static void    up_txint(struct uart_dev_s *dev, boolean enable);
static boolean up_txfifonotfull(struct uart_dev_s *dev);
static boolean up_txfifoempty(struct uart_dev_s *dev);

/****************************************************************************
 * Private Variables
 ****************************************************************************/

struct uart_ops_s g_uart_ops =
{
  up_setup,                 /* setup */
  up_shutdown,              /* shutdown */
  up_interrupt,             /* handler */
  up_ioctl,                 /* ioctl */
  up_receive,               /* receive */
  up_rxint,                 /* rxint */
  up_rxfifonotempty,        /* rxfifonotempty */
  up_send,                  /* send */
  up_txint,                 /* txint */
  up_txfifonotfull,         /* txfifonotfull */
  up_txfifoempty,           /* txfifoempty */
};

/* I/O buffers */

static char g_uartrxbuffer[CONFIG_UART_RXBUFSIZE];
static char g_uarttxbuffer[CONFIG_UART_TXBUFSIZE];

/* This describes the state of the fake UART port. */

static uart_dev_t g_uartport =
{
  0,                        /* open_count */
  0,                        /* irq */
  FALSE,                    /* xmitwaiting */
  FALSE,                    /* recvwaiting */
  TRUE,                     /* isconsole */
  { 1 },                    /* closesem */
  { 0 },                    /* xmitsem */
  { 0 },                    /* recvsem */
  {                         /* xmit */
    { 1 },                  /*   sem */
    0,                      /*   head */
    0,                      /*   tail */
    CONFIG_UART_TXBUFSIZE,  /*   size */
    g_uarttxbuffer,         /*   buffer */
  },
  {                         /* recv */
    { 1 },                  /*   sem */
    0,                      /*   head */
    0,                      /*   tail */
    CONFIG_UART_RXBUFSIZE,  /*   size */
    g_uartrxbuffer,         /*   buffer */
  },
  &g_uart_ops,              /* ops */
  NULL,                     /* priv */
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: up_setup
 *
 * Description:
 *   Configure the UART baud, bits, parity, fifos, etc. This
 *   method is called the first time that the serial port is
 *   opened.
 *
 ****************************************************************************/

static int up_setup(struct uart_dev_s *dev)
{
  return OK;
}

/****************************************************************************
 * Name: up_shutdown
 *
 * Description:
 *   Disable the UART.  This method is called when the serial
 *   port is closed
 *
 ****************************************************************************/

static void up_shutdown(struct uart_dev_s *dev)
{
}

/****************************************************************************
 * Name: up_interrupt
 *
 * Description:
 *   This is the UART interrupt handler.  It will be invoked
 *   when an interrupt received on the 'irq'  It should call
 *   uart_transmitchars or uart_receivechar to perform the
 *   appropriate data transfers.  The interrupt handling logic\
 *   must be able to map the 'irq' number into the approprite
 *   uart_dev_s structure in order to call these functions.
 *
 ****************************************************************************/

static int up_interrupt(int irq, void *context)
{
  return OK;
}

/****************************************************************************
 * Name: up_ioctl
 *
 * Description:
 *   All ioctl calls will be routed through this method
 *
 ****************************************************************************/

static int up_ioctl(struct file *filep, int cmd, unsigned long arg)
{
  *get_errno_ptr() = ENOTTY;
  return ERROR;
}

/****************************************************************************
 * Name: up_receive
 *
 * Description:
 *   Called (usually) from the interrupt level to receive one
 *   character from the UART.  Error bits associated with the
 *   receipt are provided in the the return 'status'.
 *
 ****************************************************************************/

static int up_receive(struct uart_dev_s *dev, uint32 *status)
{
  uint8 ch = up_lowgetc();
  *status = 0;
  return ch;
}

/****************************************************************************
 * Name: up_rxint
 *
 * Description:
 *   Call to enable or disable RX interrupts
 *
 ****************************************************************************/

static void up_rxint(struct uart_dev_s *dev, boolean enable)
{
}

/****************************************************************************
 * Name: up_rxfifonotempty
 *
 * Description:
 *   Return TRUE if the receive fifo is not empty
 *
 ****************************************************************************/

static boolean up_rxfifonotempty(struct uart_dev_s *dev)
{
  return TRUE;
}

/****************************************************************************
 * Name: up_send
 *
 * Description:
 *   This method will send one byte on the UART
 *
 ****************************************************************************/

static void up_send(struct uart_dev_s *dev, int ch)
{
  up_lowputc(ch);
}

/****************************************************************************
 * Name: up_txint
 *
 * Description:
 *   Call to enable or disable TX interrupts
 *
 ****************************************************************************/

static void up_txint(struct uart_dev_s *dev, boolean enable)
{
}

/****************************************************************************
 * Name: up_txfifonotfull
 *
 * Description:
 *   Return TRUE if the tranmsit fifo is not full
 *
 ****************************************************************************/

static boolean up_txfifonotfull(struct uart_dev_s *dev)
{
  return TRUE;
}

/****************************************************************************
 * Name: up_txfifoempty
 *
 * Description:
 *   Return TRUE if the transmit fifo is empty
 *
 ****************************************************************************/

static boolean up_txfifoempty(struct uart_dev_s *dev)
{
  return TRUE;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: up_serialinit
 *
 * Description:
 *   Performs the low level UART initialization early in 
 *   debug so that the serial console will be available
 *   during bootup.  This must be called before up_serialinit.
 *
 ****************************************************************************/

void up_earlyserialinit(void)
{
}

/****************************************************************************
 * Name: up_serialinit
 *
 * Description:
 *   Register serial console and serial ports.  This assumes
 *   that up_earlyserialinit was called previously.
 *
 ****************************************************************************/

void up_serialinit(void)
{
  (void)uart_register("/dev/console", &g_uartport);
  (void)uart_register("/dev/ttyS0", &g_uartport);
}
#endif /* CONFIG_NFILE_DESCRIPTORS > 0 */

/****************************************************************************
 * Name: up_putc
 *
 * Description:
 *   Provide priority, low-level access to support OS debug
 *   writes
 *
 ****************************************************************************/

int up_putc(int ch)
{
  up_lowputc(ch);
  return 0;
}



