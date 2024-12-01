#ifndef SWITCHBUTTON_H
#define SWITCHBUTTON_H

#include <QAbstractButton>

#define TRACK_OFF_COLOR_DARK (QColor(154, 160, 166))
#define THUMB_OFF_COLOR_DARK (QColor(218, 220, 224))
#define TRACK_ON_COLOR_DARK (QColor(138, 180, 248, 128))
#define THUMB_ON_COLOR_DARK (QColor(138, 180, 255, 255))

#define TRACK_OFF_COLOR_LIGHT (QColor(189, 193, 198))
#define THUMB_OFF_COLOR_LIGHT (QColor(255, 255, 255))
#define TRACK_ON_COLOR_LIGHT (QColor(26, 115, 232, 128))
#define THUMB_ON_COLOR_LIGHT (QColor(26, 115, 232, 255))

QT_BEGIN_NAMESPACE
class QPropertyAnimation;
extern Q_DECL_IMPORT void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);
QT_END_NAMESPACE

QPixmap drawShadowEllipse(const qreal &radius, const qreal &elevation, const QColor &color);

class SwitchButton final: public QAbstractButton
{
    Q_OBJECT
    Q_PROPERTY(int thumbPosition READ thumbPosition WRITE setThumbPosition NOTIFY thumbPositionChanged FINAL)
public:
    explicit SwitchButton(QWidget *const parent = nullptr);
    explicit SwitchButton(const QString &text, QWidget *const parent = nullptr);
    explicit SwitchButton(const QString &text, bool isThemeDark, QWidget *const parent = nullptr);
    virtual ~SwitchButton() = default;

    void setThemeLight();
    void setThemeDark();

    void setTrackColor(const QColor &trackOnColor, const QColor &trackOffColor);
    void setThumbColor(const QColor &thumbOnColor, const QColor &thumbOffColor);

    virtual QSize sizeHint() const override;

signals:
    void stateChanged(int);
    void thumbPositionChanged();

protected:
    QRect indicatorRect();
    QRect textRect();

    virtual void paintEvent(QPaintEvent *const) override;
    virtual void nextCheckState() override;
    virtual void checkStateSet() override;

    void toggle(const Qt::CheckState &state);

private:
    void setThumbPosition(const int &thumbPosition);
    int thumbPosition();

    const qreal CORNER_RADIUS, THUMB_RADIUS, SHADOW_ELEVATION;
    int height;
    QMargins indicatorMargin;

    QColor trackOnColor, trackOffColor,
        thumbOnColor, thumbOffColor,
        trackDisabledColor, thumbDisabledColor;
    qreal trackDisabledOpacity, disabledTextOpacity;

    QPropertyAnimation *const thumbPosAnimation;
    int _thumbPosition;
    QPixmap shadowPixmap;
};

#endif
