/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootblock_common.h>
#include <device/pnp_ops.h>
#include <northbridge/intel/x4x/x4x.h>
#include <southbridge/intel/i82801gx/i82801gx.h>

void bootblock_mainboard_early_init(void)
{
}

void mb_get_spd_map(u8 spd_map[4])
{
	spd_map[0] = 0x50;
	spd_map[2] = 0x52;
}
