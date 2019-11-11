/*to close program 
will you have to use q*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <ctype.h>
#include <errno.h>

#define KILL_KILO 'q'

#define CTRL_KEY(k) ((k) & 0x1f)

struct termios original_term;


void die(const char *error){
    perror(error);
    exit(1);
}

void disableRawMode(){
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_term)== -1)
        die("tcsetattr");
}

void enableRawMode(){   
    if(tcgetattr(STDIN_FILENO, &original_term)== -1)
        die("tcgetattr");
    atexit(disableRawMode); // is call where exit of the program
    
    struct termios raw =  original_term;
    
    raw.c_cflag &= ~( BRKINT| ICRNL| ISTRIP | IXON );
    raw.c_cflag &= ~(OPOST);
    raw.c_cflag &= ~(CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
     die("tcsetattr");

}



int main(){
    

    enableRawMode();
    
    char c='\0';

    printf("STDIN_FILENO %i \n", STDIN_FILENO);
    if(read(STDIN_FILENO, &c, 1)==-1 && errno != EAGAIN) die("read");

    while (1){
        if(iscntrl(c)) {
            printf("%d\r\n", c);
        }else {
            printf("%d ('%c')\r\n", c, c);
        }

        if(c == CTRL_KEY('q'))break;
    }

    return 0;
}