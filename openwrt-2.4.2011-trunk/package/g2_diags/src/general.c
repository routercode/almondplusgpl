/* $Id: general.c,v 1.1 2011/07/12 18:47:37 acarr Exp $
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>

#include "general.h"
#include "errorlog.h"

#define MAX_CHARS 100
#define MAX_SEARCH 500
int search_count=0;
char *search_path[MAX_SEARCH];

//
// Get timestamp in seconds
//
double timestamp(void)
{
	struct timeval t;

	gettimeofday(&t, NULL);

	return ((double) t.tv_sec + (double) t.tv_usec * 1E-6);
}


//
// wrapper of strncpy so it behaves a little nicer about always putting a trailing null in the string
//
char *tr_strncpy(char *dest, const char *src, size_t n)
{
	/* unconditionally put a null in last position of dest */
	dest[n-1] = '\0';
	/* do strncpy with one less byte so it won't overwrite that last null */
	return strncpy(dest, src, n-1);
}


/* Remove all leading spaces from the string and return the string back */
void remove_leading_spaces(char *string)
{
    char *temp;
    char *head;
    head=strdup(string);
    temp = head;

    /* Make sure we don't have a NULL on our hands */
    if (string==NULL) return;

    /* Make sure the current letter of the string is a space to keep on
     * incrementing 
     */
    while (temp[0]==32)
        temp++;

    /* Copy the new string back in *string.  We will be wastin bytes... but shit
     * if Micro$oft can waste gigabytes we'll take a few bytes... :)
     */
    strcpy(string,temp);

    free(head);
    return;
}


void remove_new_lines(char *string)
{
    if (string==NULL) return;

    while (string[0]!=0)
    {
        if ((*string==0xa) || (*string==0xd))
        {
            string[0]=0;
            return;
        }
        string++;
    }

}

/* get_item
 *
 * This function gets token item_number from a string separated
 * by a delimiter delim, and returns that item
 */
char *get_item(char *string, int item_number, const char *delim)
{
    char *copy,*out;
    char *temp=NULL;
    char *start=NULL;
    int loop;

    /* First make a copy of the string */
    copy=strdup(string);
    if (copy==NULL) return NULL;

    start=copy;

    for (loop=0;loop<=item_number;loop++) {
        if (temp!=NULL) {
            start=temp+1;
            temp++;
        }

        temp=strstr(start,delim);

        /* Return if we hit a null too soon, but make
         * sure that if we are on the last one that
         * it is okay to get to the end of the string
         */
        if (temp==NULL && loop!=item_number)
            return NULL;
    }

    /* Make sure we don't write at location 0 */
    if (temp!=NULL)
        temp[0]=0;
    remove_new_lines(start);
    out=strdup(start);
    free(copy);

    return out;
}

void s_upper(char *str)
{
    int max=0;

    while (*str) {
        (*str)=toupper(*str);
        str++;
        max++;
        if (max>=MAX_CHARS) return;
    }
}

void s_lower(char *str)
{
    int max=0;

    while (*str) {
        (*str)=tolower(*str);
        str++;
        max++;
        if (max>=MAX_CHARS) return;
    }
}

