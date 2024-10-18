# Packsend
A small utility for constructing packets from various data sources and outputting them to sockets, serial ports, or files.

TODO: Documentation

## Syntax
The program arguments can be divided into two categories: transmission, and data parameters. There is also a "-h" or "--help" parameter which shows all available parameters.

## Transmission parameters
These specify the mode of output - i.e. to a TCP socket, as a UDP datagram or to a serial port. Examples:

### Sending

`proto=tcp ip=127.0.0.1 port=7`
: Send data as a TCP stream to 127.0.0.1 port 7.
`proto=serial dev=/dev/ttyS1 baud=38400 flow=nortscts`
: Send data to the serial port at /dev/ttyS1 without rst/cts pins and at 38400 baud speed
`proto=stdout`
: Send data to standard output

### Response output

`getreply=first outfmt=ahex`
: Read the first reply and output it as human-readable ascii with a side-order of hex.
`getreply=timeout timeout=0.5 outfmt=raw`
: Read replies the first 0.5 seconds and write the raw bytes as output.

## Data parameters

These parameters append data to a buffer which is sent according to the
transmission parameters. Examples:

`text=foo`
: Append the text "foo" to the buffer in the native charset.
`hex=666f6f`
: Append the bytes 0x66 0x6f and 0x6f to the buffer.
`file=data.txt`
: Append the contents of the file "data.txt" to the buffer.
`cstype=crc32 cs=4-`
: Append a CRC32 checksum from buffer position 4 to the current position.







