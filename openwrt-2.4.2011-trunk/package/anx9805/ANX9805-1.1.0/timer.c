//  ANALOGIX Company 
//  DP_TX Demo Firmware on SST89V58RD2
//  Version 1.0	2006/07/14

#include <sys/time.h>	// Joe20110923
#include "timer.h"
#include "mcu.h"
//#include "P89v662.h"
#include "I2C_intf.h"

BYTE th0_def;
char timer_done;
unsigned int timer_tick;
BYTE hh, mm, ss;

#if 0
void init_timer(BYTE intval)
{
    stop_timer();
    th0_def = intval;
    TH0 = intval;
    TL0 = 0x00;
    TMOD = TMOD & 0xf0 | 0x01;
    ET0 = 1;
    timer_done = 0;

    timer_tick = 0;
    hh = 0;
    mm = 0;
    ss = 0;
}

void timer_isr(void)
{
    timer_tick++;
    stop_timer();
    TH0 = th0_def;
    TL0 = 0x00;
    start_timer();
    if (!timer_done) 
    {
        timer_done = 1;
    }
    if (timer_tick % 125 == 0) 
    {
        if (timer_tick >= (125*524))
            timer_tick = 0;
        ss++;
        if (ss > 59) 
        {
            ss = 0;
            mm++;
            if(mm > 59) 
            {
                mm = 0;
                hh++;
            }
        }
    }
}
#endif

void delay_ms(int msec)
{
#if 0
    int i, j;

    for (i=0; i<n; i++) 
    {
        for (j=0; j<DELAY_FACTOR; j++)
    	    ;
    }
#endif
#if 0
	struct timeval	tv;
	int usec;
	long start_usec, delta_usec;

	gettimeofday(&tv, (void *)0);
	start_usec = tv.tv_sec * 1000000 + tv.tv_usec;

	if (0 == msec)
		return;

	usec = msec * 1000;	// micro-second
	while (1) {
		gettimeofday(&tv, (void *)0);
		delta_usec = tv.tv_sec * 1000000 + tv.tv_usec;
		if (delta_usec < start_usec) {
			start_usec = 0l;	// overflow
		}
		delta_usec -= start_usec;
		if (delta_usec > usec) {
			break;
		}
	};
#endif
   usleep(msec * 1000);	
}

void delay_us(unsigned int c)
{
#if 0
	unsigned int i;
	for(i = 0; i < c; i++)
		_NOP_;
#endif
	unsigned int count = c/1000;

	if (count == 0)
		count = 1;
	sleep(count);
}

