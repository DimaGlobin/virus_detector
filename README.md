# virus_detector

## Description.

This project contains __XOR cipher__ file  `encoder.cpp` and simple __virus detector__ file `antivirus.cpp`. 

XOR cipher read all files in directory and all subdirectories as bytes, and XOR all bytes with a special key.

Virus detector based on fanotify. It monitors the actions that are performed on files in a given directory and kills the process that wants to open the file for modification. But it works not so perfect. Run it with attention.

## Build and run.

```
gcc -o antivirus antivirus.cpp
gcc -o encoder encoder.cpp
./antivirus
./encoder

```