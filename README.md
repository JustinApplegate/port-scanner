# C++ Port Scanner README
This is my version of a Port Scanner in C++. It's written to be used in the Linux terminal and currently only scans TCP ports. It only works with IP addresses, not URLs or hostnames. It has 3 main options:

* Option 0 - scan all ports
* Option 1 - scan a specific port
* Option 2 - scan all common ports

Specifying the option and subsequent choices can be done while running the binary file or using the makefile. 

To compile the program with the makefile, enter this command into the terminal:

`make`

To run the program, enter this command into the terminal (after compiling):

`./scanner`

OR

`./scanner [IP address] [option #]`

To remove the binary file after being compiled and run, use this command:

`make clean`

## Example
```
user@comp_name:~/port-scanner-cpp$ make
Hostname/IP: 128.199.4.110

OPTIONS:
[0] Scan all ports
[1] Scan for a specific port
[2] Scan all common ports

Option: 0
Port 22 is open!
Port 80 is open!
```

## Scan all ports
This option scans all TCP ports from 1-65535. All successful connections are printed at the END of the scan. This scan takes approximately 3 minutes. 

## Scan a specific port
After choosing this option, you will specify a specific port number, and the terminal will take up to 2 seconds to output the response. 

## Scan all common ports
There is a list of 328 ports in the function ScanCommonPorts that are commonly used. The service they are used for is listed in a short comment on the same line as the port number. This scan takes approximately 2 seconds to run.

## How it works
This port scanner uses multithreading and uses approximately 1000 ports at a time to test the connection to the desired port. The timeout is set to 2 seconds. 