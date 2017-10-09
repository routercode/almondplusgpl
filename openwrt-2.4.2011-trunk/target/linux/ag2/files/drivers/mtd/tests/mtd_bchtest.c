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

#define PRINT_PREF KERN_INFO "mtd_bchtest: "

static int dev;
module_param(dev, int, S_IRUGO);
MODULE_PARM_DESC(dev, "MTD device number to use");


static int testno = 1;
module_param(testno, int, S_IRUGO);
MODULE_PARM_DESC( testno, "BCH test number to test");


static int bch_method = 8;
module_param( bch_method, int, S_IRUGO);
MODULE_PARM_DESC( bch_method, "BCH correct_number: 8 or 12");

static struct mtd_info *mtd;
static unsigned char *bbt;

static int ebcnt;
static int pgcnt;
static unsigned long next = 1;
static unsigned extra_data_pos;

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


#define BCH_DATA_SIZE 512
#define BCH_ECC_SIZE   20 


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

static int toggleAllbits( struct mtd_info *mtd, unsigned char *buffer)
{
	unsigned offset;
	unsigned bch_code_len;

	for( offset =0 ; offset < BCH_DATA_SIZE ; ++offset) {
		buffer[ offset ] =  0xFF - buffer[ offset ];
	}

	if( bch_method == 8) {
		bch_code_len = 13;
	} else if( bch_method == 12) {
		bch_code_len = 20;
	}

	for( offset= 0 ; offset < bch_code_len; ++offset) {
		buffer[ mtd->writesize + mtd->ecclayout->eccpos[ offset/8 ] ] =
			0xFF -  buffer[ mtd->writesize + mtd->ecclayout->eccpos[ offset/8 ] ] ;
	}

	return 1;
}

unsigned aaa;
static void gen_error_data( struct mtd_info *mtd, unsigned char *buff, unsigned error_bit, unsigned char *map)
{
	unsigned i, offset, count;
	unsigned bch_all_bits;

	if( bch_method == 8 ) {
		memset( map, 0, ( BCH_DATA_SIZE + 13 ) );
		bch_all_bits = ( BCH_DATA_SIZE + 13 ) * 8;
	} else if( bch_method == 12 ) {
		memset( map, 0, ( BCH_DATA_SIZE + 20 ) );
		bch_all_bits = ( BCH_DATA_SIZE + 20 ) * 8;
	}


	if( error_bit > bch_all_bits/2 ) {
		/* It means error bits are more than right bits 
		*  We toggle all bits and toggle some bits again.
		*  It is much easier. 
		*  Are you Confuse ? Try to think what happens in 4183 error bits.
		*/
		toggleAllbits( mtd, buff);
		error_bit= bch_all_bits - error_bit;
	}

	for( i= 0; i < error_bit; ++i) {
		do {
			offset= simple_rand() % bch_all_bits;
		} while( map[ offset/8] & ( 1<<(offset%8) ) );	


		map[ offset/8 ] |= 1 << (offset%8);
	//	printk( "before offset=[%x:%d] %x\n", offset/8, offset%8, buff[ offset/8 ] );

		if( offset < BCH_DATA_SIZE*8 ) {
			buff[ offset/8 ] ^= 1 << (offset%8);
		} else {
			offset -= BCH_DATA_SIZE*8;
			buff[ mtd->writesize + mtd->ecclayout->eccpos[ offset/8 ] ] ^=  1 << (offset%8);
		}
//		printk( "after offset=[%x:%d] %x\n", offset/8, offset%8, buff[ offset/8 ] );
		aaa = offset;
	}

#ifdef __DEBUG__
	count=0;
	for( i = BCH_DATA_SIZE*8; i < bch_all_bits; ++i) {
		if( map[ i/8 ] & (1 <<  (i%8) )) {
			++count;
		}
	}
	printk( "error bch=%d\n", count);
#endif
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

static inline int checkHWBCHStatus( struct mtd_info *mtd, unsigned char *verifybuf, int value)
{
	if( *( verifybuf + extra_data_pos) == value ){
		return 1;
	} 
	printk( "%s = %d\n", __func__, *( verifybuf + extra_data_pos));
	return 0;
}

static int checkErrorBit( struct mtd_info *mtd, unsigned char *comparebuf, unsigned char *verifybuf, const char *errormap)
{
	unsigned i, j, count = 0, bit_error_no, bch_all_bits;

	if( bch_method == 8 ) {
		bch_all_bits = ( BCH_DATA_SIZE + 13 ) * 8;
	} else if( bch_method == 12 ) {
		bch_all_bits = ( BCH_DATA_SIZE + 20 ) * 8;
	}

	for( i= 0; i < mtd->writesize; ++i) {
		if( comparebuf[i] != verifybuf[i] ) {
			for( j=0; j < 8 ; ++j) {
				if( ( comparebuf[i]&( 0x01<<j) ) !=  ( verifybuf[i]&( 0x01<<j))) {
					++count;
				}
			}
		}
	}
	
	for( i = BCH_DATA_SIZE*8; i < bch_all_bits; ++i) {
		if( errormap[ i/8 ] & (1 << (i%8) )) {
			++count;
	//		printk( "error bch++=%d\n", count);
		}
	}



	return count ;
}

void ni_dm_byte(cs_uint32 location, int length)
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
	    printk("0x%08x: ",(cs_uint32)curr_p);// & 0xfffffff0);
	    p1 = curr_p;
	    p2 = datap;
		// dump data			    
		for (i=0; i<16; i++) {
//			if (curr_p < start_p || curr_p >= end_p) {
//					printk("   ");
				 //printk("	     ");
//			} else {
				data = *datap;
				printk("%02X ", data);
//			}
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


unsigned char *errormap= NULL;
unsigned char *correctbuf = NULL; // correct buffer
unsigned char *testbuf = NULL; 	// test buffer for write in
unsigned char *testbuf2 = NULL; // test buffer for read out

static int bch_test_run( void)
{
	int err = 0, test_count, i;

	size_t rc;
	size_t retlen;
	unsigned error_bit, bch_all_bits ;


	if( bch_method == 8 ) {
		bch_all_bits = ( BCH_DATA_SIZE + 13 ) * 8;
	} else if( bch_method == 12 ) {
		bch_all_bits = ( BCH_DATA_SIZE + 20 ) * 8;
	}



	erase_whole_device();
	mtd->write( mtd, addr0, mtd->writesize, &retlen, correctbuf);
	mtd->read( mtd, addr0, mtd->writesize, &retlen, testbuf2 );

	for ( i = 0 ; i < 10; ++i) {
		if( read_raw( mtd, testbuf) == 0 && memcmp( testbuf, testbuf2, mtd->writesize) == 0 
				&& memcmp( testbuf, correctbuf, mtd->writesize) == 0 ) {
			break;
		}  else {

			printk( PRINT_PREF "What we read is different with what we get! Try agian %d\n", i);
		}
	}

	if( i == 10 ) {
		printk( PRINT_PREF "What we read is different with what we get!\n"
				"Not software problem! Try to install this module agiane \n");
		err++;
		goto out;
	}


	memcpy( correctbuf+ mtd->writesize, testbuf+ mtd->writesize, mtd->oobsize);
	printk( PRINT_PREF "Test bits \n" );


	for( test_count=0; test_count < 20; ++test_count) {
		for( error_bit = 1; error_bit <= bch_method ; ++error_bit ) {


			printk( PRINT_PREF "Start error bits[%d] test", error_bit);

			memcpy( testbuf, correctbuf, mtd->writesize + mtd->oobsize);
			memset( testbuf2, 0 , extra_data_pos);
			memset( errormap, 0, extra_data_pos);
			gen_error_data( mtd, testbuf, error_bit, errormap);

			erase_whole_device();
			write_raw( mtd, testbuf) ;

			rc= 0;
			if(  mtd->read( mtd, addr0, mtd->writesize, &retlen, testbuf2) != 0 ) {
				printk( PRINT_PREF "Failed by read data" );
				rc=1;
			} else if( checkHWBCHStatus( mtd, testbuf2, BCH_CORRECTABLE_ERR) ){
				if( checkErrorBit( mtd, correctbuf, testbuf2, errormap) > bch_method ) {
					printk( PRINT_PREF "Failed by check HW BCH status" );
					rc=1;
				} else if( memcmp( testbuf2, correctbuf, mtd->writesize) != 0 ) {
					printk( PRINT_PREF "Failed by compare data %x %x", testbuf2[aaa], correctbuf[aaa] );
					rc=1;
				}
			} else if( checkHWBCHStatus( mtd, testbuf2, BCH_UNCORRECTABLE) ){
				if( checkErrorBit( mtd, correctbuf, testbuf2, errormap) <= bch_method ) {
					printk( PRINT_PREF "Failed status %d\n",  checkErrorBit( mtd, correctbuf, testbuf2, errormap));
					rc=1;
				}
			} else {
				printk( PRINT_PREF "Test %d bits error successful\n", error_bit);
			}

		}
	}


	/*
	* Choose random error bits from 9 bit to 4200 bits
	* The result is should unrecoverable.
	*/
	for( test_count=0; test_count < 100; ++test_count) {

		do {
			error_bit = simple_rand() % bch_all_bits;
		} while( error_bit < bch_method );


		printk( PRINT_PREF "Start error bits[%d] test", error_bit);

		memcpy( testbuf, correctbuf, mtd->writesize + mtd->oobsize);
		memset( testbuf2, 0 , extra_data_pos);
		memset( errormap, 0, extra_data_pos);
		gen_error_data( mtd, testbuf, error_bit, errormap);

		erase_whole_device();
		write_raw( mtd, testbuf) ;

		rc= 0;
		mtd->read( mtd, addr0, mtd->writesize, &retlen, testbuf2);
		if( checkHWBCHStatus( mtd, testbuf2, BCH_CORRECTABLE_ERR) ){
			if( checkErrorBit( mtd, correctbuf, testbuf2, errormap) > bch_method ) {
				printk( PRINT_PREF "Failed by check HW BCH status" );
				rc=1;
			} else if( memcmp( testbuf2, correctbuf, mtd->writesize) != 0 ) {
				printk( PRINT_PREF "Failed by compare data %x %x", testbuf2[aaa], correctbuf[aaa] );
				rc=1;
			}
		} else if( checkHWBCHStatus( mtd, testbuf2, BCH_UNCORRECTABLE) ){
			if( checkErrorBit( mtd, correctbuf, testbuf2, errormap) <= bch_method ) {
				printk( PRINT_PREF "Failed status %d\n",  checkErrorBit( mtd, correctbuf, testbuf2, errormap));
				rc=1;
			}
		} else {
			printk( PRINT_PREF "Test %d bits error successful\n", error_bit);
		}

	}


	/*
	* Toggle all bits 
	*/
	error_bit= bch_all_bits;
	memcpy( testbuf, correctbuf, mtd->writesize + mtd->oobsize);
	memset( testbuf2, 0 , extra_data_pos);

	toggleAllbits( mtd, testbuf);
	erase_whole_device();

	write_raw( mtd, testbuf);

	mtd->read( mtd, addr0, mtd->writesize, &retlen, testbuf2);
	if (checkHWBCHStatus( mtd, testbuf2, BCH_UNCORRECTABLE) == 0 ) {
		printk( PRINT_PREF "Failed by check HW BCH status" );
		rc=1;
	} else if( memcmp( testbuf2, correctbuf, mtd->writesize) == 0 ){
		printk( PRINT_PREF "Failed by compare data" );
		rc=1;
	} else {
		printk( PRINT_PREF "Test %d bits error successful\n", error_bit);
	}

	if( rc ) {
		printk( PRINT_PREF "Test %d bits error failed\n", error_bit);
		++err;
		goto out;
	}

	printk( PRINT_PREF "Congradulations!! Test successfully\n");

	return err;

out:
	printk( "correct data ===================\n");
	ni_dm_byte( correctbuf, mtd->writesize+ mtd->oobsize );
	printk( "read from data ===================\n");
	ni_dm_byte( testbuf2, mtd->writesize);
//	printk( "error map  ===================\n");
//	ni_dm_byte( errormap, BCH_ALL_SIZE); 

	return err;
}

static const unsigned char known_pattern[] = {
	0x67,0xC6,0x69,0x73,0x51,0xFF,0x4A,0xEC,0x29,0xCD,0xBA,0xAB,0xF2,0xFB,0xE3,0x46,
	0x7C,0xC2,0x54,0xF8,0x1B,0xE8,0xE7,0x8D,0x76,0x5A,0x2E,0x63,0x33,0x9F,0xC9,0x9A,
	0x66,0x32,0x0D,0xB7,0x31,0x58,0xA3,0x5A,0x25,0x5D,0x05,0x17,0x58,0xE9,0x5E,0xD4,
	0xAB,0xB2,0xCD,0xC6,0x9B,0xB4,0x54,0x11,0x0E,0x82,0x74,0x41,0x21,0x3D,0xDC,0x87,
	0x70,0xE9,0x3E,0xA1,0x41,0xE1,0xFC,0x67,0x3E,0x01,0x7E,0x97,0xEA,0xDC,0x6B,0x96,
	0x8F,0x38,0x5C,0x2A,0xEC,0xB0,0x3B,0xFB,0x32,0xAF,0x3C,0x54,0xEC,0x18,0xDB,0x5C,
	0x02,0x1A,0xFE,0x43,0xFB,0xFA,0xAA,0x3A,0xFB,0x29,0xD1,0xE6,0x05,0x3C,0x7C,0x94,
	0x75,0xD8,0xBE,0x61,0x89,0xF9,0x5C,0xBB,0xA8,0x99,0x0F,0x95,0xB1,0xEB,0xF1,0xB3,
	0x05,0xEF,0xF7,0x00,0xE9,0xA1,0x3A,0xE5,0xCA,0x0B,0xCB,0xD0,0x48,0x47,0x64,0xBD,
	0x1F,0x23,0x1E,0xA8,0x1C,0x7B,0x64,0xC5,0x14,0x73,0x5A,0xC5,0x5E,0x4B,0x79,0x63,
	0x3B,0x70,0x64,0x24,0x11,0x9E,0x09,0xDC,0xAA,0xD4,0xAC,0xF2,0x1B,0x10,0xAF,0x3B,
	0x33,0xCD,0xE3,0x50,0x48,0x47,0x15,0x5C,0xBB,0x6F,0x22,0x19,0xBA,0x9B,0x7D,0xF5,
	0x0B,0xE1,0x1A,0x1C,0x7F,0x23,0xF8,0x29,0xF8,0xA4,0x1B,0x13,0xB5,0xCA,0x4E,0xE8,
	0x98,0x32,0x38,0xE0,0x79,0x4D,0x3D,0x34,0xBC,0x5F,0x4E,0x77,0xFA,0xCB,0x6C,0x05,
	0xAC,0x86,0x21,0x2B,0xAA,0x1A,0x55,0xA2,0xBE,0x70,0xB5,0x73,0x3B,0x04,0x5C,0xD3,
	0x36,0x94,0xB3,0xAF,0xE2,0xF0,0xE4,0x9E,0x4F,0x32,0x15,0x49,0xFD,0x82,0x4E,0xA9,
	0x08,0x70,0xD4,0xB2,0x8A,0x29,0x54,0x48,0x9A,0x0A,0xBC,0xD5,0x0E,0x18,0xA8,0x44,
	0xAC,0x5B,0xF3,0x8E,0x4C,0xD7,0x2D,0x9B,0x09,0x42,0xE5,0x06,0xC4,0x33,0xAF,0xCD,
	0xA3,0x84,0x7F,0x2D,0xAD,0xD4,0x76,0x47,0xDE,0x32,0x1C,0xEC,0x4A,0xC4,0x30,0xF6,
	0x20,0x23,0x85,0x6C,0xFB,0xB2,0x07,0x04,0xF4,0xEC,0x0B,0xB9,0x20,0xBA,0x86,0xC3,
	0x3E,0x05,0xF1,0xEC,0xD9,0x67,0x33,0xB7,0x99,0x50,0xA3,0xE3,0x14,0xD3,0xD9,0x34,
	0xF7,0x5E,0xA0,0xF2,0x10,0xA8,0xF6,0x05,0x94,0x01,0xBE,0xB4,0xBC,0x44,0x78,0xFA,
	0x49,0x69,0xE6,0x23,0xD0,0x1A,0xDA,0x69,0x6A,0x7E,0x4C,0x7E,0x51,0x25,0xB3,0x48,
	0x84,0x53,0x3A,0x94,0xFB,0x31,0x99,0x90,0x32,0x57,0x44,0xEE,0x9B,0xBC,0xE9,0xE5,
	0x25,0xCF,0x08,0xF5,0xE9,0xE2,0x5E,0x53,0x60,0xAA,0xD2,0xB2,0xD0,0x85,0xFA,0x54,
	0xD8,0x35,0xE8,0xD4,0x66,0x82,0x64,0x98,0xD9,0xA8,0x87,0x75,0x65,0x70,0x5A,0x8A,
	0x3F,0x62,0x80,0x29,0x44,0xDE,0x7C,0xA5,0x89,0x4E,0x57,0x59,0xD3,0x51,0xAD,0xAC,
	0x86,0x95,0x80,0xEC,0x17,0xE4,0x85,0xF1,0x8C,0x0C,0x66,0xF1,0x7C,0xC0,0x7C,0xBB,
	0x22,0xFC,0xE4,0x66,0xDA,0x61,0x0B,0x63,0xAF,0x62,0xBC,0x83,0xB4,0x69,0x2F,0x3A,
	0xFF,0xAF,0x27,0x16,0x93,0xAC,0x07,0x1F,0xB8,0x6D,0x11,0x34,0x2D,0x8D,0xEF,0x4F,
	0x89,0xD4,0xB6,0x63,0x35,0xC1,0xC7,0xE4,0x24,0x83,0x67,0xD8,0xED,0x96,0x12,0xEC,
	0x45,0x39,0x02,0xD8,0xE5,0x0A,0xF8,0x9D,0x77,0x09,0xD1,0xA5,0x96,0xC1,0xF4,0x1F,
};

static const unsigned char known_pattern_bch[]= {
	0xAA,0x6D,0xAE,0x6F,0x74,0x45,0xD4,0x6B,0x9E, 0xBC,0xEA, 0x57, 0x51
};

static int __init mtd_bchtest_init(void)
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

//	simple_srand( 379 );
	simple_srand( current->pid );
	printk( PRINT_PREF "Random seed is %d !\n"
			"Repeat test with the same data! Feed the same seed again!\n"
			, current->pid );

	err = scan_for_bad_eraseblocks();
	if (err) {
		goto out;
	}

	addr0 = 0;
	//for (i = 0; i < ebcnt && bbt[i]; ++i)
	//	addr0 += mtd->erasesize;

	printk( PRINT_PREF " addr0=[0x%x]\n", addr0);

#define EXTAR_SIZE 64
	correctbuf = kzalloc( mtd->writesize + mtd->oobsize  + EXTAR_SIZE , GFP_KERNEL );
	testbuf = kzalloc( mtd->writesize + mtd->oobsize + EXTAR_SIZE, GFP_KERNEL ); 
	testbuf2 = kzalloc( mtd->writesize + mtd->oobsize + EXTAR_SIZE, GFP_KERNEL ); 
	errormap= kzalloc( mtd->writesize + mtd->oobsize + EXTAR_SIZE, GFP_KERNEL ); 
	
	if ( !correctbuf || !testbuf || !testbuf2 || !errormap ) {
		printk(PRINT_PREF "error: cannot allocate memory\n");
		goto out;
	}

	extra_data_pos = mtd->writesize + mtd->oobsize;

	// Ensure BCH is at right location first.
	memset( correctbuf, 0, mtd->writesize + mtd->oobsize );
	for( i =0 ; i < 16 ; ++i)  {
		memcpy( correctbuf + i*512, known_pattern, sizeof( known_pattern));
	}

#if 0

	mtd->write( mtd, addr0, mtd->writesize, &retlen, correctbuf);
	mtd->read( mtd, addr0, mtd->writesize, &retlen, testbuf2 );


	erase_whole_device();
	mtd->write( mtd, addr0, mtd->writesize, &retlen, correctbuf);
	mtd->read( mtd, addr0, mtd->writesize, &retlen, testbuf2 );


	if( read_raw( mtd, testbuf) != 0 || memcmp( testbuf, testbuf2, mtd->writesize) != 0 
			|| memcmp( testbuf, correctbuf, mtd->writesize) != 0 ) {

		printk( PRINT_PREF "What we read is different with what we get!");
		err++;
		goto out;
	} 


	printk( PRINT_PREF "test buf %p\n", testbuf);
	for( i = 0; i < sizeof( known_pattern_bch) ; ++i) {
		if ( testbuf[ mtd->writesize + mtd->ecclayout->eccpos[ i ] ] !=  known_pattern_bch[ i ] ) {
			printk( PRINT_PREF "value %d %d %02X %02X\n", 
					mtd->ecclayout->eccpos[ i ],
					mtd->writesize + mtd->ecclayout->eccpos[ i ],
					testbuf[ mtd->writesize + mtd->ecclayout->eccpos[ i ] ], known_pattern_bch[ i ]);
			printk( PRINT_PREF "BCH calaulate error! 2\n");
		}
	}

#endif
	// generate random data and start test
	//set_random_data( correctbuf, mtd->writesize);

	for( i =0; i < testno; ++i ) {
		if(  bch_test_run() != 0) {
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
module_init(mtd_bchtest_init);

static void __exit mtd_bchtest_exit(void)
{
	return;
}
module_exit(mtd_bchtest_exit);

MODULE_DESCRIPTION("BCH Algorigthm Verify Module");
MODULE_AUTHOR("Peter Lee");
MODULE_LICENSE("GPL");
