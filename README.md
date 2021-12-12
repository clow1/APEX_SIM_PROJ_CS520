###Project 2

To compile:
  >
    make

To launch the simulator:
  >
    ./apex_sim input_file

When the program starts, it will prompt the user to write a command. If the user input does not match any command or is empty, it will run the simulation until completion.

The user input should match the command exactly
Each command with "SHOW" will print the information each cycle.


Commands:
  >
    'Carriage run'  //step thru each cycle
    'RUN <#cycles>' //# cycles or finish
    'SHOWMEM <start addr> <end addr>' //prints data between addrs
    'SHOWREGS'
    'SHOWRNT'  
    'SHOWLSQ'
    'SHOWIQ'
    'SHOWROB'
    'SHOWBTB'
    'STOP'
    **STARTOVER does not work properly
