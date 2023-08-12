# IRC SERVER/CLIENT implementation in C.
It is a Terminal Based IRC(in-relay-chat) server and client implementation in C coded by me which currently supports **Windows OS**. It does not supports channel but it is not a basic IRC server/client either.

# Installation

Compile both the files. Make sure to link the lws2_32.lib. Here is what I did in my case since I have gcc compiler installed.

```
make server
make client
```
Running these executables require 2 terminals. One for the Server, another one for the client. Execute your ``server`` executable in the first and ``client`` executable in the other terminal.

Viola! You can now communicate through the client executable and you can connect as much as clients you want to a specific irc server.

# TODO
- [*] Make a CLI (giving the port inputs, buffers to send to the server)
- [*] Multithreading 
- [ ] File Transfer
- [ ] Encryption
- [ ] Channels
- [*] More CLI features
- [ ] Gui