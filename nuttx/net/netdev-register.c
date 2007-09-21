/****************************************************************************
 * net/netdev-register.c
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
#if defined(CONFIG_NET) && CONFIG_NSOCKET_DESCRIPTORS > 0

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <semaphore.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#include <net/uip/uip-arch.h>

#include "net-internal.h"

/****************************************************************************
 * Definitions
 ****************************************************************************/

/****************************************************************************
 * Priviate Types
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

static int g_next_devnum = 0;

/****************************************************************************
 * Public Data
 ****************************************************************************/

/* List of registered ethernet device drivers */
struct uip_driver_s *g_netdevices = NULL;
sem_t                g_netdev_sem;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Global Functions
 ****************************************************************************/

/****************************************************************************
 * Function: netdev_semtake
 *
 * Description:
 *   Managed access to the network device list
 *
 ****************************************************************************/

void netdev_semtake(void)
{
  /* Take the semaphore (perhaps waiting) */

  while (sem_wait(&g_netdev_sem) != 0)
    {
      /* The only case that an error should occr here is if
       * the wait was awakened by a signal.
       */

      ASSERT(*get_errno_ptr() == EINTR);
    }
}

/****************************************************************************
 * Function: netdev_register
 *
 * Description:
 *   Register a netword device driver and assign a name to it so tht it can
 *   be found in subsequent network ioctl operations on the device.
 *
 * Parameters:
 *   dev - The device driver structure to register
 *
 * Returned Value:
 *  0:Success; -1 on failure
 *
 * Assumptions:
 *  Called during system initialization from normal user mode
 *
 ****************************************************************************/

int netdev_register(FAR struct uip_driver_s *dev)
{
  if (dev)
    {
      int devnum;
      netdev_semtake();

      /* Assign a device name to the the interface */

      devnum = g_next_devnum++;
      snprintf( dev->d_ifname, IFNAMSIZ, "eth%d", devnum );

      /* Add the device to the list of known network devices */

      dev->flink  = g_netdevices;
      g_netdevices = dev;
      netdev_semgive();
      return OK;
    }
  return ERROR;
}

#endif /* CONFIG_NET && CONFIG_NSOCKET_DESCRIPTORS */