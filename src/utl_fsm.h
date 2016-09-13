/*

**                     ____
**                    /  __)
**                  _/  /_ _____   ______ 
**                 |   __//  ___) /      \
**                 /  /   \___ \ /  / /  /
**                /  /   (_____/(__/_/__/ 
**               /  /
**               \_/   


[[[

## Finite State Machines

Many piece of software are better understood (and desigend) as
[Finite State Machines](https://en.wikipedia.org/wiki/Finite-state_machine).

There are many ways to represent a FSM in C. Most common methods are:
  - Using a `state` variable and a `switch{...}` within a loop. 
  - Using a table of pointers to functions that will be invoked to
    perform the actions and move from a state to another.
    
These methods, however, hide the structure of the FSM itself which is better
understood as a graph with nodes representing states and arcs representing 
transitions. 

The following macros implement a technique explained by Tim Cooper in the
the article
[*Goto? Yes Goto!*](http://ftp.math.utah.edu/pub/tex/bib/complang.html#Cooper:1991:GYG)
published on the May 1991 issue of the *Computer Language* magazine.

The main advantage is to directly implement the FSM transition diagram
to the point that is extremely easy to draw the diagram from the code (which 
is not an easy task with the other methods).

The code for each state (which performs the actions for that state,
read the next input, etc. ) is enclosed in a `fsmSTATE(statename)` macro;
to move from a state to another the `fsmGOTO(statename)` is used.

The following is an example to show how easy is to relate the code to 
the transition diagram.

   ```C
     fsm {
       fsmSTATE(start_of_the_day) {
         num_cust = 0;
         fsmGOTO(gate_closed);
       }
       
       fsmSTATE(gate_closed) {
         if (ticket valid(getticket())) fsmGOTO(gate_opened);
         if (time_to_close() fsmGOTO(closed_for_the_day);
         fsmGOTO(gate_closed);          
       }
       
       fsmSTATE(gate_opened) {
         if (customer_passed()) {
           num_cust++;
           if (num_cust == 100) fsmGOTO(closed_for_the day);
           if (time_to_close()) fsmGOTO(closed_for_the day);
           fsmGOTO(gate_closed);
         }
         fsmGOTO(gate_opened);
       }
       
       fsmSTATE(closed_for_the_day) {
         printf("%d customer served", num_cust);
       }
     }
   ```

   ``` 
                               ,--------------.      ,----,
                              V                \    V    /
     start_of_the_day --->  gate_closed  ---> gate_opened ---,
                               \   \            ^           /
                                \   `-----------'          /
                                 \                        V
                                  `------> closed_for_the_day
   ```

The original article had many suggestions on how to extended this
basic idea. However, after so many years, I still think that this
is the best way to represent FSM in C code.

** ]]] */

//<<<//

#ifndef UTL_NOFSM

#define fsm           
#define fsmGOTO(x)    goto fsm_state_##x
#define fsmSTATE(x)   fsm_state_##x :

#endif
//>>>//

