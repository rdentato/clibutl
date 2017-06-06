# Calculator

The program reads a text file with an arithmetic expression on each line
place the parenthesis according operator precedence and calculate the 
result. For example:

    ./calculator << EOD
    3 +4
    1+2+3
    (1+2)*3
    EOD

will give as output:
    
    3 +4
    =(3)+(4)
    =7
    
    1+2*3
    =(1)+(2*3)
    =7
    
    (1+2)*3
    =(((1)+(2))*3)
    =9

# Build
Compile the library and the tests first, then just invoke:
  make
  
