     
 
 
 
# Virtual Function Daemon -- VFd 
VFd is a DPDK application which provides for the 
configuration of, and policy enforcement over, VFs which 
have been allocated on one or more PFs of a NIC. For each 
VF that VFd is to manage a JSON configuration file 
outlining the desired setup for the VF is made available to 
VFd either at process start time or dynamically while VFd 
is running. The overall vision for VFd is to be the 
hypervisor of the NIC world. 
 
This _README_ will provide a brief overview of VFd 
capabilities with respect to PF and VF configuration, 
policy enforcement and finally a quick start guide to help 
those who'd like to avoid wandering through the 
documentation contained on the repo wiki. 
 
 
## Configuration 
Via the VF's configuration file, the following may be 
specified: 
 
* The VLAN ID(s) that the VF participates in. Transmitted 
packets are ensured to have one of the ID specified, and 
received packets will be passed to the guest only if the 
VLAN tag in the packet matches one of the IDs in the 
configuration. (In addition, see MAC addresses.) 
* The MAC addres(es) which will also be used to filter 
received packets (along with the VLAN ID). Packets 
transmitted by the guest must also use one of the listed 
MAC addresses as the source address in the L2 header. 
* Whether or not broadcast packets, unknown unicast, or 
multicast packets will be allowed to be received by the VF. 
* Whether or not the outer VLAN ID will be stripped 
(removed) from the received packets, and automatically 
inserted on transmitted packets. 
 
 
 
## MAC Addresses 
In most cases the MAC addresses are **not** supplied in the 
configuration file. If a guest needs to alter the MAC 
address of a device, for instance as might be necessary to 
bond two interfaces, the address that the guest sets 
through the driver will be accepted and added to the VF 
configuration by VFd. The MAC list in the configuration 
file is available for cases where the guest is unable to 
reset their MAC address, or the address must be supplied 
from an outside source. 
 
When no MAC addresses are given in a VF's configuration, a 
randomly generated address is configured for the VF at the 
time the VF is attachd to the guest. When this is done it 
is still possible for the guest to override the random 
address with one of their choosing. 
 
 
## Policy 
VFd serves to enforce some policies with respect to limits 
and/or use of each VF under its control. If QoS is enabled, 
this extends to providing the NIC with the proper 
configuration which implements bandwidth limits for the VF. 
VFd enforces the following policies: 
 
* Only VLAN IDs which are listed for the VF may be set by 
the guest 
* MAC and VLAN ID spoof checking is always enabled 
* MAC address assignments are vetted to ensure that there 
are no duplicates on the same PF, and to ensure that the 
total limit supported by the NIC is not exceeded. 
 
 
 
# Requirements 
It is currently possible to build VFd on top of DPDK 
versions 17.11 and 18.02. The VFd library requires a third 
party basic JSON parser which is available from github, and 
will automatically be cloned and built when the archive is 
created. 
 
 
## Patches 
Occasionally, there are patches which must be applied to 
DPDK in order to permit VFd to be built. These patches can 
be found in the dpdk_patch directory and are organised by 
the DPDK version to which they apply. There is also a 
script located in the patch directory which may be used to 
apply the patches for a particular version. 
 
 
## Linux Kernel 
VFd has been tested on Linux kernels starting with 3.13, 
and has also been used successfully with Linux 4.x kernels. 
VFd may work with older kernels, but that territory is 
uncharted. 
 
 
## Additional Documentation 
The wiki associated with the github repository for VFd 
contains several different documents which should be 
useful. These can be found by clicking the _Pages_ link 
which appears on the right side of the main wiki page. The 
documents include: 
 
* User's guide 
* Building the igb_uio driver 
* Debugging Tricks 
* i40e Based Troubleshooting 
 
 
 
# Quick Start 
To jump start your experience, the following should serve 
as a quick reference to setup and test an environment with 
VFd. The user's guide, and the readme in the source 
directory have more details than will be provided in the 
following paragraphs should you need it. The following 
steps are described in more detail below 
 
* Clone and build DPDK 
* Clone and build VFd 
* Create VFs on the PFs to be controlled 
* Create the main VFd configuration file 
* Create individual VF configuration files 
* Start VFd 
* Start a guest and test 
 
 
 
## Build DPDK 
If you don't already have DPDK installed, you will need to 
pull the repo and build it. You will need to do this even 
if you are not building VFd (you have a VFd binary or .deb 
file) in order to have a working copy of the igb_uio driver 
which has been compiled on the system. The following steps 
can be followed to build the igb_uio driver, and will also 
prepare the DPDK library for a VFd build if needed. 
 
  
**1.** Create a directory; we'll assume $HOME/build   
**2.** Switch to the build directory   
**3.** Execute: git clone http://dpdk.org/git/dpdk   
**4.** Switch to the dpdk/usertools directory   
**5.** Execute the dpdk-setup.sh script and when prompted 
enter the menu option that builds the library on your 
system. This will also build the igb_uio driver.   
**6.** Following a successful build, return to the main 
menu and select the option that installs the igb_uio driver 
  
**7.** Exit the script   
**8.** Set the following environment variables (the values 
given are likely to be correct, however if you opted for a 
different directory, or are building on a different type of 
system the values will need to reflect your environment): 
 
export RTE_SDK=$HOME/build/dpdk   
export RTE_TARGET=x86_64-native-linuxapp-gcc 
 
You may need to install gcc, make and/or cmake, to be able 
to build DPDK. 
 
 
## Build VFd 
This step can be skipped if you have a package (.deb etc.) 
or binary which you will be using. 
 
If you are going to build VFd, you will need to clone it 
from github, then build the library and binary. Once built, 
you can run it from the build directory, or move it to a 
desired directory for execution. The following steps list 
what needs to be done: 
 
  
**1.** Switch to the build directory   
**2.** Run git clone https://github.com/att/vfd.git   
**3.** Switch to the vfd/src directory   
**4.** Run 'make' which should build the library (cloning 
any third party libraries and building them as are needed), 
and then will build VFd.   
**5.** Optionally build the vreq tool by switching to the 
system directory and running 'make vreq'.   
**6.** Install, if desired, by copying vfd/build/app/vfd to 
a bin directory of your choosing. You will likely also want 
to copy iplex.py and vreq (if you built it) from the 
src/system directory to the binary; ensuring that the mode 
is set properly (executable) for each. 
 
 
You can verify that VFd was built correctly by running it 
with the -? option which will produce a usage message and 
should indicate the date and time that the build occurred. 
 
 
## Creating VFs 
For each PF on each NIC that will be under the control of 
VFd, you need to establish one or more VFs. This is 
accomplished by binding the igb_uio driver to the PF(s), 
and then writing the number of desired VFs for each PF into 
a device file associated with the PF. The following 
paragraphs provide the necessary commands to accomplish 
this. 
 
 
### Binding The igb_uio Driver 
The dpdk-devbind.py script in the DPDK directory 
$HOME/build/dpdk/usertools can be used to display a list of 
available devices to which the igb_uio driver can be 
attached. Executing the script with the -s option causes a 
status display to be written to the tty device; it will 
include the following information for devices that are not 
bound to DPDK capable drivers: 
   
   
   
     Network devices using kernel driver
     ============================================
     0000:08:00.0 'Ethernet Controller 10-Gigabit X540-AT2 1528' drv=ixgbe unused=igb_uio,vfio-pci
     0000:08:00.1 'Ethernet Controller 10-Gigabit X540-AT2 1528' drv=ixgbe unused=igb_uio,vfio-pci
 
 
 
The important pieces of information in the output are the 
PCI address for the device (column 0) and the list of 
unused drivers for the device. Assuming the igb_uio driver 
was successfully loaded, and the device is capable, the 
unused list should contain the igb_uio driver. The driver 
can be bound to these devices with the dpdk-devbind.py 
script: 
   
   
   
       dpdk-devbind.py -b igb_uio 0000:08:00.0
 
 
 
This command will need to be executed for each device; all 
devices which share a NIC must be bound to the igb_uio 
driver. Once bound, running the script with the status 
option should show these devices listed under those uing a 
DPDK compatible driver, and the drv= parameter should 
indicate that it is indeed bound to the igb_uio driver. 
   
   
   
     Network devices using DPDK-compatible driver
     ============================================
     0000:08:00.0 'Ethernet Controller 10-Gigabit X540-AT2 1528' drv=igb_uio unused=vfio-pci
     0000:08:00.1 'Ethernet Controller 10-Gigabit X540-AT2 1528' drv=igb_uio unused=vfio-pci
 
 
 
 
 
### Creating VFs 
VFs are created on a given PF by communicating to the 
driver the desired number of VFs. This is typically 
accomplished by using the echo command to write the number 
into a file somewhere in the sysfs filesystem. Assuming one 
of the addresses from the previous example, the command to 
set 32 VFs on the PF would be: 
   
   
   
     $ echo 32 >/sys/devices/pci0000:00/0000:00:03.0/0000:08:00.0/sriov_numvfs
 
 
 
The number of VFs can also be adjusted by first setting the 
number to zero (0), and then setting the desired number as 
illustrated above. To change the the number of VFs 
configured for a PF, VFd must **not** be running and all 
guests which were using the VFs must be detached from them. 
 
After creating the VFs, the directory can be listed which 
should now show a set of symbolic links, one for each VF. 
   
   
   
     $ ls -al /sys/devices/pci0000:00/0000:00:03.0/0000:08:00.0
     :
     lrwxrwxrwx  1 root root       0 Jan 26 08:31 virtfn0 -> ../0000:08:10.0
     lrwxrwxrwx  1 root root       0 Jan 26 08:31 virtfn1 -> ../0000:08:10.2
     lrwxrwxrwx  1 root root       0 Jan 26 08:31 virtfn10 -> ../0000:08:12.4
     lrwxrwxrwx  1 root root       0 Jan 26 08:31 virtfn11 -> ../0000:08:12.6
     :
 
 
 
 
 
## Create The Main Configuration 
A master configuration file is used to provide overall 
information to VFd during initialisation. The default 
location for the file is /etc/vfd/vfd.cfg, but the file can 
be placed in any location that makes sense for the 
installation. If not placed in the default location, the -c 
command line option must be used to supply the alternate 
location to VFd. 
 
The configuration file contains expected information such 
as log file locations, log retention durations, and 
verbosity levels. It also contains the configuration of 
each PF that VFd is to manage. A small sample configuration 
file below illustrates the layout. For each PF described in 
the config, identified by its PCIID, the following 
information can be supplied: 
 
* Promiscuous mode 
* Maximum MTU (overrides default in the main section) 
* Enable hardware stripping of the CRC 
* Whether to allow oversubscription when QoS mode is 
enabled 
 
The traffic class and bandwidth group sections can be 
omitted if QoS is not enabled in the main section. The 
driver information for each device is not used by VFd, but 
is provided for any setup script that might configure the 
NICs automatically before starting VFd. 
 
   
   
   
     {   
      "log_dir":      "/var/log/vfd",
      "log_keep":     10,
      "log_level":    1,
      "init_log_level": 3,
      "dpdk_log_level": 1,
      "dpdk_init_log_level": 2,
      "config_dir":   
         "/var/lib/vfd/config",
      "fifo":
         "/var/lib/vfd/request",
      "cpu_mask":      "0x01",
      "default_mtu":   1500,
      "enable_qos":   false,
      "enable_flowcontrol": false,
         
      "pciids": [ 
        { "id": "0000:08:00.0",
          "promiscuous": false,
          "mtu": 9240,
          "hw_strip_crc":   true,
          "enable_loopback": true,
          "pf_driver": "igb-uio",
          "vf_driver": "vfio-pci",
          "vf_oversubscription": 
                              true,
          "tclasses": [
            { "name": "best effort",
              "pri": 0,
              "latency": false,
              "lsp": false,
              "bsp": false,
              "max_bw": 100,
              "min_bw": 10 },
            { "name": "realtime",
              "pri": 1,
              "latency": false,
              "lsp": false,
              "bsp": false,
              "max_bw": 100,
              "min_bw": 40 },
            { "name": "voice",
              "pri": 2,
              "latency": false,
              "lsp": false,
              "bsp": false,
              "max_bw": 100,
              "min_bw": 40 },
            { "name": "control",
              "pri": 3,
              "latency": false,
              "lsp": false,
              "bsp": false,
              "max_bw": 100,
              "min_bw": 10 }
         ],
     
         "bw_grps": { "bwg0": [0],
                     "bwg1": [1, 2],
                     "bwg2": [3]
                    }
      ]
     }
 
 
 
## Create A VF Configuration 
A VF configuration is needed to communicate to VFd the 
specifications and policies for a VF that VFd will 
manage. One VF configuration file must be _added_ to VFd 
for each VF that will be attached to a guest. These 
configuration files contain a simple set of JSON and are 
placed into the directory identified by the config_dir 
parameter in the main VFd configuration. The files can be 
named using any text string provided that there are no 
embedded blanks and that the file suffix is .json. 
 
VF configuration files present in the directory when VFd 
is started are read and added to the list of managed VFs 
during initialisation. While VFd is running, 
configurations can be added by creating a new file and 
using the iplex add command which causes VFd to read the 
configuration file and add it to the mix. If a 
configuration is no longer needed, the iplex delete 
command can be used to remove the VF from those managed. 
The following show how the configuration in kad_0_1.json 
can be added and deleted (note that the suffix is **not** 
given on the command line): 
   
   
   
      $ iplex add /var/lib/vfd/config/kad_0_1
      $ iplex delete /var/lib/vfd/config/kad_0_1
 
 
 
 
 
### VF Configuration Contents 
The contents of a VF config file defines the PF that the 
VF is managed by, the VF ID (number), and other 
parameters as illustrated below. 
   
   
   
     {
         "comments":      "VF test config for PF 0, VF 1 ",
      
         "name":             "kad_0_1",
         "pciid":            "0000:08:00.0",
         "vfid":             1,
      
         "strip_stag":       true,
      
         "allow_bcast":      true,
         "allow_mcast":      true,
         "allow_un_ucast":   false,
      
         "vlan_anti_spoof":  true,
         "mac_anti_spoof":   true,
      
         "vlans":            [ 22, 21 ],
         "macs":             [ "fa:ce:ed:09:01:01" ],
      
         "queues": [
             { "priority": 0, "share": "10%" },
             { "priority": 1, "share": "10%" },
             { "priority": 2, "share": "10%" },
             { "priority": 3, "share": "10%" },
         ]
     }
 
 
 
Figure 1: A sample VF configuration file. 
 
For most guests, there is no need to specify any MAC 
addresses, and allowing unknown unicast should generally 
be false unless you are very sure that you want the guest 
to receive every packet destined for an unknown MAC 
address on one of the conifigured VLANs. When the 
strip_stag setting is true, the NIC will remove the VLAN 
ID (the outer tag if QinQ encapsulation is being used) 
before the packet is presented to the VF. On transmit, 
the VLAN ID listed, if only one is listed, will 
automatically be inserted by the NIC. When more than one 
VLAN IDs are listed, and stripping is set to true, it is 
the responsibility of the guest, or DPDK application in 
the guest, to identify the correct VLAN ID to insert into 
the packet on transmit and to place the ID into the 
packet's transmit descriptor. When stripping is false, 
the VLAN ID is not removed, and the guest must place the 
ID into each packet transmitted. 
 
While it appears that the spoofing parameters can be 
changed, they will always be set to true by VFd, so 
changing them in the config has no affect. 
 
 
### QoS Configuration 
If QoS is enabled in the main VFd configuration file, the 
queues array must be present in the VF configuration in 
order to define the amount (bandwidth percentage) that 
each traffic class, or queue, the VF will be given. When 
QoS is disabled, this section of the config file can be 
omitted. 
 
 
## Starting VFd 
Starting VFd is as simple as issuing the command on the 
command line, as root. Assuming that the VFd binary was 
installed in the /usr/bin directory, the command would be 
as follows: 
   
   
   
     sudo /usr/bin/vfd >/var/log/vfd/vfd.std 2>&1
 
 
 
By default, VFd will detach from the tty and create a log 
file in the log directory identified in the configuration 
file. The log file is named using the format 
vfd.log.<YYYYMMDD> and is _rolled_ about midnight each 
night. It is necessary to redirect the standard output 
and error files as the underlying DPDK library can 
sometimes be noisy, and writes to one, or both, of these 
files. 
 
Once VFd is running, the iplex ping command can be used 
to verify that VFd is alive and well. The iplex show all 
command can be used to display a set of statistics about 
the configured VFs. Because VFd must be executed as root, 
and to prevent any unauthorised access, the 
communications pipe between iplex and VFd is writable 
only by root, and as such the iplex commands must be run 
as root. For example: 
   
   
   
     $ sudo iplex ping
     { "state": "OK", "msg": "pong: +++cec1ed857b823d64671bcaad9b629fd2273efa44-notag    
     build: Jan 31 2018 14:40:41" }
 
 
 
The resulting output to a ping request is the build 
information (commit id and build date). 
 
 
## Starting A Guest 
There are many ways that a guest can be started which 
present the guest with one or more of the VFd managed 
VFs. It is beyond the scope of this readme to go into 
details about starting a guest, but we will complete our 
running configuration example by showing the qemu command 
that was used to attached the VF illustrated in this 
document. 
   
   
   
     sudo qemu-system-x86_64 \
        -display curses \
        -nographic \
        -smp 2 \
        -cpu host \
        -m 4096M \
        -net nic \
        -net user,net=192.168.1.10,hostfwd=tcp::5551-:22 \
        -boot d \
        -hda thin_ubuntu1.qcow2 \
        -vnc :1 \
        -enable\
        -kvm \
        -device vfio\
        -pci,host=0000:08:10.3,id=net0 \
        -device vfio-pci,host=0000:08:10.2,id=net1
 
 
 
The last line of the command shows how the configured VF 
is attached to the guest. Once the guest is running, an 
ifconfig command issued after logging in should show two 
ethernet devices; one that the 'main' network is 
configured on and the SR-IOV device. If the VF was given 
a MAC address in the configuration file, that address 
should be presented as a part of the ifconfig output. If 
a guest is to be given multiple VFs, one device option 
per VF on the command line will be needed. 
 
At this point, it should be possible to use ssh, or other 
utility across the second network interface; properly 
configuring it with an IP address, and setting routes 
will be necessary before it can be used. If the goal is 
to use a DPDK application in the guest, then the proper 
driver will need to be bound to the device before it can 
be started. 
 
 
_________________________________________________________
2 February 2018; tfm V2.2/0a266 
