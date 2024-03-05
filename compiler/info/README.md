I included the link to a MIPS resource in gencode.c
Feel free to use others

I gave a small example for a couple of cases,
**these may not be complete and correct or may "skip over" some logic.**

***These are not meant to be solutions but as a strategy starting point***

***Some things, e.g., what should gencode() return, are left as open questions***

***You do not need to start with or use any of these files***

Assignment 4 should require modifications / additions *such as* these provided:

# driver.c:
**We will need to include a new header, gencode.h**
  this will give us the function definition for a gencode() function
**Other (optional) additions allow**
  optional specification of output file rather than stdout
  optional usage of stdin rather than an input file

# gencode.c:
**Here we will have the function we call with the root of the AST**
  This function will recursively print the AST, similar to assignment 2
  The prints will contain assembly instructions for nodes
**Things that are good to have:**
  Label counter
    An integer which allows us to make generic (but unique) labels
  Register name char * array
    Allows us to index registers and get their human-readable name
  Function to wipe / reset registers
    However we "count" registers will always reset at 2 points:
      Function definition (after we print a label for the function name)
      The line following a jal (jump-and-link) to some other function
  Manner to get an available register
    Included is the most naiive method

# makefile:
**Add the new gencode file to the project as the other files were added**
