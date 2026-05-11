#ifndef CHESSUI_H
#define CHESSUI_H

#include "raylib.h"
#include <string>
#include <utility>
#include "Game.h"


class ChessUI
{
public:
    ChessUI();
    ~ChessUI();
    void run();

private:
    static const int CELL_SIZE = 60;    // pixels per board square
    static const int WIN_W = 720;   // window width
    static const int WIN_H = 700;   // window height
    static const int BOARD_X = (WIN_W - CELL_SIZE * 8) / 2;;    // left edge of board
    static const int BOARD_Y = (WIN_H - CELL_SIZE * 8) / 2;

    Font             font;
    bool             fontOwned;   // false when using GetFontDefault() 
    Game             game;
    int                    selRow;
    int                    selCol;
    bool                   hasSel;
    bool                   showMoves;
    bool                   gameOver;
    std::pair<int, int>     validMoves[64];
    int                    validMoveCount;
    string            statusMsg;
    void processEvents();
    void handleClick(int px, int py);
    void render();
    void drawTitle();
    void drawBoard();
    void drawCoords();
    void drawHighlight();
    void drawMoveHints();
    void drawPieces();
    void drawStatusBar();
    void drawToggleButton();
    void drawGameOverOverlay();

    bool                  onBoard(int px, int py) const;
    bool                  onToggleButton(int px, int py) const;
    std::pair<int, int>    pixelToCell(int px, int py) const;
    void                  drawPieceShape(char symbol, float cx, float cy);
    Color                 squareColor(int row, int col) const;
};
#endif 