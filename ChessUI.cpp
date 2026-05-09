#include "ChessUI.h"
#include "raylib.h"
#include <string>
#include <cctype>

// ─────────────────────────────────────────────────────────────────────────────
//  Helper: colour literal
// ─────────────────────────────────────────────────────────────────────────────
static inline Color rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255)
{
    return { r, g, b, a };
}

// ─────────────────────────────────────────────────────────────────────────────
//  Drawing helpers
// ─────────────────────────────────────────────────────────────────────────────
static void DrawCentredRect(float cx, float cy, float hw, float hh,
    Color fill, Color outline, float thick)
{
    DrawRectangle((int)(cx - hw), (int)(cy - hh), (int)(hw * 2), (int)(hh * 2), fill);
    DrawRectangleLinesEx({ cx - hw, cy - hh, hw * 2, hh * 2 }, thick, outline);
}

static void DrawCircleOutlined(float cx, float cy, float r,
    Color fill, Color outline, float thick)
{
    DrawCircleV({ cx, cy }, r, fill);
    if (thick > 0)
        DrawRing({ cx, cy }, r - thick, r, 0.f, 360.f, 36, outline);
}

static void DrawConvexPoly(const Vector2* pts, int n, float ox, float oy,
    Color fill, Color outline, float thick)
{
    // DrawTriangleFan: first element = fan centre, remaining = edge verts (close by repeating first)
    // This handles CW and CCW polygons correctly unlike DrawTriangle.
    float sumX = 0, sumY = 0;
    for (int i = 0; i < n; i++) { sumX += pts[i].x; sumY += pts[i].y; }

    Vector2 fan[66]; // centre + up to 64 verts + closing vert
    fan[0] = { ox + sumX / n, oy + sumY / n };
    for (int i = 0; i < n; i++) fan[i + 1] = { ox + pts[i].x, oy + pts[i].y };
    fan[n + 1] = { ox + pts[0].x, oy + pts[0].y }; // close the fan

    DrawTriangleFan(fan, n + 2, fill);

    for (int i = 0; i < n; i++)
    {
        int j = (i + 1) % n;
        DrawLineEx({ ox + pts[i].x, oy + pts[i].y },
            { ox + pts[j].x, oy + pts[j].y }, thick, outline);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Constructor / Destructor
// ─────────────────────────────────────────────────────────────────────────────
ChessUI::ChessUI()
    : selRow(-1), selCol(-1), hasSel(false),
    showMoves(true), gameOver(false), validMoveCount(0),
    fontOwned(false),
    statusMsg("White's turn to move")
{
    InitWindow(WIN_W, WIN_H, "Chess Game");
    SetTargetFPS(60);

    // Codepoint list: printable ASCII (32-127) + chess glyphs U+2654..U+265F
    int codepoints[110];
    int cpCount = 0;
    for (int i = 32; i <= 127; i++) codepoints[cpCount++] = i;
    for (int i = 0x2654; i <= 0x265F; i++) codepoints[cpCount++] = i;

    // Load at 64 px — exact match to CELL_SIZE*0.8 so no scaling artefacts
    const int FONT_SIZE = (int)((float)CELL_SIZE * 0.80f);
    font = { 0 };
    fontOwned = false;
    const char* fontCandidates[] = {
        "C:/Windows/Fonts/seguisym.ttf",
        "C:/Windows/Fonts/Seguisym.ttf",
        "C:/Windows/Fonts/arial.ttf",
        nullptr
    };
    for (int i = 0; fontCandidates[i] != nullptr; i++)
    {
        Font tmp = LoadFontEx(fontCandidates[i], FONT_SIZE, codepoints, cpCount);
        if (tmp.baseSize > 0 && tmp.texture.id > 0)
        {
            font = tmp;
            fontOwned = true;
            break;
        }
        // Partial load — release whatever was allocated before trying next
        if (tmp.baseSize > 0 || tmp.texture.id > 0)
            UnloadFont(tmp);
    }
    if (!fontOwned)
    {
        // GetFontDefault() returns a pointer to raylib's internal font — do NOT unload it
        font = GetFontDefault();
        fontOwned = false;
    }

    // Only apply filter to a texture we own
    if (fontOwned && font.texture.id > 0)
        SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
}

ChessUI::~ChessUI()
{
    if (fontOwned && font.texture.id > 0)
        UnloadFont(font);
    CloseWindow();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Main loop
// ─────────────────────────────────────────────────────────────────────────────
void ChessUI::run()
{
    while (!WindowShouldClose())
    {
        processEvents();
        render();

    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Event processing
// ─────────────────────────────────────────────────────────────────────────────
void ChessUI::processEvents()
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mp = GetMousePosition();
        handleClick((int)mp.x, (int)mp.y);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Click handler
// ─────────────────────────────────────────────────────────────────────────────
void ChessUI::handleClick(int px, int py)
{
    if (onToggleButton(px, py))
    {
        showMoves = !showMoves;
        if (!showMoves) validMoveCount = 0;
        return;
    }

    if (gameOver) return;

    if (!onBoard(px, py))
    {
        hasSel = false;
        selRow = selCol = -1;
        validMoveCount = 0;
        return;
    }

    std::pair<int, int> cell = pixelToCell(px, py);
    int row = cell.first;
    int col = cell.second;

    if (!hasSel)
    {
        pieces* p = game.getPieceAt(row, col);
        if (p && p->getColor() == game.getCurrentTurn())
        {
            selRow = row; selCol = col; hasSel = true;
            statusMsg = "Piece selected -- click destination";
            if (showMoves) validMoveCount = game.getValidMoves(row, col, validMoves);
        }
        else if (p)
        {
            statusMsg = (game.getCurrentTurn() == 'W')
                ? "It is White's turn!" : "It is Black's turn!";
        }
    }
    else
    {
        if (row == selRow && col == selCol)
        {
            hasSel = false; selRow = selCol = -1; validMoveCount = 0;
            statusMsg = (game.getCurrentTurn() == 'W')
                ? "White's turn to move" : "Black's turn to move";
            return;
        }

        pieces* target = game.getPieceAt(row, col);
        if (target && target->getColor() == game.getCurrentTurn())
        {
            selRow = row; selCol = col;
            statusMsg = "Piece selected -- click destination";
            if (showMoves) validMoveCount = game.getValidMoves(row, col, validMoves);
            return;
        }

        bool ok = game.playMove(selRow, selCol, row, col);
        if (ok)
        {
            if (game.isGameOver())
            {
                gameOver = true;
                statusMsg = (game.getCurrentTurn() == 'B')
                    ? "WHITE WINS!  Black king captured!"
                    : "BLACK WINS!  White king captured!";
            }
            else
            {
                statusMsg = (game.getCurrentTurn() == 'W')
                    ? "White's turn to move" : "Black's turn to move";
            }
        }
        else
            statusMsg = "Invalid move! Try again.";

        hasSel = false; selRow = selCol = -1; validMoveCount = 0;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Rendering
// ─────────────────────────────────────────────────────────────────────────────
void ChessUI::render()
{
    BeginDrawing();
    ClearBackground(rgba(28, 28, 28));

    drawTitle();
    drawToggleButton();
    drawBoard();
    drawCoords();
    drawHighlight();
    drawMoveHints();
    drawPieces();
    drawStatusBar();
    if (gameOver) drawGameOverOverlay();

    EndDrawing();
}

void ChessUI::drawTitle()
{
    const char* titleStr = "CHESS";
    int fontSize = 30;
    Vector2 tsize = MeasureTextEx(font, titleStr, (float)fontSize, 1.f);
    DrawTextEx(font, titleStr,
        { WIN_W / 2.f - tsize.x / 2.f, BOARD_Y / 2.f - tsize.y / 2.f },
        (float)fontSize, 1.f, rgba(215, 190, 130));

    bool wTurn = (game.getCurrentTurn() == 'W');
    const float pW = 192.f, pH = 44.f;
    const float pX = (float)BOARD_X;
    const float pY = ((float)BOARD_Y - pH) / 2.f;

    DrawRectangle((int)pX, (int)pY, (int)pW, (int)pH,
        wTurn ? rgba(240, 225, 185) : rgba(32, 22, 10));
    DrawRectangleLinesEx({ pX, pY, pW, pH }, 2.f,
        wTurn ? rgba(155, 120, 55) : rgba(175, 140, 70));

    DrawCircleOutlined(pX + 22.f, pY + pH / 2.f, 13.f,
        wTurn ? rgba(255, 255, 255) : rgba(18, 10, 2),
        wTurn ? rgba(130, 100, 40) : rgba(185, 150, 75), 2.f);

    const char* turnStr = wTurn ? "WHITE'S TURN" : "BLACK'S TURN";
    Vector2 tlsize = MeasureTextEx(font, turnStr, 13.f, 1.f);
    DrawTextEx(font, turnStr,
        { pX + 42.f, pY + pH / 2.f - tlsize.y / 2.f },
        13.f, 1.f, wTurn ? rgba(75, 50, 10) : rgba(215, 185, 105));
}

void ChessUI::drawBoard()
{
    const float borderPad = 5.f;
    Rectangle border = {
        (float)(BOARD_X - borderPad), (float)(BOARD_Y - borderPad),
        (float)(CELL_SIZE * 8 + borderPad * 2), (float)(CELL_SIZE * 8 + borderPad * 2)
    };
    DrawRectangleRec(border, rgba(90, 60, 30));
    DrawRectangleLinesEx(border, 3.f, rgba(160, 120, 70));

    for (int row = 0; row < 8; row++)
        for (int col = 0; col < 8; col++)
            DrawRectangle(BOARD_X + col * CELL_SIZE, BOARD_Y + row * CELL_SIZE,
                CELL_SIZE, CELL_SIZE, squareColor(row, col));
}

void ChessUI::drawCoords()
{
    Color coordColor = rgba(170, 145, 100);
    float fontSize = 14.f;

    for (int row = 0; row < 8; row++)
    {
        std::string s = std::to_string(8 - row);
        Vector2 sz = MeasureTextEx(font, s.c_str(), fontSize, 1.f);
        float cy = BOARD_Y + row * CELL_SIZE + CELL_SIZE / 2.f;
        DrawTextEx(font, s.c_str(), { BOARD_X - 20.f - sz.x / 2.f, cy - sz.y / 2.f },
            fontSize, 1.f, coordColor);
    }

    for (int col = 0; col < 8; col++)
    {
        char ch[2] = { (char)('a' + col), '\0' };
        Vector2 sz = MeasureTextEx(font, ch, fontSize, 1.f);
        float cx = BOARD_X + col * CELL_SIZE + CELL_SIZE / 2.f;
        DrawTextEx(font, ch, { cx - sz.x / 2.f, BOARD_Y + 8 * CELL_SIZE + 18.f - sz.y / 2.f },
            fontSize, 1.f, coordColor);
    }
}

void ChessUI::drawHighlight()
{
    if (!hasSel) return;
    DrawRectangle(BOARD_X + selCol * CELL_SIZE, BOARD_Y + selRow * CELL_SIZE,
        CELL_SIZE, CELL_SIZE, rgba(80, 200, 80, 170));
}

void ChessUI::drawPieces()
{
    for (int row = 0; row < 8; row++)
        for (int col = 0; col < 8; col++)
        {
            pieces* p = game.getPieceAt(row, col);
            if (!p) continue;
            float cx = (float)(BOARD_X + col * CELL_SIZE + CELL_SIZE / 2);
            float cy = (float)(BOARD_Y + row * CELL_SIZE + CELL_SIZE / 2);
            drawPieceShape(p->getSymbol(), cx, cy);
        }
}

void ChessUI::drawPieceShape(char symbol, float cx, float cy)
{
    bool isWhite = (symbol >= 'A' && symbol <= 'Z');
    char upper = (char)toupper((unsigned char)symbol);

    // Map piece letter → Unicode chess codepoint
    // White: ♔♕♖♗♘♙  (U+2654-U+2659)
    // Black: ♚♛♜♝♞♟  (U+265A-U+265F)
    int cp = 0;
    switch (upper)
    {
    case 'K': cp = isWhite ? 0x2654 : 0x265A; break;
    case 'Q': cp = isWhite ? 0x2655 : 0x265B; break;
    case 'R': cp = isWhite ? 0x2656 : 0x265C; break;
    case 'B': cp = isWhite ? 0x2657 : 0x265D; break;
    case 'N': cp = isWhite ? 0x2658 : 0x265E; break;
    case 'P': cp = isWhite ? 0x2659 : 0x265F; break;
    default: return;
    }

    // DrawTextCodepoint takes the codepoint integer directly — no UTF-8 encoding needed.
    // Font was loaded at CELL_SIZE*0.8 px so no scaling is applied.
    float fontSize = (float)CELL_SIZE * 0.80f;

    // Chess glyphs in Segoe UI Symbol are roughly square; center them in the cell.
    float halfW = fontSize * 0.42f;
    float halfH = fontSize * 0.46f;
    Vector2 pos = { cx - halfW, cy - halfH };

    Color mainCol = isWhite ? rgba(248, 232, 185) : rgba(22, 12, 4);
    Color shadowCol = rgba(0, 0, 0, 150);
    Color rimCol = isWhite ? rgba(110, 72, 10, 210) : rgba(210, 168, 72, 210);

    // Drop shadow
    DrawTextCodepoint(font, cp, { pos.x + 2.f, pos.y + 3.f }, fontSize, shadowCol);
    // Rim (4-directional outline)
    DrawTextCodepoint(font, cp, { pos.x - 1.f, pos.y - 1.f }, fontSize, rimCol);
    DrawTextCodepoint(font, cp, { pos.x + 1.f, pos.y - 1.f }, fontSize, rimCol);
    DrawTextCodepoint(font, cp, { pos.x - 1.f, pos.y + 1.f }, fontSize, rimCol);
    DrawTextCodepoint(font, cp, { pos.x + 1.f, pos.y + 1.f }, fontSize, rimCol);
    // Main glyph
    DrawTextCodepoint(font, cp, pos, fontSize, mainCol);
}

void ChessUI::drawStatusBar()
{
    // barY: board bottom (BOARD_Y+640) + 26px gap below coord labels
    const float barY = BOARD_Y + 8 * CELL_SIZE + 26.f;
    const float barW = (float)(WIN_W - 2 * BOARD_X);
    const float barH = 32.f;
    const float midY = barY + barH / 2.f;

    DrawRectangle((int)BOARD_X, (int)barY, (int)barW, (int)barH, rgba(45, 45, 45));
    DrawRectangleLinesEx({ (float)BOARD_X, barY, barW, barH }, 1.5f, rgba(100, 85, 60));

    const float dotR = 11.f;
    DrawCircleOutlined(BOARD_X + 20.f, midY, dotR,
        game.getCurrentTurn() == 'W' ? rgba(245, 235, 210) : rgba(50, 35, 20),
        rgba(150, 125, 90), 2.f);

    Vector2 msz = MeasureTextEx(font, statusMsg.c_str(), 15.f, 1.f);
    DrawTextEx(font, statusMsg.c_str(),
        { BOARD_X + 38.f, midY - msz.y / 2.f }, 15.f, 1.f, rgba(210, 190, 150));
}

bool ChessUI::onBoard(int px, int py) const
{
    return px >= BOARD_X && px < BOARD_X + 8 * CELL_SIZE &&
        py >= BOARD_Y && py < BOARD_Y + 8 * CELL_SIZE;
}

std::pair<int, int> ChessUI::pixelToCell(int px, int py) const
{
    return { (py - BOARD_Y) / CELL_SIZE, (px - BOARD_X) / CELL_SIZE };
}

Color ChessUI::squareColor(int row, int col) const
{
    return ((row + col) % 2 == 0) ? rgba(240, 217, 181) : rgba(181, 136, 99);
}

void ChessUI::drawMoveHints()
{
    if (!hasSel || !showMoves) return;
    float cs = (float)CELL_SIZE;
    for (int i = 0; i < validMoveCount; i++)
    {
        const std::pair<int, int>& mv = validMoves[i];
        float sx = (float)(BOARD_X + mv.second * CELL_SIZE);
        float sy = (float)(BOARD_Y + mv.first * CELL_SIZE);
        if (game.getPieceAt(mv.first, mv.second) == nullptr)
            DrawCircleV({ sx + cs * 0.5f, sy + cs * 0.5f }, cs * 0.18f, rgba(15, 15, 15, 130));
        else
            DrawRectangleLinesEx({ sx, sy, cs, cs }, 5.f, rgba(210, 50, 50, 200));
    }
}

void ChessUI::drawToggleButton()
{
    const float BTN_X = (float)(WIN_W - 178);
    const float BTN_Y = 13.f, BTN_W = 165.f, BTN_H = 44.f;

    DrawRectangle((int)BTN_X, (int)BTN_Y, (int)BTN_W, (int)BTN_H,
        showMoves ? rgba(28, 78, 38) : rgba(55, 38, 18));
    DrawRectangleLinesEx({ BTN_X,BTN_Y,BTN_W,BTN_H }, 2.f,
        showMoves ? rgba(75, 168, 88) : rgba(135, 105, 50));

    DrawCircleV({ BTN_X + 22.f, BTN_Y + BTN_H * 0.5f }, 11.f,
        showMoves ? rgba(90, 210, 108) : rgba(140, 105, 45));

    if (showMoves)
        DrawCircleV({ BTN_X + 22.f, BTN_Y + BTN_H * 0.5f }, 4.5f, rgba(12, 42, 16));
    else
        DrawLineEx({ BTN_X + 13.f, BTN_Y + BTN_H * 0.5f + 9.f },
            { BTN_X + 31.f, BTN_Y + BTN_H * 0.5f - 9.f }, 3.f, rgba(55, 38, 18));

    const char* lbl = showMoves ? "HIDE MOVES" : "SHOW MOVES";
    Vector2 lsz = MeasureTextEx(font, lbl, 14.f, 1.f);
    DrawTextEx(font, lbl,
        { BTN_X + 40.f, BTN_Y + BTN_H * 0.5f - lsz.y / 2.f }, 14.f, 1.f,
        showMoves ? rgba(155, 225, 165) : rgba(195, 160, 85));
}

bool ChessUI::onToggleButton(int px, int py) const
{
    return px >= WIN_W - 178 && px < WIN_W - 13 && py >= 13 && py < 57;
}

void ChessUI::drawGameOverOverlay()
{
    DrawRectangle(0, 0, WIN_W, WIN_H, rgba(0, 0, 0, 175));

    const float cW = 460.f, cH = 220.f;
    const float cX = WIN_W / 2.f - cW / 2.f, cY = WIN_H / 2.f - cH / 2.f;
    DrawRectangle((int)cX, (int)cY, (int)cW, (int)cH, rgba(28, 20, 10));
    DrawRectangleLinesEx({ cX,cY,cW,cH }, 3.f, rgba(190, 155, 65));

    const char* hdr = "GAME  OVER";
    Vector2 hsz = MeasureTextEx(font, hdr, 36.f, 1.f);
    DrawTextEx(font, hdr,
        { WIN_W / 2.f - hsz.x / 2.f, WIN_H / 2.f - 60.f - hsz.y / 2.f },
        36.f, 1.f, rgba(215, 175, 60));

    Vector2 msz = MeasureTextEx(font, statusMsg.c_str(), 17.f, 1.f);
    DrawTextEx(font, statusMsg.c_str(),
        { WIN_W / 2.f - msz.x / 2.f, WIN_H / 2.f + 10.f - msz.y / 2.f },
        17.f, 1.f, rgba(210, 185, 130));

    const char* esc = "Press  Esc  to close";
    Vector2 esz = MeasureTextEx(font, esc, 13.f, 1.f);
    DrawTextEx(font, esc,
        { WIN_W / 2.f - esz.x / 2.f, WIN_H / 2.f + 72.f - esz.y / 2.f },
        13.f, 1.f, rgba(130, 105, 50));
}