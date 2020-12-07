# How do you use this?
## 1 - Get logisim
Logisim is a free cross-platform logic simulation/visualization program. You can download it (here)[http://www.cburch.com/logisim/]. Once you have it, use it to open cpu.circ.

## 2 - Assemble part1.s into a rom image
Use the included python program to create a ROM image for the CPU (e.g. part1.s). It has a shmansy command line interface (courtesy of docopt) so you can use `./asm.py -h` and it will help you out.

Oh, that reminds me, you may need to `python3 -m pip install docopt`.

## 3 - Encode the problem input to use in the ram
For this you need to use `./filetorom.py`. It also has a handy help message. Use this on the included (or your own) problem inputs to generate a file that Logisim will understand.

## 4 - Load program and ROM into the CPU
In logisim, right click on the memory labeled "random access memory" and select "load image". Then navigate to the problem input file you generated in step 3. Next, right click the memory labeled "program memory" and load the program you assembled in step 2. All done!

Stored program concept? What's that?

## 5 - Run the simulation
Here are some useful controls for logisim:
- `CNTRL + k` enable/disable clock, use this to start the simulation.
- `CNTRL + r` reset system (warning! this will clear the RAM, so you will have to load it again as per step 4)

To speed things up click simulate -> tick frequency and select the highest one. Even at the highest speed it will take a few minutes to run.

For puzzle 1 you can see the progress of the computation in registers r0-r2:
- r0 = current line output
- r1 = current line address
- r2 = highest output

## 6 - Read outputs
I've spent far too much time on this, so I didn't bother to implement a clever output system. The result will simply appear in r0 in hexadecimal. You will know when the program is complete because most of the blinking will have stopped (very sophisticated I know).
