/***************************************************************************
 *   Copyright (C) 2023 by Aleksanteri                                     *
 *   e2202945@edu.vamk.fi                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/*********************************************************************
 1.  Game of Life   
 2.  DESCRIPTION
 3.  VERSIONS
       Original: 8.4.2023 Aleksanteri
       Version history:
       First version with working gamelogic 12.4.2023
       Version with file reading and 
       randomizing game situation           27.4.2023
       Final working product without
       using ncurses (only terminal)        27.4.2023
       Working Ncurses version with
       one population, colors, fileread
       and randomized event (also menu)     03.5.2023
       Fixed how the printing of the
       matrixes (the game board) appears
       on screen, and also the exit
       button (F3) works now better.
       Also some minor visual edits         05.5.2023
**********************************************************************/

/*So there are 3 key principles in the game;
-Death
-Birth
-Survival
  There are certain rules which defines the outcomes
  Each cell is either alive or dead (1-0).
  
  Death: if cell is in state 1, it goes to 0 if the cell has:
  <1 or >4 neighbors alive
  Birth happens when cell is in condition 0 and it has
  exactly 3 alive neighbors.
  
  Survival: Cell remains in the state 1 if it has 2 or 3 live
  neighbors
  
  We will use two-dimensional array of structs in this code (like matrix)
  e.g int ar[x][y]; <---Where x is the n of rows and y is n of columns
  These cells will be represented in the matrix with their conditions
  0 or 1*/

/*-------------------------------------------------------------------*
*    HEADER FILES                                                    *
*--------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <ncurses.h>
#include <curses.h>
#include <unistd.h>

/*-------------------------------------------------------------------*
*    GLOBAL VARIABLES AND CONSTANTS                                  *
*--------------------------------------------------------------------*/
/* Control flags */

/* Global constants */

#define ROWS 9
#define COLUMNS 9
#define DEAD 0
#define ALIVE 1
#define MAXFILENAMESIZE 128

/* Global variables */

/* Global structures */
struct cell {
  int current;
  int future;
};

/*-------------------------------------------------------------------*
*    FUNCTION PROTOTYPES                                             *
*--------------------------------------------------------------------*/
void print_ncurses_matrix(WINDOW *gamewindow, struct cell (*array)[COLUMNS], int rows, int cols);
void calculate_game(struct cell (*array)[COLUMNS], int rows, int cols);
int count_neighbors(struct cell (*array)[COLUMNS], int rows, int cols);
void game_from_file_ncurses(struct cell (*array)[COLUMNS], char *filename, int x, int y);
int check_if_file_is(char *filename);
void randomize_event(struct cell (*array)[COLUMNS], int rows, int cols);
void print_ncurses_title(int x, int y);
WINDOW *create_selectbutton(int height, int width, int starty, int startx, int y, int x);
WINDOW *create_gamewindow(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);
//void instructions_for_user();

/*********************************************************************
*    MAIN PROGRAM                                                      *
**********************************************************************/

int main()  {
  initscr();
  clear();
  has_colors();

  if (has_colors() == FALSE) {
    endwin();
    printf("Your terminal does not support color\r\n");
    napms(5000);
    exit(1);
  }

  start_color();
  init_pair(1, COLOR_WHITE, COLOR_GREEN);  /*for menu*/
  init_pair(5, COLOR_WHITE, COLOR_BLUE); /*for matrix*/
  init_pair(3, COLOR_RED, COLOR_BLACK); /*For fileread error msg*/
  init_pair(9, COLOR_BLACK, COLOR_GREEN); /*for menu title and underline*/
  curs_set(0);
  WINDOW *selectbutton;
  WINDOW *gamewindow;

  /*We declare an array of "cell" structs*/
  struct cell board [ROWS][COLUMNS]={0,0};

  /*Variables for holding the placement of the selectbox*/
  int startx, starty, width, height;

  /*Variable for storing user input,*/
  int input;

  /*For reading the game from file,*/
  /*for the filename containing the game*/
  char filename[MAXFILENAMESIZE]="";
  
  int check; /*Variable used for checking if user inputted file existed*/
  int y = LINES/2;
  int x = COLS/2;
  height = 3;
  width = 3;
  starty = ((LINES - height) / 2) +5;
  startx = ((COLS - width) / 2) -17;

  /*The select button what the player uses to navigate in the menu is
  a window*/
  selectbutton = create_selectbutton(height, width, starty, startx, y, x);
  wbkgd(selectbutton, COLOR_PAIR(1));
  bkgd(COLOR_PAIR(1));
  print_ncurses_title(y,x);
  refresh();
  wrefresh(selectbutton);
  keypad(stdscr, TRUE);
  noecho();
  nodelay(stdscr, TRUE);

  while((input = getch()) != KEY_F(1))  {

    switch(input) {
	      case KEY_UP:
        /*Check the selectbutton's location and if it's lower
          than the highest option (read from file) it allows
          to move the selectbar up*/
        if (starty<y+2) {
          NULL;
          break;
        }

        else if (starty>y+2) {
        destroy_win(selectbutton);
        starty--;
        selectbutton=create_selectbutton(height, width, starty, startx, y, x);
        wrefresh(selectbutton);
        break;
        }

        else {
          NULL;
          break;
        }
        break;

        case KEY_DOWN:
        /*Same here but for moving the selectbutton down*/
        if (starty>y+4) {
          NULL;
          break;
        }
        
        else if (starty<y+4) {
        destroy_win(selectbutton);
        starty++;
        selectbutton=create_selectbutton(height, width, starty, startx, y, x);
        wrefresh(selectbutton);
        break;
        }

        else {
          NULL;
          break;
        }
        break;

        /*These following cases includes the calls
          for the game's actual functions*/
        case '\n': /*When player presses enter*/
        
        /*When player selects to read the game from file
          (if statement check used for checking if the
          selectbutton was in the point of "Read game
          from file")*/
        if (starty==y+2) {
          clear();
          WINDOW* askwin = newwin(10, 42, y-10, x-21);
          box(askwin, 0, 0);
          mvwprintw(askwin,1,1,"Enter filename");
          mvwprintw(askwin,2,1,"(Give it in format: name.txt)");
          wrefresh(askwin);
  
          /*Read filename from the user*/
          echo();
          wmove(askwin, 4, 1);
          wgetnstr(askwin, filename, MAXFILENAMESIZE-1);
          noecho();

        /*To check whether the file existed or not, and execute the
          game if true*/
          check=check_if_file_is(filename);

        if (check==1) {
          delwin(askwin);
          game_from_file_ncurses(board, filename, x, y);
          WINDOW* askwin = newwin(10, 42, y-10, x-21);
          box(askwin, 0, 0);
          mvwprintw(askwin,5,6,"Initializing game from file");
          wrefresh(askwin);
          napms(1500);
          wclear(askwin);
          delwin(askwin);
          clear();

          /*Here we create a window for printing the matrix*/
          gamewindow=create_gamewindow(ROWS,COLUMNS,starty,startx);
          int command;
          attron(COLOR_PAIR(1));
          mvaddstr(y-10, x-8, "F3 BACK TO MENU");
          attroff(COLOR_PAIR(1));
          nodelay(gamewindow, TRUE);
          nodelay(stdscr, TRUE);
          keypad(gamewindow, TRUE);
          keypad(stdscr, TRUE);
          noecho();
          cbreak();
          refresh();
          
          /*The game plays until player has pressed F3*/
          do {
            print_ncurses_matrix(gamewindow, board, ROWS, COLUMNS);
            calculate_game(board, ROWS, COLUMNS);
            napms(1000);
            command = getch();
          } while (command!=KEY_F(3));

          print_ncurses_title(y,x);
          refresh();
          break;
          }

          else {
          mvwprintw(askwin,5,1,"Couldn't find a file named: %s ",filename);
          mvwprintw(askwin,6,1,"Returning to menu");
          wrefresh(askwin);
          napms(1500);
          delwin(askwin);
          clear();
          print_ncurses_title(y,x);
          refresh();
          break;
          }
          
        }
        
        /*Here we have an if statement to check
          if the selectbutton was in point of the
          "Randomize game"*/
        else if (starty==y+3) {
          clear();
          randomize_event(board, ROWS, COLUMNS);
          gamewindow=create_gamewindow(ROWS,COLUMNS,starty,startx);
          int command;
          attron(COLOR_PAIR(1));
          mvaddstr(y-10, x-8, "F3 BACK TO MENU");
          attroff(COLOR_PAIR(1));
          nodelay(gamewindow, TRUE);
          nodelay(stdscr, TRUE);
          keypad(gamewindow, TRUE);
          keypad(stdscr, TRUE);
          noecho();
          cbreak();
          refresh();

          do {
            calculate_game(board, ROWS, COLUMNS);
            print_ncurses_matrix(gamewindow, board, ROWS, COLUMNS);
            napms(1000);
            command = getch();
          } while (command!=KEY_F(3));
          
          clear();
          print_ncurses_title(y,x);
          refresh();
          break;
        }

        /*If player selects to read instructions*/
        else if (starty==y+4) {
          mvaddstr(1, 1, "-TEST, NO INSTRUCTIONS DELIVERED YET-");
          break;
        }

        else {
          NULL;
          break;
        }

        break;

        default:
        NULL;
        break;
	    }
    }

    endwin();			/*End curses mode*/
    return 0;
    
} /* end of main */

/*********************************************************************
*    FUNCTIONS                                                       *
**********************************************************************/
/*********************************************************************
 NAME:  print_ncurses_matrix
 DESCRIPTION: Prints the current situation of cells
	Input:
	Output: matrix
  Used global variables:
 REMARKS when using this function: consts COLUMNS and ROWS used
*********************************************************************/

void print_ncurses_matrix(WINDOW *gamewindow, struct cell (*array)[COLUMNS], int rows, int cols) {
  int i,j;
  
  for (i=0;i<rows;i++) {
    for (j=0;j<cols;j++) {
      mvwprintw(gamewindow,i,j,"%d",array[i][j].current);
    }
  }

  wrefresh(gamewindow);

}

/*********************************************************************
 NAME:  calculate_game()
 DESCRIPTION: This function loops through the whole board and checks
 whether the cell is alive or dead in the next generation. This is where
 the actual game's logic is located
	Input:
	Output: 
  Used global variables:
 REMARKS when using this function:
 consts COLUMNS, ROWS, ALIVE, DEAD used
*********************************************************************/

void calculate_game(struct cell (*array)[COLUMNS], int rows, int cols) {
  int i,j;
  int neighbors=0;
  
  for (i=0;i<rows;i++) {
    for (j=0;j<cols;j++) {
      
      /*First we count how many neighbors each cell has*/
      neighbors=count_neighbors(array,i,j);

      /*Then we check each cell's state in future,
        is it dead, alive or born*/
      if (array[i][j].current==ALIVE&&(neighbors==2||neighbors==3)) {
          array[i][j].future=ALIVE;
      }

      else if (array[i][j].current==DEAD && neighbors==3) {
          array[i][j].future=ALIVE;
      }

      else 
      array[i][j].future=DEAD;
      neighbors=0;
    }
  }

  for (i=0;i<rows;i++) {
    for (j=0;j<cols;j++) {
      array[i][j].current = array[i][j].future;
    } /*assign the future generation as current, so the next*/
  } /*generation gets printed*/

}

/*********************************************************************
 NAME:  count_neighbors()
 DESCRIPTION: Counts amount of neighbors next to a given cell and
  returns the amount. This is also part of the game's logic
	Input:
	Output:
  Used global variables:
 REMARKS when using this function:
 consts COLUMNS, ROWS, ALIVE used
*********************************************************************/

int count_neighbors(struct cell (*array)[COLUMNS], int rows, int cols) {
  int i,j;
  int amount=0;
  
  /*Loops through 3x3 grid around the given cell, if it finds an alive
    cell, it increments the amount by one*/

  for (i=rows-1;i<=rows+1;i++) {
    for (j=cols-1;j<=cols+1;j++) {
      if (i>=0 && j>=0 && i<ROWS && j<COLUMNS && 
      (i!=rows || j!=cols) && array[i][j].current==ALIVE) {
        amount++;
      } 
    }
  }

  return amount;
 
}

/*********************************************************************
 NAME:  randomize_event()
 DESCRIPTION: Generates random amount of alive and dead cells to the
  board
	Input:
	Output:
  Used global variables:
 REMARKS when using this function:
 consts COLUMNS, ROWS, ALIVE used
*********************************************************************/

void randomize_event(struct cell (*array)[COLUMNS], int rows, int cols) {
  int i,j,state,source;
  srand(time(NULL));

  for(i=0;i<rows;i++) {
   for(j=0;j<cols;j++) {
    source=rand() % 100;  /*we generate a random number and we use it*/
    state=source%2;     /*as a "source" for generating a remainder*/
                        /*either 1 or 0*/
    if (state==1) {
      array[i][j].current=ALIVE;
    }
    
    if (state==0) {
      array[i][j].current=DEAD;
    }
   }
  }
}

/*********************************************************************
 NAME: check_if_file_is()
 DESCRIPTION: Checks the filename user gave and if it can be opened,
  returns 1, else 0;
	Input:
	Output: 
  Used global variables:
 REMARKS when using this function:
*********************************************************************/

int check_if_file_is(char *filename) {
  FILE *fp=fopen(filename,"r");
  
  if (fp!=NULL) {
    fclose(fp);
    return 1;
  }

  else {
    return 0;
  }
}

/*********************************************************************
 NAME:  print_ncurses_title();
 DESCRIPTION: Prints menu
	Input:
	Output:
  Used global variables:
 REMARKS when using this function:
*********************************************************************/

void print_ncurses_title(int y, int x) {
  clear();
  bkgd(COLOR_PAIR(1));

  attron(COLOR_PAIR (9));
  mvaddstr(y-6, x-14, "****************************");
  mvaddstr(y-5, x-14, "*");
  mvaddstr(y-5, x+13, "*");
  mvaddstr(y-4, x-14, "*                          *");
  mvaddstr(y-3, x-14, "*");
  mvaddstr(y-3, x+13, "*");
  mvaddstr(y-2, x-14, "*");
  mvaddstr(y-2, x+13, "*");
  mvaddstr(y-1, x-14, "*");
  mvaddstr(y-1, x+13, "*");
  mvaddstr(y-0, x-14, "****************************");
  attroff(COLOR_PAIR (9));

  mvaddstr(y-5, x-13, "       Game of life       ");
  mvaddstr(y-3, x-13, "         Made by:         ");
  mvaddstr(y-2, x-13, "   Aleksanteri Koivisto   ");
  mvaddstr(y-1, x-13, "           2023           ");
  mvaddstr(y+3, x-14, "-    READ GAME FROM FILE   -");
  mvaddstr(y+4, x-14, "-      RANDOMIZE GAME      -");
  mvaddstr(y+5, x-14, "-READ INSTRUCTIONS FOR USER-");
  mvaddstr(y+10, x-14, "         F1 TO QUIT");
  attron(COLOR_PAIR (9));
  mvaddstr(y+11, x-14, "         __________");
  attroff(COLOR_PAIR (9));
  refresh();
}

/*********************************************************************
 NAME:  create_selectbutton();
 DESCRIPTION: This creates a new selectbutton when player presses
 up or down key
	Input:
	Output:
  Used global variables:
 REMARKS when using this function:
*********************************************************************/

WINDOW *create_selectbutton(int height, int width, int starty, int startx, int y, int x)  {
  WINDOW *local_win;
  local_win = newwin(height, width, starty, startx);
  wbkgd(local_win, COLOR_PAIR(9));
  box(local_win, 0 , 0);
  wrefresh(local_win);
  return local_win;
}

/*********************************************************************
 NAME:  create_gamewindow();
 DESCRIPTION: Create's a window what is used for printing the matrix
	Input:
	Output:
  Used global variables:
 REMARKS when using this function:
*********************************************************************/

WINDOW *create_gamewindow(int height, int width, int starty, int startx) {
  WINDOW *local_win;
  local_win = newwin(height, width, starty, startx);
  wbkgd(local_win, COLOR_PAIR(5));
  box(local_win, 0 , 0);
  wrefresh(local_win);
  return local_win;
}

/*********************************************************************
 NAME:  destroy_win();
 DESCRIPTION: Destroys the old window and it's borders, used when
 moving the menu selection button
	Input:
	Output:
  Used global variables:
 REMARKS when using this function:
*********************************************************************/

void destroy_win(WINDOW *local_win) {
  wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
  wrefresh(local_win);
  delwin(local_win);
}

/*********************************************************************
 NAME:  game_from_file_ncurses();
 DESCRIPTION: Reads the file given in parameters, loops the content of
 the text file and converts the numbers. Temporarily only accepts
 files with the content's size of defined global contants
	Input:
	Output:
  Used global variables:
 REMARKS when using this function:
 Global consts ROWS, COLUMNS, ALIVE, DEAD used
 The x and y attributes in the parameters are for this function to
 know where to print the possible error texts
*********************************************************************/

void game_from_file_ncurses(struct cell (*array)[COLUMNS], char *filename, int x, int y) {
  clear();
  attron(COLOR_PAIR (5));
  refresh();
  int r=0; /*Variables for looping the rows and columns in the board*/
  int c=0;
  int i,j;
  int state=0; /*Variable for converting char read from file to int*/
  char state_c; /*Variable for storing the read statevalue from file*/

  /*If the player had randomized the cells states before choosing
  to read game from file, we clean the board for the file reading
  so there would be no mixing up with the cells from the randomized
  game and the ones read from the file*/
  for (i=0;i<ROWS;i++) {
    for (j=0;j<COLUMNS;j++) {
      array[i][j].current=DEAD;
      array[i][j].future=DEAD;
    }
  }

  FILE *fp = fopen(filename, "r");

  /*This is like a "doublecheck", since we have already used the
    check_if_file_is() function before this function is executed
    in main*/ 
  if (fp==NULL) {
    attron(COLOR_PAIR(3));
    mvaddstr(y+2,x-16,"                                             ");
    mvprintw(y+3,x-16," ERROR OPENING THE FILE                      ");
    mvprintw(y+4,x-16," FILENAME WAS %s, CHECK INSTRUCTIONS FOR USER", filename);
    mvaddstr(y+5,x-16,"                                             ");
    attroff(COLOR_PAIR(3));
    refresh();
    napms(4000);
    return;
  }

  else if (ferror(fp)) {
    attron(COLOR_PAIR(3));
    mvaddstr(y+2,x-16,"                            ");
    mvprintw(y+3,x-16," ERROR READING FILE %s: %s  ", filename, strerror(errno));
    mvaddstr(y+4,x-16," CHECK INSTRUCTIONS FOR USER");
    mvaddstr(y+5,x-16,"                            ");
    attroff(COLOR_PAIR(3));
    refresh();
    napms(4000);
    fclose(fp);
    return;
  } 

  else if (feof(fp)) {
    attron(COLOR_PAIR(3));
    mvaddstr(y+6,x-16,"                          ");
    mvprintw(y+7,x-16," UNEXPECTED END OF FILE %s", filename);
    mvaddstr(y+8,x-16,"                          ");
    attroff(COLOR_PAIR(3));
    refresh();
    napms(4000);
    fclose(fp);
    return;
  } 

  while (r<ROWS && fscanf(fp,"%c",&state_c)!=EOF) {
    /*The character from the file is stored in the state_c*/
    /*By substracting any digit character with 0, we get the*/
    /*corresponding integer value*/
    /*check if the character is a digit--->*/
    if (state_c>='0' && state_c<='9') { 
      state=state_c-'0';  /*convert the character to an integer*/
      array[r][c].current=state;

      /*This next printf is for debugging, to see what values these 
      variables get during the read from file*/
      /*printf("<%d,%d:%d>", r, c, state);*/
        c++;

    if (c==COLUMNS) {
      r++;
      c=0;

      /*Reads the newline characters away*/
      fscanf(fp, "%c", &state_c);
    }
  } 
    else {
      attron(COLOR_PAIR (3));
      mvaddstr(y+2,x-16,"                                ");
      mvaddstr(y+3,x-16," INVALID CHARACTERS IN THE FILE ");
      mvaddstr(y+4,x-16," CHECK INSTRUCTIONS FOR USER    ");
      mvaddstr(y+5,x-16,"                                ");
      attroff(COLOR_PAIR (3));
      refresh();
      napms(4000);
      fclose(fp);
      return;
    }
  }
    fclose(fp);
}