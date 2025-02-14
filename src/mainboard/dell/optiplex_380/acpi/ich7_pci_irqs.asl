/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This is board specific information:
 * IRQ routing for the 0:1e.0 PCI bridge of the ICH7
 * Temporarily taken from ./src/mainboard/intel/dg41wv/acpi/ich7_pci_irqs.asl
 */

If (PICM) {
	Return (Package() {
		/* API4 */
		Package (){ 0x0001FFFF, 0, 0, 0x11 }, 
		Package (){ 0x0001FFFF, 1, 0, 0x12 }, 
		Package (){ 0x0001FFFF, 2, 0, 0x13 }, 
		Package (){ 0x0001FFFF, 3, 0, 0x10 }, 

		/* Package (){0xFFFF, 0, 0, 0x10}, 
		Package (){0xFFFF, 1, 0, 0x11}, 
		Package (){0xFFFF, 2, 0, 0x12}, 
		Package (){0xFFFF, 3, 0, 0x13},  */

		Package (){ 0x0002FFFF, 0, 0, 0x12 }, 
		Package (){ 0x0002FFFF, 1, 0, 0x13 }, 
		Package (){ 0x0002FFFF, 2, 0, 0x10 }, 
		Package (){ 0x0002FFFF, 3, 0, 0x11 }
	})
} Else {
	Return (Package() {
		/* PIC4 */
		Package (){ 0x0001FFFF, 0, \_SB.PCI0.LPCB.LNKB, 0 },
		Package (){ 0x0001FFFF, 1, \_SB.PCI0.LPCB.LNKC, 0 },
		Package (){ 0x0001FFFF, 2, \_SB.PCI0.LPCB.LNKD, 0 }, 
		Package (){ 0x0001FFFF, 3, \_SB.PCI0.LPCB.LNKA, 0 }, 

		/* Package (){0xFFFF, 0, \_SB.PCI0.LPCB.LNKA, 0}, 
		Package (){0xFFFF, 1, \_SB.PCI0.LPCB.LNKB, 0}, 
		Package (){0xFFFF, 2, \_SB.PCI0.LPCB.LNKC, 0}, 
		Package (){0xFFFF, 3, \_SB.PCI0.LPCB.LNKD, 0}, */

		Package (){ 0x0002FFFF, 0, \_SB.PCI0.LPCB.LNKC, 0 }, 
		Package (){ 0x0002FFFF, 1, \_SB.PCI0.LPCB.LNKD, 0 }, 
		Package (){ 0x0002FFFF, 2, \_SB.PCI0.LPCB.LNKA, 0 }, 
		Package (){ 0x0002FFFF, 3, \_SB.PCI0.LPCB.LNKB, 0 }

	})
}
