# This is a small piece of C code to build a fifo reader

It creates a fifo file in /tmp/fifobar where info can be updated.

It will ignore everything except this string types:

+ "DESK@ Whatever you want to write"
+ "VOL@ Whatever you want to write"
+ "CLOCK@ Whatever you want to write"
+ "exit"

This strings must have "\n" as end of line, so must be written with echo command or printf "... \n".
If the reader do not find the end of line, it will ignore the string too.
