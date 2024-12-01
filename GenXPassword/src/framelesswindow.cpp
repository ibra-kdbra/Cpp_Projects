#include "framelesswindow.h"

#include <QStyle>
#include <QHoverEvent>
#include <QWindow>

FramelessWindow::FramelessWindow(QWidget *const parent)
    : QMainWindow{ parent, Qt::FramelessWindowHint },
    m_titleBar{ nullptr }
{ }

bool FramelessWindow::event(QEvent *const event)
{
    if (windowFlags() & Qt::FramelessWindowHint) {
        static Qt::Edges edges{ Qt::Edges() };
        const int margin{ style()->pixelMetric(QStyle::PM_LayoutTopMargin) };
        switch (event->type()) {
        case QEvent::MouseButtonPress:
            updateCursor(edges);
            if ((m_titleBar != nullptr) && m_titleBar->underMouse())
                windowHandle()->startSystemMove();
            else if (edges)
                windowHandle()->startSystemResize(edges);
            break;
        case QEvent::MouseButtonRelease:
            edges = Qt::Edges();
            updateCursor(edges);
            setFocus();
            break;
        case QEvent::HoverMove: {
            edges = Qt::Edges();
            QPoint p{ static_cast<QHoverEvent *>(event)->position().toPoint() };

            if (p.x() < margin)
                edges |= Qt::LeftEdge;
            if (p.x() > (width() - margin))
                edges |= Qt::RightEdge;
            if (p.y() < margin)
                edges |= Qt::TopEdge;
            if (p.y() > (height() - margin))
                edges |= Qt::BottomEdge;
            updateCursor(edges);
            break;
        }
        default:
            break;
        }
    }
    return QMainWindow::event(event);
}

void FramelessWindow::setTitleBar(QWidget *const widget)
{
    m_titleBar = widget;
}

QWidget* FramelessWindow::getTitleBar() const
{
    return m_titleBar;
}

void FramelessWindow::updateCursor(const Qt::Edges &edges)
{
    switch (edges) {
    case 0:
        setCursor(Qt::ArrowCursor);
        break;
    case Qt::LeftEdge:
    case Qt::RightEdge:
        setCursor(Qt::SizeHorCursor);
        break;
    case Qt::TopEdge:
    case Qt::BottomEdge:
        setCursor(Qt::SizeVerCursor);
        break;
    case Qt::LeftEdge | Qt::TopEdge:
    case Qt::RightEdge | Qt::BottomEdge:
        setCursor(Qt::SizeFDiagCursor);
        break;
    case Qt::RightEdge | Qt::TopEdge:
    case Qt::LeftEdge | Qt::BottomEdge:
        setCursor(Qt::SizeBDiagCursor);
        break;
    }
}
