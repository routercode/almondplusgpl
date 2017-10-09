#ifndef DDR_STATS_H
#define DDR_STATS_H
void gather_bw_stats(int count, int *port_bw);
void cs_ddr_status(int output, int sample_count);
void cs_ddr_error_read(int count);
#endif
