#include "gomokuai.h"

#include <QRandomGenerator>
#include <QSet>
#include <QtMath>
#include <limits>

namespace {
constexpr int kBoardSize = 15;
constexpr int kPlayer = 1; // 黑棋（玩家）
constexpr int kAi = 2;     // 白棋（电脑）

bool inBounds(int r, int c)
{
    return r >= 0 && r < kBoardSize && c >= 0 && c < kBoardSize;
}

// 单方向上"连续 count 子、开放端 open 个"的分值
int lineScore(int count, int openEnds)
{
    if (count >= 5) return 100000000; // 五连
    switch (count) {
    case 4:
        if (openEnds == 2) return 10000000; // 活四
        if (openEnds == 1) return 1000000;  // 冲四
        return 10000;                       // 死四
    case 3:
        if (openEnds == 2) return 100000;   // 活三
        if (openEnds == 1) return 10000;    // 眠三
        return 100;
    case 2:
        if (openEnds == 2) return 1000;     // 活二
        if (openEnds == 1) return 100;      // 眠二
        return 10;
    case 1:
        return openEnds == 2 ? 10 : 1;
    default:
        return 0;
    }
}
} // namespace

GomokuAI::GomokuAI(Difficulty difficulty)
    : m_difficulty(difficulty)
{
}

QVector<QPoint> GomokuAI::candidateMoves(const QVector<QVector<int>> &board) const
{
    bool hasStone = false;
    QSet<QPoint> unique;
    for (int r = 0; r < kBoardSize; ++r) {
        for (int c = 0; c < kBoardSize; ++c) {
            if (board[r][c] == 0)
                continue;
            hasStone = true;
            for (int dr = -2; dr <= 2; ++dr) {
                for (int dc = -2; dc <= 2; ++dc) {
                    const int nr = r + dr, nc = c + dc;
                    if (inBounds(nr, nc) && board[nr][nc] == 0)
                        unique.insert(QPoint(nc, nr)); // QPoint(x=col, y=row)
                }
            }
        }
    }

    if (!hasStone)
        return { QPoint(kBoardSize / 2, kBoardSize / 2) };

    return QVector<QPoint>(unique.cbegin(), unique.cend());
}

int GomokuAI::evaluatePoint(const QVector<QVector<int>> &board,
                            int row, int col, int color) const
{
    static const int dirs[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };
    int total = 0;
    for (const auto &d : dirs) {
        int left = 0, right = 0;
        int r = row - d[0], c = col - d[1];
        while (inBounds(r, c) && board[r][c] == color) {
            ++left; r -= d[0]; c -= d[1];
        }
        const bool openLeft = inBounds(r, c) && board[r][c] == 0;

        r = row + d[0]; c = col + d[1];
        while (inBounds(r, c) && board[r][c] == color) {
            ++right; r += d[0]; c += d[1];
        }
        const bool openRight = inBounds(r, c) && board[r][c] == 0;

        total += lineScore(1 + left + right, (openLeft ? 1 : 0) + (openRight ? 1 : 0));
    }
    return total;
}

QPoint GomokuAI::chooseMove(const QVector<QVector<int>> &board) const
{
    const QVector<QPoint> candidates = candidateMoves(board);
    if (candidates.isEmpty()) {
        // 候选为空说明棋盘已满
        for (int r = 0; r < kBoardSize; ++r)
            for (int c = 0; c < kBoardSize; ++c)
                if (board[r][c] == 0)
                    return QPoint(c, r);
        return QPoint(-1, -1);
    }

    auto scoreCandidate = [&](const QPoint &p) {
        const int row = p.y(), col = p.x();
        const int attack = evaluatePoint(board, row, col, kAi);       // 自己进攻
        const int defense = evaluatePoint(board, row, col, kPlayer);  // 对方威胁（拦截）
        return qMax(attack, static_cast<int>(defense * 0.9));
    };

    if (m_difficulty == Difficulty::Easy) {
        // 简单难度：80% 随机落子，20% 按正常评估，整体"偏随机"
        if (QRandomGenerator::global()->bounded(100) < 80) {
            const int idx = QRandomGenerator::global()->bounded(candidates.size());
            return candidates.at(idx);
        }
    }

    // 正常难度（以及简单难度的 20% 部分）：选评估分最高的点
    int bestScore = std::numeric_limits<int>::min();
    QPoint bestMove;
    for (const QPoint &p : candidates) {
        // 加一点随机抖动，避免完全固定不变
        const int score = scoreCandidate(p) + QRandomGenerator::global()->bounded(10);
        if (score > bestScore) {
            bestScore = score;
            bestMove = p;
        }
    }
    return bestMove;
}
