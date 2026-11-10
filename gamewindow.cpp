#include "gamewindow.h"

#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QFrame>

namespace {
constexpr int kBoardSize = 15;
constexpr int kBlack = 1; // 玩家
constexpr int kWhite = 2; // 电脑
} // namespace

GameWindow::GameWindow(const QString &username, Difficulty difficulty, QWidget *parent)
    : QWidget(parent)
    , m_username(username)
    , m_difficulty(difficulty)
    , m_ai(difficulty)
    , m_grid(kBoardSize, QVector<int>(kBoardSize, 0))
{
    setWindowTitle(QStringLiteral("五子棋 · 人机对战"));
    setAttribute(Qt::WA_DeleteOnClose, true);
    setStyleSheet(QStringLiteral(
        "QWidget#gameRoot { background-color: #f7f3ec; }"
        "QLabel#sideTitle { font-size: 20px; font-weight: bold; color: #5b3a1e; }"
        "QLabel#infoLabel { font-size: 13px; color: #6b5233; }"
        "QLabel#statusLabel { font-size: 16px; font-weight: bold; }"
        "QPushButton#sideButton {"
        "  font-size: 14px; padding: 9px 0; border-radius: 8px;"
        "  border: 1px solid #b98a4e; background-color: #e8d5b0; color: #4a3018; }"
        "QPushButton#sideButton:hover { background-color: #dcc493; }"
        "QPushButton#primaryButton {"
        "  font-size: 15px; font-weight: bold; padding: 10px 0; border-radius: 8px;"
        "  border: none; background-color: #b34700; color: #ffffff; }"
        "QPushButton#primaryButton:hover { background-color: #8f3800; }"));
    setObjectName(QStringLiteral("gameRoot"));

    auto *rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(16, 16, 16, 16);
    rootLayout->setSpacing(16);

    // 左侧棋盘
    m_board = new BoardWidget(this);
    rootLayout->addWidget(m_board, 0, Qt::AlignTop);
    connect(m_board, &BoardWidget::moveRequested, this, &GameWindow::onMoveRequested);

    // 右侧信息面板
    auto *sidePanel = new QWidget(this);
    sidePanel->setFixedWidth(240);
    auto *sideLayout = new QVBoxLayout(sidePanel);
    sideLayout->setContentsMargins(12, 12, 12, 12);
    sideLayout->setSpacing(12);

    auto *sideTitle = new QLabel(QStringLiteral("人机对战"), sidePanel);
    sideTitle->setObjectName(QStringLiteral("sideTitle"));
    sideLayout->addWidget(sideTitle);

    auto *playerLabel = new QLabel(QStringLiteral("玩家（黑棋）：%1").arg(m_username), sidePanel);
    playerLabel->setObjectName(QStringLiteral("infoLabel"));
    sideLayout->addWidget(playerLabel);

    m_difficultyLabel = new QLabel(sidePanel);
    m_difficultyLabel->setObjectName(QStringLiteral("infoLabel"));
    m_difficultyLabel->setText(QStringLiteral("难度：%1")
                                   .arg(m_difficulty == Difficulty::Easy ? QStringLiteral("简单")
                                                                        : QStringLiteral("正常")));
    sideLayout->addWidget(m_difficultyLabel);

    m_moveCountLabel = new QLabel(sidePanel);
    m_moveCountLabel->setObjectName(QStringLiteral("infoLabel"));
    sideLayout->addWidget(m_moveCountLabel);

    auto *line = new QFrame(sidePanel);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    sideLayout->addWidget(line);

    auto *ruleLabel = new QLabel(QStringLiteral("规则：黑棋先行，15×15 棋盘，\n五子连珠（横、竖、斜）获胜。\n无禁手，不可悔棋。"), sidePanel);
    ruleLabel->setObjectName(QStringLiteral("infoLabel"));
    ruleLabel->setWordWrap(true);
    sideLayout->addWidget(ruleLabel);

    m_statusLabel = new QLabel(sidePanel);
    m_statusLabel->setObjectName(QStringLiteral("statusLabel"));
    m_statusLabel->setWordWrap(true);
    sideLayout->addWidget(m_statusLabel);

    sideLayout->addStretch(1);

    auto *restartButton = new QPushButton(QStringLiteral("重新开始"), sidePanel);
    restartButton->setObjectName(QStringLiteral("sideButton"));
    sideLayout->addWidget(restartButton);
    connect(restartButton, &QPushButton::clicked, this, &GameWindow::resetGame);

    auto *backButton = new QPushButton(QStringLiteral("返回主菜单"), sidePanel);
    backButton->setObjectName(QStringLiteral("sideButton"));
    sideLayout->addWidget(backButton);
    connect(backButton, &QPushButton::clicked, this, &GameWindow::backToMenu);

    rootLayout->addWidget(sidePanel);

    resetGame();
}

void GameWindow::updateStatus(const QString &text, const QString &color)
{
    m_statusLabel->setText(text);
    m_statusLabel->setStyleSheet(QStringLiteral("QLabel#statusLabel { color: %1; }").arg(color));
}

void GameWindow::updateMoveCount()
{
    m_moveCountLabel->setText(QStringLiteral("已落子：%1 手").arg(m_moveCount));
}

bool GameWindow::isBoardFull() const
{
    for (const auto &row : m_grid)
        for (int v : row)
            if (v == 0)
                return false;
    return true;
}

bool GameWindow::checkWin(int row, int col, int color) const
{
    static const int dirs[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };
    for (const auto &d : dirs) {
        int count = 1;
        for (int sign : {1, -1}) {
            int r = row + sign * d[0], c = col + sign * d[1];
            while (r >= 0 && r < kBoardSize && c >= 0 && c < kBoardSize && m_grid[r][c] == color) {
                ++count;
                r += sign * d[0];
                c += sign * d[1];
            }
        }
        if (count >= 5)
            return true;
    }
    return false;
}

void GameWindow::finishIfGameOver(int lastRow, int lastCol, int color)
{
    if (checkWin(lastRow, lastCol, color)) {
        m_gameOver = true;
        m_board->setInteractionEnabled(false);
        const bool playerWon = (color == kBlack);
        updateStatus(playerWon ? QStringLiteral("你赢了！五子连珠！")
                               : QStringLiteral("电脑获胜，再来一局？"),
                      playerWon ? QStringLiteral("#1a7f37") : QStringLiteral("#b3261e"));
        QMessageBox box(this);
        box.setWindowTitle(QStringLiteral("对局结束"));
        box.setIcon(QMessageBox::Information);
        box.setText(playerWon ? QStringLiteral("恭喜你，五子连珠获胜！")
                              : QStringLiteral("电脑赢得了本局，别灰心！"));
        QPushButton *again = box.addButton(QStringLiteral("再来一局"), QMessageBox::AcceptRole);
        box.addButton(QStringLiteral("返回主菜单"), QMessageBox::RejectRole);
        box.exec();
        if (box.clickedButton() == again)
            resetGame();
        else
            emit backToMenu();
        return;
    }
    if (isBoardFull()) {
        m_gameOver = true;
        m_board->setInteractionEnabled(false);
        updateStatus(QStringLiteral("棋盘已满，本局和棋。"), QStringLiteral("#666666"));
        QMessageBox::information(this, QStringLiteral("对局结束"),
                                 QStringLiteral("棋盘已满，本局和棋！"));
        return;
    }
}

void GameWindow::onMoveRequested(int row, int col)
{
    if (m_gameOver || !m_blackTurn)
        return;
    if (m_grid[row][col] != 0)
        return;

    m_grid[row][col] = kBlack;
    ++m_moveCount;
    m_board->setGrid(m_grid);
    m_board->setLastMove(QPoint(row, col));
    updateMoveCount();

    if (checkWin(row, col, kBlack) || isBoardFull()) {
        finishIfGameOver(row, col, kBlack);
        return;
    }

    m_blackTurn = false;
    m_board->setInteractionEnabled(false);
    updateStatus(QStringLiteral("电脑思考中…"), QStringLiteral("#b34700"));
    QTimer::singleShot(450, this, &GameWindow::onAiTurn);
}

void GameWindow::onAiTurn()
{
    if (m_gameOver)
        return;

    const QPoint move = m_ai.chooseMove(m_grid);
    if (move.x() < 0 || move.y() < 0) {
        m_gameOver = true;
        updateStatus(QStringLiteral("棋盘已满，本局和棋。"), QStringLiteral("#666666"));
        return;
    }

    const int col = move.x(), row = move.y();
    m_grid[row][col] = kWhite;
    ++m_moveCount;
    m_board->setGrid(m_grid);
    m_board->setLastMove(QPoint(row, col));
    updateMoveCount();

    if (checkWin(row, col, kWhite) || isBoardFull()) {
        finishIfGameOver(row, col, kWhite);
        return;
    }

    m_blackTurn = true;
    m_board->setInteractionEnabled(true);
    updateStatus(QStringLiteral("轮到你落子（黑棋）"), QStringLiteral("#1a1a1a"));
}

void GameWindow::resetGame()
{
    for (auto &row : m_grid)
        row.fill(0);
    m_blackTurn = true;
    m_gameOver = false;
    m_moveCount = 0;

    m_board->setGrid(m_grid);
    m_board->setLastMove(QPoint(-1, -1));
    m_board->setInteractionEnabled(true);
    updateMoveCount();
    updateStatus(QStringLiteral("轮到你落子（黑棋）"), QStringLiteral("#1a1a1a"));
}
