/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : helpMsg.h
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility header file
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/

#ifndef _HELO_MSG_H_
#define _HELO_MSG_H_

static const char UsageMsg[] =
    "\n%s\n"
    "Usage: %s [-h?...] [-m <module>] [-s] [-c <command>]\n"
    " \n"
    " ----------------------------------------------------------------------\n"
    "	-s              :Command start \n"
    "	-e              :Command stop \n"
    "	-m  <module>    :module <ni/fe/tm/sch/qm/inqos/voqcntr> \n"
    "	-t  <table>     :Table \n"
    "	-f  <field>     :Field \n"
    "	-v  <value>     :Value of Field \n"
    "	-c  <command>   :Command <add/delete/flush/get/replace/set/rest> \n"
    "	-b  <value>     :Bypass NE(HW). 0: Enable NE, 1: Disable NE\n"
    " ----------------------------------------------------------------------\n"
    "	-h              : Help\n";

static const char ModuleHelp[] = "Usage: %s -m <ni/fe/tm/sch/qm/inqos/voqcntr> \n";
static const char StartHelp[] =
    "Usage: %s -m <ni/fe/tm/sch/qm> -t <Classifier/SDB/...> "
    "-s -c <add/delete/flush/get/replace>\n";
static const char StopHelp[] =
    "Usage: %s -m <ni/fe/tm/sch/qm/inqos/voqcntr> -t <Classifier/SDB/...> -e\n";
static const char TableHelp[] =
    "Usage: %s -m <ni/..> -t <Classifier/SDB/Hash_Mask/LPM/Hash_Match/"
    "FWDRSLT/QOSRSLT/L3_IP/L2_MAC/VoQ_Policer/LPB/AN_MAC/Port_Range/"
    "VLAN/ACL_Rule/ACL_Action/PE_VOQ_DROP/ETYPE/LLC_HDR/FVLAN>\n";
static const char FieldValueHelp[] = "Usage: %s -f <.....> -v <.....>\n";
static const char CommandHelp[] =
    "Usage: %s -c <add/delete/flush/get/replace> \n";
static const char BypassHelp[] = "Usage: %s -b <0/1> \n";

static const char InqosStartHelp[] =    
    "Usage: %s -m <inqos> -t <Qos_table/SP_voq/SP_port/qos_api> "
    "-s -c <set/rest>\n";
static const char InqosStopHelp[] =
    "Usage: %s -m <inqos> -t <Qos_table/SP_voq/SP_port/qos_api> -e\n";
static const char InqosTableHelp[] =  
    "Usage: %s -m <inqos> -t <Qos_table/SP_voq/SP_port/qos_api>\n";   
static const char InqosCommandHelp[] =    
    "Usage: %s -c <set/rest> \n";

static const char VoqcntrStartHelp[] =    
    "Usage: %s -m <voqcntr> -t <voqcntr_api> "
    "-s -c <set/rest>\n";
static const char VoqcntrStopHelp[] =
    "Usage: %s -m <voqcntr> -t <voqcntr_api> -e\n";
static const char VoqcntrTableHelp[] =  
    "Usage: %s -m <voqcntr> -t <voqcntr_api>\n";   
static const char VoqcntrCommandHelp[] =    
    "Usage: %s -c <set/rest> \n";

#endif /* _HELO_MSG_H_ */
