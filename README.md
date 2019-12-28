# simplified-NTP
**Protocol:**

Resembles NTP implemented in Linux with C and will use two types of messages:TimeRequests and TimeResponses.
In this project, the packet payloads will include the time on the machine when the packet was sent. These values will be referred to hereon as the time in seconds and the time in nanoseconds. The client sends a TimeRequest that contains a sequence number for the request and a timestamp of when it sent the payload. Upon receiving the TimeRequest, the
server replies with a TimeResponse that contains the same sequence number and timestamp in the
TimeRequest, as well as a timestamp of when the server sends the TimeResponse. The formats of these messages are shown below.
![GitHub Logo](/imgs/protocol-msgs.png)

![GitHub Logo](/imgs/msgs-format.png)

(Remember to make first)

To run **server**:

``./server -p <(1)> -d <(2)>``

1. -p <Number> = Port that the server binds to and listens on. Represented as a base-10
integer. Must be specified, with a value > 1024.
2. -d <Number> = Percentage chance that the server drops any given UDP payload that it
receives. Represented as a base-10 integer. This parameter is optional, but if included, must
have a value in [0, 100]. (A 0 value means that the server does not purposefully ignore any
packets, and a value of 100 means that the server ignores all packets.) Omitting this flag
means that the server does not drop any packets.
  
To run **client**:

``./client -a <(1)> -p <(2)> -n <(3)> -t <(4)>``

1. -a <String> = The IP address of the server, represented as an ASCII string (e.g., 128.8.126.63).
Must be specified.
2. -p <Number> = The port that the server is bound listening on. Represented as a base-10
integer. Must be specified.
3
3. -n <Number> = The number of TimeRequests (N) that the client will send to the server,
represented as a base-10 integer. Must be specified, and have a value ≥ 0.
4. -t <Number> = Timout. The time in seconds (T) that the client will wait after sending
its last TimeRequest to receive a TimeResponse. The timeout resets on each TimeResponse
that the client receives. Must be specified. A value of 0 indicates the client does not have a
timeout and thus will wait indefinitely for dropped TimeResponses.
