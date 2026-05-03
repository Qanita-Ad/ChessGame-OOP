#include "ChessUI.h"
#include <string>
#include <cctype>
#include <fstream>
#include <sstream>

// ─────────────────────────────────────────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────────────────────────────────────────
ChessUI::ChessUI()
    : window(sf::VideoMode(WIN_W, WIN_H), "Chess Game",
        sf::Style::Titlebar | sf::Style::Close),
    selRow(-1), selCol(-1), hasSel(false),
    showMoves(true), gameOver(false), validMoveCount(0),
    statusMsg("White's turn to move")
{
    window.setFramerateLimit(60);

    // Redirect SFML's error stream so it never prints
    // "Failed to load font" regardless of which paths exist.
    std::ostringstream devNull;
    std::streambuf* savedErrBuf = sf::err().rdbuf(devNull.rdbuf());

    const char* fontPaths[] = {
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/Arial.ttf",
        "C:/Windows/Fonts/calibri.ttf",
        "C:/Windows/Fonts/tahoma.ttf",
        "C:/Windows/Fonts/verdana.ttf",
        "C:/Windows/Fonts/segoeui.ttf"
    };
    for (const char* fp : fontPaths)
    {
        std::ifstream test(fp);
        if (test.good()) { test.close(); if (font.loadFromFile(fp)) break; }
    }

    // Restore SFML error output
    sf::err().rdbuf(savedErrBuf);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Main loop
// ─────────────────────────────────────────────────────────────────────────────
void ChessUI::run()
{
    while (window.isOpen())
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
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::Escape)
            window.close();

        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left)
            handleClick(event.mouseButton.x, event.mouseButton.y);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Click handler  –  implements select-then-move interaction
// ─────────────────────────────────────────────────────────────────────────────
void ChessUI::handleClick(int px, int py)
{
    // Toggle button has priority over everything
    if (onToggleButton(px, py))
    {
        showMoves = !showMoves;
        if (!showMoves) validMoveCount = 0;
        return;
    }

    // No moves allowed once the game is over
    if (gameOver) return;

    if (!onBoard(px, py))
    {
        // Click outside board: cancel selection
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
        // ── First click: try to select a piece ──────────────────────────
        pieces* p = game.getPieceAt(row, col);
        if (p && p->getColor() == game.getCurrentTurn())
        {
            selRow = row;
            selCol = col;
            hasSel = true;
            statusMsg = "Piece selected — click destination";
            if (showMoves) validMoveCount = game.getValidMoves(row, col, validMoves);
        }
        else if (p)
        {
            statusMsg = (game.getCurrentTurn() == 'W')
                ? "It is White's turn!"
                : "It is Black's turn!";
        }
    }
    else
    {
        // ── Second click ─────────────────────────────────────────────────

        // Same cell: deselect
        if (row == selRow && col == selCol)
        {
            hasSel = false;
            selRow = selCol = -1;
            validMoveCount = 0;
            statusMsg = (game.getCurrentTurn() == 'W')
                ? "White's turn to move"
                : "Black's turn to move";
            return;
        }

        // Another friendly piece: re-select
        pieces* target = game.getPieceAt(row, col);
        if (target && target->getColor() == game.getCurrentTurn())
        {
            selRow = row;
            selCol = col;
            statusMsg = "Piece selected — click destination";
            if (showMoves) validMoveCount = game.getValidMoves(row, col, validMoves);
            return;
        }

        // Attempt the move (capture or normal)
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
                    ? "White's turn to move"
                    : "Black's turn to move";
            }
        }
        else
            statusMsg = "Invalid move! Try again.";

        hasSel = false;
        selRow = selCol = -1;
        validMoveCount = 0;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Rendering
// ─────────────────────────────────────────────────────────────────────────────
void ChessUI::render()
{
    window.clear(sf::Color(28, 28, 28));

    drawTitle();
    drawToggleButton();
    drawBoard();
    drawCoords();
    drawHighlight();
    drawMoveHints();
    drawPieces();
    drawStatusBar();
    if (gameOver) drawGameOverOverlay();
    window.display();
}

void ChessUI::drawTitle()
{
    // ── CHESS title ────────────────────────────────────────────────────────
    sf::Text title;
    title.setFont(font);
    title.setString("CHESS");
    title.setCharacterSize(30);
    title.setStyle(sf::Text::Bold);
    title.setFillColor(sf::Color(215, 190, 130));
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.left + tb.width / 2.f,
        tb.top + tb.height / 2.f);
    title.setPosition(WIN_W / 2.f, BOARD_Y / 2.f);
    window.draw(title);

    // ── Turn indicator panel (left side of header) ────────────────────────
    bool  wTurn = (game.getCurrentTurn() == 'W');
    const float pW = 192.f;
    const float pH = 44.f;
    const float pX = static_cast<float>(BOARD_X);
    const float pY = (static_cast<float>(BOARD_Y) - pH) / 2.f;

    sf::RectangleShape turnPanel(sf::Vector2f(pW, pH));
    turnPanel.setPosition(pX, pY);
    turnPanel.setFillColor(wTurn ? sf::Color(240, 225, 185) : sf::Color(32, 22, 10));
    turnPanel.setOutlineColor(wTurn ? sf::Color(155, 120, 55) : sf::Color(175, 140, 70));
    turnPanel.setOutlineThickness(2.f);
    window.draw(turnPanel);

    const float cR = 13.f;
    sf::CircleShape circ(cR, 22);
    circ.setOrigin(cR, cR);
    circ.setPosition(pX + 22.f, pY + pH / 2.f);
    circ.setFillColor(wTurn ? sf::Color(255, 255, 255) : sf::Color(18, 10, 2));
    circ.setOutlineColor(wTurn ? sf::Color(130, 100, 40) : sf::Color(185, 150, 75));
    circ.setOutlineThickness(2.f);
    window.draw(circ);

    sf::Text turnTxt;
    turnTxt.setFont(font);
    turnTxt.setString(wTurn ? "WHITE'S TURN" : "BLACK'S TURN");
    turnTxt.setCharacterSize(13);
    turnTxt.setStyle(sf::Text::Bold);
    turnTxt.setFillColor(wTurn ? sf::Color(75, 50, 10) : sf::Color(215, 185, 105));
    sf::FloatRect tlb = turnTxt.getLocalBounds();
    turnTxt.setOrigin(0.f, tlb.top + tlb.height / 2.f);
    turnTxt.setPosition(pX + 42.f, pY + pH / 2.f);
    window.draw(turnTxt);
}

void ChessUI::drawBoard()
{
    // Wooden border around the 8x8 grid
    const float borderPad = 5.f;
    sf::RectangleShape border(sf::Vector2f(CELL_SIZE * 8 + borderPad * 2,
        CELL_SIZE * 8 + borderPad * 2));
    border.setPosition(BOARD_X - borderPad, BOARD_Y - borderPad);
    border.setFillColor(sf::Color(90, 60, 30));
    border.setOutlineColor(sf::Color(160, 120, 70));
    border.setOutlineThickness(3.f);
    window.draw(border);

    // 64 squares
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            sf::RectangleShape sq(sf::Vector2f(static_cast<float>(CELL_SIZE), static_cast<float>(CELL_SIZE)));
            sq.setPosition(static_cast<float>(BOARD_X + col * CELL_SIZE),
                static_cast<float>(BOARD_Y + row * CELL_SIZE));
            sq.setFillColor(squareColor(row, col));
            window.draw(sq);
        }
    }
}

void ChessUI::drawCoords()
{
    const sf::Color coordColor(170, 145, 100);

    // Rank numbers  8 → 1  on the left
    for (int row = 0; row < 8; row++)
    {
        sf::Text t;
        t.setFont(font);
        t.setString(std::to_string(8 - row));
        t.setCharacterSize(14);
        t.setFillColor(coordColor);

        float cy = BOARD_Y + row * CELL_SIZE + CELL_SIZE / 2.f;
        sf::FloatRect b = t.getLocalBounds();
        t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
        t.setPosition(BOARD_X - 20.f, cy);
        window.draw(t);
    }

    // File letters  a → h  below the board
    for (int col = 0; col < 8; col++)
    {
        sf::Text t;
        t.setFont(font);
        t.setString(std::string(1, static_cast<char>('a' + col)));
        t.setCharacterSize(14);
        t.setFillColor(coordColor);

        float cx = BOARD_X + col * CELL_SIZE + CELL_SIZE / 2.f;
        sf::FloatRect b = t.getLocalBounds();
        t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
        t.setPosition(cx, BOARD_Y + 8 * CELL_SIZE + 18.f);
        window.draw(t);
    }
}

void ChessUI::drawHighlight()
{
    if (!hasSel) return;

    sf::RectangleShape hl(sf::Vector2f(static_cast<float>(CELL_SIZE), static_cast<float>(CELL_SIZE)));
    hl.setPosition(static_cast<float>(BOARD_X + selCol * CELL_SIZE),
        static_cast<float>(BOARD_Y + selRow * CELL_SIZE));
    hl.setFillColor(sf::Color(80, 200, 80, 170));
    window.draw(hl);
}

void ChessUI::drawPieces()
{
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            pieces* p = game.getPieceAt(row, col);
            if (!p) continue;

            float cx = static_cast<float>(BOARD_X + col * CELL_SIZE + CELL_SIZE / 2);
            float cy = static_cast<float>(BOARD_Y + row * CELL_SIZE + CELL_SIZE / 2);
            drawPieceShape(p->getSymbol(), cx, cy);
        }
    }
}

void ChessUI::drawPieceShape(char symbol, float cx, float cy)
{
    bool  isWhite = (symbol >= 'A' && symbol <= 'Z');
    char  upper = static_cast<char>(toupper(static_cast<unsigned char>(symbol)));
    float s = static_cast<float>(CELL_SIZE) / 80.f;

    sf::Color fill = isWhite ? sf::Color(245, 230, 190) : sf::Color(45, 30, 15);
    sf::Color dk = isWhite ? sf::Color(195, 165, 115) : sf::Color(22, 14, 5);
    sf::Color outline = isWhite ? sf::Color(105, 68, 14) : sf::Color(190, 155, 90);
    sf::Color hilite = isWhite ? sf::Color(255, 250, 235, 220)
        : sf::Color(130, 95, 45, 195);

    auto applyFill = [&](sf::Shape& sh)
        {
            sh.setFillColor(fill);
            sh.setOutlineColor(outline);
            sh.setOutlineThickness(1.6f * s);
            window.draw(sh);
        };
    auto applyDark = [&](sf::Shape& sh)
        {
            sh.setFillColor(dk);
            sh.setOutlineColor(outline);
            sh.setOutlineThickness(1.6f * s);
            window.draw(sh);
        };
    auto drawHilite = [&](float ox, float oy, float r)
        {
            sf::CircleShape h(r, 14);
            h.setOrigin(r, r);
            h.setPosition(cx + ox, cy + oy);
            h.setFillColor(hilite);
            h.setOutlineThickness(0);
            window.draw(h);
        };

    // ── Shared base (all pieces) ──────────────────────────────────────────
    {
        sf::ConvexShape base(4);
        base.setPoint(0, sf::Vector2f(-19 * s, 30 * s));
        base.setPoint(1, sf::Vector2f(19 * s, 30 * s));
        base.setPoint(2, sf::Vector2f(13 * s, 20 * s));
        base.setPoint(3, sf::Vector2f(-13 * s, 20 * s));
        base.setPosition(cx, cy);
        applyDark(base);
    }

    if (upper == 'P')               // ── PAWN ─────────────────────────────
    {
        sf::RectangleShape neck(sf::Vector2f(7 * s, 15 * s));
        neck.setOrigin(3.5f * s, 15 * s);
        neck.setPosition(cx, cy + 20 * s);
        applyDark(neck);

        sf::CircleShape head(15 * s, 30);
        head.setOrigin(15 * s, 15 * s);
        head.setPosition(cx, cy + 2 * s);
        applyFill(head);

        drawHilite(-6 * s, -6 * s, 4.5f * s);
    }
    else if (upper == 'R')          // ── ROOK ─────────────────────────────
    {
        sf::RectangleShape body(sf::Vector2f(22 * s, 30 * s));
        body.setOrigin(11 * s, 30 * s);
        body.setPosition(cx, cy + 20 * s);
        applyFill(body);

        sf::RectangleShape ledge(sf::Vector2f(28 * s, 5 * s));
        ledge.setOrigin(14 * s, 5 * s);
        ledge.setPosition(cx, cy - 10 * s);
        applyDark(ledge);

        const float mpos[3] = { -9.5f * s, 0.f, 9.5f * s };
        for (int i = 0; i < 3; i++)
        {
            sf::RectangleShape m(sf::Vector2f(7 * s, 12 * s));
            m.setOrigin(3.5f * s, 12 * s);
            m.setPosition(cx + mpos[i], cy - 10 * s);
            applyFill(m);
        }

        sf::RectangleShape shine(sf::Vector2f(3.5f * s, 20 * s));
        shine.setOrigin(0, 0);
        shine.setPosition(cx - 8 * s, cy - 4 * s);
        shine.setFillColor(hilite);
        shine.setOutlineThickness(0);
        window.draw(shine);
    }
    else if (upper == 'N')          // ── KNIGHT ───────────────────────────
    {
        sf::ConvexShape horse(12);
        horse.setPoint(0, sf::Vector2f(-11 * s, 20 * s));
        horse.setPoint(1, sf::Vector2f(11 * s, 20 * s));
        horse.setPoint(2, sf::Vector2f(14 * s, 8 * s));
        horse.setPoint(3, sf::Vector2f(19 * s, 0 * s));
        horse.setPoint(4, sf::Vector2f(21 * s, -11 * s));
        horse.setPoint(5, sf::Vector2f(14 * s, -24 * s));
        horse.setPoint(6, sf::Vector2f(2 * s, -29 * s));
        horse.setPoint(7, sf::Vector2f(-8 * s, -25 * s));
        horse.setPoint(8, sf::Vector2f(-15 * s, -13 * s));
        horse.setPoint(9, sf::Vector2f(-15 * s, 2 * s));
        horse.setPoint(10, sf::Vector2f(-9 * s, 10 * s));
        horse.setPoint(11, sf::Vector2f(-9 * s, 20 * s));
        horse.setPosition(cx, cy);
        applyFill(horse);

        // ear
        sf::ConvexShape ear(3);
        ear.setPoint(0, sf::Vector2f(-4 * s, -19 * s));
        ear.setPoint(1, sf::Vector2f(3 * s, -19 * s));
        ear.setPoint(2, sf::Vector2f(-1 * s, -29 * s));
        ear.setPosition(cx - 2 * s, cy);
        applyDark(ear);

        // eye
        sf::CircleShape eye(3.2f * s, 12);
        eye.setOrigin(3.2f * s, 3.2f * s);
        eye.setPosition(cx + 11 * s, cy - 14 * s);
        eye.setFillColor(outline);
        eye.setOutlineThickness(0);
        window.draw(eye);

        drawHilite(-10 * s, -19 * s, 3.2f * s);
    }
    else if (upper == 'B')          // ── BISHOP ───────────────────────────
    {
        sf::ConvexShape body(6);
        body.setPoint(0, sf::Vector2f(-10 * s, 20 * s));
        body.setPoint(1, sf::Vector2f(10 * s, 20 * s));
        body.setPoint(2, sf::Vector2f(8 * s, 8 * s));
        body.setPoint(3, sf::Vector2f(4 * s, -2 * s));
        body.setPoint(4, sf::Vector2f(-4 * s, -2 * s));
        body.setPoint(5, sf::Vector2f(-8 * s, 8 * s));
        body.setPosition(cx, cy);
        applyFill(body);

        sf::RectangleShape band(sf::Vector2f(16 * s, 5 * s));
        band.setOrigin(8 * s, 2.5f * s);
        band.setPosition(cx, cy + 10 * s);
        applyDark(band);

        sf::CircleShape head(11 * s, 28);
        head.setOrigin(11 * s, 11 * s);
        head.setPosition(cx, cy - 13 * s);
        applyFill(head);

        sf::ConvexShape tip(3);
        tip.setPoint(0, sf::Vector2f(-3 * s, 0));
        tip.setPoint(1, sf::Vector2f(3 * s, 0));
        tip.setPoint(2, sf::Vector2f(0, -11 * s));
        tip.setPosition(cx, cy - 24 * s);
        applyDark(tip);

        drawHilite(-6 * s, -20 * s, 4 * s);
    }
    else if (upper == 'Q')          // ── QUEEN ────────────────────────────
    {
        sf::ConvexShape body(4);
        body.setPoint(0, sf::Vector2f(-13 * s, 20 * s));
        body.setPoint(1, sf::Vector2f(13 * s, 20 * s));
        body.setPoint(2, sf::Vector2f(9 * s, -6 * s));
        body.setPoint(3, sf::Vector2f(-9 * s, -6 * s));
        body.setPosition(cx, cy);
        applyFill(body);

        sf::RectangleShape band(sf::Vector2f(28 * s, 6 * s));
        band.setOrigin(14 * s, 6 * s);
        band.setPosition(cx, cy - 6 * s);
        applyDark(band);

        // 5 crown balls — centre tallest
        const float bx[5] = { -12 * s, -6 * s,   0 * s,   6 * s,  12 * s };
        const float bh[5] = { 5 * s,  8 * s,  11 * s,   8 * s,   5 * s };
        const float br[5] = { 4 * s,  5 * s, 6.5f * s,  5 * s,   4 * s };
        for (int i = 0; i < 5; i++)
        {
            sf::CircleShape ball(br[i], 20);
            ball.setOrigin(br[i], br[i]);
            ball.setPosition(cx + bx[i], cy - 6 * s - bh[i] - br[i]);
            applyFill(ball);
        }

        drawHilite(-6 * s, -2 * s, 4 * s);
    }
    else if (upper == 'K')          // ── KING ─────────────────────────────
    {
        sf::ConvexShape body(4);
        body.setPoint(0, sf::Vector2f(-13 * s, 20 * s));
        body.setPoint(1, sf::Vector2f(13 * s, 20 * s));
        body.setPoint(2, sf::Vector2f(9 * s, -6 * s));
        body.setPoint(3, sf::Vector2f(-9 * s, -6 * s));
        body.setPosition(cx, cy);
        applyFill(body);

        sf::RectangleShape band(sf::Vector2f(24 * s, 6 * s));
        band.setOrigin(12 * s, 6 * s);
        band.setPosition(cx, cy - 6 * s);
        applyDark(band);

        // Vertical bar of cross
        sf::RectangleShape cv(sf::Vector2f(6 * s, 26 * s));
        cv.setOrigin(3 * s, 26 * s);
        cv.setPosition(cx, cy - 12 * s);
        applyFill(cv);

        // Horizontal bar of cross
        sf::RectangleShape ch(sf::Vector2f(20 * s, 6 * s));
        ch.setOrigin(10 * s, 3 * s);
        ch.setPosition(cx, cy - 24 * s);
        applyFill(ch);

        drawHilite(-6 * s, -2 * s, 4 * s);
    }
}
void ChessUI::drawStatusBar()
{
    const float barY = BOARD_Y + 8 * CELL_SIZE + 40.f;
    const float barW = static_cast<float>(WIN_W - 2 * BOARD_X);

    // Background panel
    sf::RectangleShape panel(sf::Vector2f(barW, 52.f));
    panel.setPosition(static_cast<float>(BOARD_X), barY);
    panel.setFillColor(sf::Color(45, 45, 45));
    panel.setOutlineColor(sf::Color(100, 85, 60));
    panel.setOutlineThickness(1.5f);
    window.draw(panel);

    // Turn indicator dot
    const float dotR = 14.f;
    sf::CircleShape dot(dotR);
    dot.setOrigin(dotR, dotR);
    dot.setPosition(BOARD_X + 26.f, barY + 26.f);
    dot.setFillColor(game.getCurrentTurn() == 'W'
        ? sf::Color(245, 235, 210)
        : sf::Color(50, 35, 20));
    dot.setOutlineColor(sf::Color(150, 125, 90));
    dot.setOutlineThickness(2.f);
    window.draw(dot);

    // Status message
    sf::Text msg;
    msg.setFont(font);
    msg.setString(statusMsg);
    msg.setCharacterSize(18);
    msg.setFillColor(sf::Color(210, 190, 150));

    sf::FloatRect mb = msg.getLocalBounds();
    msg.setOrigin(0.f, mb.top + mb.height / 2.f);
    msg.setPosition(BOARD_X + 52.f, barY + 26.f);
    window.draw(msg);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Utility helpers
// ─────────────────────────────────────────────────────────────────────────────
bool ChessUI::onBoard(int px, int py) const
{
    return px >= BOARD_X && px < BOARD_X + 8 * CELL_SIZE &&
        py >= BOARD_Y && py < BOARD_Y + 8 * CELL_SIZE;
}

std::pair<int, int> ChessUI::pixelToCell(int px, int py) const
{
    int col = (px - BOARD_X) / CELL_SIZE;
    int row = (py - BOARD_Y) / CELL_SIZE;
    return std::make_pair(row, col);
}


sf::Color ChessUI::squareColor(int row, int col) const
{
    bool light = (row + col) % 2 == 0;
    return light ? sf::Color(240, 217, 181)   // warm cream
        : sf::Color(181, 136, 99);  // warm brown
}

// ───────────────────────────────────────────────────────────────────────────────
void ChessUI::drawMoveHints()
{
    if (!hasSel || !showMoves) return;

    float cs = static_cast<float>(CELL_SIZE);
    for (int i = 0; i < validMoveCount; i++)
    {
        const std::pair<int, int>& mv = validMoves[i];
        float sx = static_cast<float>(BOARD_X + mv.second * CELL_SIZE);
        float sy = static_cast<float>(BOARD_Y + mv.first * CELL_SIZE);

        if (game.getPieceAt(mv.first, mv.second) == NULL)
        {
            // Empty square — dark dot in centre
            float dotR = cs * 0.18f;
            sf::CircleShape dot(dotR, 24);
            dot.setOrigin(dotR, dotR);
            dot.setPosition(sx + cs * 0.5f, sy + cs * 0.5f);
            dot.setFillColor(sf::Color(15, 15, 15, 130));
            dot.setOutlineThickness(0);
            window.draw(dot);
        }
        else
        {
            // Enemy piece — red inset ring signals a capture
            sf::RectangleShape ring(sf::Vector2f(cs, cs));
            ring.setPosition(sx, sy);
            ring.setFillColor(sf::Color(0, 0, 0, 0));
            ring.setOutlineColor(sf::Color(210, 50, 50, 200));
            ring.setOutlineThickness(-5.f);
            window.draw(ring);
        }
    }
}

void ChessUI::drawToggleButton()
{
    const float BTN_X = static_cast<float>(WIN_W - 178);
    const float BTN_Y = 13.f;
    const float BTN_W = 165.f;
    const float BTN_H = 44.f;

    sf::RectangleShape btn(sf::Vector2f(BTN_W, BTN_H));
    btn.setPosition(BTN_X, BTN_Y);
    btn.setFillColor(showMoves ? sf::Color(28, 78, 38) : sf::Color(55, 38, 18));
    btn.setOutlineColor(showMoves ? sf::Color(75, 168, 88) : sf::Color(135, 105, 50));
    btn.setOutlineThickness(2.f);
    window.draw(btn);

    // Eye / hide icon circle
    const float iR = 11.f;
    sf::CircleShape icon(iR, 18);
    icon.setOrigin(iR, iR);
    icon.setPosition(BTN_X + 22.f, BTN_Y + BTN_H * 0.5f);
    icon.setFillColor(showMoves ? sf::Color(90, 210, 108) : sf::Color(140, 105, 45));
    icon.setOutlineThickness(0);
    window.draw(icon);

    if (showMoves)
    {
        sf::CircleShape pupil(4.5f, 12);
        pupil.setOrigin(4.5f, 4.5f);
        pupil.setPosition(BTN_X + 22.f, BTN_Y + BTN_H * 0.5f);
        pupil.setFillColor(sf::Color(12, 42, 16));
        pupil.setOutlineThickness(0);
        window.draw(pupil);
    }
    else
    {
        sf::RectangleShape slash(sf::Vector2f(18.f, 3.f));
        slash.setOrigin(9.f, 1.5f);
        slash.setPosition(BTN_X + 22.f, BTN_Y + BTN_H * 0.5f);
        slash.setRotation(45.f);
        slash.setFillColor(sf::Color(55, 38, 18));
        slash.setOutlineThickness(0);
        window.draw(slash);
    }

    sf::Text label;
    label.setFont(font);
    label.setString(showMoves ? "HIDE MOVES" : "SHOW MOVES");
    label.setCharacterSize(14);
    label.setStyle(sf::Text::Bold);
    label.setFillColor(showMoves ? sf::Color(155, 225, 165) : sf::Color(195, 160, 85));
    sf::FloatRect lb = label.getLocalBounds();
    label.setOrigin(0.f, lb.top + lb.height * 0.5f);
    label.setPosition(BTN_X + 40.f, BTN_Y + BTN_H * 0.5f);
    window.draw(label);
}

bool ChessUI::onToggleButton(int px, int py) const
{
    return px >= WIN_W - 178 && px < WIN_W - 13 &&
        py >= 13 && py < 57;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Game-over overlay
// ─────────────────────────────────────────────────────────────────────────────
void ChessUI::drawGameOverOverlay()
{
    // Semi-transparent dim
    sf::RectangleShape dim(sf::Vector2f(static_cast<float>(WIN_W),
        static_cast<float>(WIN_H)));
    dim.setFillColor(sf::Color(0, 0, 0, 175));
    window.draw(dim);

    // Card
    const float cW = 460.f, cH = 220.f;
    sf::RectangleShape card(sf::Vector2f(cW, cH));
    card.setOrigin(cW / 2.f, cH / 2.f);
    card.setPosition(WIN_W / 2.f, WIN_H / 2.f);
    card.setFillColor(sf::Color(28, 20, 10));
    card.setOutlineColor(sf::Color(190, 155, 65));
    card.setOutlineThickness(3.f);
    window.draw(card);

    // "GAME OVER" header
    sf::Text header;
    header.setFont(font);
    header.setString("GAME  OVER");
    header.setCharacterSize(36);
    header.setStyle(sf::Text::Bold);
    header.setFillColor(sf::Color(215, 175, 60));
    sf::FloatRect hb = header.getLocalBounds();
    header.setOrigin(hb.left + hb.width / 2.f, hb.top + hb.height / 2.f);
    header.setPosition(WIN_W / 2.f, WIN_H / 2.f - 60.f);
    window.draw(header);

    // Winner message (use statusMsg which was set on capture)
    sf::Text msg;
    msg.setFont(font);
    msg.setString(statusMsg);
    msg.setCharacterSize(17);
    msg.setFillColor(sf::Color(210, 185, 130));
    sf::FloatRect mb = msg.getLocalBounds();
    msg.setOrigin(mb.left + mb.width / 2.f, mb.top + mb.height / 2.f);
    msg.setPosition(WIN_W / 2.f, WIN_H / 2.f + 10.f);
    window.draw(msg);

    // Instruction
    sf::Text esc;
    esc.setFont(font);
    esc.setString("Press  Esc  to close");
    esc.setCharacterSize(13);
    esc.setFillColor(sf::Color(130, 105, 50));
    sf::FloatRect eb = esc.getLocalBounds();
    esc.setOrigin(eb.left + eb.width / 2.f, eb.top + eb.height / 2.f);
    esc.setPosition(WIN_W / 2.f, WIN_H / 2.f + 72.f);
    window.draw(esc);
}