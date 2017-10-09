/*
 * Copyright (C) 2006-2008 Nokia Corporation
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; see the file COPYING. If not, write to the Free Software
 * Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Test OOB read and write on MTD device.
 *
 * Author: Adrian Hunter <ext-adrian.hunter@nokia.com>
 */

#include <asm/div64.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/err.h>
#include <linux/mtd/mtd.h>
#include <linux/sched.h>
#include <linux/io.h>
#include <mach/hardware.h>
#include <mach/cs752x_flash.h>
#include <linux/slab.h>

#define PRINT_PREF KERN_INFO "mtd_hammingtest: "

static int dev;
module_param(dev, int, S_IRUGO);
MODULE_PARM_DESC(dev, "MTD device number to use");


static int testno = 1;
module_param(testno, int, S_IRUGO);
MODULE_PARM_DESC(testno, "ECC test number to test");

static int ecc_data_size = 256;
module_param(ecc_data_size, int, S_IRUGO);
MODULE_PARM_DESC(ecc_data_size, "ECC ecc data to test");


static struct mtd_info *mtd;
static unsigned char *bbt;

static int ebcnt;
static int pgcnt;
static unsigned long next = 1;

static inline unsigned int simple_rand(void)
{
	next = next * 1103515245 + 12345;
	return (unsigned int)((next / 65536) % 32768);
}

static inline void simple_srand(unsigned long seed)
{
	next = seed;
}

static void set_random_data(unsigned char *buf, size_t len)
{
	size_t i;

	for (i = 0; i < len; ++i)
		buf[i] = simple_rand();
}


static int is_block_bad(int ebnum)
{
	int ret;
	loff_t addr = ebnum * mtd->erasesize;

	ret = mtd->block_isbad(mtd, addr);
	if (ret)
		printk(PRINT_PREF "block %d is bad\n", ebnum);
	return ret;
}

static int scan_for_bad_eraseblocks(void)
{
	int i, bad = 0;

	bbt = kmalloc(ebcnt, GFP_KERNEL);
	if (!bbt) {
		printk(PRINT_PREF "error: cannot allocate memory\n");
		return -ENOMEM;
	}
	memset(bbt, 0 , ebcnt);

	printk(PRINT_PREF "scanning for bad eraseblocks\n");
	for (i = 0; i < ebcnt; ++i) {
		bbt[i] = is_block_bad(i) ? 1 : 0;
		if (bbt[i])
			bad += 1;
		cond_resched();
	}
	printk(PRINT_PREF "scanned %d eraseblocks, %d are bad\n", i, bad);
	return 0;
}


loff_t addr0;

static int write_raw( struct mtd_info *mtd, unsigned char *buf)
{
	struct mtd_oob_ops ops;

	ops.mode      = MTD_OOB_RAW;
	ops.len       = mtd->writesize;
	ops.retlen    = 0;
	ops.ooblen    = mtd->oobsize;
	ops.oobretlen = 0;
	ops.ooboffs   = 0;
	ops.datbuf    = buf;
	ops.oobbuf    = buf + mtd->writesize ;

	return mtd->write_oob(mtd, addr0, &ops);
}


static int read_raw( struct mtd_info *mtd, unsigned char *buf)
{
	struct mtd_oob_ops ops;

	ops.mode      = MTD_OOB_RAW;
	ops.len       = mtd->writesize;
	ops.retlen    = 0;
	ops.ooblen    = mtd->oobsize;
	ops.oobretlen = 0;
	ops.ooboffs   = 0;
	ops.datbuf    = buf;
	ops.oobbuf    = buf + mtd->writesize ;
	
	return mtd->read_oob( mtd, addr0, &ops);
}

static unsigned ecc_value_size	= 3;
#define EXTAR_SIZE 32 
#define DATA_SIDE 0x00
#define ECC_SIDE 0x01

static int write_mtd( struct mtd_info *mtd, unsigned char *buf)
{
	int rc;
	rc= write_raw( mtd, buf);
	if( rc != 0 ) {
		return rc;
	}

	memset( buf, 0, mtd->writesize + mtd->oobsize + EXTAR_SIZE );

	return read_raw( mtd, buf);
}


static void gen_error_data( struct mtd_info *mtd, unsigned char *buffer, 
		unsigned buffer_len, unsigned error_bit, 
		unsigned char *map, int side)
{
	unsigned i, offset, total_ecc_bits;

	memset( map, 0, buffer_len );


	for( i =0; i < error_bit; ++i) {
		if( side == DATA_SIDE ) {
			do {
				offset= simple_rand() % (ecc_data_size*8);
			} while( map[ offset/8] & ( 1<<(offset%8) ) );	
		} else if ( side == ECC_SIDE ){
			do {
				offset= ecc_data_size * 8 + simple_rand() % (ecc_value_size * 8);
			} while( map[ offset/8] & ( 1<<(offset%8) ) );	
		} else {
			total_ecc_bits= (ecc_data_size + ecc_value_size )* 8;
			do {
				offset= simple_rand() % total_ecc_bits;
			} while( map[ offset/8] & ( 1<<(offset%8) ) );
		}

		map[ offset/8 ] |= 1 << (offset%8);

		if( offset < ecc_data_size*8 ) {
			printk( "data pos [%d:%d]", offset/8 , offset%8 );
			buffer[ offset/8 ] ^= 1 << (offset%8);
		} else {
			offset -= ecc_data_size*8;
			printk( "oob pos [%d:%d]", offset/8 , offset%8 );
			buffer[ mtd->writesize + mtd->ecclayout->eccpos[ offset/8 ] ] ^=  1 << (offset%8);
		}
	}
}

static int erase_eraseblock(int ebnum)
{
	int err;
	struct erase_info ei;
	loff_t addr = ebnum * mtd->erasesize;

	memset(&ei, 0, sizeof(struct erase_info));
	ei.mtd  = mtd;
	ei.addr = addr;
	ei.len  = mtd->erasesize;

	err = mtd->erase(mtd, &ei);
	if (err) {
		printk(PRINT_PREF "error %d while erasing EB %d\n", err, ebnum);
		return err;
	}

	if (ei.state == MTD_ERASE_FAILED) {
		printk(PRINT_PREF "some erase error occurred at EB %d\n",
		       ebnum);
		return -EIO;
	}

	return 0;
}


static int erase_whole_device(void)
{
	int err;
	unsigned int i;

	printk(PRINT_PREF "erasing whole device\n");
	for (i = 0; i < ebcnt; ++i) {
		if (bbt[i])
			continue;
		err = erase_eraseblock(i);
		if (err)
			return err;
		cond_resched();
	}
	printk(PRINT_PREF "erased %u eraseblocks\n", i);
	return 0;
}

static inline int checkHWStatus( struct mtd_info *mtd, unsigned char *verifybuf, int value)
{

	unsigned extra_data_pos = mtd->writesize + mtd->oobsize ;
	printk( "checkHWStatus extra_data_pos [%d] %d\n", extra_data_pos, *( verifybuf + extra_data_pos) );
	return *( verifybuf + extra_data_pos) == value ? 1 : 0;
}

static int checkErrorBit( struct mtd_info *mtd, unsigned char *comparebuf, unsigned char *verifybuf, const char *errormap)
{
	unsigned i, j, count = 0, bit_error_no, all_ecc_bits;

	unsigned extra_data_pos = mtd->writesize + mtd->oobsize ;

	for( i= 0; i < mtd->writesize; ++i) {
		if( comparebuf[i] != verifybuf[i] ) {
			for( j=0; j < 8 ; ++j) {
				if( ( comparebuf[i]&( 0x01<<j) ) !=  ( verifybuf[i]&( 0x01<<j))) {
					if( errormap[i] & (0x01<<j )) {
						++count;
					} else {
						return 1;
					}
				}
			}
		}
	}
	
	all_ecc_bits= ( ecc_data_size + ecc_value_size) * 8;

	bit_error_no= *( verifybuf + extra_data_pos + 1 );
	if( count == bit_error_no ){
		return 0;
	} else if ( count < bit_error_no) {
		for( i = ecc_data_size*8; i < all_ecc_bits; ++i) {
			if( errormap[ i/8 ] & (1 << i )) {
				++count;
			}
		}
		
		if( count == bit_error_no ) {
			return 0;
		}
	}

	return 1;
}

static void ni_dm_byte(cs_uint32 location, int length)
{
	u8		*start_p, *end_p, *curr_p;
	u8		*datap, data;
	int		i;

	start_p = (u8 *)location;
	end_p = (u8 *)start_p + length;
	curr_p=(u8 *)((cs_uint32)location);// & 0xfffffff0);
	datap = (u8 *)location;

	while (curr_p < end_p) {
		u8 *p1, *p2;
		printk("0x%08x: ",(cs_uint32)curr_p);
		p1 = curr_p;
		p2 = datap;

		// dump data			    
		for (i=0; i<16; i++) {
			data = *datap;
			printk("%02X ", data);
			if (i==7)
				printk("- ");
			curr_p++;
			datap++;
		}

		/* dump ascii */
		curr_p = p1;
		datap = p2;
		for (i=0; i<16; i++) {
			if (curr_p < start_p || curr_p >= end_p) {
				printk(".");
			} else {
				data = *datap ;
				if (data<0x20 || data>0x7f || data==0x25) 
					printk(".");
				else
					printk("%c", data);;
			}
			curr_p++;
			datap++;
		}
		printk("\n");
	} 
}


unsigned char *errormap = NULL;
unsigned char *correctbuf = NULL; // correct buffer
unsigned char *testbuf = NULL; 	// test buffer for write in
unsigned char *testbuf2 = NULL; // test buffer for read out

static int hamming_test_run( void)
{
	int err = 0;
	size_t rc, retlen;
	unsigned error_bit, test_count, buf_len ;

	erase_whole_device();
	mtd->write( mtd, addr0, mtd->writesize, &retlen, correctbuf );
	mtd->read( mtd, addr0, mtd->writesize, &retlen, testbuf2 );

	
	if( read_raw( mtd, testbuf) != 0 
			|| memcmp( testbuf, testbuf2, mtd->writesize) != 0 
			|| memcmp( testbuf, correctbuf, mtd->writesize) != 0 ) {

		printk( PRINT_PREF "What we read is different with what we get!");
		err++;
		goto out;
	} 

	printk( " correctbuf %p\n", correctbuf);
	memcpy( correctbuf+ mtd->writesize, testbuf+ mtd->writesize, mtd->oobsize);
	printk( PRINT_PREF "Test bits \n" );

	//ni_dm_byte( testbuf+ mtd->writesize , mtd->oobsize);
	/*
	* test recoverable 1 bit error in data.
	*/
	error_bit = 1;
	buf_len = mtd->writesize + mtd->oobsize + EXTAR_SIZE;


	memset( testbuf2, 0 , buf_len );
	memset( errormap, 0, buf_len );
	gen_error_data( mtd, testbuf, ecc_value_size + ecc_data_size, 
			error_bit, errormap, DATA_SIDE );

	erase_whole_device();
	write_raw( mtd, testbuf);

	rc= 0;
	mtd->read( mtd, addr0, mtd->writesize, &retlen, testbuf2);


	if( memcmp( testbuf2, correctbuf, mtd->writesize) != 0  ) {
		printk( PRINT_PREF "Failed by compare data\n" );
		ni_dm_byte( testbuf, mtd->writesize);
		printk( PRINT_PREF "===================================================\n" );
		ni_dm_byte( correctbuf, mtd->writesize);
		rc=1;
	} else if( checkHWStatus( mtd, testbuf2, ECC_1BIT_DATA_ERR) == 0 ) {
		printk( PRINT_PREF "Failed by check HW status" );
		rc=1;
	} else {
		printk( PRINT_PREF "Test %d bits error successful\n", error_bit);
	}

	if( rc ) {
		printk( PRINT_PREF "Test %d bits error failed\n", error_bit);
		++err;
		goto out;
	}


	/*
	* test recoverable 1 bit error in ecc.
	*/

	error_bit = 1;
	buf_len = mtd->writesize + mtd->oobsize + EXTAR_SIZE;

	memcpy( testbuf, correctbuf, buf_len);
	memset( testbuf2, 0 , buf_len);
	memset( errormap, 0, buf_len);
	gen_error_data( mtd, testbuf, ecc_value_size + ecc_data_size, 
			error_bit, errormap, ECC_SIDE );

	erase_whole_device();
	write_raw( mtd, testbuf);

	rc= 0;
	mtd->read( mtd, addr0, mtd->writesize, &retlen, testbuf2);
	if( memcmp( testbuf2, correctbuf, mtd->writesize) != 0  ) {
		printk( PRINT_PREF "Failed by compare data" );
		rc=1;
	} else if( checkHWStatus( mtd, testbuf2, ECC_1BIT_ECC_ERR) == 0 ) {
		printk( PRINT_PREF "Failed by check HW status" );
		rc=1;
	} else {
		printk( PRINT_PREF "Test %d bits ECC error successful\n", error_bit);
	}

	if( rc ) {
		printk( PRINT_PREF "Test %d bits ECC error failed\n", error_bit);

		++err;
		goto out;
	}


	/*
	* test recoverable 2 bit error.
	*/
	error_bit = 2;
	buf_len = mtd->writesize + mtd->oobsize + EXTAR_SIZE;

	memcpy( testbuf, correctbuf, buf_len);
	memset( testbuf2, 0, buf_len);
	memset( errormap, 0, buf_len);
	gen_error_data( mtd, testbuf, buf_len, error_bit, errormap, DATA_SIDE|ECC_SIDE);

	erase_whole_device();
	write_raw( mtd, testbuf);

	rc= 0;
	mtd->read( mtd, addr0, mtd->writesize, &retlen, testbuf2);
	if( checkHWStatus( mtd, testbuf2, ECC_UNCORRECTABLE) == 0 ) {
		printk( PRINT_PREF "Failed by check HW BCH status" );
		rc=1;
	} else if( checkErrorBit( mtd, testbuf, testbuf2, errormap) != 0) {
		printk( PRINT_PREF "Failed by check errob bits" );
		rc=1;
	} else {
		printk( PRINT_PREF "Test %d bits error successful\n", error_bit);
	}

	if( rc ) {
		printk( PRINT_PREF "Test %d bits error successful\n", error_bit);
		++err;
		goto out;
	}

	return err;
out:
	printk( "correct data ===================\n");
	ni_dm_byte( correctbuf, mtd->writesize+ mtd->oobsize );
	printk( "read from data ===================\n");
	ni_dm_byte( testbuf2, mtd->writesize);
	printk( "error map  ===================\n");
	ni_dm_byte( errormap, ecc_data_size+ ecc_value_size); 

	return err;
}


static int __init mtd_hammingtest_init(void)
{
	int err = 0;
	uint64_t tmp;
	size_t i, retlen;

	printk(KERN_INFO "\n");
	printk(KERN_INFO "=================================================\n");
	printk(PRINT_PREF "MTD device: %d\n", dev);

	mtd = get_mtd_device(NULL, dev);
	if (IS_ERR(mtd)) {
		err = PTR_ERR(mtd);
		printk(PRINT_PREF "error: cannot get MTD device\n");
		return err;
	}

	if (mtd->type != MTD_NANDFLASH) {
		printk(PRINT_PREF "this test requires NAND flash\n");
		goto out;
	}
	cs752x_ecc_check_enable( 1);

	tmp = mtd->size;
	do_div(tmp, mtd->erasesize);
	ebcnt = tmp;
	pgcnt = mtd->erasesize / mtd->writesize;

	printk(PRINT_PREF "MTD device size %llu, eraseblock size %u, "
	       "page size %u, count of eraseblocks %u, pages per "
	       "eraseblock %u, OOB size %u\n",
	       (unsigned long long)mtd->size, mtd->erasesize,
	       mtd->writesize, ebcnt, pgcnt, mtd->oobsize);

	err = -ENOMEM;
	mtd->erasesize = mtd->erasesize;

		
	//simple_srand( current->pid );
	simple_srand( 379 );
	printk( PRINT_PREF "Random seed is %d !\n"
			"Repeat test with the same data! Feed the same seed again!\n"
			, current->pid );

	err = scan_for_bad_eraseblocks();
	if (err) {
		goto out;
	}

	addr0 = 0;
	for (i = 0; i < ebcnt && bbt[i]; ++i)
		addr0 += mtd->erasesize;

	printk( PRINT_PREF " addr0=[0x%x]\n", addr0);

	correctbuf = kzalloc( mtd->writesize + mtd->oobsize  + EXTAR_SIZE , GFP_KERNEL );
	testbuf = kzalloc( mtd->writesize + mtd->oobsize + EXTAR_SIZE, GFP_KERNEL ); 
	testbuf2 = kzalloc( mtd->writesize + mtd->oobsize + EXTAR_SIZE, GFP_KERNEL ); 
	errormap= kzalloc( mtd->writesize + mtd->oobsize + EXTAR_SIZE, GFP_KERNEL ); 
	
	if ( !correctbuf || !testbuf || !testbuf2 || !errormap ) {
		printk(PRINT_PREF "error: cannot allocate memory\n");
		goto out;
	}

	// generate random data and start test
	set_random_data( correctbuf, mtd->writesize);

	for( i =0; i < testno; ++i ) {
		if( hamming_test_run() != 0 ) {
			break;
		}
	}

out:
	kfree( bbt);
	kfree( correctbuf );
	kfree( testbuf );
	kfree( errormap );

	cs752x_ecc_check_enable( 0);
	return -err;
}
module_init(mtd_hammingtest_init);

static void __exit mtd_hammingtest_exit(void)
{
	return;
}
module_exit(mtd_hammingtest_exit);

MODULE_DESCRIPTION("BCH Algorigthm Verify Module");
MODULE_AUTHOR("Peter Lee");
MODULE_LICENSE("GPL");
