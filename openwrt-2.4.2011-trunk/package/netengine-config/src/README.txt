=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
\/\/\/       Goldengate NetEngine Configuration Utility Readme File      \/\/\/
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                                                                     08/20/2010

-------
Content
=======
    1. Software Stack 
    2. Files and Dessription
    3. Code Definitions Dessription


1. Software Stack 
===============================================================================

                +------------+  Internal Socket  +------+-----+
      User      |   ne_cfg   +-------------------+  neSendCmd |
      Space     +------------+                   +------+-----+
                                                        | IOCTL 
    ====================================================|============
                                                        |
                                            +-----------+---------+
      Kernel                                |                     |
      Space                                 |   NetEngine Driver  |
                                            |                     |
                                            +---------------------+


2. Files and Dessription
===============================================================================
   Files and    
   Subdirectories                Description
   ----------------------------------------------------------------------------
   ./README.txt                  This README file.

   ./Makefile                    makefile for ne_cfg project.

   <inculde file>
   ./include/ipc.h               Header file for ne_cfg and neSendCmd.

   ./include/ne_types.h          Header file for general definitions.
  
   ./include/fe_table.h          Header file of FE module tables.

   ----------------------------------------------------------------------------
   <ne_cfg>
   ./ne_cfg/Makefile             makefile for ne_cfg.
   ./ne_cfg/ne_cfg.c             ne_cfg main program.
   ./ne_cfg/ne_cfg.h             ne_cfg main program header file.
   ./ne_cfg/helpMsg.h            Header file for help message.
   ./ne_cfg/parser.c             ne_cfg program parameter parser function.
   ./ne_cfg/ne_ipc.c             IPC function, communicate ne_cfg with neSendCmd. 

   ./ne_cfg/fe/fe.c              Setting fe table.
   ./ne_cfg/fe/fe.h              Header file for setting fe table.

   ./ne_cfg/ni/ni.c              Setting ni table.
   ./ne_cfg/ni/ni.h              Header file for setting ni table.

   ./ne_cfg/qm/qm.c              Setting qm table.
   ./ne_cfg/qm/qm.h              Header file for setting qm table.

   ./ne_cfg/sch/sch.c            Setting sch table.
   ./ne_cfg/sch/sch.h            Header file for setting sch table.

   ./ne_cfg/tm/tm.c              Setting tm table.
   ./ne_cfg/tm/tm.h              Header file for setting tm table.

   ----------------------------------------------------------------------------
   <neSendCmd>
   ./neSendCmd/Makefile          makefile for neSendCmd.
   ./neSendCmd/neSendCmd.c       neSendCmd main program.
   ./neSendCmd/neSendCmd.h       Header file for neSendCmd.
   ./neSendCmd/neSend_ioctl.c    IPC function, communicate ne_cfg with neSendCmd. 
   ./neSendCmd/neSend_ioctl.h    Header file for IPC function.
   ./neSendCmd/misc.c            Miscellaneous functions.
   ./neSendCmd/misc.h            Header file for miscellaneous functions.

   ./neSendCmd/fe/feTblClassifier.c     Process all fields of FE module Classifier Table.
   ./neSendCmd/fe/feTblSDB.c            Process all fields of FE module SDB Table.
   ./neSendCmd/fe/feTblHashMask.c       Process all fields of FE module HashMask Table.
   ./neSendCmd/fe/feTblLPM.c            Process all fields of FE module LPM Table.
   ./neSendCmd/fe/feTblHashMatch.c      Process all fields of FE module HashMatch Table.
   ./neSendCmd/fe/feTblFWDRSLT.c        Process all fields of FE module FWDRSLT Table.
   ./neSendCmd/fe/feTblQOSRSLT.c        Process all fields of FE module QOSRSLT Table.
   ./neSendCmd/fe/feTblL3IP.c           Process all fields of FE module L3IP Table.
   ./neSendCmd/fe/feTblL2MAC.c          Process all fields of FE module L2MAC Table.
   ./neSendCmd/fe/feTblVoQPolicer.c     Process all fields of FE module VoQPolicer Table.
   ./neSendCmd/fe/feTblLPB.c            Process all fields of FE module LPB Table.
   ./neSendCmd/fe/feTblANBNGMAC.c       Process all fields of FE module ANBNGMAC Table.
   ./neSendCmd/fe/feTblPortRange.c      Process all fields of FE module PortRange Table.
   ./neSendCmd/fe/feTblVLAN.c           Process all fields of FE module VLAN Table.
   ./neSendCmd/fe/feTblACLRule.c        Process all fields of FE module ACLRule Table.
   ./neSendCmd/fe/feTblACLAction.c      Process all fields of FE module ACLAction Table.
   ./neSendCmd/fe/feTblPEVOQDROP.c      Process all fields of FE module PEVOQDROP Table.
   ./neSendCmd/fe/feTblETYPE.c          Process all fields of FE module ETYPE Table.
   ./neSendCmd/fe/feTblLLCHDR.c         Process all fields of FE module LLCHDR Table.
   ./neSendCmd/fe/feTblFVLAN.c          Process all fields of FE module FVLAN Table.
   ./neSendCmd/fe/feTblHashHash.c       Process all fields of FE module HashHash Table.
   ./neSendCmd/fe/feTblHashHash.h       Header file for HashHash Table.


3. Code Definitions Dessription
===============================================================================
    (No Define)          -- NONE
    ---------------------------------------------------------------------------


