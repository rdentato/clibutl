 
  (C) 2014 by Remo Dentato (rdentato@gmail.com)
 
 This software is distributed under the terms of the MIT license:
  https://opensource.org/licenses/MIT
     
                     ___   __
                  __/  /_ /  )
          ___  __(_   ___)  /
         /  / /  )/  /  /  /  Minimalist
        /  /_/  //  (__/  /  C utility 
       (____,__/(_____(__/  Library
 
 
Here you can find:

 - `utl.h`    : The header to include in your project's source files
 - `utl.c`    : The C source file with the functions. You can add it
                among your project's source files and compile it with them.                
 - `utl.o`    : The object file you can link directly
 - `libutl.a` : The static linking library
 
  The best option is to statically link with `libutl.a` or recompile `utl.c` 
as a static library. If you just link the object file, you might have warnings
about function that are defined but not used (e.g. if you just use the log
functions but not the pmx functions.

