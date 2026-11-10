#ifndef GOMOKUAI_H
#define GOMOKUAI_H

#include <QPoint>
#include <QVector>

enum class Difficulty {
    Easy,   // 简单：偏随机
    Normal  // 正常：拦截玩家四连 / 自己冲四连
};

// 五子棋 AI。棋盘使用 15x15，0=空，1=黑（玩家），2=白（电脑）
class GomokuAI
{
public:
    explicit GomokuAI(Difficulty difficulty = Difficulty::Normal);

    // 返回 AI 应当落子的位置；棋盘下满时返回 (-1, -1)
    QPoint chooseMove(const QVector<QVector<int>> &board) const;

private:
    Difficulty m_difficulty;

    // 收集"靠近已有棋子（距离 <= 2）"的空位作为候选；空盘时返回中心点
    QVector<QPoint> candidateMoves(const QVector<QVector<int>> &board) const;

    // 假设在 (row, col) 落 color 色子，评估这一步的进攻/防守分值
    int evaluatePoint(const QVector<QVector<int>> &board,
                      int row, int col, int color) const;
};

#endif // GOMOKUAI_H
