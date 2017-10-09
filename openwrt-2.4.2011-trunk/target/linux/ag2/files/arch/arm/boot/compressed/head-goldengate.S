#include <linux/linkage.h>
#include <asm/mach-types.h>

		.section	".start", "ax"
		mov	r7, #MACH_TYPE_CORTINA_G2
