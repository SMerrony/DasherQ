# DasherQ
DASHER Terminal Emulator (Qt version)

DasherQ is a new free terminal emulator for the Data General DASHER-series terminals.  It is written in C++/Qt5 and should
build and run on all desktop platforms supported by that combination.

## Key Features

* Serial interface support at 300, 1200, 2400, 9600 & 19200 baud, 7 or 8 data bits (currently fixed at DG defaults of 1 stop bit, no parity)
* Network Interface (Telnet) support
* Dasher D200 & D210 Emulation
* 15 (plus Ctrl & Shift) Dasher Function keys, Hold, Local Print and Cmd-Break keys
* Reverse video, blinking, dim and underlined characters
* Pixel-for-pixel copy of D410 character set
* Session logging to file

## Using DasherQ

### Function Keys
Use the keys simulated on the toolbar in DasherQ - your OS might interfere with the F-keys on your keyboard.  
The Shift and Control keys can be used in conjunction with the simulated F-keys just like a real Dasher.  
The "Brk" key sends a Command-Break signal to the host when connected via the serial interface.  "
Hold" and "Local Print" work as you would expect.

### Emulation Details
[See this chart](http://stephenmerrony.co.uk/dg/uploads/Documentation/Third-Party/ImplementationChart.pdf)
