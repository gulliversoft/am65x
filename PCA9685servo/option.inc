#ifndef OPTION
#error OPTION(short-name, long-name, number-of-trailing-args, description) was not defined!
#endif

OPTION(d, demo, 1, "moves the arm around")
OPTION(h, help, 0, "print all the options and exit")

#undef OPTION