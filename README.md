# C++ Port Scanner README
This is my version of a Port Scanner in C++. It's written to be used in the Linux terminal and currently only scans TCP ports. It only works with IP addresses, not URLs or hostnames. It has 3 main options:

* Option 0 - scan all ports
* Option 1 - scan a specific port
* Option 2 - scan all common ports

Specifying the option and subsequent choices can be done while running the binary file or using the makefile. 

To use the makefile, run this in the terminal:

`make`

OR

`make arguments="[IP address] [option #]"`

To remove the binary file after being compiled and run, use this command:

`make clean`

## Scan all ports
This option scans all TCP ports from 1-65000. All successful connections are pushed into a vector, where all the results are printed at the END of the scan. This scan takes approximately 2 minutes. 

## Scan a specific port
After choosing this option, you will specify a specific port number, and the terminal will take up to 2 seconds to output the response. 

## Scan all common ports
There is a list of 328 ports in the function ScanCommonPorts that are commonly used. The service they are used for is listed in a short comment on the same line as the port number. This scan takes approximately 10 seconds to run. 