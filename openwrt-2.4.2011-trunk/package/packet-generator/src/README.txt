=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
\/\/\/          Goldengate Packet Generator Utility Readme File          \/\/\/
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                                                                     11/11/2010

-------
Usage
=======
    pktgen [input file] [output file]
    or
    pktgen -s [Server Interface Name] -c [Client Interface Name] -i [input file] -o [output file]
        -s: Server Interface Name, for ex. "eth0". Default "eth0"
        -c: Client Interface Name, for ex. "eth1". Default "eth1"
        -i: input file: Cortina Systems Packet Description File
        -o: output file: Cortina Systems Packet Description File

-------
Example
=======
    pktgen -s eth0 -c eth1 -i packet_p0_out.des -o packet_p0_in.des
    
    packet_p0_out.des is the file of packet descriptor and will send from eth0.
    packet_p0_in.des is the file of packet descriptor which describe the packets will receive by eth1.
    
    packet scripts path:
    /sbin/cs75xx/pktgen_des/ 
