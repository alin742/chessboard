#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#define W 8
#define H 8
#define BG_WHITE "\x1b[48;5;250m"
#define BG_GRAY "\x1b[48;5;245m"
#define BG_CYAN "\x1b[30;46m"
#define FG_BLACK "\x1b[1;30m"
#define FG_WHITE "\x1b[1;37m"
#define RESET "\x1b[0m"

typedef enum
{
  ROOK,
  KNIGHT,
  BISHOP,
  QUEEN,
  KING,
  PAWN
}
PieceType;

typedef enum
{
  BLACK, WHITE
}
Color;

typedef enum
{
  UP, DOWN
}
AttackDirection;

typedef struct
{
  int row;
  int col;
}
Position;

typedef struct
{
  PieceType type;
  Color color;
  Position pos;
  int n_movements;
}
Piece;

typedef struct
{
  char name[2];
  bool occupied;
  Piece *piece;
  Color color;
}
Cell;

typedef struct
{
  Cell board[W*H];
  Piece white_player[2*W];
  Piece black_player[2*W];
  int turn;
  bool whitesTurn;
}
ChessGame;

Color
oppositeColor ( Color color )
{
  switch (color) {
    case WHITE: color = BLACK; break;
    case BLACK: color = WHITE; break;
  }
  return color;
}

int
sgn (int num)
{
  return (num > 0) - (num < 0);
}

void
cellName ( char *name, int index )
{
  char column       = 'A';
  int row           = 1;
  int column_offset = index % H;
  int row_offset    = index / W;
  snprintf(name, 2, "%c%i", column+column_offset, row+row_offset);
}

const char *
formatPiece ( Piece *piece )
{
  bool isWhite = piece->color == WHITE;
  static char format[] = "                   ";
  int n = 18;
  switch (piece->type) {
    case PAWN:
      snprintf(format, n, "%s♟︎", isWhite ? FG_WHITE : FG_BLACK);
      break;
    case ROOK:
      snprintf(format, n, "%s♜", isWhite ? FG_WHITE : FG_BLACK);
      break;
    case KNIGHT:
      snprintf(format, n, "%s♞", isWhite ? FG_WHITE : FG_BLACK);
      break;
    case BISHOP:
      snprintf(format, n, "%s♝", isWhite ? FG_WHITE : FG_BLACK);
      break;
    case KING:
      snprintf(format, n, "%s♚", isWhite ? FG_WHITE : FG_BLACK);
      break;
    case QUEEN:
      snprintf(format, n, "%s♛", isWhite ? FG_WHITE : FG_BLACK);
      break;
    default:
      assert(false && "Unreachable!!\n");
  }
  return format;
}

void
initBoard ( Cell *board )
{
  Color curr_color = WHITE;
  for (int i = 0; i < W*H; i++) {
    board[i].color    = curr_color;
    board[i].occupied = false;
    board[i].piece    = NULL;
    cellName(board[i].name, i);
    if (i%W != W-1)
      curr_color = oppositeColor(curr_color);
  }
}

void
initPlayer ( Color color, Piece *player )
{
  for (int i = 0; i < W/2; i++) {
    player[i].color          = color;
    player[W - i-1].color    = color;
    switch (i) {
      case 0: 
        player[i].type       = ROOK;
        player[W - i-1].type = ROOK;
        break;
      case 1:
        player[i].type       = KNIGHT;
        player[W - i-1].type = KNIGHT;
        break;
      case 2:
        player[i].type       = BISHOP;
        player[W - i-1].type = BISHOP;
        break;
      case 3:
        player[i].type       = QUEEN;
        player[W - i-1].type = KING;
        break;
      default:
        assert(false && "UNREACHABLE!!");
    }
  }
  for (int i = W; i < 2*W; i++) player[i].color = color;
  for (int i = W; i < 2*W; i++) player[i].type  = PAWN;
  for (int i = 0; i < 2*W; i++) player[i].n_movements = 0;
}

Position
checkWay(Cell *board, Position curr, Position dest)
{
  if (curr.row == dest.row && curr.col == dest.col) return curr;
  if (board[curr.col + W*curr.row].piece != NULL) return curr;
  Position delta = {0};
  delta.row = sgn(dest.row - curr.row);
  delta.col = sgn(dest.col - curr.col);
  Position new = { 0 };
  new.row = curr.row - delta.row;
  new.col = curr.col - delta.col;
  return checkWay(board, new, dest);
}

bool
checkMovement ( Cell *board, Piece *piece, AttackDirection dir, Position dest )
{
  int drow = dest.row - piece->pos.row;
  int dcol = dest.col - piece->pos.col;
  bool with_dir = ( (dcol > 0 && dir == UP) || (dcol < 0 && dir == DOWN) );
  switch (piece->type) {
    case ROOK:
      if ( drow == 0 || dcol == 0 )
        return true;
      else return false;
    case BISHOP:
      if ( abs(drow) == abs(dcol) ) return true;
      else return false;
    case KNIGHT:
      if ( (abs(drow) == 2 && abs(dcol) == 1) || (abs(drow) == 1 && abs(dcol) == 2) ) return true;
      else return false;
    case QUEEN:
      return true;
    case KING:
      if ( abs(drow) == 1 && abs(dcol) == 1 ) return true;
      else return false;
    case PAWN:
      if ( abs(dcol) == 1 && abs(drow) == 1 && board[dest.col + W*dest.row].piece != NULL ) return with_dir;
      if ( piece->n_movements == 1 ) {
        if ( abs(dcol) <= 2 && abs(dcol) > 0  && abs(drow) == 0 ) {
          return with_dir;
        }
        else return false;
      } else {
        if ( abs(dcol) == 1 && abs(drow) == 0 ) {
          return with_dir;
        }
        else return false;
      }
  }
}

void
movePiece ( Cell *board, Piece *piece, AttackDirection dir, Position dest )
{
  if (checkMovement(board, piece, dir, dest)) {
    // TODO: Change Check Movement to accomodate for the pieces in the middle
  }
}

void
putPieces ( Cell *board, Piece *player1, Piece *player2 )
{
  for (int i = 0; i < W; i++) {
    ////// ROW 0
    board[i].piece = &player2[i];
    player2[i].pos.col = i;
    player2[i].pos.row = 0;
    ///// ROW 1
    board[W+i].piece = &player2[W+i];
    player2[W+i].pos.col = i;
    player2[W+i].pos.row = 1;
    ///// ROW 6
    board[6*W+i].piece = &player1[W+i];
    player1[6*W+i].pos.col = i;
    player1[6*W+i].pos.row = 6;
    ///// ROW 7
    board[7*W+i].piece = &player1[i];
    player1[7*W+i].pos.col = i;
    player1[7*W+i].pos.row = 7;
  }
}

void
printBoard( Cell *board )
{
  char space[15] = "      ";
  char format[256];
  printf("%s A  B  C  D  E  F  G  H \n", space);
  for (int i = 0; i < W; i++) {
    printf("%s", space);
    for (int j = 0; j < H; j++) {
      Cell cell = board[i*W+j];
      if (cell.piece == NULL) {
        switch (cell.color) {
          case WHITE: printf("%s   ", BG_WHITE); break;
          case BLACK: printf("%s   ", BG_CYAN); break;
        }
      } else {
        switch (cell.color) {
          case WHITE: printf("%s %s ", BG_WHITE, formatPiece(cell.piece)); break;
          case BLACK: printf("%s %s ", BG_CYAN, formatPiece(cell.piece)); break;
        }
      }
    }
    printf("%s %d\n", RESET, 8 - i);
  }
}

void
printPlayer ( Piece *player )
{
  printf("%s ", BG_GRAY);
  for (int i = 0; i < 2*W-1; i++) {
    printf("%s, ", formatPiece(&player[i]));
  }
  printf("%s  %s\n", formatPiece(&player[2*W-1]), RESET);
}

void
newGame ( ChessGame *game )
{
  initBoard(game->board);
  initPlayer(WHITE, game->white_player);
  initPlayer(BLACK, game->black_player);
  putPieces(game->board, game->white_player, game->black_player);
}

int
main ( int argc, char **argv )
{
  ChessGame game = {0};
  newGame(&game);
  printf("      White Player: ");
  printPlayer(game.white_player);
  printf("      Black Player: ");
  printPlayer(game.black_player);
  printf("\n");
  printBoard(game.board);
  return 0;
}
