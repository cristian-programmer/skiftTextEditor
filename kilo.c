/*to close program 
will you have to use q*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <ctype.h>
#include <errno.h>
#include <sys/ioctl.h>

#define KILL_KILO 'q'
#define INFINITY_LOOP 1

#define CTRL_KEY(k) ((k) & 0x1f)

struct editorConfig {
    int sceenrows;
    int screencols;
    struct termios original_term;
};
struct editorConfig E;


void editorRawRows(){
    int y;
    for(y=0; y < 24; y++){
        write(STDOUT_FILENO, "~\r\n", 3);
    }
}

void editorRefreshScreen(){
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    editorRawRows();
    write(STDOUT_FILENO, "\x1b[H", 3);
}

void editorRefreshScreenExit(){
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
}

void die(const char *error){
    editorRefreshScreenExit();
    perror(error);
    exit(1);
}

char editorReadKey(){
    int  nread;
    char c;
    while((nread = read(STDIN_FILENO, &c, 1)) != 1){
        if(nread == -1 && errno != EAGAIN) die("read");
    }
    return c;
}

int getWindowSize(int *rows, int *cols){
    struct winsize ws;

    if(1 || ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 
    || ws.ws_col == 0){
        if(write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
        editorReadKey();
        return -1;
    }else{
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

void editorProcessKeyPress(){
    char c = editorReadKey();
    switch (c)
    {
    case CTRL_KEY('q'):
        editorRefreshScreenExit();
        exit(0);
        break;
    }
}

void disableRawMode(){
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.original_term)== -1)
        die("tcsetattr");
}

void enableRawMode(){   
    if(tcgetattr(STDIN_FILENO, &E.original_term)== -1)
        die("tcgetattr");
    atexit(disableRawMode); // is call where exit of the program
    
    struct termios raw =  E.original_term;
    
    raw.c_cflag &= ~( BRKINT| ICRNL| ISTRIP | IXON );
    raw.c_cflag &= ~(OPOST);
    raw.c_cflag &= ~(CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
     die("tcsetattr");

}

void initEditor(){
    if(getWindowSize(&E.sceenrows, &E.screencols) == -1) 
        die("getWindowSize");
}

void editorDrawRows(){
    int y;
    for(y =0; y < E.sceenrows; y++){
        write(STDOUT_FILENO, "~\r\n", 3);
    }
}

int main(){

    enableRawMode();
    initEditor();

    while (INFINITY_LOOP){
        editorRefreshScreen();
        editorProcessKeyPress();
    }

    return 0;
}