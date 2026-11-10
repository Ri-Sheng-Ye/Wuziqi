#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include "boardwidget.h"
#include "gomokuai.h"

#include <QLabel>
#include <QPushButton>
#include <QVector>
#include <QWidget>

// 棋盘对局界面：15x15，黑棋（玩家）先行，五子连珠获胜，无禁手、不可悔棋
class GameWindow : public QWidget
{
    Q_OBJECT

public:
    explicit GameWindow(const QString &username, Difficulty difficulty,
                        QWidget *parent = nullptr);

signals:
    // 请求返回主菜单（窗口关闭/返回主菜单按钮）
    void backToMenu();

private slots:
    void onMoveRequested(int row, int col);
    void onAiTurn();
    void resetGame();

private:
    bool isBoardFull() const;
    bool checkWin(int row, int col, int color) const;
    void finishIfGameOver(int lastRow, int lastCol, int color);
    void updateStatus(const QString &text, const QString &color);
    void updateMoveCount();

    QString m_username;
    Difficulty m_difficulty;
    GomokuAI m_ai;

    QVector<QVector<int>> m_grid;
    bool m_blackTurn = true;   // 黑棋先
    bool m_gameOver = false;
    int m_moveCount = 0;

    BoardWidget *m_board = nullptr;
    QLabel *m_statusLabel = nullptr;
    QLabel *m_moveCountLabel = nullptr;
    QLabel *m_difficultyLabel = nullptr;
};

#endif // GAMEWINDOW_H
