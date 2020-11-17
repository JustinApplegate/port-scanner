/*
- How to turn URLs into IP addresses?
*/

#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <bits/stdc++.h>

std::mutex bufferLock;

bool TestPortConnection(std::string ip, int port) {

    //creates a socket on your machine and connects to the port of the IP address specified
    struct sockaddr_in address;
    int myNetworkSocket = -1;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip.c_str());
    address.sin_port = htons(port);

    myNetworkSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (myNetworkSocket==-1) {
      std::cout << "Socket creation failed on port " << port << std::endl;
      return false;
    }

    fcntl(myNetworkSocket, F_SETFL, O_NONBLOCK);

    connect(myNetworkSocket, (struct sockaddr *)&address, sizeof(address)); 

    //creates a file descriptor set and timeout interval
    fd_set fileDescriptorSet;
    struct timeval timeout;

    FD_ZERO(&fileDescriptorSet);
    FD_SET(myNetworkSocket, &fileDescriptorSet);
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    int connectionResponse = select(myNetworkSocket + 1, NULL, &fileDescriptorSet, NULL, &timeout);
    if (connectionResponse == 1) {
      int socketError;
      socklen_t len = sizeof socketError;

      getsockopt(myNetworkSocket, SOL_SOCKET, SO_ERROR, &socketError, &len);

      if (socketError==0) {
        close(myNetworkSocket);
        return true;
      }
      else {
        close(myNetworkSocket);
        return false;
      }
    }
    close(myNetworkSocket);
    return false;
}

std::string GetHost() {
  std::string inputHost;

  std::cout << "Hostname/IP: ";
  std::getline(std::cin, inputHost);

  return inputHost;
}

void DisplayOptions() {
  std::cout << std::endl << "OPTIONS:" << std::endl;

  std::vector<std::string> optionDescriptions;

  optionDescriptions.push_back("Scan all ports");
  optionDescriptions.push_back("Scan for a specific port");
  optionDescriptions.push_back("Scan all common ports");

  for (int i = 0; i < optionDescriptions.size(); i++) {
    std::cout << "[" << i << "] " << optionDescriptions.at(i) << std::endl;
  }

  std::cout << std::endl;
}

int GetOption() {
  DisplayOptions();

  std::string optionToReturn;
  std::cout << "Option: ";
  std::getline(std::cin, optionToReturn);

  try {
    return std::stoi(optionToReturn);
  }
  catch (...) {
    return -1;
  }
}

void ThreadTask(std::vector<int>* bufferArg, std::string hostNameArg, int port) {
  if (TestPortConnection(hostNameArg, port)){
    bufferLock.lock();
    bufferArg->push_back(port);
    bufferLock.unlock();
  }
}

void ScanAllPorts(std::string hostNameArg) {
  
  std::vector<std::thread*> portTests;

  std::vector<int> buffer;

  int numOfTasks = 1000;

  for (int i = 0; i < 65; i++) {
    for (int j = 1; j < numOfTasks+1; j++) {
      portTests.push_back(new std::thread(ThreadTask, &buffer, hostNameArg, (i*numOfTasks)+j));
    }
    for (int j = 0; j < numOfTasks; j++) {
      portTests.at(j)->join();
    }
    for (int j = 0; j < numOfTasks; j++) {
      delete portTests.at(j);
    }
    portTests = {};
  }

  for (int i = 1; i <= 535; i++) {
    portTests.push_back(new std::thread(ThreadTask, &buffer, hostNameArg, i+65000));
  }
  for (int i = 0; i < 535; i++) {
    portTests.at(i)->join();
  }
  for (int i = 0; i < 535; i++) {
    delete portTests.at(i);
  }

  std::sort(buffer.begin(), buffer.end());

  //print out the list of all the open ports
  if (buffer.size()==0) {
    std::cout << "No open ports" << std::endl;
  }
  else {
    for (int i = 0; i < buffer.size(); i++) {
      std::cout << "Port " << buffer.at(i) << " is open!" << std::endl;
    }
  }

}

void ScanSpecificPort(std::string hostNameArg) {
  
  //get port
  int port;

  std::string portString;
  std::cout << "Port #: ";
  std::getline(std::cin, portString);

  try {
    port = std::stoi(portString);
  }
  catch (...) {
    std::cout << "Invalid port number." << std::endl;
    exit(0);
  }


  //test port number
  if (port<1||port>65535) {
    std::cout << "Invalid port number." << std::endl;
    exit(0);
  }


  //test connection
  if (TestPortConnection(hostNameArg, port)){
    std::cout << "Port " << port << " is open!" << std::endl;
  }
  else {
    std::cout << "Port " << port << " is closed." << std::endl;
  }
}

void ScanCommonPorts(std::string hostNameArg) {

  //initialize list of common ports
  std::vector<int> commonPorts;
  commonPorts = {7, //Echo
    19, //Chargen
    20, //FTP Data Transfer
    21, //FTP Command Control
    22, //FTPS/SSH
    23, //Telnet
    25, //SMTP
    26, //SMTP
    42, //WINS Replication
    43, //WHOIS
    49, //TACACS
    53, //DNS service
    67, //DHCP
    68, //DHCP
    69, //TFTP
    70, //Gopher
    79, //Finger
    80, //HTTP
    88, //Kerberos
    102, //MS Exchange
    110, //POP3
    113, //Ident
    119, //NNTP (Usenet)
    123, //NTP
    135, //Microsoft RPC
    137, //NetBIOS
    138, //NetBIOS
    139, //NetBIOS
    143, //IMAP
    161, //SNMP
    162, //SNMP
    177, //XDMCP
    179, //BGP
    194, //IRC
    201, //AppleTalk
    264, //BGMP
    318, //TSP
    381, //HP Openview
    382, //HP Openview
    383, //HP Openview
    389, //LDAP
    411, //Direct Connect
    412, //Direct Connect
    443, //HTTPS
    445, //Microsoft DS
    464, //Kerberos
    465, //SMTP over SSL
    497, //Retrospect
    500, //ISAKMP
    512, //rexec
    513, //rlogin
    514, //syslog
    515, //LPD/LPR
    520, //RIP
    521, //RIPng (IPv6)
    540, //UUCP
    554, //RTSP
    546, //DHCPv6
    547, //DHCPv6
    560, //rmonitor
    563, //NNTP over SSL
    587, //SMTP SSL
    591, //FileMaker
    593, //Microsoft DCOM
    631, //Internet Printing Protocol
    636, //LDAP over SSL
    639, //MSDP (PIM)
    646, //LDP (MPLS)
    691, //MS Exchange
    860, //iSCSI
    873, //rsync
    902, //VMware Server
    989, //FTP over SSL
    990, //FTP over SSL
    993, //IMAP SSL
    995, //POP3 SSL
    1025, //Microsoft RPC
    1026, //Windows Messenger
    1027, //Windows Messenger
    1028, //Windows Messenger
    1029, //Windows Messenger
    1080, //SOCKS Proxy
    1080, //MyDoom
    1194, //OpenVPN
    1214, //Kazaa
    1241, //Nessus
    1311, //Dell OpenManage
    1337, //WASTE
    1433, //Microsoft SQL
    1434, //Microsoft SQL
    1512, //WINS
    1589, //Cisco VQP
    1701, //L2TP
    1723, //MS PPTP
    1725, //Steam
    1741, //CiscoWorks 2000
    1755, //MS Media Server
    1812, //RADIUS
    1813, //RADIUS
    1863, //MSN
    1985, //Cisco HSRP
    2000, //Cisco SCCP
    2002, //Cisco ACS
    2049, //NFS
    2077, //WebDAV/WebDisk
    2078, //WebDAV/WebDisk SSL
    2082, //cPanel
    2083, //cPanel SSL
    2086, //WHM
    2087, //WHM SSL
    2095, //Webmail
    2096, //Webmail SSL
    2100, //Oracle XDB
    2222, //DirectAdmin
    2302, //Halo
    2483, //Oracle DB
    2484, //Oracle DB
    2745, //Bagle.H
    2967, //Symantec AV
    3050, //Interbase DB
    3074, //XBOX Live
    3124, //HTTP Proxy
    3127, //MyDoom
    3128, //HTTP Proxy
    3222, //GLBP
    3260, //iSCSI Target
    3306, //MySQL
    3389, //Terminal Server
    3689, //iTunes
    3690, //Subversion
    3724, //World of Warcraft
    3784, //Ventrilo
    3785, //Ventrilo
    4333, //mSQL
    4444, //Blaster
    4664, //Google Desktop
    4672, //eMule
    4899, //- Radmin
    5000, //UPnP
    5001, //Slingbox
    5001, //iperf
    5004, //RTP
    5005, //RTP
    5050, //Yahoo! Messenger
    5060, //SIP
    5190, //AIM/ICQ
    5222, //XMPP/Jabber
    5223, //XMPP/Jabber
    5432, //PostgreSQL
    5500, //VNC Server
    5554, //Sasser
    5631, //pcAnywhere
    5632, //pcAnywhere
    5800, //VNC over HTTP
    5900, //VNC Server
    5901, //VNC Server
    5902, //VNC Server
    5903, //VNC Server
    5904, //VNC Server
    5905, //VNC Server
    5906, //VNC Server
    5907, //VNC Server
    5908, //VNC Server
    5909, //VNC Server
    5910, //VNC Server
    5911, //VNC Server
    5912, //VNC Server
    5913, //VNC Server
    5914, //VNC Server
    5915, //VNC Server
    5916, //VNC Server
    5917, //VNC Server
    5918, //VNC Server
    5919, //VNC Server
    5920, //VNC Server
    5921, //VNC Server
    5922, //VNC Server
    5923, //VNC Server
    5924, //VNC Server
    5925, //VNC Server
    5926, //VNC Server
    5927, //VNC Server
    5928, //VNC Server
    5929, //VNC Server
    5930, //VNC Server
    5931, //VNC Server
    5932, //VNC Server
    5933, //VNC Server
    5934, //VNC Server
    5935, //VNC Server
    5936, //VNC Server
    5937, //VNC Server
    5938, //VNC Server
    5939, //VNC Server
    5940, //VNC Server
    5941, //VNC Server
    5942, //VNC Server
    5943, //VNC Server
    5944, //VNC Server
    5945, //VNC Server
    5946, //VNC Server
    5947, //VNC Server
    5948, //VNC Server
    5949, //VNC Server
    5950, //VNC Server
    5951, //VNC Server
    5952, //VNC Server
    5953, //VNC Server
    5954, //VNC Server
    5955, //VNC Server
    5956, //VNC Server
    5957, //VNC Server
    5958, //VNC Server
    5959, //VNC Server
    5960, //VNC Server
    5961, //VNC Server
    5962, //VNC Server
    5963, //VNC Server
    5964, //VNC Server
    5965, //VNC Server
    5966, //VNC Server
    5967, //VNC Server
    5968, //VNC Server
    5969, //VNC Server
    5970, //VNC Server
    5971, //VNC Server
    5972, //VNC Server
    5973, //VNC Server
    5974, //VNC Server
    5975, //VNC Server
    5976, //VNC Server
    5977, //VNC Server
    5978, //VNC Server
    5979, //VNC Server
    5980, //VNC Server
    5981, //VNC Server
    5982, //VNC Server
    5983, //VNC Server
    5984, //VNC Server
    5985, //VNC Server
    5986, //VNC Server
    5987, //VNC Server
    5988, //VNC Server
    5989, //VNC Server
    5990, //VNC Server
    5991, //VNC Server
    5992, //VNC Server
    5993, //VNC Server
    5994, //VNC Server
    5995, //VNC Server
    5996, //VNC Server
    5997, //VNC Server
    5998, //VNC Server
    5999, //VNC Server
    6000, //X11
    6001, //X11
    6112, //Battle.net
    6129, //DameWare
    6257, //WinMX
    6346, //Gnutella
    6347, //Gnutella
    6500, //GameSpy Arcade
    6566, //SANE
    6588, //AnalogX
    6665, //IRC
    6666, //IRC
    6667, //IRC
    6668, //IRC
    6669, //IRC
    6679, //IRC over SSL
    6697, //IRC over SSL
    6699, //Napster
    6881, //BitTorrent
    6891, //Windows Live
    6892, //Windows Live
    6893, //Windows Live
    6894, //Windows Live
    6895, //Windows Live
    6896, //Windows Live
    6897, //Windows Live
    6898, //Windows Live
    6899, //Windows Live
    6900, //Windows Live
    6901, //Windows Live
    6970, //Quicktime
    7212, //GhostSurf
    7648, //CU-SeeMe
    7649, //CU-SeeMe
    8000, //Internet Radio
    8080, //HTTP Proxy
    8086, //Kaspersky AV
    8087, //Kaspersky AV
    8118, //Privoxy
    8200, //VMware Server
    8500, //Adobe ColdFusion
    8767, //TeamSpeak
    8866, //Bagle.B
    9100, //HP JetDirect
    9101, //Bacula
    9102, //Bacula
    9103, //Bacula
    9119, //MXit
    9800, //WebDAV
    9898, //Dabber
    9988, //Rbot/Spybot
    9999, //Urchin
    10000, //Webmin
    10000, //BackupExec
    10113, //NetIQ
    10114, //NetIQ
    10115, //NetIQ
    10116, //NetIQ
    11371, //OpenPGP
    12035, //Second Life
    12036, //Second Life
    12345, //NetBus
    13720, //NetBackup
    13721, //NetBackup
    14567, //Battlefield
    15118, //Dipnet/Oddbob
    19226, //AdminSecure
    19638, //Ensim
    20000, //Usermin
    24800, //Synergy
    25999, //Xfire
    27015, //Half-Life
    27017, //MongoDB
    27374, //Sub7
    28960, //Call of Duty
    31337}; //Back Orifice

  std::vector<std::thread> portTests;

  std::vector<int> buffer;

  //spawn threads
  for (int i = 0; i < commonPorts.size(); i++) {
    portTests.push_back(std::thread(ThreadTask, &buffer, hostNameArg, commonPorts.at(i)));
  }

  //wait for all threads to complete
  for (int i = 0; i < portTests.size(); i++) {
    portTests.at(i).join();
  }

  std::sort(buffer.begin(), buffer.end());

  //print out the list of all the open ports
  if (buffer.size()==0) {
    std::cout << "No open ports" << std::endl;
  }
  else {
    for (int i = 0; i < buffer.size(); i++) {
      std::cout << "Port " << buffer.at(i) << " is open!" << std::endl;
    }
  }
}

int main(int argc, char* argv[]){
    
    //get the IP address from user input
    std::string hostname;

    if (argc > 1) {
      hostname = argv[1];
    }
    else {
      hostname = GetHost();
    }

    //let them choose an option
    int option = -1;

    int count = 0; //if option selection fails, this disables reading the option from commandline
    
    while (option<0||option>2) {

      if (argc>=2&&count==0) {
        try {
          option = std::stoi(argv[2]);
        }
        catch (...) {
          option = -1;
        }
      }
      else {
        option = GetOption();
      }
      
      if (option==0) {
        ScanAllPorts(hostname);
      }
      else if (option==1) {
        ScanSpecificPort(hostname);
      }
      else if (option==2) {
        ScanCommonPorts(hostname);
      }
      else {
        std::cout << "Invalid option. Please try again." << std::endl;
        count++;
      }
    }

    return 0;
}
