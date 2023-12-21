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

/*-------------------------------------------------------------------*
*    GLOBAL VARIABLES AND CONSTANTS                                  *
*--------------------------------------------------------------------*/
/* Control flags */

/* Global constants */

#define ROWS 9
#define COLUMNS 9
#define DEAD 0
#define ALIVE 1
#define MAXFILENAMESIZE 64

/* Global variables */

/* Global structures */
struct cell {
  int current;
  int future;
};

/*-------------------------------------------------------------------*
*    FUNCTION PROTOTYPES                                             *
*--------------------------------------------------------------------*/
void printmatrix(struct cell (*array)[COLUMNS]);
void calculate_game(struct cell (*array)[COLUMNS], int rows, int cols);
int count_neighbors(struct cell (*array)[COLUMNS], int rows, int cols);
void game_from_file(struct cell (*array)[COLUMNS], char *filename);
int check_if_file_is(char *filename);
void randomize_event(struct cell (*array)[COLUMNS], int rows, int cols);
void print_terminal_title();
char ask_terminal_command();
//void instructions_for_user();

/*********************************************************************
*    MAIN PROGRAM                                                      *
**********************************************************************/

int main()  {
  /*We declare an array of "cell" structs*/
  struct cell board [ROWS][COLUMNS]={0,0};

  /*Variable for storing user input,*/
  /*declared default value '\0'*/
  char input='\0';

  /*For reading the game from file,*/
  /*a variable with default "game.txt"*/
  /*for the filename containing the game*/
  char filename[MAXFILENAMESIZE]="";
  
  int check; /*Variable used for checking if user inputted file existed*/
  print_terminal_title();

  do {
      printf("\r\nWhat do you want to do? ");
      input=toupper(ask_terminal_command());

      switch(input) {
        
        /*****Case for option a), reading cells state from file******/
        case 65:
        printf("What is the filename? (Give it in format: name.txt) ");
        fgets(filename, MAXFILENAMESIZE,stdin);

        /*Remove the newline character from string,
          so the filename is in correct form when
          passing it to the file function*/
        filename[strcspn(filename, "\r\n")] = '\0';
        game_from_file(board, filename);
        
        /*To check whether the file existed or not, and execute the
          game if true*/
        check=check_if_file_is(filename);
        
        if (check==1) {
        for (int loop=0;loop<10;loop++) {
          printmatrix(board); /*print first what the situation is in file*/
          calculate_game(board, ROWS, COLUMNS);
          printf("\n\r");
          }
        }

        else
        printf("\n\r");
        break;
        /*----------------------------------------------------------*/    

        /*********Case for option b) randomizing cells state*********/
        case 66:
        randomize_event(board,ROWS,COLUMNS);

        for (int loop=0;loop<10;loop++) {
          calculate_game(board, ROWS, COLUMNS);
          printmatrix(board);
          printf("\n\r");
        }

        printf("\n\r");
        break;
        /*----------------------------------------------------------*/

        default:
        printf("\r\n");
      }
    }   while (input!='q' && input!='Q');
  
  return 0;

} /* end of main */

/*********************************************************************
*    FUNCTIONS                                                       *
**********************************************************************/

/*********************************************************************
 NAME:  printmatrix()
 DESCRIPTION: prints the current situation of cells
	Input: Gets matrixarray as a reference
	Output: matrix
  Used global variables:
 REMARKS when using this function: consts COLUMNS and ROWS used
*********************************************************************/

void printmatrix(struct cell (*array)[COLUMNS]) {
  int i,j;
  int newlinebreak=0;

  for (i=0;i<ROWS;i++) {
    for (j=0;j<COLUMNS;j++) {
      printf("%d ",array[i][j].current);
      newlinebreak++;

      if (newlinebreak==COLUMNS) { 
        newlinebreak=0;
        printf("\r\n"); /*Starts printing a new row from new line*/
      } 
    }
  }
}

/*********************************************************************
 NAME:  calculate_game()
 DESCRIPTION: this function loops through the whole board and checks
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
      
      /*First we count how many neighbor each cell has*/
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
 DESCRIPTION: counts amount of neighbors next to a given cell and
  returns the amount

	Input:
	Output: amount of neighbors
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
 NAME:  game_from_file()
 DESCRIPTION: reads the game starting situation from file
	Input: user given file
	Output:
  Used global variables:
 REMARKS when using this function:
 consts COLUMNS and ROWS used
*********************************************************************/

void game_from_file(struct cell (*array)[COLUMNS], char *filename) {
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
  
  if (fp==NULL) {
    printf("\r\nError; couldn't open the file,");
    printf(" filename was %s\r\ncheck instructions for", filename);
    printf(" user\r\n");
    return;
  }

  else if (ferror(fp)) {
    printf("Error reading file %s: %s\n", filename, strerror(errno));
    fclose(fp);
    return;
  } 

  else if (feof(fp)) {
    printf("Unexpected end of file %s\n", filename);
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
      printf("Invalid characters in the file\r\n");
      printf("Check instructions for user\r\n");
      fclose(fp);
      return;
    }
  }
    getchar();
    fclose(fp);
}

/*********************************************************************
 NAME:  randomize_event()
 DESCRIPTION: generates random amount of alive and dead cells to the
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
 NAME:  print_terminal_title()
 DESCRIPTION: prints options for player
	Input:
	Output:
  Used global variables:
 REMARKS when using this function:
 this is temporary demo before using ncurses
*********************************************************************/

void print_terminal_title() {
  printf("****************************");
  printf("\r\n    Game of life           * ");
  printf("\r\n a) Read game from file    *" );
  printf("\r\n b) Randomize event        *" );
  printf("\r\n q) to quit game           *\r\n");
  printf("****************************");
}

/*********************************************************************
 NAME: ask_command()
 DESCRIPTION: Reads user input (string) and returns the first element
  of the user inputted string array

	Input: User command
	Output: 
  Used global variables:
 REMARKS when using this function:
*********************************************************************/

char ask_terminal_command() {    
  char cmd[64];
  fgets(cmd,64,stdin);
  return cmd[0];
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