/*
 * IXP4XX EHCI Host Controller Driver
 *
 * Author: Vladimir Barinov <vbarinov@embeddedalley.com>
 *
 * Based on "ehci-fsl.c" by Randy Vinson <rvinson@mvista.com>
 *
 * 2007 (c) MontaVista Software, Inc. This file is licensed under
 * the terms of the GNU General Public License version 2. This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */

#include <linux/platform_device.h>
static const char hcd_name[] = "g2-ehci";
#define software_override_sysopmode_en 0x80000000;
#define software_override_sysopmode 0x40000000;
#define Hst_ss_ena_incr4 0x2000000;
#define Hst_ss_ena_incr8 0x4000000;
#define Hst_ss_ena_incr16 0x8000000;
#define usb_por           0x20000000;

static int g2_ehci_init(struct usb_hcd *hcd)
{
	struct ehci_hcd *ehci = hcd_to_ehci(hcd);
	int retval = 0;

	ehci->big_endian_desc = 1;
	ehci->big_endian_mmio = 1;

	ehci->caps = hcd->regs;
	ehci->regs = hcd->regs + HC_LENGTH(ehci_readl(ehci, &ehci->caps->hc_capbase));
	ehci->hcs_params = ehci_readl(ehci, &ehci->caps->hcs_params);

	hcd->has_tt = 1;
	ehci_reset(ehci);

	retval = ehci_init(hcd);
	if (retval)
		return retval;

	ehci_port_power(ehci, 1);

	return retval;
}

static const struct hc_driver g2_ehci_hc_driver = {
	.description		= hcd_name,
	.product_desc		= "Golden Gate EHCI Host Controller",
	.hcd_priv_size		= sizeof(struct ehci_hcd),
	.irq			= ehci_irq,
	.flags			= HCD_MEMORY | HCD_USB2,
	.reset			= g2_ehci_init,
	.start			= ehci_run,
	.stop			= ehci_stop,
	.shutdown		= ehci_shutdown,
	.urb_enqueue		= ehci_urb_enqueue,
	.urb_dequeue		= ehci_urb_dequeue,
	.endpoint_disable	= ehci_endpoint_disable,
	.endpoint_reset		= ehci_endpoint_reset,
	.get_frame_number	= ehci_get_frame,
	.hub_status_data	= ehci_hub_status_data,
	.hub_control		= ehci_hub_control,
#if defined(CONFIG_PM)
	.bus_suspend		= ehci_bus_suspend,
	.bus_resume		= ehci_bus_resume,
#endif
	.relinquish_port	= ehci_relinquish_port,
	.port_handed_over	= ehci_port_handed_over,

	.clear_tt_buffer_complete	= ehci_clear_tt_buffer_complete,
};

static int g2_ehci_probe(struct platform_device *pdev)
{
	struct usb_hcd *hcd;
	const struct hc_driver *driver = &g2_ehci_hc_driver;
	struct resource *res;
	int irq;
	int retval;
	int temp;

	if (usb_disabled())
		return -ENODEV;

#ifndef CONFIG_USB_GADGET_SNPS_DWC_OTG
	//Add for G2 Global register init
	//======================================
	//1. Setting the port1 in host mode before POR
	temp = readl(GLOBAL_GLOBAL_USB_REG0);
	temp |= software_override_sysopmode_en;
	temp |= software_override_sysopmode;
	writel(temp,GLOBAL_GLOBAL_USB_REG0);

#ifndef CONFIG_CORTINA_FPGA
	//2. De-assert USB PHY reset
	temp = readl(GLOBAL_PHY_CONTROL);
	temp &= ~(usb_phy1_por | usb_phy0_por);
	writel(temp,GLOBAL_PHY_CONTROL);
#endif

	//3. Wait for USB PHY Clocks come up
	udelay(1000);
    printk("%s-host: wait for USB PHY Clocks come up \n",__func__);


	//5. De-assert USB Controller POR
	//Stone add for USB Power on reset
    writel(0x0,GLOBAL_GLOBAL_USB_REG1);


    //4. Set the USB Host Strap pins before de-asserting the USB Host Reset
    temp = readl(GLOBAL_GLOBAL_USB_REG0);
    //For 2 host test
    //temp |= 0xce000000;
    temp |= 0x0e000000;
  	writel(temp,GLOBAL_GLOBAL_USB_REG0);
#else
    //Add for G2 Global register init
	//======================================
	//1. Setting the OTG in Device mode before POR
	temp = readl(GLOBAL_GLOBAL_USB_REG0);
	temp |= software_override_sysopmode_en;
	temp &= ~software_override_sysopmode;
	writel(temp,GLOBAL_GLOBAL_USB_REG0);

#ifndef CONFIG_CORTINA_FPGA
	//2. De-assert USB PHY reset
	temp = readl(GLOBAL_PHY_CONTROL);
	temp &= ~(usb_phy1_por | usb_phy0_por);
	writel(temp,GLOBAL_PHY_CONTROL);
#endif

	//3. Wait for USB PHY Clocks come up
	udelay(1000);
	printk("%s-gadget: wait for USB PHY Clocks come up \n",__func__);

	//4. Set the USB Host Strap pins before de-asserting the USB Host Reset
	temp = readl(GLOBAL_GLOBAL_USB_REG0);
	temp |= Hst_ss_ena_incr4;
    temp |= Hst_ss_ena_incr8;
    temp |= Hst_ss_ena_incr16;
    writel(temp,GLOBAL_GLOBAL_USB_REG0);

	//5. De-assert USB Controller POR
	temp = readl(GLOBAL_GLOBAL_USB_REG1);
	temp &= ~usb_por;
	writel(temp,GLOBAL_GLOBAL_USB_REG1);
	printk("%s: G2 driver init global register !!!! \n",__func__);
	//======================================
#endif

	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res) {
		dev_err(&pdev->dev,
			"Found HC with no IRQ. Check %s setup!\n",
			dev_name(&pdev->dev));
		return -ENODEV;
	}
	irq = res->start;

	hcd = usb_create_hcd(driver, &pdev->dev, dev_name(&pdev->dev));
	if (!hcd) {
		retval = -ENOMEM;
		goto fail_create_hcd;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev,
			"Found HC with no register addr. Check %s setup!\n",
			dev_name(&pdev->dev));
		retval = -ENODEV;
		goto fail_request_resource;
	}
	hcd->rsrc_start = res->start;
	hcd->rsrc_len = res->end - res->start + 1;

	if (!request_mem_region(hcd->rsrc_start, hcd->rsrc_len,
				driver->description)) {
		dev_dbg(&pdev->dev, "controller already in use\n");
		retval = -EBUSY;
		goto fail_request_resource;
	}

	hcd->regs = ioremap_nocache(hcd->rsrc_start, hcd->rsrc_len);
	printk("==>%s hcd->regs %x \n",__func__,hcd->regs);
	if (hcd->regs == NULL) {
		dev_dbg(&pdev->dev, "error mapping memory\n");
		retval = -EFAULT;
		goto fail_ioremap;
	}

	retval = usb_add_hcd(hcd, irq, IRQF_SHARED);
	if (retval)
		goto fail_add_hcd;

	return retval;

fail_add_hcd:
	iounmap(hcd->regs);
fail_ioremap:
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
fail_request_resource:
	usb_put_hcd(hcd);
fail_create_hcd:
	dev_err(&pdev->dev, "init %s fail, %d\n", dev_name(&pdev->dev), retval);
	return retval;
}

static int g2_ehci_remove(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);

	usb_remove_hcd(hcd);
	iounmap(hcd->regs);
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
	usb_put_hcd(hcd);

	return 0;
}

MODULE_ALIAS("platform:g2-ehci");

static struct platform_driver g2_ehci_driver = {
	.probe = g2_ehci_probe,
	.remove = g2_ehci_remove,
	.driver = {
		.name = "g2-ehci",
	},
};
