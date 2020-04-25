# Text Editor

App built using code for ARM framework. Main.c can be compiled and run on https://cpulator.01xz.net/?sys=arm-de1soc to simulate the ARM DE1-SoC environment. 

How to use the text editor:
- Ensure that you are typing in the first PS/2 CPULator “Type Here” box (address FF200100, not FF200108).
Type freely using your keyboard and move the cursor (denoted with “<”) with arrow keys. 
*Note: due to FPGA/CPUlator limitations, if you type too fast some keys won’t register, this was an unavoidable limitation. 
- To access upper case letters and special characters press shift once. Then press the key with the special character. Shift will stay on until pressed a second time.
- To copy, paste and cut, press control once and then highlight text using the left arrow key. Then use c, v or x respectively to perform your operation. Control will remain on until pressed a second time.

Notable features:
- Use of character buffer and pixel buffer simultaneously 
- Use of dynamic memory allocation and freeing for copy/cut words on to the heap, then pasting at any desired location 
- Reduction of PS/2 key signal bouncing using simple boolean flags 
- Ability to move cursor to mid sentence and add/delete spaces/characters and cause other characters in that line to shift forward/back 
- All formatting keys do their respective jobs (e.g. tab, enter, space etc.)
