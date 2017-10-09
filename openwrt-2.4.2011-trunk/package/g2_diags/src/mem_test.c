/***********************************************************************/
/* This file contains unpublished documentation and software           */
/* proprietary to Cortina Systems Incorporated. Any use or disclosure, */
/* in whole or in part, of the information in this file without a      */
/* written consent of an officer of Cortina Systems Incorporated is    */
/* strictly prohibited.                                                */
/* Copyright (c) 2009 by Cortina Systems Incorporated.                 */
/***********************************************************************/

#include <stdio.h>
#include "post.h"

#define POST_MEM_MAX_ERROR      0x0
static unsigned int g_test_mode;
static unsigned int *load_start;
static unsigned int *store_start;

extern unsigned int gp_value;



#define REG_READ( _register_ ) \
   *((volatile unsigned int *)(_register_))

#define REG_READ_UINT32( _register_, _value_ ) \
        ((_value_) = *((volatile unsigned int *)(_register_)))

#define REG_WRITE_UINT32( _register_, _value_ ) \
        (*((volatile unsigned int *)(_register_)) = (_value_))

#define REG_WRITE_UINT16( _register_, _value_ ) \
        (*((volatile unsigned short *)(_register_)) = (_value_))

#define REG_WRITE_UINT8( _register_, _value_ ) \
        (*((volatile unsigned char *)(_register_)) = (_value_))


extern unsigned char div_value (unsigned int sum, unsigned divider);

void run_print_meter (void)
{
    static int mi = 0;

    printf("\b");


    if (mi < 8) {
        printf("|");
    } else if (mi < 16) {
        printf("/");
    } else if (mi < 24) {
        printf("-");
    } else {
        printf("\\");
    }

    mi++;
    mi %= 32;

    return;
}


void init_print_meter(void) 
{
    printf(" ");
    return;
}


void clear_print_meter(void) 
{
    printf("\b");
    printf(" ");
    printf("\b");
    return;
}


static void serial_error_print (char *str, unsigned int addr, unsigned int val, unsigned int exp) 
{
    printf("%s %4.4x val %4.4x exp %4.4x\n",
		    str,addr, val, exp);
}


/* 
 * mem_test32
 * 
 */
static int mem_test32(void * test_start, unsigned int test_size) {
    unsigned int * p = (unsigned int *) test_start;
    unsigned int i;
    int erro_count = 0;

    init_print_meter();
    for (i = 0; i < (test_size >> 2); i++) {
        run_print_meter();
        REG_WRITE_UINT32(&p[i], 0xaa55aa55);
        if (p[i] != 0xaa55aa55) {
            printf("32Bit Word Error 1 addr %4.4x %4.4x %4.4x",
                               (unsigned int) &p[i], p[i], 0xaa55aa55);
            erro_count++;
            goto finished;
        }
        REG_WRITE_UINT32(&p[i], 0x55aa55aa);
        if (p[i] != 0x55aa55aa) {
            printf("32Bit Word Error 2 addr %4.4x %4.4x %4.4x",
                              (unsigned int)  &p[i], p[i], 0x55aa55aa);
            erro_count++;
            goto finished;
        }
    }

    for (i = 0; i < (test_size >> 2); i++) {
        run_print_meter();
        REG_WRITE_UINT32(&p[i], 0xaa55aa55);
    }
    for (i = 0; i < (test_size >> 2); i++) {
        run_print_meter();
        if (p[i] != 0xaa55aa55) {
            printf("32Bit Word Error 1 addr %4.4x %4.4x %4.4x",
                               (unsigned int) &p[i], p[i], 0xaa55aa55);
            erro_count++;
            goto finished;
        }
    }

    for (i = 0; i < (test_size >> 2); i++) {
        run_print_meter();
        REG_WRITE_UINT32(&p[i], 0x55aa55aa);
    }
    for (i = 0; i < (test_size >> 2); i++) {
        run_print_meter();
        if (p[i] != 0x55aa55aa) {
            printf("32Bit Word Error 2 addr %4.4x %4.4x %4.4x",
                               (unsigned int)&p[i], p[i], 0x55aa55aa);
            erro_count++;
            goto finished;
        }
    }
        
    for (i = 0; i < (test_size >> 2); i++) {
        run_print_meter();
        REG_WRITE_UINT32(&p[i], (unsigned int)(&p[i]));
    }
    for (i = 0; i < (test_size >> 2); i++) {
        run_print_meter();
        if (p[i] != (unsigned int) (&p[i])) {
            printf("32Bit Word Error 3 addr %4.4x %4.4x %4.4x",
                               (unsigned int)&p[i], p[i], (unsigned int) (&p[i]));
            erro_count++;
            goto finished;
        }
    }
        
 finished:
    clear_print_meter();
    return erro_count;
}

static int mem_test16(void * test_start, unsigned int test_size) {
    unsigned short int * p = (unsigned short int *) test_start;
    int i;
    int erro_count = 0;

    init_print_meter();
    for (i = 0; i < (test_size >> 1); i++) {
        run_print_meter();
        REG_WRITE_UINT16(&p[i], 0xaa55);
        if (p[i] != 0xaa55) {
            printf("16Bit Halfword Error 1 addr %4.4x %4.4x %4.4x",
                         (unsigned int) &p[i], p[i], 0xaa55);
            erro_count++;
            goto finished;
        }
        REG_WRITE_UINT16(&p[i], 0x55aa);
        if (p[i] != 0x55aa) {
            printf("16Bit Halfword Error 2 addr %4.4x %4.4x %4.4x",
                         (unsigned int) &p[i], p[i], 0x55aa);
            erro_count++;
            goto finished;
        }
    }

    for (i = 0; i < (test_size >> 1); i++) {
        run_print_meter();
        REG_WRITE_UINT16(&p[i], 0xaa55);
    }

    for (i = 0; i < (test_size >> 1); i++) {
        run_print_meter();
        if (p[i] != 0xaa55) {
            printf("16Bit Halfword Error 1 addr %4.4x %4.4x %4.4x ",
                         (unsigned int)&p[i], p[i], 0xaa55);
            erro_count++;
            goto finished;
        }
    }

    for (i = 0; i < (test_size >> 1); i++) {
        run_print_meter();
        REG_WRITE_UINT16(&p[i], 0x55aa);
    }

    for (i = 0; i < (test_size >> 1); i++) {
        run_print_meter();
        if (p[i] != 0x55aa) {
            printf("16Bit Halfword Error 2 addr %4.4x %4.4x %4.4x",
                         (unsigned int)&p[i], p[i], 0x55aa);
            erro_count++;
            goto finished;
        }
    }

    for (i = 0; i < (test_size >> 2); i++) {
        run_print_meter();
        REG_WRITE_UINT16(&p[i<<1], ((unsigned int)(&p[i<<1])) & 0xffff);
        REG_WRITE_UINT16(&p[(i<<1) + 1], (((unsigned int)(&p[i<<1]))>> 16) & 0xffff);
    }
    for (i = 0; i < (test_size >> 2); i++) {
        run_print_meter();
        if (p[i<<1] != (((unsigned int) (&p[i<<1])) & 0xffff)) {
            printf("16Bit Halfword Error 3 addr %4.4x %4.4x %4.4x",
                               (unsigned int)&p[i<<1], p[i<<1], 
                               (((unsigned int) (&p[i<<1])) & 0xffff));
            erro_count++;
            goto finished;
        }
        if (p[(i<<1) + 1] != (((unsigned int) (&p[i<<1]) >> 16) & 0xffff)) {
            printf("16Bit Halfword Error 4 addr %4.4x %4.4x %4.4x",
                               (unsigned int)&p[(i<<1) + 1], p[(i<<1) + 1], 
                               (((unsigned int) (&p[i<<1]) >> 16) & 0xffff));
            erro_count++;
            goto finished;
        }
    }

 finished:
    clear_print_meter();
    return erro_count;
}

static int mem_test8(void * test_start, unsigned int test_size) {

    char * p = (char *) test_start;
    int i;
    int erro_count = 0;

    init_print_meter();
    for (i = 0; i < test_size; i++) {
        run_print_meter();
        REG_WRITE_UINT8(&p[i], 0xaa);
        if (p[i] != 0xaa) {
                printf("Byte Error 1 addr %4.4x %4.4x %4.4x", 
                            (unsigned int) &p[i], p[i], 0xaa);
                erro_count++;
                goto finished;
        }
        REG_WRITE_UINT8(&p[i], 0x55);
        if ( p[i] != 0x55) {
            printf("Byte Error 2 addr %4.4x %4.4x %4.4x", 
                         (unsigned int)&p[i], p[i], 0x55);
                erro_count++;
                goto finished;
        }
    }

    for (i = 0; i < test_size; i++) {
        run_print_meter();
        REG_WRITE_UINT8(&p[i], 0xaa);
    }
    for (i = 0; i < test_size; i++) {
        run_print_meter();
        if (p[i] != 0xaa) {
                printf("Byte Error 1 addr %4.4x %4.4x %4.4x", 
                             (unsigned int)&p[i], p[i], 0xaa);
                erro_count++;
                goto finished;
        }
    }

    for (i = 0; i < test_size; i++) {
        run_print_meter();
        REG_WRITE_UINT8(&p[i], 0x55);
    }

    for (i = 0; i < test_size; i++) {
        run_print_meter();
        if ( p[i] != 0x55) {
            printf("Byte Error 2 addr %4.4x %4.4x %4.4x", 
                            (unsigned int) &p[i], p[i], 0x55);
            erro_count++;
            goto finished;
        }
    }

    for (i = 0; i < (test_size >> 2); i++) {
        run_print_meter();
        REG_WRITE_UINT8(&p[i<<2], ((unsigned int)(&p[i<<2])) & 0xff);
        REG_WRITE_UINT8(&p[(i<<2) + 1], (((unsigned int)(&p[i<<2]))>> 8) & 0xff);
        REG_WRITE_UINT8(&p[(i<<2) + 2], (((unsigned int)(&p[i<<2]))>> 16) & 0xff);
        REG_WRITE_UINT8(&p[(i<<2) + 3], (((unsigned int)(&p[i<<2]))>> 24) & 0xff);
    }
    
    for (i = 0; i < (test_size >> 2); i++) {
        run_print_meter();
        if (p[i<<2] != (((unsigned int) (&p[i<<2])) & 0xff)) {
            printf("Byte Error 3 addr %4.4x %4.4x %4.4x", 
                               (unsigned int)&p[i<<2], p[i<<2], 
                               (((unsigned int) (&p[i<<2])) & 0xff));
            erro_count++;
            goto finished;
        }
        if (p[(i<<2) + 1] != (((unsigned int) (&p[i<<2]) >> 8) & 0xff)) {
            printf("Byte Error 4 addr %4.4x %4.4x %4.4x", 
                               (unsigned int)&p[(i<<2)+1], p[(i<<2) + 1], 
                               (((unsigned int) (&p[i<<2]) >> 8) & 0xff));
            erro_count++;
            goto finished;
        }
        if (p[(i<<2) + 2] != (((unsigned int) (&p[i<<2]) >> 16) & 0xff)) {
            printf("Byte Error 5 addr %4.4x %4.4x %4.4x", 
                               (unsigned int)&p[(i<<2)+2], p[(i<<2) + 2], 
                               (((unsigned int) (&p[i<<2]) >> 16) & 0xff));
            erro_count++;
            goto finished;
        }
        if (p[(i<<2) + 3] != (((unsigned int) (&p[i<<2]) >> 24) & 0xff)) {
            printf("Byte Error 6 addr %4.4x %4.4x %4.4x", 
                               (unsigned int)&p[(i<<2)+3], p[(i<<2) + 3], 
                               (((unsigned int) (&p[i<<2]) >> 24) & 0xff));
            erro_count++;
            goto finished;
        }
    }

 finished:
    clear_print_meter();
    return erro_count;
}


/* 
 * mem_data_bus_test:
 *     Walk 1 test. Walk 1 on a given address to see 
 * the data bus for the memory are all correct.
 */
int mem_rand_wr_test(unsigned int *test_start, 
                     unsigned int test_size, 
                     unsigned char *wr_count,
                     unsigned int  size)
{
    int ir, jr, idx;
    unsigned int *start_addr;

    init_print_meter();
    for (ir = 0; ir < size; ir++) {
        idx = wr_count[ir];
        start_addr = &test_start[idx]; 

        for (jr = 0; jr < (int)wr_count[ir]; jr++) {
            run_print_meter();
            /* Write the value to given address */
            start_addr[jr] = 0x0;
            start_addr[jr] = (0x5a5a * wr_count[ir] + jr);
        }

        for (jr = 0; jr < (int)wr_count[ir]; jr++) {
            run_print_meter();
            /* Read the value to given address */
            if (start_addr[jr] != (0x5a5a * wr_count[ir] + jr)) {
                clear_print_meter();
                printf("Address:%4.4x set_value:%4.4x get_value:%4.4x\n",
				((unsigned int)&start_addr[jr]), 
				(0x5a5a * wr_count[ir] + jr), 
				start_addr[jr]);
                return (1);
            }
        }
    }

    clear_print_meter();
    return (0);
}



int mem_pattern_wr_test(unsigned int *test_start, 
                        unsigned int test_size, 
                        unsigned int iteration,
                        unsigned int pattern,
                        unsigned int wr_len,
                        unsigned int w_loop,
                        unsigned int r_loop)
{
    int ir, j0, j1;
    unsigned int *start_addr;
    unsigned int len;

    init_print_meter();
    for (ir = 0; ir < iteration; ir++) {
        start_addr = test_start; 

        len = test_size >> 2;

        for (j1 = 0; j1 < w_loop; j1++) {
            for (j0 = 0; j0 < len; j0++) {
                run_print_meter();

                if (wr_len) {
                    start_addr[j0] = pattern;
                    if (start_addr[j0] != pattern) {
                        clear_print_meter();
                        printf("Address %4.4x set_value:%4.4x get_value:%4.4x\n", 
					(unsigned int)&start_addr[j0],
					pattern,
                        		start_addr[j0]);
                        return (1);
                    }
                } else {
                    start_addr[j0] = pattern;
                }
            }
        }

        if (wr_len == 0) {
            for (j1 = 0; j1 < r_loop; j1++) {
                for (j0 = 0; j0 < len; j0++) {
                    run_print_meter();
                    /* Read the value to given address */
                    if (start_addr[j0] != pattern) {
                        clear_print_meter();
                        printf("Address:%4.4x set_value:%4.4x get_value:%4.4x\n",
					(unsigned int)&start_addr[j0], 
					pattern,
                        		start_addr[j0]);
                        return (1);
                    }
                }
            }
        }
    }

    clear_print_meter();
    return (0);
}



/* 
 * mem_data_bus_test:
 *     Walk 1 test. Walk 1 on a given address to see 
 * the data bus for the memory are all correct.
 */
static int mem_data_bus_test(volatile  unsigned int *test_start, int flag)
{
    int value;
    int rvalue;
    
    init_print_meter();
    for (value = 1; value != 0; value <<= 1) {
        run_print_meter();
        /* Write the value to given address */
        if (flag) {
            rvalue = value;
        } else {
            rvalue = !value;
        }
        *test_start = rvalue;

        /* Read the value and check */
        if (*test_start != rvalue)  {
            clear_print_meter();
            printf("Address:%4.4x set_value:%4.4x get_value:%4.4x\n", 
			    (unsigned int)test_start, 
			    rvalue, 
			    *test_start);
            return (1);
        }
    }

    clear_print_meter();
    return (0);
}

/* 
 * mem_address_bus_test:
 *     Walk 1 on the relevent bits on the address
 */
static int
mem_address_bus_test(volatile unsigned int *test_start, unsigned int test_size)
{
    unsigned int addressMask = (test_size/sizeof(int) - 1);
    unsigned int offset;
    unsigned int test_offset;

    int value0 = (int) 0xAAAAAAAA;
    int value1 = (int) 0x55555555;


    /*
     * Write the default value0 at each of the power-of-two offsets.
     */
    for (offset = 1; (offset & addressMask) != 0; offset <<= 1) {
        run_print_meter();
        test_start[offset] = value0;
    }

    /* 
     * Check for address bits stuck high.
     */
    test_offset = 0;
    test_start[test_offset] = value1;

    for (offset = 1; (offset & addressMask) != 0; offset <<= 1)  {
        run_print_meter();
        if (test_start[offset] != value0) {
            return 1;
        }
    }

    test_start[test_offset] = value0;

        /*
         * Check for address bits stuck low or shorted.
         */
        for (test_offset = 1; (test_offset & addressMask) != 0; test_offset <<= 1) {
            run_print_meter();
            test_start[test_offset] = value1;
            
            if (test_start[0] != value0) {
                return 1;
            }
            
            for (offset = 1; (offset & addressMask) != 0; offset <<= 1) {
                run_print_meter();
                if ((test_start[offset] != value0) && 
                    (offset != test_offset)) {
                    return 1;
                }
            }
            
            test_start[test_offset] = value0;
        }

    return (0);
}  


/* 
 * mem_device_test:
 *     Test the memory device location by writing a pattern and reading 
 * then revert the value for the same location and read.
 * This test should be done for the full memory region.
 */
static int
mem_device_test(volatile unsigned int *test_start, unsigned int test_size)
{
    unsigned int offset;
    unsigned int nWords = test_size / sizeof(int);

    int value0;
    int value1;
    
    /* Write test */
    for (value0 = 1, offset = 0; offset < nWords; value0++, offset++) {
        run_print_meter();
        test_start[offset] = value0;
    }
    
    /* Check last write and invert */
    for (value0 = 1, offset = 0; offset < nWords; value0++, offset++) {
        run_print_meter();
        if (test_start[offset] != value0) {
            return 1;
        }
        
        value1 = ~value0;
        test_start[offset] = value1;
    }

        /* check the inverted value */
        for (value0 = 1, offset = 0; offset < nWords; value0++, offset++) {
            run_print_meter();
            value1 = ~value0;
            if (test_start[offset] != value1) {
                return 1;
            }
        }
    
    return (0);

}

/*
 * mem_test_burst:
 *     Memory access load and store in burst instruction.
 */
static int
mem_test_burst(unsigned int *test_start, unsigned int test_size)
{
    unsigned int offset;
    unsigned int new_test_size = (test_size>>3);
    unsigned int lcount = new_test_size;
    unsigned int lcount_asm = (new_test_size>>3);

    load_start = test_start;
    store_start = (unsigned int *) (((unsigned char *) test_start) + (new_test_size * 4));
#if 0
    serial_puts("\nLoad addr: ");
    serial_put_hex((unsigned int)load_start);
    serial_puts(" Store addr: ");
    serial_put_hex((unsigned int)store_start);
    serial_puts(" lcount: ");
    serial_put_hex((unsigned int)lcount);
    serial_puts(" lcount_asm: ");
    serial_put_hex((unsigned int)lcount_asm);
    serial_puts(" \n");
#endif

    /* Fill pattern first */
    for (offset = 0; offset < lcount; offset++) {
        run_print_meter();
#if 0
        switch(offset%4) {
            case 0: REG_WRITE_UINT32(&load_start[offset], 0xaa55aa55); break;
            case 1: REG_WRITE_UINT32(&load_start[offset], 0x55aa55aa); break;
            case 2: REG_WRITE_UINT32(&load_start[offset], 0xffffffff); break;
            case 3: REG_WRITE_UINT32(&load_start[offset], 0x00000000); break;
        }
        REG_WRITE_UINT32(&store_start[offset], 0x00000000);
#endif
        REG_WRITE_UINT32(&load_start[offset], offset);
    }

    /* load and store burst mode */
    for (offset = 0; offset < lcount_asm; offset++) {
        run_print_meter();
        asm volatile (
            "MOV   r8, %[v0]\n\t"
            "MOV   r9, %[v1]\n\t"
            "LDMIA r8!, {r0-r7}\n\t"
            "STMIA r9!, {r0-r7}\n\t"
            :: [v0]"r" (load_start), [v1]"r" (store_start):"r0","r1","r2","r3","r4","r5","r6","r7","r8","r9"
            );
        load_start = (unsigned int *) ((unsigned char *)load_start + (0x8 * 0x4));
        store_start = (unsigned int *) ((unsigned char *)store_start + (0x8 * 0x4));
    }

    /* Read the two block and compare */
    load_start = test_start;
    store_start = (unsigned int *) (((unsigned char *) test_start) + (new_test_size * 4));

    for (offset = 0; offset < lcount; offset++) {
        run_print_meter();
        if (load_start[offset] != store_start[offset]) {
            printf("\nLoad location address:%4.4x value:%4.4x\n", 
			    (unsigned int)&load_start[offset], 
			    load_start[offset]);

            printf("Store location address:%4.4x value:%4.4x\n", 
			    (unsigned int)&store_start[offset], 
			    store_start[offset]);

            return (1);
        }
    }

    return (0);
}


/*
 * mem_test_burst:
 *     Memory access load and store in burst instruction.
 * 1.  1 word
 * 2.  2 word burst
 * 4.  4 word burst
 * 8.  8 word burst
 */
int
mem_test_burst_1(unsigned int *test_start, unsigned int test_size, 
                 unsigned int iteration, unsigned int pattern, 
                 unsigned int wr_idx)
{
    unsigned int offset;
    unsigned int new_test_size = (test_size>>3);
    unsigned int lcount = new_test_size;
    unsigned int lcount_asm;
    int iloop;


    for (iloop=0; iloop < iteration; iloop++) {

        load_start = test_start;
        store_start = (unsigned int *) (((unsigned char *) test_start) + (new_test_size * 4));

        /* Fill pattern first */
        for (offset = 0; offset < lcount; offset++) {
            run_print_meter();
            if (pattern) {
                REG_WRITE_UINT32(&load_start[offset], pattern);
            } else {
                REG_WRITE_UINT32(&load_start[offset], offset);
            }
        }
        
        /* load and store burst mode */
        switch (wr_idx) {
            case 1: lcount_asm = new_test_size; break;
            case 2: lcount_asm = new_test_size>>1; break;
            case 4: lcount_asm = new_test_size>>2; break;
            case 8: lcount_asm = new_test_size>>3; break;
        } 

        for (offset = 0; offset < lcount_asm; offset++) {
//            run_print_meter();
            switch (wr_idx) {
                case 1:
                    asm volatile (
                        "MOV   r8, %[v0]\n\t"
                        "MOV   r9, %[v1]\n\t"
                        "LDMIA r8!, {r0}\n\t"
                        "STMIA r9!, {r0}\n\t"
                        :: [v0]"r" (load_start), [v1]"r" (store_start):"r0","r8","r9"
                        );
                    break;
                case 2:
                    asm volatile (
                        "MOV   r8, %[v0]\n\t"
                        "MOV   r9, %[v1]\n\t"
                        "LDMIA r8!, {r0-r1}\n\t"
                        "STMIA r9!, {r0-r1}\n\t"
                        :: [v0]"r" (load_start), [v1]"r" (store_start):"r0","r1","r8","r9"
                        );
                    break;
                case 4:
                    asm volatile (
                        "MOV   r8, %[v0]\n\t"
                        "MOV   r9, %[v1]\n\t"
                        "LDMIA r8!, {r0-r3}\n\t"
                        "STMIA r9!, {r0-r3}\n\t"
                        :: [v0]"r" (load_start), [v1]"r" (store_start):"r0","r1","r2","r3","r8","r9"
                        );
                    break;
                case 8:
                    asm volatile (
                        "MOV   r8, %[v0]\n\t"
                        "MOV   r9, %[v1]\n\t"
                        "LDMIA r8!, {r0-r7}\n\t"
                        "STMIA r9!, {r0-r7}\n\t"
                        :: [v0]"r" (load_start), [v1]"r" (store_start):"r0","r1","r2","r3","r4","r5","r6","r7","r8","r9"
                        );
                    break;
            }
            load_start = (unsigned int *) ((unsigned char *)load_start + (wr_idx * 0x4));
            store_start = (unsigned int *) ((unsigned char *)store_start + (wr_idx *0x4));

        }
        
        /* Read the two block and compare */
        load_start = test_start;
        store_start = (unsigned int *) (((unsigned char *) test_start) + (new_test_size * 4));
        
        for (offset = 0; offset < lcount; offset++) {
            run_print_meter();
            if (load_start[offset] != store_start[offset]) {

                printf("\nLoad location address:%4.4x value:%4.4x\n", 
				(unsigned int)&load_start[offset], 
				load_start[offset]);

                printf("Store location address:%4.4x value:%4.4x\n",
                	(unsigned int)&store_start[offset],
                	store_start[offset]);
                
                return (1);
            }
        }
    }

    return (0);
}




/*
 * mem_test
 *   Main function to perform the memory test
 *   test_mode = 0 for short test; 1 for long test
 */
void g2_mem_test(unsigned int test_mode,
                      void *test_start,
                      unsigned int test_size,
                      char *mem_name) {

    int result;
    unsigned int tmp_test_size = test_size;

    /*
     *   MEMORY START AND TEST SIZE
     */
    g_test_mode = test_mode;


    printf("********************************************\n");
    printf("       Post Memory Test Report - %s\n",mem_name);
    printf("********************************************\n");
    printf("MEM Start Addres: %4.4x\n", (unsigned int)test_start);

    printf("MEM Test Size (given):%4.4x\n",test_size);

    /*
     * 1. memory data bus test
     * 2. memory address bus test
     * 3. memory device test 
     * From: 
     *   http://www.netrino.com/Embedded-Systems/How-To/Memory-Test-Suite-C
     */


    /* 1. memory data bus test walk 1\'s */
    printf("1. memory data bus test walk 1\'s: ");
    init_print_meter();
    result = mem_data_bus_test((unsigned int *)gp_value, 1);
    clear_print_meter();
    if (result > POST_MEM_MAX_ERROR) { 
        printf("Failed\n");
        goto finished;
    }
    printf("Passed\n");

    /* 1.1 memory data bus test walk 0\'s */
    printf("1.1 memory data bus test walk 0\'s: ");
    init_print_meter();
    result = mem_data_bus_test((unsigned int *)gp_value, 0);
    clear_print_meter();
    if (result > POST_MEM_MAX_ERROR) { 
        printf("Failed\n");
        goto finished;
    }
    printf("Passed\n");

    printf("1.2 memory load/store burst test: ");
    init_print_meter();
    result = mem_test_burst((unsigned int *)gp_value, tmp_test_size);
    clear_print_meter();

    if (result > POST_MEM_MAX_ERROR) {
        printf("Failed\n");
        goto finished;
    }    
    printf("Passed\n");

    
    /* 2. memory address bus test */
    printf("2. memory address bus test: ");
    init_print_meter();
    result = mem_address_bus_test((unsigned int *)gp_value, test_size);
    clear_print_meter();
    if (result > POST_MEM_MAX_ERROR) {
        printf("Failed\n");
        goto finished;
    }
    printf("Passed\n");

    /* 3. memory device test */
    printf("3. memory device test: ");
    init_print_meter();
    result = mem_device_test((unsigned int *)gp_value, test_size);
    clear_print_meter();
    if (result > POST_MEM_MAX_ERROR) {
        printf("Failed\n");
        goto finished;
    }
    printf("Passed\n");

    // If test mode==2 then we are doing 32-bit accesses only and no 16 or 8
    // bit tests
    if (test_mode!=2) { 
    	/* 4. memory byte test */
    	printf("4. memory byte test: ");
    	result += mem_test8((unsigned int *)gp_value, test_size);
    	if (result > POST_MEM_MAX_ERROR) {
        	printf("Failed\n");
        	goto finished;
	}
    	printf("Passed\n");

    	/* 5. memory half word test */
    	printf("5. memory half word test: ");
    	result += mem_test16((unsigned int *)gp_value, test_size);
    	if (result > POST_MEM_MAX_ERROR) {
        	printf("Failed\n");
        	goto finished;
    	}
    	printf("Passed\n");
    }

    /* 6. memory word test */
    printf("6. memory word test: ");
    result += mem_test32((unsigned int *)gp_value, test_size);
    if (result > POST_MEM_MAX_ERROR) {
        printf("Failed\n");
        goto finished;
    }
    printf("Passed\n");



 finished:
    if (result >= 1) {
        printf("\nMEM Test FAILED  :(\n");
    } else {
        printf("\nMEM Test PASSED  :)\n");
    }
    printf("********************************************\n");
}

