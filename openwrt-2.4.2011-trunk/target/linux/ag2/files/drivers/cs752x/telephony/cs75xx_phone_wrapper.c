#include <linux/io.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/bitops.h>
#include <linux/spi/spi.h>
#include <linux/cs75xx_phone_wrapper.h>
#include <mach/hardware.h>
#include <mach/platform.h>
#include <mach/gpio_alloc.h>
#include <mach/spi_agent.h>


/* SSP#, SLOT#(SPI CS) selection */
#if defined(CONFIG_CORTINA_REFERENCE)
#define SLIC_SSP_INDEX	0
#define SLIC_SPI_CS	0
#elif defined(CONFIG_CORTINA_ENGINEERING)
#define SLIC_SSP_INDEX	1
#ifdef (CONFIG_SLIC_SLOT0)
#define SLIC_SPI_CS	1
#else
#define SLIC_SPI_CS	2
#endif
#endif


typedef struct {
	spinlock_t lock;
	int (*hw_poll_timer_callback)(unsigned long);
} phone_wrapper_t;

static phone_wrapper_t phone_wrapper;


/******************************************************************************/
int cs_slic_pcm_select(void)
{
	return SLIC_SSP_INDEX;
}
EXPORT_SYMBOL(cs_slic_pcm_select);


int cs_slic_hw_reset(void)
{
	gpio_set_value(GPIO_SLIC_RESET, 0);
	mdelay(250);
	gpio_set_value(GPIO_SLIC_RESET, 1);

	return 0;
}
EXPORT_SYMBOL(cs_slic_hw_reset);


int cs_slic_read_byte(unsigned char *value)
{
	spi_slave_dbs[SLIC_SPI_CS].spi_rx(SLIC_SPI_CS, value);

	return 0;
}
EXPORT_SYMBOL(cs_slic_read_byte);


int cs_slic_write_byte(unsigned char value)
{
	spi_slave_dbs[SLIC_SPI_CS].spi_tx(SLIC_SPI_CS, &value);

	return 0;
}
EXPORT_SYMBOL(cs_slic_write_byte);


int cs_register_hw_poll_timer(unsigned int scale, int (*callback)(unsigned long))
{
	struct platform_clk clk;
	unsigned long ctrl;
	unsigned int reload;

	printk("register_hw_poll_timer\n");

	get_platform_clk(&clk);
	reload = (clk.apb_clk / 1000000) * (scale * 1000);
	writel(reload, IO_ADDRESS(PER_TMR_LD1));
	ctrl = BIT(6) | BIT(7);

	phone_wrapper.hw_poll_timer_callback = callback;

	writel(0x2, IO_ADDRESS(PER_TMR_LOADE));
	writel(ctrl, IO_ADDRESS(PER_TMR_CTRL1));
	writel(1, IO_ADDRESS(PER_TMR_IE1_0));

	return 0;
}
EXPORT_SYMBOL(cs_register_hw_poll_timer);


int cs_unregister_hw_poll_timer(void)
{
	/* disable interrupt */
	writel(0, IO_ADDRESS(PER_TMR_IE1_0));
	writel(0, IO_ADDRESS(PER_TMR_CTRL1));

	phone_wrapper.hw_poll_timer_callback = NULL;

	return 0;
}
EXPORT_SYMBOL(cs_unregister_hw_poll_timer);

int cs_voip_led_trigger(voip_led_state_t state)
{
	return 0;
}
EXPORT_SYMBOL(cs_voip_led_trigger);


/******************************************************************************/
static irqreturn_t hw_timer_hanlder(int irq, void *dev_instance)
{
	/* disable interrupt */
	writel(0, IO_ADDRESS(PER_TMR_IE1_0));

	/* clear interrupt */
	writel(1, IO_ADDRESS(PER_TMR_INT1_0));

	phone_wrapper.hw_poll_timer_callback(1234);

	/* enable interrupt */
	writel(1, IO_ADDRESS(PER_TMR_IE1_0));

	return IRQ_HANDLED;
}

static int __init phone_wrapper_init(void)
{
	spin_lock_init(&phone_wrapper.lock);
	phone_wrapper.hw_poll_timer_callback = NULL;

	printk("###phone_wrapper_init...");

	if (gpio_request(GPIO_SLIC_RESET, "SLIC_RESET")) {
		printk("Can't reserve GPIO %d\n", GPIO_SLIC_RESET);
		goto FAIL;
	}
	gpio_direction_output(GPIO_SLIC_RESET, 1);

	if (gpio_request(GPIO_VOIP_LED, "VOIP_LED")) {
		printk("Can't reserve GPIO %d\n", GPIO_VOIP_LED);
		goto FAIL;
	}
	gpio_direction_output(GPIO_VOIP_LED, 0);

	writel(0, IO_ADDRESS(PER_TMR_IE1_0));
	if (request_irq(GOLDENGATE_IRQ_TIMER0, hw_timer_hanlder, IRQF_DISABLED, "phone_poll_timer", NULL)) {
		printk("ERROR: can't register IRQ %s\n", "phone_poll_timer");
		gpio_free(GPIO_SLIC_RESET);
		gpio_free(GPIO_VOIP_LED);
		goto FAIL;
	}

	printk("OK!\n");
	return 0;

FAIL:
	gpio_free(GPIO_SLIC_RESET);
	gpio_free(GPIO_VOIP_LED);

	printk("FAIL!\n");
	return -EPERM;
}
module_init(phone_wrapper_init);

static void __exit phone_wrapper_exit(void)
{
	gpio_free(GPIO_SLIC_RESET);
	gpio_free(GPIO_VOIP_LED);
	free_irq(GOLDENGATE_IRQ_TIMER0, NULL);
}
module_exit(phone_wrapper_exit);

