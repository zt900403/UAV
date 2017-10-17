#include "aircraftcontroller.h"
#include <QPainter>
#include <QPoint>
#include <QMouseEvent>
#include <cmath>

AircraftController::AircraftController(QWidget *parent)
    : QWidget(parent)
    , m_mouseLeftPressed(false)
    , m_redballFirstPaint(true)
    , m_redballClicked(false)
    , m_panel(":/images/aircraft_controller/panel.png")
    , m_redball(":/images/aircraft_controller/smallredball.png")
{
    m_panel = m_panel.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_redball = m_redball.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void AircraftController::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    drawPanel(&painter);
    drawRedball(&painter);
}

void AircraftController::drawPanel(QPainter *painter)
{
    painter->save();
    int targetWidgth = m_panel.width();
    int targetHeight = m_panel.height();
    int pixX = rect().center().x() - targetWidgth / 2;
    int pixY = rect().center().y() - targetHeight / 2;
    QPoint point(pixX, pixY);
    painter->drawPixmap(point, m_panel);
    painter->restore();
}

void AircraftController::drawRedball(QPainter *painter)
{
    painter->save();

    if (m_redballFirstPaint) {
        int pixX = rect().center().x() - m_redball.width() / 2;
        int pixY = rect().center().y() - m_redball.height() / 2;
        m_redballLeftTop.setX(pixX);
        m_redballLeftTop.setY(pixY);
        painter->drawPixmap(m_redballLeftTop, m_redball);

        m_redballFirstPaint = !m_redballFirstPaint;

    } else {
        painter->drawPixmap(m_redballLeftTop, m_redball);
    }

    painter->restore();
}

void AircraftController::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_mouseLeftPressed = true;

        QPoint point = event->pos();
        QRect rect(m_redballLeftTop.x(), m_redballLeftTop.y(),
                   m_redball.width(), m_redball.height());
        if (pointInRect(point, rect)) {
            m_redballClicked = true;
        }
    }
}

void AircraftController::mouseMoveEvent(QMouseEvent *event)
{
    if (m_mouseLeftPressed && m_redballClicked) {
        QPoint point = event->pos();
        QPoint widgetCenter = rect().center();
        QPoint redCenter = m_redballLeftTop;
        redCenter += QPoint(m_redball.width() / 2, m_redball.height() / 2);
        if (distBetween2Points(widgetCenter, redCenter) <= m_panel.width() / 2) {
            int x = point.x() - m_redball.width() / 2;
            int y = point.y() - m_redball.height() / 2;
            m_redballLeftTop.setX(x);
            m_redballLeftTop.setY(y);
        } else {
            //当红点圆心超出大圆圆心时更新红点圆心在大圆边界上

        }
        update();
    }
}

void AircraftController::mouseReleaseEvent(QMouseEvent *)
{
    m_mouseLeftPressed = false;
    m_redballClicked = false;
}

bool AircraftController::pointInRect(const QPoint &point, const QRect &rect)
{
    return rect.contains(point);
}

int AircraftController::distBetween2Points(const QPoint &p1, const QPoint &p2)
{
    int dx = p1.x() - p2.x();
    int dy = p1.y() - p2.y();
    return int(std::sqrt(dx * dx + dy * dy));
}
