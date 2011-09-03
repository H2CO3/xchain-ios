/* APPLE LOCAL begin mainline 2005-04-06 4071679 */
/* Darwin host-specific hook definitions.
   Copyright (C) 2003, 2004, 2005 Free Software Foundation, Inc.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 2, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING.  If not, write to the
   Free Software Foundation, 59 Temple Place - Suite 330, Boston,
   MA 02111-1307, USA.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include <sys/mman.h>
#include "toplev.h"
#include "config/host-darwin.h"

/* Yes, this is really supposed to work.  */
/* APPLE LOCAL begin ARM native compiler support */
/* Not all Darwins are created equal.  */
static char pch_address_space[DARWIN_PCH_ADDR_SPACE_SIZE]
  __attribute__((aligned (4096)));
/* APPLE LOCAL end ARM native compiler support */

/* Return the address of the PCH address space, if the PCH will fit in it.  */

void *
darwin_gt_pch_get_address (size_t sz, int fd ATTRIBUTE_UNUSED)
{
  if (sz <= sizeof (pch_address_space))
    return pch_address_space;
  else
    return NULL;
}

/* Check ADDR and SZ for validity, and deallocate (using munmap) that part of
   pch_address_space beyond SZ.  */

int
darwin_gt_pch_use_address (void *addr, size_t sz, int fd, size_t off)
{
  const size_t pagesize = getpagesize();
  void *mmap_result;
  int ret;

  if ((size_t)pch_address_space % pagesize != 0
      || sizeof (pch_address_space) % pagesize != 0)
    abort ();
  
  ret = (addr == pch_address_space && sz <= sizeof (pch_address_space));
  if (! ret)
    sz = 0;

  /* Round the size to a whole page size.  Normally this is a no-op.  */
  sz = (sz + pagesize - 1) / pagesize * pagesize;

  if (munmap (pch_address_space + sz, sizeof (pch_address_space) - sz) != 0)
    fatal_error ("couldn't unmap pch_address_space: %m\n");

  if (ret)
    {
      mmap_result = mmap (addr, sz,
			  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_FIXED,
			  fd, off);

      /* The file might not be mmap-able.  */
      ret = mmap_result != (void *) MAP_FAILED;

      /* Sanity check for broken MAP_FIXED.  */
      if (ret && mmap_result != addr)
	abort ();
    }

  return ret;
}
/* APPLE LOCAL end mainline 2005-04-06 4071679 */