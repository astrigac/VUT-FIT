# IPK Project 1: IPK Calculator Protocol
Goal is to implement a client for the IPK Calculator Protocol. The client should be able to communicate with any server using IPKCP, not just the provided reference implementation.

## Usage
- Extract the file
- Translate the file into binary:
    

```
make 
```
&nbsp;&nbsp;&nbsp;&nbsp; or
```
make run 
```
- Launch the program:
```
ipkcpc -h <host> -p <port> -m <mode> 
```
&nbsp;&nbsp;&nbsp;&nbsp;where `host` is the IPv4 address of the server, `port` the server port, and `mode` either `tcp` or `udp` (e.g. `ipkcpc -h 1.2.3.4 -p 2023 -m udp`).

## Description of individual functions
- `void  sig_handler()` - Function to handle ctrl-C capture. If the client is running in tcp mode, it must send the server a 'BYE' message, wait for the server's response, close the socket and exit the program. If the client is running in udp mode, he just closes the socket and exits the program.
- `bool  check_argument_format(int  argc,  const  char  argv[])` - Checks whether the program was called with arguments in the right order, or in the correct form (USAGE section). It returns 'true' if everything is fine, otherwise it returns 'false'.
- `void  print_udp_packet(char  udp_packet[])` - Because of the udp packet format, I created this function that prints the text content of the packet.
- `void  buf_to_udp(char  buf[],  char  udp_packet_buf[])` - This function converts the contents of a buffer (containing text entered by the user) into a buffer that is ready for sending to a udp server.
## Testing
I tested the program in 3 ways: <br />
1.) **Wireshark** - Capture of individual packets. Especially came in handy while programming the udp section. <br />
&nbsp;&nbsp;Communication in terminal: <br />
&nbsp;![Screenshot 2023-03-21 145920](https://user-images.githubusercontent.com/101597718/226671615-47c13196-5b6c-40f9-bc0d-81d9920db014.png) <br />
&nbsp;&nbsp;Request in Wireshark: <br />
&nbsp;![Screenshot 2023-03-21 150013](https://user-images.githubusercontent.com/101597718/226671885-e68833c6-ad0d-452a-aef3-497ae4d72026.png) <br />
&nbsp;![Screenshot 2023-03-21 145949](https://user-images.githubusercontent.com/101597718/226671933-90275400-80f9-40b6-a105-deb3e10e0614.png) <br />
&nbsp;&nbsp;Response in Wireshark: <br />
&nbsp;![Screenshot 2023-03-21 150034](https://user-images.githubusercontent.com/101597718/226671995-97cd9bac-a75b-45af-8541-0f1c40f32f86.png) <br />
&nbsp;![Screenshot 2023-03-21 150002](https://user-images.githubusercontent.com/101597718/226672157-5432beee-a547-4416-92b8-2a36c348a633.png) <br />
2.) **Tracing** - Systematically placing printf() in different places in the program <br />
3.) **Rubber duck debugging** - Explaining my code to an object
## Sources
- https://stackoverflow.com/questions/16163260/setting-timeout-for-recv-fcn-of-a-udp-socket
- https://stackoverflow.com/questions/4217037/catch-ctrl-c-in-c
- https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs/cpp/DemoTcp/client.c
- https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs/cpp/DemoUdp/client.c

 