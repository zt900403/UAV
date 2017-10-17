#ifndef AIRCRAFTCONTROLLER_H
#define AIRCRAFTCONTROLLER_H

#include <QWidget>
class QPixmap;

class AircraftController : public QWidget
{
    Q_OBJECT
public:
    explicit AircraftController(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *);
    void drawPanel(QPainter *painter);
    void drawRedball(QPainter *painter);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    bool pointInRect(const QPoint &point, const QRect &rect);
    int distBetween2Points(const QPoint &p1, const QPoint &p2);
signals:

public slots:

private:
    bool m_mouseLeftPressed;
    bool m_redballFirstPaint;
    bool m_redballClicked;
    QPixmap m_panel;
    QPixmap m_redball;
    QPoint m_redballLeftTop;
};

#endif // AIRCRAFTCONTROLLER_H
