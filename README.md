# 441A1 test3
Requirements:
C++ with GCC compiler
Tested on Firefox in Ubuntu 16.04

Instructions:
Open the .cpp file and set the PROXYPORT number.
Open Firefox and configure the proxy using 127.0.0.1 and the PROXYPORT number set in the last step.
Set any optional parameters
Run the .cpp file.
Navigate to an http site.

Optional Parameters:
SLOWDOWN: 
If enabled, sloxy will append ranged requests to every request load the page in several different pieces and stich them together.
If disabled, sloxy will (try to) behave as a regular client and onl

BYTE_WIDTH;
How many bytes should be requested from each range request.

MESSAGE_BUFFER:
The size of the string which stores the response recieved from the web server.

Testing:
Tested on home and university network.
SLOWDOWN = True, BYTE_WIDTH = 100 
http://pages.cpsc.ucalgary.ca/~carey/CPSC441/test0.txt,  nearly instant 
http://pages.cpsc.ucalgary.ca/~carey/CPSC441/test1.html, nearly instant
http://pages.cpsc.ucalgary.ca/~carey/CPSC441/test2.html, a few seconds
http://pages.cpsc.ucalgary.ca/~carey/CPSC441/test3.html, about 30 seconds
SLOWDOWN = True, BYTE_WIDTH = 100,000 
http://www.ucalgary.ca, about 1 minute

Limitations:
Only works with certain http sites. 
Does not accept any message encoding (ie gzip).

Troubeshooting:
 -Error on bind: This can happen after stopping the program and immediately restarting it. The cause is that the proxy port needs more time before it can be reused. The solution is to wait for a few moment (usually around a minute) or temporarily use a different port number.
