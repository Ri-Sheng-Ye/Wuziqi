#include "gamewindow.h"

#include "uitheme.h"

#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>

namespace {
constexpr int kBoardSize = 15;
constexpr int kBlack = 1; // 玩家
constexpr int kWhite = 2; // 电脑

// 状态徽章的三种配色：正常/思考/胜负
void applyStatusBadge(QLabel *label, const QString &fg, const QString &bg, const QString &border)
{
    label->setStyleSheet(QStringLiteral(
        "QLabel { background-color: %1; color: %2; border: 1px solid %3;"
        "         border-radius: 10px; padding: 10px 12px;"
        "         font-size: 14px; font-weight: bold; }")
                             .arg(bg, fg, border));
}
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
    setStyleSheet(wuziqiStyleSheet());

    auto *rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(18, 18, 18, 18);
    rootLayout->setSpacing(18);

    // ---------- 左侧：棋盘卡片 ----------
    auto *boardCard = new QWidget(this);
    boardCard->setObjectName(QStringLiteral("card"));
    auto *boardCardLayout = new QVBoxLayout(boardCard);
    boardCardLayout->setContentsMargins(14, 14, 14, 14);

    m_board = new BoardWidget(boardCard);
    boardCardLayout->addWidget(m_board, 0, Qt::AlignCenter);
    connect(m_board, &BoardWidget::moveRequested, this, &GameWindow::onMoveRequested);

    rootLayout->addWidget(boardCard, 0, Qt::AlignVCenter);

    // ---------- 右侧：信息卡片 ----------
    auto *sideCard = new QWidget(this);
    sideCard->setObjectName(QStringLiteral("card"));
    sideCard->setFixedWidth(252);
    auto *sideLayout = new QVBoxLayout(sideCard);
    sideLayout->setContentsMargins(20, 18, 20, 18);
    sideLayout->setSpacing(8);

    auto *sideTitle = new QLabel(QStringLiteral("人 机 对 战"), sideCard);
    sideTitle->setStyleSheet(QStringLiteral(
        "font-size: 17px; font-weight: bold; color: #5b3a1e;"));
    sideLayout->addWidget(sideTitle);
    sideLayout->addSpacing(2);

    auto *playerLabel = new QLabel(sideCard);
    playerLabel->setObjectName(QStringLiteral("infoLabel"));
    playerLabel->setTextFormat(Qt::RichText);
    playerLabel->setText(QStringLiteral("<span style='font-size:15px'>●</span> 你 · 黑棋（先行）<br/>"
                         "<span style='font-size:11px; color:#a89478'>账号：%1</span>")
                             .arg(m_username.toHtmlEscaped()));
    sideLayout->addWidget(playerLabel);

    m_difficultyLabel = new QLabel(sideCard);
    m_difficultyLabel->setObjectName(QStringLiteral("infoLabel"));
    m_difficultyLabel->setText(QStringLiteral("难度：%1")
                                   .arg(m_difficulty == Difficulty::Easy ? QStringLiteral("简单（偏随机）")
                                                                          : QStringLiteral("正常（会拦四连·冲四连）")));
    m_difficultyLabel->setWordWrap(true);
    sideLayout->addWidget(m_difficultyLabel);

    m_moveCountLabel = new QLabel(sideCard);
    m_moveCountLabel->setObjectName(QStringLiteral("infoLabel"));
    sideLayout->addWidget(m_moveCountLabel);

    auto *line1 = new QFrame(sideCard);
    line1->setFrameShape(QFrame::HLine);
    line1->setStyleSheet(QStringLiteral("color: #ece2cd;"));
    sideLayout->addWidget(line1);

    auto *ruleLabel = new QLabel(QStringLiteral(
        "① 黑棋先行，你先落子\n"
        "② 横、竖、斜任意方向五子连珠获胜\n"
        "③ 无禁手，不可悔棋"), sideCard);
    ruleLabel->setObjectName(QStringLiteral("infoLabel"));
    ruleLabel->setWordWrap(true);
    ruleLabel->setStyleSheet(QStringLiteral("color: #8b7355; font-size: 12px;"));
    sideLayout->addWidget(ruleLabel);

    auto *line2 = new QFrame(sideCard);
    line2->setFrameShape(QFrame::HLine);
    line2->setStyleSheet(QStringLiteral("color: #ece2cd;"));
    sideLayout->addWidget(line2);

    m_statusLabel = new QLabel(sideCard);
    m_statusLabel->setObjectName(QStringLiteral("statusBadge"));
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setWordWrap(true);
    m_statusLabel->setMinimumHeight(48);
    sideLayout->addWidget(m_statusLabel);

    sideLayout->addStretch(1);

    auto *restartButton = new QPushButton(QStringLiteral("重 新 开 始"), sideCard);
    restartButton->setObjectName(QStringLiteral("secondaryButton"));
    restartButton->setMinimumHeight(42);
    sideLayout->addWidget(restartButton);
    connect(restartButton, &QPushButton::clicked, this, &GameWindow::resetGame);

    auto *backButton = new QPushButton(QStringLiteral("返 回 主 菜 单"), sideCard);
    backButton->setObjectName(QStringLiteral("ghostButton"));
    sideLayout->addWidget(backButton, 0, Qt::AlignHCenter);
    connect(backButton, &QPushButton::clicked, this, &GameWindow::backToMenu);

    rootLayout->addWidget(sideCard);

    resetGame();
}

void GameWindow::updateStatus(const QString &text, const QString &foreground,
                              const QString &background)
{
    m_statusLabel->setText(text);
    const QString border = foreground; // 徽章边框与文字同色，视觉更统一
    applyStatusBadge(m_statusLabel, foreground, background, border);
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
        if (playerWon)
            updateStatus(QStringLiteral("你赢了 · 五子连珠！"),
                         QStringLiteral("#1a7f37"), QStringLiteral("#e3f5e9"));
        else
            updateStatus(QStringLiteral("电脑获胜"),
                         QStringLiteral("#b3261e"), QStringLiteral("#fbe4e2"));

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
        updateStatus(QStringLiteral("和棋 · 棋盘已满"),
                     QStringLiteral("#5f6368"), QStringLiteral("#ececec"));
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
    updateStatus(QStringLiteral("电脑思考中…"),
                 QStringLiteral("#b34700"), QStringLiteral("#fdeede"));
    QTimer::singleShot(450, this, &GameWindow::onAiTurn);
}

void GameWindow::onAiTurn()
{
    if (m_gameOver)
        return;

    const QPoint move = m_ai.chooseMove(m_grid);
    if (move.x() < 0 || move.y() < 0) {
        m_gameOver = true;
        updateStatus(QStringLiteral("和棋 · 棋盘已满"),
                     QStringLiteral("#5f6368"), QStringLiteral("#ececec"));
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
    updateStatus(QStringLiteral("轮到你落子（黑棋）"),
                 QStringLiteral("#3f2c17"), QStringLiteral("#f3ead6"));
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
    updateStatus(QStringLiteral("轮到你落子（黑棋）"),
                 QStringLiteral("#3f2c17"), QStringLiteral("#f3ead6"));
}
