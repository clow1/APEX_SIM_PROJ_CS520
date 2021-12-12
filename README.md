Hannah Burkhard, Crystal Low, and Joseph Raskind
12-11-2021

To compile the program run: make all
To run the program execute:
    ./apex_sim <input_file>


When the program starts, it will prompt the user to enter a command. If the user input does not match any of the following commands or is empty, it will run the simulation until completion.

The user input should match the command exactly
Each command with "SHOW" will print the information each cycle, once you have selected a command you must use the carriage return to continue. 


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


Our implementation doesn't include a cycle delay for inserting insturctions into the IQ. Besides the extra delay cycle for inserting into the IQ, we believe we have completed all parts of the given assignment fully. 
