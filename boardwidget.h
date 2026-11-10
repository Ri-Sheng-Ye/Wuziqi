#ifndef BOARDWIDGET_H
#define BOARDWIDGET_H

#include <QPoint>
#include <QVector>
#include <QWidget>

// 15x15 五子棋棋盘自绘控件
class BoardWidget : public QWidget
{
    Q_OBJECT

public:
    static constexpr int kBoardN = 15;
    static constexpr int kMargin = 34;   // 棋盘外边距（像素）
    static constexpr int kCell = 36;     // 格距（像素）
    static constexpr int kWidth = 2 * kMargin + (kBoardN - 1) * kCell; // 572

    explicit BoardWidget(QWidget *parent = nullptr);

    // 整体刷新棋盘状态（0=空 1=黑 2=白）
    void setGrid(const QVector<QVector<int>> &grid);
    void setLastMove(const QPoint &cell); // cell 为 (row, col)，无则传 (-1, -1)
    void setInteractionEnabled(bool enabled);

    QSize sizeHint() const override { return QSize(kWidth, kWidth); }
    QSize minimumSizeHint() const override { return QSize(kWidth, kWidth); }

signals:
    // 用户请求在 (row, col) 落子（只发空位、棋盘可交互时）
    void moveRequested(int row, int col);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QVector<QVector<int>> m_grid; // 15x15
    QPoint m_lastMove;            // (row, col)，未落子时为 (-1, -1)
    bool m_enabled = true;
};

#endif // BOARDWIDGET_H
