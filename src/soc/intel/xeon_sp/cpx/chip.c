/* SPDX-License-Identifier: GPL-2.0-only */
#include <acpi/acpigen_pci.h>
#include <arch/ioapic.h>
#include <console/console.h>
#include <console/debug.h>
#include <cpu/x86/mp.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <gpio.h>
#include <intelblocks/acpi.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/p2sb.h>
#include <soc/acpi.h>
#include <soc/chip_common.h>
#include <soc/numa.h>
#include <soc/pch.h>
#include <soc/soc_pch.h>
#include <soc/ramstage.h>
#include <soc/p2sb.h>
#include <soc/soc_util.h>
#include <soc/util.h>
#include <soc/pci_devs.h>

/* UPD parameters to be initialized before SiliconInit */
void platform_fsp_silicon_init_params_cb(FSPS_UPD *silupd)
{
	mainboard_silicon_init_params(silupd);
}

static struct device_operations cpu_bus_ops = {
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources,
	.init = mp_cpu_bus_init,
	.acpi_fill_ssdt = generate_cpu_entries,
};

struct pci_operations soc_pci_ops = {
	.set_subsystem = pci_dev_set_subsystem,
};

static void chip_enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		/* domain ops are assigned at their creation */
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	} else if (dev->path.type == DEVICE_PATH_GPIO) {
		block_gpio_enable(dev);
	}
}

static void iio_write_mask(u16 bus, u16 dev, u8 func)
{
	pci_devfn_t device = PCI_DEV(bus, dev, func);
	u32 val = pci_s_read_config32(device, IIO_XPUNCCERRMSK_REG);
	val |= (SENT_PCIE_UNSUPP_MASK | RCVD_PCIE_CA_STS_MASK | RCVD_PCIE_UR_STS_MASK);
	pci_s_write_config32(device, IIO_XPUNCCERRMSK_REG, val);

	val = pci_s_read_config32(device, RP_UNCERRMSK);
	val |= (SURPRISE_DWN_ERR_MSK | UNSUPPORTED_REQ_ERR_MSK);
	pci_s_write_config32(device, RP_UNCERRMSK, val);
}

static void iio_dmi_en_masks(void)
{
	pci_devfn_t device;
	u32 val;
	device = PCI_DEV(DMI_BUS_INDEX, DMI_DEV, DMI_FUNC);
	val = pci_s_read_config32(device, IIO_XPUNCCERRMSK_REG);
	val |= (SENT_PCIE_UNSUPP_MASK | RCVD_PCIE_CA_STS_MASK | RCVD_PCIE_UR_STS_MASK);
	pci_s_write_config32(device, IIO_XPUNCCERRMSK_REG, val);

	val = pci_s_read_config32(device, DMI_UNCERRMSK);
	val |= (ECRC_ERR | MLFRMD_TLP | RCV_BUF_OVRFLOW | FLOW_CNTR | POISON_TLP | DLL_PRT_ERR);
	pci_s_write_config32(device, DMI_UNCERRMSK, val);
}

static void iio_enable_masks(void)
{
	struct iiostack_resource iio = {0};
	get_iiostack_info(&iio);
	int i, k;
	for (i = 0; i < iio.no_of_stacks; i++) {
		const STACK_RES *st = &iio.res[i];
		if (st->BusBase > 0 && st->BusBase != 0xff) {
			for (k = 0; k < DEVICES_PER_IIO_STACK; k++) {
				printk(BIOS_DEBUG, "%s: bus:%x dev:%x func:%x\n", __func__,
					st->BusBase, k, 0);
				iio_write_mask(st->BusBase, k, 0);
			}
		}
	}
	iio_dmi_en_masks();
}

static void set_imc_locks(void)
{
	struct device *dev = 0;
	while ((dev = dev_find_device(PCI_VID_INTEL, IMC_M2MEM_DEVID, dev)))
		pci_or_config32(dev, IMC_M2MEM_TIMEOUT, TIMEOUT_LOCK);
}

static void set_upi_locks(void)
{
	struct device *dev = 0;
	while ((dev = dev_find_device(PCI_VID_INTEL, UPI_LL_CR_DEVID, dev)))
		pci_or_config32(dev, UPI_LL_CR_KTIMISCMODLCK, KTIMISCMODLCK_LOCK);
}

static void chip_final(void *data)
{
	/* Lock SBI */
	pci_or_config32(PCH_DEV_P2SB, P2SBC, SBILOCK);

	/* LOCK PAM */
	pci_or_config32(pcidev_path_on_root(PCI_DEVFN(0, 0)), 0x80, 1 << 0);

	set_imc_locks();
	set_upi_locks();

	p2sb_hide();
	iio_enable_masks();
}

static void chip_init(void *data)
{
	printk(BIOS_DEBUG, "coreboot: calling fsp_silicon_init\n");
	fsp_silicon_init();

	setup_pds();
	attach_iio_stacks();

	override_hpet_ioapic_bdf();
	pch_enable_ioapic();
	pch_lock_dmictl();
	p2sb_unhide();
}

struct chip_operations soc_intel_xeon_sp_cpx_ops = {
	.name = "Intel Cooper Lake-SP",
	.enable_dev = chip_enable_dev,
	.init = chip_init,
	.final = chip_final,
};
