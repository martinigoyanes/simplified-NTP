# simplified-NTP
**Protocol:**
Resembles NTP and will use two types of messages:TimeRequests and TimeResponses.
In this project, the packet payloads will include the time on the machine when the packet was sent. These values will be referred to hereon as the time in seconds and the time in nanoseconds. The client sends a TimeRequest that contains a sequence number for the request and a timestamp of when it sent the payload. Upon receiving the TimeRequest, the
server replies with a TimeResponse that contains the same sequence number and timestamp in the
TimeRequest, as well as a timestamp of when the server sends the TimeResponse. The formats of
these messages are shown below.
![GitHub Logo](/protocol-msgs.png)

![GitHub Logo](/msgs-format.png)
