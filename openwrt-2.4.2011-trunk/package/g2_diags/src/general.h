#ifndef GENERAL_H
#define GENERAL_H

#include <math.h>
#include <string.h>

//
// Get timestamp in seconds
//
double timestamp(void);


char *get_item(char *string, int item_number, const char *delim);
void remove_leading_spaces(char *string);
void remove_new_lines(char *string);

void s_upper(char *str);
void s_lower(char *str);

#endif
