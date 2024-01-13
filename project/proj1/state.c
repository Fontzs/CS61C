#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row,
                         unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Task 1 */
game_state_t* create_default_state() {
  game_state_t* state = (game_state_t*)malloc(sizeof(game_state_t));
  state->num_rows = 18;
  long unsigned int col =
      21;  // one more position for '\0' since it's a string.
  state->board = (char**)malloc(state->num_rows * sizeof(char*));
  for (int i = 0; i < state->num_rows; i++) {
    state->board[i] = (char*)malloc(sizeof(char) * col);
  }
  for (int i = 0; i < state->num_rows; i++) {
    for (int j = 0; j < col; j++) {
      if (i == 0 || j == 0 || i == state->num_rows - 1 || j == col - 2)
        state->board[i][j] = '#';
      else if (i == 2 && j == 9)
        state->board[i][j] = '*';
      else
        state->board[i][j] = ' ';

      if (j == col - 1) state->board[i][j] = '\0';
    }
  }
  state->num_snakes = 1;
  state->snakes = malloc(sizeof(snake_t) * state->num_snakes);

  unsigned int head_row = 2;
  unsigned int head_col = 4;
  state->snakes[0].head_row = head_row;
  state->snakes[0].head_col = head_col;

  unsigned int tail_row = 2;
  unsigned int tail_col = 2;
  state->snakes[0].tail_row = tail_row;
  state->snakes[0].tail_col = tail_col;
  state->snakes[0].live = true;

  state->board[head_row][head_col] = 'D';
  state->board[tail_row][tail_col] = 'd';
  state->board[tail_row][tail_col + 1] = '>';

  return state;
}

/* Task 2 */
void free_state(game_state_t* state) {
  free(state->snakes);
  for (unsigned int i = 0; i < state->num_rows; i++) {
    free(state->board[i]);
  }
  free(state->board);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  for (int i = 0; i < state->num_rows; i++) {
    fprintf(fp, "%s\n", state->board[i]);
  }

  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row,
                         unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  if (c == 'w' || c == 'a' || c == 's' || c == 'd')
    return true;
  else
    return false;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  if (c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x')
    return true;
  else
    return false;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  if (is_head(c) || is_tail(c) || c == '<' || c == '^' || c == 'v' || c == '>')
    return true;
  return false;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  if (c == '^')
    return 'w';
  else if (c == '<')
    return 'a';
  else if (c == 'v')
    return 's';
  else if (c == '>')
    return 'd';
  return '\0';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  if (c == 'W')
    return '^';
  else if (c == 'A')
    return '<';
  else if (c == 'S')
    return 'v';
  else if (c == 'D')
    return '>';
  return '\0';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  if (c == 'v' || c == 's' || c == 'S') return cur_row + 1;
  if (c == '^' || c == 'w' || c == 'W') return cur_row - 1;
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  if (c == '>' || c == 'd' || c == 'D') return cur_col + 1;
  if (c == '<' || c == 'a' || c == 'A') return cur_col - 1;
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake
  is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  char x = get_board_at(state, state->snakes[snum].head_row,
                        state->snakes[snum].head_col);
  unsigned int row = get_next_row(state->snakes[snum].head_row, x);
  unsigned int col = get_next_col(state->snakes[snum].head_col, x);

  x = get_board_at(state, row, col);
  return x;
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the
  head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  unsigned int row = state->snakes[snum].head_row;
  unsigned int col = state->snakes[snum].head_col;
  char x = get_board_at(state, row, col);
  state->snakes[snum].head_row = get_next_row(row, x);
  state->snakes[snum].head_col = get_next_col(col, x);
  char y = head_to_body(x);
  state->board[row][col] = y;
  state->board[state->snakes[snum].head_row][state->snakes[snum].head_col] = x;

  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  unsigned int pre_row = state->snakes[snum].tail_row;
  unsigned int pre_col = state->snakes[snum].tail_col;
  char x = get_board_at(state, pre_row, pre_col);
  unsigned int cur_row = get_next_row(pre_row, x);
  unsigned int cur_col = get_next_col(pre_col, x);
  state->snakes[snum].tail_row = cur_row;
  state->snakes[snum].tail_col = cur_col;
  char y = get_board_at(state, cur_row, cur_col);
  char z = body_to_tail(y);

  state->board[cur_row][cur_col] = z;
  state->board[pre_row][pre_col] = ' ';
  return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  for (unsigned int i = 0; i < state->num_snakes; i++) {
    char x = next_square(state, i);
    if (x == '#' || is_snake(x)) {
      state->board[state->snakes[i].head_row][state->snakes[i].head_col] = 'x';
      state->snakes[i].live = false;
    } else if (x == '*') {
      update_head(state, i);
      add_food(state);
    } else {
      update_head(state, i);
      update_tail(state, i);
    }
  }
  // print_board(state, stdout);
  return;
}

/* Task 5 */
game_state_t* load_board(char* filename) {
  game_state_t* state = malloc(sizeof(game_state_t));
  FILE* f = fopen(filename, "r");
  if (f == NULL) {
    free(state);
    exit(-1);
  }

  char c;
  unsigned int num_rows = 0;
  while (!feof(f)) {
    c = (char)fgetc(f);
    if (c == '\n') {
      num_rows++;
    }
  }

  state->num_rows = num_rows;
  state->board = malloc(num_rows * sizeof(char*));

  rewind(f);
  char str[1000000];
  for (int i = 0; fgets(str, 1000000, f); i++) {
    state->board[i] = malloc((strlen(str) + 1) * sizeof(char));
    strcpy(state->board[i], str);
    state->board[i][strlen(str) - 1] = '\0';
  }

  fclose(f);
  return state;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
  unsigned int counter = 0;
  unsigned int row, col;
  for (unsigned int i = 0; i < state->num_rows; i++) {
    for (unsigned int j = 0; get_board_at(state, i, j) != '\0'; j++) {
      char x = get_board_at(state, i, j);
      if (is_tail(x)) counter++;
      if (counter > snum) {
        row = i;
        col = j;
        break;
      }
    }
    if (counter > snum) break;
  }

  char y = get_board_at(state, row, col);
  // print_board(state, stdout);

  while (!is_head(y)) {
    row = get_next_row(row, y);
    col = get_next_col(col, y);
    y = get_board_at(state, row, col);
    // print_board(state, stdout);
    // printf("%d %d\n", row, col);
  }
  state->snakes[snum].head_row = row;
  state->snakes[snum].head_col = col;

  return;
}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  unsigned int num = 0;
  unsigned int tail_row[100000];
  unsigned int tail_col[100000];
  // find、set tail and get the num of snakes
  for (unsigned int i = 0; i < state->num_rows; i++) {
    for (unsigned int j = 0; j < strlen(state->board[i]); j++) {
      char tmp = get_board_at(state, i, j);
      if (is_tail(tmp)) {
        tail_row[num] = i;
        tail_col[num] = j;
        num++;
      }
    }
  }

  state->snakes = malloc(num * sizeof(snake_t));
  state->num_snakes = num;

  for (unsigned i = 0; i < num; i++) {
    state->snakes[i].tail_row = tail_row[i];
    state->snakes[i].tail_col = tail_col[i];
    state->snakes[i].live = true;
    find_head(state, i);
  }
  return state;
}
