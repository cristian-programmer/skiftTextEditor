/*to close program 
will you have to use q*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <ctype.h>

struct termios original_term;

void disableRawMode(){
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_term);
}

void enableRawMode(){   
    tcgetattr(STDIN_FILENO, &original_term);

    struct termios raw =  original_term;
    
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_cflag &= ~( BRKINT| ICRNL| ISTRIP | IXON );
    raw.c_cflag &= ~(OPOST);
    raw.c_cflag &= ~(CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN];
    raw.c_cc[VTIME];
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    atexit(disableRawMode); // is call where exit of the program
}



int main(){
    

    enableRawMode();
    
    char c;
    printf("STDIN_FILENO %i \n", STDIN_FILENO);
    
    while (read(STDIN_FILENO, &c, 1) == 1 && c!= 'q'){
        if(iscntrl(c)) {
            printf("%d\n", c);
        }else {
            printf("%d ('%c')\n", c, c);
        }
    }

    return 0;
}