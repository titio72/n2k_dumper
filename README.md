# n2k_dumper
Dump n2k data on flat files.
This is a sandbox project for testing features of the excellent libray https://github.com/ttlappalainen/NMEA2000 (thanks https://github.com/ttlappalainen).

Initially I used to log n2k stream and analyze it later, but one day my speed sensor gave up, and I modified the code to also detect when the speed-through-water message was not on the n2k stream and replace it with the SOG (not the same, but good enough).

If you don't want to dump info but just the speed-through-water emulator, just commend the line:
#define _DO_DUMP

The physically access the n2k network, the application uses the socket can infrastructure. To change the can device, edit the N2K.cpp (you'll see what to change...).

To build:
# make_deps.sh will download the libraries from https://github.com/ttlappalainen repositories and build them
./make_deps.sh
mkdir build
cd build
cmake ..
make
# have fun



