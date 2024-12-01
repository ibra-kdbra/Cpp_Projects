#include "switchbutton.h"

#include <QPainter>
#include <QPropertyAnimation>

QPixmap drawShadowEllipse(const qreal &radius, const qreal &elevation, const QColor &color)
{
    QPixmap px{ qRound(radius * 2), qRound(radius * 2) };
    px.fill(Qt::transparent);

    QPainter p{ &px };

    p.setBrush(color);
    p.setPen(Qt::NoPen);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawEllipse(QRectF(0, 0, px.size().width(), px.size().height()).center(),
                  radius - elevation,
                  radius - elevation);
    p.end();

    QImage tmp{ px.size(), QImage::Format_ARGB32_Premultiplied };
    tmp.setDevicePixelRatio(px.devicePixelRatioF());
    tmp.fill(0);
    QImage blurred{ tmp };
    p.begin(&tmp);
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.drawPixmap(QPointF(), px);
    p.end();

    p.begin(&blurred);
    qt_blurImage(&p, tmp, elevation * 4, true, false);
    p.end();

    return QPixmap::fromImage(blurred);
}

SwitchButton::SwitchButton(QWidget *const parent)
    : QAbstractButton{ parent },
    CORNER_RADIUS{ 8.0 }, THUMB_RADIUS{ 14.5 }, SHADOW_ELEVATION{ 2.0 },
    height{ 36 }, indicatorMargin { 8, 8, 8, 8 },

    trackDisabledColor{ Qt::black }, thumbDisabledColor{ Qt::gray },
    trackDisabledOpacity{ 0.25 }, disabledTextOpacity { 0.25 },

    thumbPosAnimation{ new QPropertyAnimation(this, "thumbPosition", this) },
    _thumbPosition{ isChecked()? (indicatorMargin.left() + indicatorMargin.right() + 2): 0 },
    shadowPixmap{ drawShadowEllipse(THUMB_RADIUS, SHADOW_ELEVATION, QColor(0, 0, 0, 70)) }
{
    setCursor(Qt::PointingHandCursor);
    setCheckable(true);

    thumbPosAnimation->setDuration(150);
    thumbPosAnimation->setEasingCurve(QEasingCurve::OutQuad);
}

SwitchButton::SwitchButton(const QString &text, QWidget *const parent)
    : SwitchButton{ parent }
{
    setText(text);
}

SwitchButton::SwitchButton(const QString &text, bool isThemeDark, QWidget *const parent)
    : SwitchButton{ text, parent }
{
    if (isThemeDark)
        setThemeDark();
    else setThemeLight();
}

void SwitchButton::setThemeLight()
{
    setTrackColor(TRACK_ON_COLOR_LIGHT, TRACK_OFF_COLOR_LIGHT);
    setThumbColor(THUMB_ON_COLOR_LIGHT, THUMB_OFF_COLOR_LIGHT);
}

void SwitchButton::setThemeDark()
{
    setTrackColor(TRACK_ON_COLOR_DARK, TRACK_OFF_COLOR_DARK);
    setThumbColor(THUMB_ON_COLOR_DARK, THUMB_OFF_COLOR_DARK);
}

void SwitchButton::setTrackColor(const QColor &trackOnColor, const QColor &trackOffColor)
{
    this->trackOnColor = trackOnColor;
    this->trackOffColor = trackOffColor;
}

void SwitchButton::setThumbColor(const QColor &thumbOnColor, const QColor &thumbOffColor)
{
    this->thumbOnColor = thumbOnColor;
    this->thumbOffColor = thumbOffColor;
}

QSize SwitchButton::sizeHint() const
{
    auto w{ indicatorMargin.left()
           + height
           + indicatorMargin.right()
           + fontMetrics().horizontalAdvance(text()) };

    return QSize{ w, height };
}

QRect SwitchButton::indicatorRect()
{
    const auto w{ indicatorMargin.left()
                 + height
                 + indicatorMargin.right() };

    return isLeftToRight()? QRect(0, 0, w, height): QRect(width() - w, 0, w, height);
}

QRect SwitchButton::textRect()
{
    const auto w{ indicatorMargin.left()
                 + height
                 + indicatorMargin.right() };

    return isLeftToRight()?
               rect().marginsRemoved(QMargins(w, 0, 0, 0)):
               rect().marginsRemoved(QMargins(0, 0, w, 0));
}

void SwitchButton::paintEvent(QPaintEvent *const)
{
    QPainter p{ this };
    p.setPen(Qt::NoPen);
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    QMargins trackMargin{ indicatorMargin };
    trackMargin.setTop(trackMargin.top() + 2);
    trackMargin.setBottom(trackMargin.bottom() + 2);

    QRectF trackRect{ indicatorRect().marginsRemoved(trackMargin) };

    if (isEnabled()) {
        /* DRAW TRACK */
        p.setBrush(isChecked()? trackOnColor: trackOffColor);
        p.drawRoundedRect(trackRect, CORNER_RADIUS, CORNER_RADIUS);

        /*  DRAW THUMB */
        trackRect.setX(trackRect.x() - trackMargin.left() - trackMargin.right() - 2
                       + thumbPosAnimation->currentValue().toInt());
        if (!shadowPixmap.isNull())
            p.drawPixmap(trackRect.center() - QPointF(THUMB_RADIUS, THUMB_RADIUS - 1.0), shadowPixmap);
        p.setBrush(isChecked()? thumbOnColor: thumbOffColor);
        p.drawEllipse(trackRect.center(), THUMB_RADIUS - SHADOW_ELEVATION - 1.0, THUMB_RADIUS - SHADOW_ELEVATION - 1.0);

        /* DRAW TEXT */
        if (!text().isEmpty()) {
            p.setPen(palette().color(QPalette::Active, QPalette::ButtonText));
            p.drawText(textRect(), Qt::AlignLeft | Qt::AlignVCenter, text());
        }
    } else {
        /* DRAW TRACK */
        p.setBrush(trackDisabledColor);
        p.setOpacity(trackDisabledOpacity);
        p.drawRoundedRect(trackRect, CORNER_RADIUS, CORNER_RADIUS);

        /* DRAW THUMB */
        p.setOpacity(1.0);
        if (isChecked())
            trackRect.setX(trackRect.x() + trackMargin.left() + trackMargin.right() + 2);
        else trackRect.setX(trackRect.x() - trackMargin.left() - trackMargin.right() - 2);
        if (!shadowPixmap.isNull())
            p.drawPixmap(trackRect.center() - QPointF(THUMB_RADIUS, THUMB_RADIUS - 1.0), shadowPixmap);
        p.setBrush(thumbDisabledColor);
        p.drawEllipse(trackRect.center(), THUMB_RADIUS - SHADOW_ELEVATION - 1.0, THUMB_RADIUS - SHADOW_ELEVATION - 1.0);

        /* DRAW TEXT */
        if (!text().isEmpty()) {
            p.setOpacity(disabledTextOpacity);
            p.setPen(palette().color(QPalette::Disabled, QPalette::ButtonText));
            p.drawText(textRect(), Qt::AlignLeft | Qt::AlignVCenter, text());
        }
    }
}

void SwitchButton::nextCheckState()
{
    QAbstractButton::nextCheckState();
    checkStateSet();
}

void SwitchButton::checkStateSet()
{
    const Qt::CheckState state{ isChecked()? Qt::Checked: Qt::Unchecked };
    toggle(state);
    emit stateChanged(state);
}

void SwitchButton::toggle(const Qt::CheckState &state)
{
    thumbPosAnimation->stop();
    if (state == Qt::Unchecked)
        thumbPosAnimation->setEndValue(0);
    else thumbPosAnimation->setEndValue((indicatorMargin.left() + indicatorMargin.right() + 2) * 2);
    thumbPosAnimation->start();
}

void SwitchButton::setThumbPosition(const int &thumbPosition)
{
    this->_thumbPosition = thumbPosition;
    update();
    emit thumbPositionChanged();
}

int SwitchButton::thumbPosition()
{
    return _thumbPosition;
}
