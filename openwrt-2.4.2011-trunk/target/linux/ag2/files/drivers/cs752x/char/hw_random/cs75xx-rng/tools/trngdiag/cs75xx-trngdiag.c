#include <linux/errno.h>
#include <linux/random.h>
#include <linux/jiffies.h>
#include "elptrng.h"        /* for trng interface */


#define READBUFSIZE TRNG_DATA_SIZE_BYTES

void dump_bytes (uint8_t * buf, uint32_t len)
{
   uint32_t i;
   for (i = 0; i < len; i++) {
      printk ("%02X", buf[i]);
   }
}

/* Useful function to get data from the trng with optional nonce seeding */
static int32_t prng_get_data (trng_hw * trng, uint8_t * sw_buffer, uint32_t buffersize, uint8_t do_nonce)
{
	uint8_t seed_buffer[TRNG_NONCE_SIZE_BYTES];
	int32_t res;

	if (do_nonce) {
		/* Set the nonce with system prng data */
		get_random_bytes(seed_buffer, TRNG_NONCE_SIZE_BYTES);

		if ((res = trng_reseed_nonce (trng, (uint32_t *) seed_buffer)) != ELPTRNG_OK) {
			printk ("prng_get_data: Error: can't seed buffer '%d'..... FAILED\n", res);
			return res;
		}
	}

	memset (sw_buffer, 0, sizeof (sw_buffer));
	if ((res = trng_rand (trng, sw_buffer, buffersize)) != ELPTRNG_OK) {
		printk ("prng_get_data: Error: can't generate random number ..... FAILED [%d]\n", res);
		return res;
	}

   return ELPTRNG_OK;
}

/* Write buffers of rng data to a file 'rand.bin' */
static int32_t write_rng (trng_hw * trng, uint32_t bytes_per_buf, uint32_t iterations)
{
	unsigned char buf[bytes_per_buf];
	int i, j, res;

	if ((res = trng_reseed_random (trng, ELPTRNG_WAIT)) != ELPTRNG_OK) {
		printk ("write_rng: trng_reseed_random failed %d\n", res);
		return res;
	}

	for (i = 0; i < iterations; i++) {
		if ((res = trng_rand (trng, buf, sizeof (buf))) < 0) {
			printk ("rng_test: Error: can't generate random number ..... FAILED [%d]\n", res);
			return res;
		}

		for (j = 0; j < bytes_per_buf; j++) {
			printk("%02X", buf[j]);

			if (((j+1) % 4) == 0)
				printk("  ");
			if (((j+1) % 16) == 0)
				printk("\n");
		}
	}

   return ELPTRNG_OK;
}

int cs752x_trng_diag(uint32_t base_addr, int32_t seedval)
{
	int32_t ret;
	uint8_t buff[READBUFSIZE], buff2[READBUFSIZE];
	trng_hw trng;

	/* Initialize trng hardware */
	/* Don't enable IRQ pin and don't reseed */
	ret = trng_init (&trng, base_addr, ELPTRNG_IRQ_PIN_DISABLE, ELPTRNG_NO_RESEED);
	if (ret != ELPTRNG_OK) {
		printk ("ERR[line %d]: trng_init: %d\n", __LINE__, ret);
		return ret;
	}
	/* Dump the registers to see if they make sense */
	trng_dump_registers (&trng);

	trng_close (&trng);

	/* Initialize trng hardware */
	/* Enable IRQ pin and reseed */
	ret = trng_init (&trng, base_addr, ELPTRNG_IRQ_PIN_ENABLE, ELPTRNG_RESEED);
	if (ret != ELPTRNG_OK) {
		printk ("ERR[line %d]: trng_init: %d\n", __LINE__, ret);
		return ret;
	}
	/* Dump the registers to see if they make sense */
	trng_dump_registers (&trng);
	printk ("trng_init: [PASSED]\n");
	printk ("\n");

	/* Start the actual tests */

	/* Test 1: */
	/* Should be able to read random data */

	if ((ret = trng_rand (&trng, buff, READBUFSIZE)) != ELPTRNG_OK) {
		printk ("trng_rand: Error: can't generate random number ..... FAILED [%d]\n", ret);
		goto END;
	} else {
		printk ("Rand data: ");
		dump_bytes (buff, READBUFSIZE);
		printk (" [PASSED]\n");
	}
	printk ("\n");

	/* Test 2: */
	/* Given different initial nonce seeds the rng data is different */

	printk ("Unique Nonce Reseed test: \n");

	/* Get random data with a random nonce seed */
	srandom32 (jiffies);         /* Seed the system prng with a time stamp */
	ret = prng_get_data (&trng, buff2, READBUFSIZE, 1);
	if (ret != ELPTRNG_OK) {
		printk ("ERR[line %d]: prng_get_data: %d\n", __LINE__, ret);
		goto END;
	}
	printk ("Reseed 1: \t");
	dump_bytes (buff2, READBUFSIZE);
	printk ("\n");

	/* Get random data with a random nonce seed */
	/* The system prng will create nonce with it's next unique data */
	ret = prng_get_data (&trng, buff, READBUFSIZE, 1);
	if (ret != ELPTRNG_OK) {
		printk ("ERR[line %d]: prng_get_data: %d\n", __LINE__, ret);
		goto END;
	}
	printk ("Reseed 2: \t");
	dump_bytes (buff, READBUFSIZE);

	/* Compare the data buffers */
	if (memcmp (buff, buff2, READBUFSIZE) != 0) {
		printk (" [PASSED]\n");
	} else {
		printk (" [FAILED]\n");
		ret = -1;
		goto END;
	}
	printk ("\n");

	/* Test 3: */
	/* Given the same initial nonce seed the rng data is the same */

	printk ("Same Nonce Reseed test:\n");

	srandom32 (seedval);
	ret = prng_get_data (&trng, buff, READBUFSIZE, 1);
	if (ret != ELPTRNG_OK) {
		printk ("ERR[line %d]: prng_get_data: %d\n", __LINE__, ret);
		goto END;
	}
	printk ("Reseed 1: \t");
	dump_bytes (buff, READBUFSIZE);
	printk ("\n");

	srandom32 (seedval);
	ret = prng_get_data (&trng, buff2, READBUFSIZE, 1);
	if (ret != ELPTRNG_OK) {
		printk ("ERR[line %d]: prng_get_data: %d\n", __LINE__, ret);
		goto END;
	}
	printk ("Reseed 2: \t");
	dump_bytes (buff2, READBUFSIZE);

	if (memcmp (buff, buff2, READBUFSIZE) == 0) {
		printk (" [PASSED]\n");
	} else {
		printk (" [FAILED]\n");
		ret = -1;
		goto END;
	}
	printk ("\n");

#if 0	// Not Support Hardware Reset
	/* Test 4: */
	/* Reset the hardware and test to see if a nonce reseed is correct */

	printk ("Nonce Reseed while initial seed is underway:\n");

	srandom32 (seedval);
	ret = prng_get_data (&trng, buff2, READBUFSIZE, 1);
	if (ret != ELPTRNG_OK) {
		printk ("ERR[line %d]: prng_get_data: %d\n", __LINE__, ret);
		goto END;
	}
	printk ("Run 1: \t\t");
	dump_bytes (buff, READBUFSIZE);
	printk ("\n");

	/* reset hardware which should kick off hw reseed */
	//trng_reset (&tif);

	srandom32 (seedval);
	ret = prng_get_data (&trng, buff, READBUFSIZE, 1);
	if (ret != ELPTRNG_OK) {
		printk ("ERR[line %d]: prng_get_data: %d\n", __LINE__, ret);
		goto END;
	}
	printk ("Run 2: \t\t");
	dump_bytes (buff, READBUFSIZE);

	if (memcmp (buff, buff2, READBUFSIZE) == 0) {
		printk (" [PASSED]\n ");
	} else {
		printk (" [FAILED]\n");
		ret = -1;
		goto END;
	}
	printk ("\n");
#endif

	/* Test 5: */
	/* Force a nonce reseed while a random reseed is processing */

	printk ("Nonce Reseed while random reseed is underway:\n");

	srandom32 (seedval);
	ret = prng_get_data (&trng, buff2, READBUFSIZE, 1);
	if (ret != ELPTRNG_OK) {
		printk ("ERR[line %d]: prng_get_data_nonce: %d\n", __LINE__, ret);
	}
	printk ("Run 1: \t\t");
	dump_bytes (buff2, READBUFSIZE);
	printk ("\n");

	/* start a random reseed operation without waiting */
	if ((ret = trng_reseed_random (&trng, ELPTRNG_NO_WAIT)) != ELPTRNG_OK) {
		printk ("trng_reseed_random: failed %d\n", ret);
		goto END;
	}

	srandom32 (seedval);
	ret = prng_get_data (&trng, buff, READBUFSIZE, 1);
	if (ret != ELPTRNG_OK) {
      printk ("ERR[line %d]: prng_get_data: %d\n", __LINE__, ret);
	}

	printk ("Run 2: \t\t");
	dump_bytes (buff, READBUFSIZE);
	/* previous buffer should be the same as this one */
	if (memcmp (buff, buff2, READBUFSIZE) == 0) {
		printk (" [PASSED]\n");
	} else {
		printk (" [FAILED]\n");
		ret = -1;
		goto END;
	}
	printk ("\n");

	/* Test 6: */
	/* Generate a large amount of random data */

	ret = write_rng (&trng, 1024, 500);
	if (ret != ELPTRNG_OK) {
		printk ("ERR[line %d]: write_rng: %d\n", __LINE__, ret);
		goto END;
	}
	printk ("Large generate test: Check file 'rand.bin': [PASSED]\n");

END:
	trng_close (&trng);

	return ret;
}
