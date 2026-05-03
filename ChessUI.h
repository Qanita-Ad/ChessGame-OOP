#ifndef CHESSUI_H
#define CHESSUI_H

#include <SFML/Graphics.hpp>
#include <string>
#include <utility>
#include "Game.h"

// SFML Chess Frontend
// Requires SFML 2.x linked to the project.
// Chess unicode glyphs render best with a font that supports the chess block
// (e.g. Segoe UI Symbol  –  C:/Windows/Fonts/seguisym.ttf).
// Falls back to Arial / Calibri automatically.

class ChessUI
{
public:
    ChessUI();
    void run();

private:
    // ── Layout constants ──────────────────────────────────────────────────
    static const int CELL_SIZE = 80;    // pixels per board square
    static const int BOARD_X = 50;    // left edge of board (room for rank labels)
    static const int BOARD_Y = 70;    // top  edge of board (room for title)
    static const int WIN_W = 740;   // window width
    static const int WIN_H = 820;   // window height

    // ── SFML / game objects ───────────────────────────────────────────────
    sf::RenderWindow window;
    sf::Font         font;
    Game             game;

    // ── Selection & move-hint state ───────────────────────────────────────
    int                    selRow;
    int                    selCol;
    bool                   hasSel;
    bool                   showMoves;
    bool                   gameOver;
    std::pair<int, int>     validMoves[64];
    int                    validMoveCount;
    std::string            statusMsg;

    // ── Private helpers ───────────────────────────────────────────────────
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
    sf::Color             squareColor(int row, int col) const;
};

#endif // CHESSUI_H