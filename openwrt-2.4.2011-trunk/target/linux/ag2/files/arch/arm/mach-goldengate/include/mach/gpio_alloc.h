#ifndef __GPIO_ALLOC_H__
#define __GPIO_ALLOC_H__

#include <mach/gpio.h>

#ifdef CONFIG_CORTINA_ENGINEERING

#define GPIO_PFLASH_RESETN_CNTL GPIO2_BIT18
#define GPIO_FLASH_WP_CNTRL     GPIO2_BIT21
#define GPIO_SLIC0_RESET        GPIO2_BIT22
#define GPIO_SLIC1_RESET        GPIO2_BIT23
#define GPIO_SLIC_INT           GPIO2_BIT26
#define GPIO_HDMI_INT           GPIO2_BIT27	/* GPIO INTR from HDMI and Touch panel */
#define GPIO_PCIE_RESET         GPIO2_BIT28	/* #define GPIO2_BIT28  32*2+28 */
#define GPIO_USB_VBUS_POWER_0   GPIO2_BIT24
#define GPIO_USB_VBUS_POWER_1   GPIO2_BIT25

#elif defined(CONFIG_CORTINA_REFERENCE)

#define GPIO_FLASH_WP_CNTRL     GPIO2_BIT19
#define GPIO_VOIP_LED           GPIO2_BIT20
#define GPIO_SLIC_RESET         GPIO2_BIT22
#define GPIO_SLIC_INT           GPIO2_BIT26
#define GPIO_PCIE_RESET         GPIO2_BIT28	/* #define GPIO2_BIT28  32*2+28 */
#define GPIO_USB_VBUS_POWER_0   GPIO2_BIT24
#define GPIO_USB_VBUS_POWER_1   GPIO2_BIT25

#endif	

#endif/* __GPIO_ALLOC_H__ */

