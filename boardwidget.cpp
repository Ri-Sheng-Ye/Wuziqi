#include "boardwidget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QRadialGradient>

BoardWidget::BoardWidget(QWidget *parent)
    : QWidget(parent)
    , m_grid(kBoardN, QVector<int>(kBoardN, 0))
    , m_lastMove(-1, -1)
{
    setBackgroundRole(QPalette::Window);
    setAutoFillBackground(true);
}

void BoardWidget::setGrid(const QVector<QVector<int>> &grid)
{
    m_grid = grid;
    update();
}

void BoardWidget::setLastMove(const QPoint &cell)
{
    m_lastMove = cell;
    update();
}

void BoardWidget::setInteractionEnabled(bool enabled)
{
    m_enabled = enabled;
}

void BoardWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // 木质棋盘底色
    QLinearGradient bg(0, 0, kWidth, kWidth);
    bg.setColorAt(0.0, QColor(238, 210, 150));
    bg.setColorAt(0.5, QColor(226, 190, 125));
    bg.setColorAt(1.0, QColor(210, 172, 110));
    p.fillRect(rect(), bg);

    const int last = kMargin + (kBoardN - 1) * kCell;

    // 网格线
    p.setPen(QPen(QColor(70, 45, 20), 1.4));
    for (int i = 0; i < kBoardN; ++i) {
        const int pos = kMargin + i * kCell;
        p.drawLine(kMargin, pos, last, pos); // 横线
        p.drawLine(pos, kMargin, pos, last); // 竖线
    }

    // 外框
    p.setPen(QPen(QColor(70, 45, 20), 2.6));
    p.drawRect(kMargin, kMargin, last - kMargin, last - kMargin);

    // 星位（天元与四个星）
    const int starPoints[5][2] = { {3, 3}, {3, 11}, {7, 7}, {11, 3}, {11, 11} };
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(70, 45, 20));
    for (const auto &sp : starPoints) {
        const QPoint center(kMargin + sp[1] * kCell, kMargin + sp[0] * kCell);
        p.drawEllipse(center, 4, 4);
    }

    // 棋子
    const double stoneR = kCell * 0.42;
    for (int row = 0; row < kBoardN; ++row) {
        for (int col = 0; col < kBoardN; ++col) {
            const int v = m_grid[row][col];
            if (v == 0)
                continue;
            const QPoint center(kMargin + col * kCell, kMargin + row * kCell);

            QRadialGradient grad(center.x() - stoneR * 0.35,
                                 center.y() - stoneR * 0.35,
                                 stoneR * 1.6);
            if (v == 1) { // 黑子
                grad.setColorAt(0.0, QColor(140, 140, 140));
                grad.setColorAt(0.35, QColor(60, 60, 60));
                grad.setColorAt(1.0, QColor(0, 0, 0));
            } else {      // 白子
                grad.setColorAt(0.0, QColor(255, 255, 255));
                grad.setColorAt(0.6, QColor(235, 235, 235));
                grad.setColorAt(1.0, QColor(150, 150, 155));
            }
            p.setPen(QPen(QColor(40, 40, 40, 120), 1));
            p.setBrush(grad);
            p.drawEllipse(center, static_cast<int>(stoneR), static_cast<int>(stoneR));

            // 最后一手标记（红点）
            if (m_lastMove.x() == row && m_lastMove.y() == col) {
                p.setPen(Qt::NoPen);
                p.setBrush(QColor(220, 50, 50));
                p.drawEllipse(center, 4, 4);
            }
        }
    }
}

void BoardWidget::mousePressEvent(QMouseEvent *event)
{
    if (!m_enabled)
        return;
    if (event->button() != Qt::LeftButton)
        return;

    const int col = qRound((event->position().x() - kMargin) / static_cast<double>(kCell));
    const int row = qRound((event->position().y() - kMargin) / static_cast<double>(kCell));
    if (row < 0 || row >= kBoardN || col < 0 || col >= kBoardN)
        return;
    if (m_grid[row][col] != 0)
        return;

    emit moveRequested(row, col);
}
