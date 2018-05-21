# Extract XIO to plain text

Read SLIP encoded OSC from files and write as space-separated plain
text. For use with x-io Technologies NGIMU implementation of OSC.

## Usage

Extracts data to csv files (one for each message type sent according
to the [User
Manual](http://x-io.co.uk/downloads/NGIMU-User-Manual-v1.3.pdf)) in
the specified directory (defaults to input filename without `.XIO`
extension).

```shell
./xio2csv FILE [DIR]
```
