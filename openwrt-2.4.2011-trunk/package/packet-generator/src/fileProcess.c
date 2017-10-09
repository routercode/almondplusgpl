/*
 * Copyright (c) Cortina-Systems Limited 2010.  All rights reserved.
 *				CH Hsu <ch.hsu@cortina-systems.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "fileProcess.h"
#include "public.h"
#include "rawSocketHdr.h"


#define COMMENT_CHAR(c)	(*c == ';' || *c == '#' || (*c == '/' && c[1] == '/')) 
#define SPACE_CHAR(c)	(c == ' ' ||  c == '\t' ||  c == ':' ||  c == ',' ||  c == '.') 
#define EOL_CHAR(c)	(c == '\0' ||  c == '\r' ||  c == '\n') 

unsigned char		pkt_file_name[FILENAME_SIZE+1];

extern unsigned char *build_L2_packet(DESCRIPTION_INFO_T *info, unsigned char *cp, unsigned int *len);
extern unsigned char *build_L3_packet(DESCRIPTION_INFO_T *info, unsigned char *cp, unsigned int *len);
extern unsigned char *build_L4_packet(DESCRIPTION_INFO_T *info, unsigned char *cp, unsigned int *len);
extern unsigned char *build_L5_packet(DESCRIPTION_INFO_T *info, unsigned char *cp, unsigned int *len);
extern unsigned char *build_payload_packet(DESCRIPTION_INFO_T *info, unsigned char *cp,unsigned char *buf_1, unsigned int *len);
extern void build_L3_chksum(DESCRIPTION_INFO_T *info);
extern void build_L4_chksum(DESCRIPTION_INFO_T *info);

extern DESCRIPTION_INFO_T *pInfo;

//=================================================================
//                        Loacl Functions
//=================================================================


/*----------------------------------------------------------------------
* Keyword Definition
*
*	Level-1		Level-2			Level-3/L2	Level-3/L3		Level-3/L4  
*	-------		-------------	----------	----------      ----------  
*	Storlink	PacketType		DA			HeaderLength    Sport
*	begin		PacketLength	SA			SIP             Dport         
*	end			PacketNumber	VLAN		DIP             ...         
*				L2				EtherType	Option                
*				L3				PPPoE		TOS                      
*				L4							...                      
*				Payload
*----------------------------------------------------------------------*/

/* Function prototype */
// for keyword_1st_tbl
int parse_cortina(int argc, unsigned char *argv[]);
int parse_pkt_begin(int argc, unsigned char *argv[]);
int parse_pkt_end(int argc, unsigned char *argv[]);
// for keyword_2nd_tbl
int parse_PacketType(int argc, unsigned char *argv[]);
int parse_PacketLength(int argc, unsigned char *argv[]);
int parse_PacketNumber(int argc, unsigned char *argv[]);
int parse_swtxq(int argc, unsigned char *argv[]);
int parse_txdesc(int argc, unsigned char *argv[]);
int parse_txflag(int argc, unsigned char *argv[]);
int parse_txmtu(int argc, unsigned char *argv[]);
extern int parse_L2(int argc, unsigned char *argv[]);
extern int parse_L3(int argc, unsigned char *argv[]);
extern int parse_L4(int argc, unsigned char *argv[]);
extern int parse_L5(int argc, unsigned char *argv[]);
extern int parse_payload(int argc, unsigned char *argv[]);

KEYWORD_T keyword_1st_tbl[]=
{
	/* Name     	handler 	*/
	{"cortina",	    (void *)parse_cortina},
	{"begin",		(void *)parse_pkt_begin},
    {"end",			(void *)parse_pkt_end},
    {NULL,			NULL}
};

KEYWORD_T keyword_2nd_tbl[]=
{
	/* Name     		handler 	*/
	{"PacketType",		(void *)parse_PacketType},
	{"PacketLength",	(void *)parse_PacketLength},
	{"PacketNumber",	(void *)parse_PacketNumber},
	{"SWTXQ",			(void *)parse_swtxq},
	{"DESC",			(void *)parse_txdesc},
	{"TXFLAG",			(void *)parse_txflag},
	{"MTU",				(void *)parse_txmtu},
    {"L2",				(void *)parse_L2},
    {"L3",				(void *)parse_L3},
    {"L4",				(void *)parse_L4},
    {"L5",				(void *)parse_L5},
    {"Payload",			(void *)parse_payload},
    {NULL,			NULL}
};

/*----------------------------------------------------------------------
* parse_cortina
*----------------------------------------------------------------------*/
int parse_cortina(int argc, unsigned char *argv[])
{
	return 0;
}

/*----------------------------------------------------------------------
* parse_pkt_begin
*----------------------------------------------------------------------*/
int parse_pkt_begin(int argc, unsigned char *argv[])
{
	if (pInfo->pkt.begin)
		return -1;
		
	memset((char *)&pInfo->pkt, 0, sizeof(pInfo->pkt));
	memset((char *)&pInfo->L2, 0, sizeof(pInfo->L2));
	memset((char *)&pInfo->L3, 0, sizeof(pInfo->L3));
	memset((char *)&pInfo->L4, 0, sizeof(pInfo->L4));
	memset((char *)&pInfo->L5, 0, sizeof(pInfo->L5));
	memset((char *)&pInfo->Payload, 0, sizeof(pInfo->Payload));
	pInfo->pkt.begin = 1;
	pInfo->pkt.parse_level = PARSER_LEVEL_2;
	pInfo->curr_table = (KEYWORD_T *)&keyword_2nd_tbl[0];
	pInfo->pkt.number = 1;	// default 1
	return 0;
}

/*----------------------------------------------------------------------
* write_a_pkt_hex_file
* Description:	
*		Write the packet buffer into a target file
* Parameters :	
*		FILE *ofile		: output file for .txt file
*		char *pktbuf	: points to packet buffer
*		int size		: total size
* Return:
*		int				: return code.
*					  		0: OK, 
*					  		-1: Failed to open file
*					  		-2: Failed to Write data into destination file
*							-3: Size id zero
*							-4: No free buffer
*----------------------------------------------------------------------*/
int write_a_pkt_hex_file(FILE *ofile, char *in_pktbuf, unsigned int size)
{
	unsigned char	*in_ptr, *out_ptr;
	unsigned int	total, len;
	unsigned char	*output_pktbuf;

	if (!size || !in_pktbuf)
	{
		printf("Nothing to write!\n");
		return -3;
	}
	
	output_pktbuf = (unsigned char *)malloc(MAX_JUMBO_PKT_SIZE * 4 + 64);
	if (!output_pktbuf)
	{
		printf("Failed for malloc()!\n");
		return -4;
	}
	
	in_ptr = (unsigned char *)in_pktbuf;
	out_ptr = (unsigned char *)output_pktbuf;
	total = 0;
	
	// packet length
	total += sprintf((char *)out_ptr, "%02x\n", (size>>8) & 0xff);
	out_ptr  = output_pktbuf + total;
	total += sprintf((char *)out_ptr, "%02x\n", size & 0xff);
	out_ptr  = output_pktbuf + total;
	
	// packet data
	len = size;
	while (len--)
	{
		total += sprintf((char *)out_ptr, "%02x\n", *in_ptr++);
		out_ptr  = output_pktbuf + total;
	}
	total += sprintf((char *)out_ptr, PAYLOAD_TAG);
	out_ptr  = output_pktbuf + total;

	if (fwrite(output_pktbuf, 1, total, ofile) != total)
	{
        printf("Error Writing data to OUTPUT file\n");
		free(output_pktbuf);
        return -2;
	}
	
//	printf("Write %d bytes. \tFrame Size: %u\n", total, size);
	
	free(output_pktbuf);
	return 0;
}

unsigned char read_a_pkt_hex_file(unsigned long SeqNo, unsigned char *pBuffer, unsigned long *pPktLength)
{
	FILE            *fpRead;
    int             tmpBuf, CurPos=0, PktStartPos=0;  
    char            buf[80];
    unsigned char   GetPktLenState=RS_READ_PACKET_LENGTH_HIGH;
    unsigned long   SequenceNumber=0, PktLength=0, KeepPktLength=0;
    unsigned char   *pPacket = pBuffer;
    
	fpRead = fopen(HEX_RECEIVE_FILENAME, "rb");
	if (!fpRead)
	{
		printf("Failed to open input file: %s\n", HEX_RECEIVE_FILENAME);
		return CS_STATUS_ERROR_OPEN_FILE;
	}

    while(1)
    {
        if(!fgets(buf, sizeof(buf), fpRead))
        {
			if (ferror(fpRead))
			{
                if(fpRead)   fclose(fpRead);
	            return CS_STATUS_ERROR_READ_FILE;
			}
			else if (feof(fpRead))
			{
				break;
			}
        }

        switch(GetPktLenState)
        {
            case RS_READ_PACKET_CONTENT:
                sscanf(buf, "%x\n", &tmpBuf);
                *pPacket = (unsigned char)tmpBuf;
                pPacket++;
                PktLength--;
                if(PktLength == 0)
                {
                    GetPktLenState = RS_READ_PACKET_TAG;
                    SequenceNumber = SWAP_DWORD(*(unsigned long*)(&pBuffer[PAYLOAD_SEQUENCE_NUMBER_OFFSET]));
//                    printf("SequenceNumber 0x%8.8x\n", (int)SequenceNumber);
                    if(SeqNo == SequenceNumber)
                    {
//                        printf("FOUND\n");
                        *pPktLength = KeepPktLength;
                        if(fpRead)   fclose(fpRead);
                        return CS_STATUS_SUCCESS;
                    }
                }
                break;

            case RS_READ_PACKET_LENGTH_HIGH:
                sscanf(buf, "%x\n", &tmpBuf);
                PktLength = (tmpBuf & 0xFF) << 8;
                CurPos += strlen(buf);
                GetPktLenState = RS_READ_PACKET_LENGTH_LOW;
                break;

            case RS_READ_PACKET_LENGTH_LOW:
                sscanf(buf, "%x\n", &tmpBuf);
                PktLength |= (tmpBuf & 0xFF);
                KeepPktLength = PktLength;
                CurPos += strlen(buf);
                PktStartPos = CurPos;
//                printf("PktLength %d\n", (int)KeepPktLength);

                CurPos += (PAYLOAD_SEQUENCE_NUMBER_OFFSET * 3);
                fseek(fpRead, CurPos, SEEK_SET);
                GetPktLenState = RS_READ_PACKET_SEQUENCE_B1;
                break;

            case RS_READ_PACKET_SEQUENCE_B1:
                sscanf(buf, "%x\n", &tmpBuf);
                SequenceNumber = (tmpBuf & 0x000000FF) << 24;
                CurPos += strlen(buf);
                GetPktLenState = RS_READ_PACKET_SEQUENCE_B2;
                break;

            case RS_READ_PACKET_SEQUENCE_B2:
                sscanf(buf, "%x\n", &tmpBuf);
                SequenceNumber |= (tmpBuf & 0x000000FF) << 16;
                CurPos += strlen(buf);
                GetPktLenState = RS_READ_PACKET_SEQUENCE_B3;
                break;

            case RS_READ_PACKET_SEQUENCE_B3:
                sscanf(buf, "%x\n", &tmpBuf);
                SequenceNumber |= (tmpBuf & 0x000000FF) << 8;
                CurPos += strlen(buf);
                GetPktLenState = RS_READ_PACKET_SEQUENCE_B4;
                break;

            case RS_READ_PACKET_SEQUENCE_B4:
                sscanf(buf, "%x\n", &tmpBuf);
                SequenceNumber |= (tmpBuf & 0x000000FF);
//                printf("SequenceNumber 0x%8.8x\n", (int)SequenceNumber);
                if(SeqNo == SequenceNumber)
                {
                    CurPos = PktStartPos;
                    fseek(fpRead, CurPos, SEEK_SET);
                    GetPktLenState = RS_READ_PACKET_CONTENT;
                }else
                {
                    CurPos += strlen(buf);
                    CurPos += ((PktLength - (PAYLOAD_SEQUENCE_NUMBER_OFFSET+4))*3);
                    fseek(fpRead, CurPos, SEEK_SET);
                    GetPktLenState = RS_READ_PACKET_TAG;
                }
                break;

            case RS_READ_PACKET_TAG:
                if (strcmp(buf, PAYLOAD_TAG) != 0)
                {
                    printf("RS_READ_PACKET_TAG ERROR %s\n", buf);
                }
                CurPos += strlen(buf);
                GetPktLenState = RS_READ_PACKET_LENGTH_HIGH;
                break;
        }//end switch(GetPktLenState)
    }//end while(1)
    
    if(fpRead)   fclose(fpRead);
	return CS_STATUS_FAILURE;
}/* read_a_pkt_hex_file */

/*----------------------------------------------------------------------
* build_packet
*----------------------------------------------------------------------*/
static int build_packet(void)
{
	DESCRIPTION_INFO_T	*info = pInfo;
	unsigned char 		*buf, *cp,*buf_crc,*cp_1;
	int					payload_length;
    int                 rc = 0;
	int					pkt_num, pkt_id;
	
	
	pkt_num = info->pkt.number;
	pkt_id = 0;
	
	while (pkt_num--)
	{
		if (info->pkt.length_random)
		{
			// info->pkt.length = get_random_number(60, 1514);
			// 2006/6/7 07:50PM
			info->pkt.length = get_random_number(info->pkt.length_random_min,
												info->pkt.length_random_max);
		}

		payload_length = info->pkt.length;
		if (payload_length == 0)
		{
			printf("Packet length = 0!\n");
			return -1;
		}
		
		buf = (unsigned char *)malloc(MAX_JUMBO_PKT_SIZE);
		buf_crc = (unsigned char *)malloc(MAX_JUMBO_PKT_SIZE);
		if (!buf)
		{
			printf("Failed for malloc()! (%s, %d)\n", __FILE__, __LINE__);
			return -1;
		}
		cp_1 = cp = buf;
		cp = build_L2_packet(info, cp, (unsigned int*)&payload_length);
		
		if (payload_length < 0)
		{
			printf("Payload error(%d)! (%s %d)\n", payload_length, __FILE__, __LINE__);
			free(buf);
			return -1;
		}
		
		if (info->pkt.pkt_type >= PACKET_TYPE_L3 && info->pkt.have_L3)
		{
			cp = build_L3_packet(info, cp, (unsigned int*)&payload_length);
		}
		if (payload_length < 0)
		{
			printf("Payload error(%d)! (%s %d)\n", payload_length, __FILE__, __LINE__);
			free(buf);
			return -1;
		}
		if (info->pkt.pkt_type >= PACKET_TYPE_L4 && info->pkt.have_L4)
		{
			//printf("build_L4_packet======>cp = %p , *cp =0x%x\n",cp,*cp);
			cp = build_L4_packet(info, cp, (unsigned int*)&payload_length);
		}
		if (info->pkt.pkt_type >= PACKET_TYPE_L5 && info->pkt.have_L5)
		{
			cp = build_L5_packet(info, cp, (unsigned int*)&payload_length);
		}
		// Payload
		if (payload_length < 0)
		{
			printf("Payload error(%d)! (%s %d)\n", payload_length, __FILE__, __LINE__);
			free(buf);
			return -1;
		}
//		printf("build_payload_packet======>cp = %p , *cp =%d, payload_length %d\n",cp,*cp,payload_length);
		cp = build_payload_packet(info, cp, cp_1, (unsigned int*)&payload_length);
			
		// checksum
		
		if (info->L3.chksum)
			build_L3_chksum(info);
		if (info->L4.chksum)
			build_L4_chksum(info);
			
		if(pInfo->saveHexRcvFile == 1)
		{
    		if(info->hexrev_fp)
    		{
//    		    printf("Save packet\n");
    		    rc = write_a_pkt_hex_file(info->hexrev_fp, (char*)buf, info->pkt.length);
        		if (rc < 0)
        		{
        		    free(buf);
        			return -1;
        		}
    		}
    	}else
    	{
//		    printf("send packet\n");
		    sendRawData(buf, info->pkt.length);
		    usleep(50000);
//{
//    int i;
//    printf("pkt: %d\n", info->pkt.length);
//    for(i=0; i<info->pkt.length; i++)
//    {
//    printf("%2.2x ", buf[i]);
//    if((i!=0) && ((i+1)%16)==0)
//        printf("\n");
//    }
//    printf("\n");
//}
    	}

		free(buf);

		
		info->pktnum++;
		
		if (info->pkt.length_incremental)
		{
			info->pkt.length += info->pkt.length_inc_step;
			if (info->pkt.length > 90000)
				info->pkt.length = info->pkt.length_inc_start;
		}
		pkt_id++;
	} 	
	return 0;
}

/*----------------------------------------------------------------------
* parse_pkt_end
*----------------------------------------------------------------------*/
int parse_pkt_end(int argc, unsigned char *argv[])
{
	int rc = 0;
	
	if (pInfo->pkt.begin != 1)
		return -1;
		
	if (pInfo->pkt.length && pInfo->pkt.number)
	{
		rc = build_packet();
	}
	
	pInfo->pkt.begin = 0;
	pInfo->curr_table = NULL;
//	printf("Write Frame Size : %u\t Packet Number : %d\n", pInfo->pkt.length, pInfo->pkt.number);
	
	return rc;
}

/*----------------------------------------------------------------------
* parse_PacketType
*----------------------------------------------------------------------*/
int parse_PacketType(int argc, unsigned char *argv[])
{
	if (argc == 2)
	{
		pInfo->pkt.have_L5 = 0;
		if (stricmp(argv[1], "L2") == 0)
			pInfo->pkt.pkt_type = PACKET_TYPE_L2;
		else if (stricmp(argv[1], "L3") == 0)
			pInfo->pkt.pkt_type = PACKET_TYPE_L3;
		else if (stricmp(argv[1], "L4") == 0)
			pInfo->pkt.pkt_type = PACKET_TYPE_L4;
		else if (stricmp(argv[1], "L5") == 0)
		{
			pInfo->pkt.pkt_type = PACKET_TYPE_L5;
			pInfo->pkt.have_L5 = 1;
		}
		else if (stricmp(argv[1], "Payload") == 0)
			pInfo->pkt.pkt_type = PACKET_TYPE_PAYLOAD;
		else
			return -1;
	}
	else
		return -1;
		
	return 0;
}

/*----------------------------------------------------------------------
* parse_PacketLength
*	Packetlength [length] or [random] or [inc start step]
*	
*----------------------------------------------------------------------*/
int parse_PacketLength(int argc, unsigned char *argv[])
{
	if (argc >= 2)
	{
		if (IS_RANDOM(argv[1]))
		{
			pInfo->pkt.length = 1;
			pInfo->pkt.length_random = 1;
			pInfo->pkt.length_incremental = 0;
			if (argc == 4)
			{
				// pInfo->pkt.length = get_random_number(60, 1514);
				// 2006/6/7 07:49PM
				pInfo->pkt.length_random_min = string2value(argv[2]);
				pInfo->pkt.length_random_max = string2value(argv[3]);
				return 0;
			}
			else
			{
				pInfo->pkt.length_random_min = 60;
				pInfo->pkt.length_random_max = 1514;
				return 0;
			}
		}
		if (IS_INCREMENTAL(argv[1]))
		{
			if (argc != 4)
				return -1;
			pInfo->pkt.length = string2value(argv[2]);
			pInfo->pkt.length_inc_start = pInfo->pkt.length;
			pInfo->pkt.length_inc_step = string2value(argv[3]);
			pInfo->pkt.length_random = 0;
			pInfo->pkt.length_incremental = 1;
			return 0;
		}
		
		pInfo->pkt.length = string2value(argv[1]);

		if (pInfo->pkt.length < 14 || pInfo->pkt.length > MAX_JUMBO_PKT_SIZE)
		{
			printf("Incorrect Packet Length (%d)!\n", pInfo->pkt.length);
			printf("Must be in the range %d ~ %d\n", 14, MAX_JUMBO_PKT_SIZE);
			return -1;
		}
		return 0;
	}
	
	return -1;
}

/*----------------------------------------------------------------------
* parse_PacketNumber
*----------------------------------------------------------------------*/
int parse_PacketNumber(int argc, unsigned char *argv[])
{
	if (argc == 2)
	{
		if (IS_RANDOM(argv[1]))
		{
			pInfo->pkt.number = get_random_number(1, 99);
			pInfo->pkt.number_random = 1;
			return 0;
		}
		
		pInfo->pkt.number = string2value(argv[1]);
		return 0;
	}
	
	return -1;
}

/*----------------------------------------------------------------------
* parse_swtxq
*----------------------------------------------------------------------*/
int parse_swtxq(int argc, unsigned char *argv[])
{
	if (argc == 2)
	{
		if (IS_RANDOM(argv[1]))
		{
			pInfo->pkt.swtxq = get_random_number(0, 5);
			pInfo->pkt.is_tx_pkt = 1;
			return 0;
		}
		
		pInfo->pkt.swtxq = string2value(argv[1]);
		pInfo->pkt.is_tx_pkt = 1;
		return 0;
	}
	
	return -1;
}

/*----------------------------------------------------------------------
* parse_txdesc
*----------------------------------------------------------------------*/
int parse_txdesc(int argc, unsigned char *argv[])
{
	if (argc == 2)
	{
		if (IS_RANDOM(argv[1]))
		{
			pInfo->pkt.txdesc = get_random_number(0, 4096-1);
			pInfo->pkt.is_tx_pkt = 1;
			return 0;
		}
		
		pInfo->pkt.txdesc = string2value(argv[1]);
		pInfo->pkt.is_tx_pkt = 1;
		return 0;
	}
	
	return -1;
}

/*----------------------------------------------------------------------
* parse_txdesc
*----------------------------------------------------------------------*/
int parse_txflag(int argc, unsigned char *argv[])
{
	if (argc == 2)
	{
		if (IS_RANDOM(argv[1]))
		{
			pInfo->pkt.txflag = 0x1f;
			pInfo->pkt.is_tx_pkt = 1;
			return 0;
		}
		
		pInfo->pkt.txflag = string2value(argv[1]);
		pInfo->pkt.is_tx_pkt = 1;
		return 0;
	}
	
	return -1;
}

/*----------------------------------------------------------------------
* parse_txdesc
*----------------------------------------------------------------------*/
int parse_txmtu(int argc, unsigned char *argv[])
{
	if (argc == 2)
	{
		if (IS_RANDOM(argv[1]))
		{
			pInfo->pkt.txmtu = get_random_number(128, 1514);
			pInfo->pkt.is_tx_pkt = 1;
			return 0;
		}
		
		pInfo->pkt.txmtu = string2value(argv[1]);
		pInfo->pkt.is_tx_pkt = 1;
		return 0;
	}
	
	return -1;
}

/*----------------------------------------------------------------------
* parse_next_argument
*----------------------------------------------------------------------*/
unsigned char *parse_next_argument(unsigned char *datap)
{
     int	found = 0;
     int	double_q = FALSE;

     while (1)
     {
          char c = *datap;
          if (EOL_CHAR(c) || (SPACE_CHAR(c) && (double_q==FALSE))) break;
          if (c=='"') double_q ^= 1;
          datap++;
          found++;
     }

     if (SPACE_CHAR(*datap) || EOL_CHAR(*datap)) 
     	*datap++ = 0x00;

     if (found)
        return(datap);
     else
        return(0);
}/* parse_next_argument */

/*----------------------------------------------------------------------
* parse_arguments
*----------------------------------------------------------------------*/
int parse_arguments(DESCRIPTION_INFO_T *info)
{
	unsigned char *datap;
	
	datap = pInfo->buf;
	pInfo->argc = 0;
	pInfo->argv[0] = NULL;
	
	do {
		// skip space and TAB characters
		while (SPACE_CHAR(*datap))
			datap++;
	
		if (EOL_CHAR(*datap))
			break;
			
		if (COMMENT_CHAR(datap))
			return CS_STATUS_SUCCESS;

		pInfo->argv[pInfo->argc] = datap;
		pInfo->argc++;
		
		datap = parse_next_argument(datap);
		
	} while (datap);
	
	return CS_STATUS_SUCCESS;
}/* parse_arguments */

/*----------------------------------------------------------------------
* find_keyword
*----------------------------------------------------------------------*/
static KEYWORD_T *find_keyword(unsigned char *keyword, KEYWORD_T *tbl)
{
     /* comapre command table */
     while(tbl->name)
     {
          if(stricmp(keyword, tbl->name) == 0)
          {
            	return tbl;
          }
          tbl++;
     }

     return	NULL;
}

/*----------------------------------------------------------------------
* handle_keyword
*----------------------------------------------------------------------*/
int handle_keyword(DESCRIPTION_INFO_T *info)
{
	KEYWORD_T		*keyword, *tbl;
	int				rc;
	
	// check Level 1 packet
	tbl = (KEYWORD_T *)&keyword_1st_tbl[0];
	keyword = (KEYWORD_T *)find_keyword(pInfo->argv[0], tbl);
	if (!keyword && pInfo->pkt.parse_level == PARSER_LEVEL_1)
		return -1;
		
	if (keyword && keyword->handler)
	{
		rc = keyword->handler((int)pInfo->argc, (char **)pInfo->argv);
		return rc;
	}
	
	tbl = pInfo->curr_table;
	if (tbl)
	{
		keyword = (KEYWORD_T *)find_keyword(pInfo->argv[0], tbl);
		if (keyword && keyword->handler)
		{
			rc = keyword->handler((int)pInfo->argc, (char **)pInfo->argv);
			return rc;
		}
	}
	
	printf("Unknow to handle keyword: %s\n", pInfo->argv[0]);
	return -1;
}/* handle_keyword */

/*----------------------------------------------------------------------
* handle_storlink_file
*	Parse Storlink Description File
*	Format of Description File 
*   ----------------------------
*	Storlink Packet Description File	// Must be the first line
*	"//", ';', "#": comment after these characters
*   
*	Begin Packet: 
*				// Packet Description for Packet 1
*	End Packet: 
*	Begin Packet: 
*				// Packet Description for Packet 2
*	End Packet: 
*	¡K
*	Begin Packet: 
*				// Packet Description for Packet n
*	End Packet: 
*----------------------------------------------------------------------*/
int handle_storlink_file(char *infile, unsigned char pktType)
{
	FILE 				*in_fp = NULL;
	int					line_num = 0, num;
	int					rc, err;
	unsigned char		ouput_fn2[FILENAME_SIZE+1], filename[64];
	
	if(pInfo->saveHexRcvFile == 1)
	{
    	pInfo->hexrev_fp = fopen(HEX_RECEIVE_FILENAME, "w+b");
    	if (!pInfo->hexrev_fp)
    	{
    		printf("Failed to create output file %s\n", HEX_RECEIVE_FILENAME);
    		return -1;
    	}
    }

	in_fp = (FILE *)fopen(infile, "rt");
	if (!in_fp)
	{
		printf("%s line #%d: Failed to opne input file: %s\n", __FILE__, __LINE__, infile);
		rc = -1;
		goto end;
	}
	
	rc = 0;
	while (rc == 0)
	{
		if (!(fgets((char *)pInfo->buf, sizeof(pInfo->buf), in_fp)))
		{
			if (ferror(in_fp))
			{
				printf("%s line #%d: Failed to read input file: %s\n", __FILE__, __LINE__, infile);
				rc = -1;
				break;
			}
			else if (feof(in_fp))
			{
				break;
			}
		}
		line_num++;
		pInfo->argc = 0;
		if (parse_arguments(pInfo) < 0)
		{
			printf(" Parsing error in file %s line #%d\n", infile, line_num);
			rc = -1;
			break;
		}
		if (pInfo->argc && handle_keyword(pInfo) < 0)
		{
			printf("Keyword error in file %s line #%d\n", infile, line_num);
			rc = -1;
			break;
		}
	}
	
end:	
	if (in_fp) fclose(in_fp);
	if ((pInfo->saveHexRcvFile == 1) && (pInfo->hexrev_fp))     fclose(pInfo->hexrev_fp);

	// remove all files after pktnum
	err = 0;
	num = pInfo->pktnum;
	while (num && num < MAX_OUTPUT_FILE_NUM && !err)
	{
		sprintf((char*)filename, OUTPUT_FILENAME_NAME, num);
		sprintf((char*)ouput_fn2,"%s\\%s", pkt_file_name, filename);
		err = remove((char*)ouput_fn2);
		num++;
	}
//	printf("Write Total Packet Number : %d\n", pInfo->pktnum);
	
	return rc;
}/* handle_storlink_file */




//=================================================================
//                       Global Functions
//=================================================================
int cs_process_specify_file(char *sndFile, char *rcvFile, char *pHelp)
{
    FILE            *sndfp=NULL, *rcvfp=NULL;
	char	        buf[80];
	int		        size;

	memset((char *)pInfo, 0, sizeof(DESCRIPTION_INFO_T));
    pInfo->SequenceNumBackup = pInfo->SequenceNum = 1;

	sndfp = fopen(sndFile, "rb");
	if (!sndfp)
	{
		printf("Failed to open input file: %s\n", sndFile);
		printf(pHelp);
		goto error_exit;
	}
    
	rcvfp = fopen(rcvFile, "rb");
	if (!rcvfp)
	{
		printf("Failed to open input file: %s\n", rcvFile);
		goto error_exit;
	}
    
	// check output receive header
	bzero(buf, sizeof(buf));
	size = fread(buf, 1, sizeof(buf), rcvfp);
	if (size != sizeof(buf))
	{
		printf("Failed to read receive file: %s\n", rcvFile);
		goto error_exit;
	}
    if(rcvfp)   fclose(rcvfp);
	
	if (memicmp(buf, CORTINA_MAGIC, strlen(CORTINA_MAGIC)) == 0)
	{
	    pInfo->saveHexRcvFile = 1;
        pInfo->SequenceNumBackup = pInfo->SequenceNum;
	    handle_storlink_file(rcvFile, TYPE_RECEIVE_PACKET);
	    pInfo->SequenceNum = pInfo->SequenceNumBackup;
	    pInfo->saveHexRcvFile = 0;
	}else
	{
		printf("Unknown file format of input file: %s\n", rcvFile);
		goto error_exit;
	}

	// check input send header
	bzero(buf, sizeof(buf));
	size = fread(buf, 1, sizeof(buf), sndfp);
	if (size != sizeof(buf))
	{
		printf("Failed to read send file: %s\n", sndFile);
		goto error_exit;
	}
    if(sndfp)   fclose(sndfp);

	if (memicmp(buf, CORTINA_MAGIC, strlen(CORTINA_MAGIC)) == 0)
	{
	    handle_storlink_file(sndFile, TYPE_SEND_PACKET);
	}else
	{
		printf("Unknown file format of input file: %s\n", sndFile);
		goto error_exit;
	}

	return CS_STATUS_SUCCESS;

error_exit:
    if(sndfp)   fclose(sndfp);
    if(rcvfp)   fclose(rcvfp);
        
	return CS_STATUS_FAILURE;
}/* cs_process_specify_file */
