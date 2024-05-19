### Usage:

Use the following command to get the executable file:

g++ -O3 Fiduccia_Mattheyses.h Fiduccia_Mattheyses.cpp -o Fiduccia_Mattheyses 

./Fiduccia_Mattheyses 100.txt 3 6 1

100.txt : input file name

3 : how many iteration you wanna the program to run, at least 1;

6 : random seed, decide the initial partition, default is 0 which will based on timer to get the result;

1 : verbose flag, if you wanna see the progress set it to 1, not set it to 0;

For 100.txt, best seed is 3, Best cut num is 19

For 500.txt, best seed is 2, Best cut num is 146

For 5000.txt, best seed is 67, Best cut num is 2624

For 10000.txt, best seed is 11, Best cut num is 7724
