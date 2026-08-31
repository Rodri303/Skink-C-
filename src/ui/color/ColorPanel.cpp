#include "ui/color/ColorPanel.hpp"

#include "ui/color/ColorWheel.hpp"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QSignalBlocker>
#include <QSlider>
#include <QVBoxLayout>

namespace Skink::Ui::Color {

namespace {

// Provisional panel geometry: SWINK only supplied a native system picker.
constexpr int kPanelWidth = 280;
constexpr int kPanelMinimumHeight = 390;

QString sliderStyle(const QColor& start, const QColor& end)
{
    return QString(R"(
        QSlider::groove:horizontal {
            height: 6px;
            border-radius: 3px;
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 %1, stop:1 %2);
        }
        QSlider::sub-page:horizontal,
        QSlider::add-page:horizontal {
            background: transparent;
        }
        QSlider::handle:horizontal {
            width: 14px;
            height: 14px;
            margin: -5px 0;
            border: 2px solid #f3f3f3;
            border-radius: 7px;
            background: #151619;
        }
    )")
        .arg(start.name(QColor::HexRgb), end.name(QColor::HexRgb));
}

} // namespace

ColorPanel::ColorPanel(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("colorPanel");
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedWidth(kPanelWidth);
    setMinimumHeight(kPanelMinimumHeight);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(14);

    m_wheel = new ColorWheel(this);
    layout->addWidget(m_wheel, 0, Qt::AlignHCenter);

    const auto addLinearControl = [this, layout](
                                      const QString& title,
                                      QSlider*& slider,
                                      QLabel*& valueLabel) {
        auto* control = new QWidget(this);
        auto* controlLayout = new QVBoxLayout(control);
        controlLayout->setContentsMargins(0, 0, 0, 0);
        controlLayout->setSpacing(6);

        auto* header = new QWidget(control);
        auto* headerLayout = new QHBoxLayout(header);
        headerLayout->setContentsMargins(0, 0, 0, 0);
        auto* titleLabel = new QLabel(title, header);
        titleLabel->setObjectName("colorControlTitle");
        valueLabel = new QLabel(header);
        valueLabel->setObjectName("colorControlValue");
        headerLayout->addWidget(titleLabel);
        headerLayout->addStretch();
        headerLayout->addWidget(valueLabel);

        slider = new QSlider(Qt::Horizontal, control);
        slider->setRange(0, 255);
        slider->setSingleStep(1);

        controlLayout->addWidget(header);
        controlLayout->addWidget(slider);
        layout->addWidget(control);
    };

    addLinearControl("SATURACIÓN", m_saturation, m_saturationValue);
    addLinearControl("BRILLO", m_value, m_valueValue);

    auto* colorSummary = new QWidget(this);
    auto* colorSummaryLayout = new QHBoxLayout(colorSummary);
    colorSummaryLayout->setContentsMargins(0, 2, 0, 0);
    colorSummaryLayout->setSpacing(12);

    m_preview = new QFrame(colorSummary);
    m_preview->setFixedSize(62, 46);
    m_hexValue = new QLabel(colorSummary);
    m_hexValue->setObjectName("colorHexValue");
    m_hexValue->setAlignment(Qt::AlignCenter);
    m_hexValue->setMinimumWidth(112);
    m_hexValue->setFixedHeight(46);
    colorSummaryLayout->addWidget(m_preview);
    colorSummaryLayout->addWidget(m_hexValue, 1);
    layout->addWidget(colorSummary);
    layout->addStretch();

    setStyleSheet(R"(
        #colorPanel {
            background: #0e0f11;
        }
        #colorControlTitle {
            color: #b8bbc0;
            font-size: 10px;
            font-weight: 600;
        }
        #colorControlValue {
            color: #f0f1f2;
            font-size: 10px;
        }
        #colorHexValue {
            border: 1px solid #303237;
            border-radius: 8px;
            background: #17181b;
            color: #f1f2f3;
            font-size: 12px;
            font-weight: 600;
        }
    )");

    connect(m_wheel, &ColorWheel::hueChanged, this, &ColorPanel::updateColorFromControls);
    connect(m_saturation, &QSlider::valueChanged, this, &ColorPanel::updateColorFromControls);
    connect(m_value, &QSlider::valueChanged, this, &ColorPanel::updateColorFromControls);

    setColor(m_currentColor);
}

QColor ColorPanel::currentColor() const
{
    return m_currentColor;
}

void ColorPanel::setColor(const QColor& color)
{
    if (!color.isValid()) return;

    m_currentColor = color.toRgb();
    m_currentColor.setAlpha(255);

    int hue = 0;
    int saturation = 0;
    int value = 0;
    m_currentColor.getHsv(&hue, &saturation, &value);

    const QSignalBlocker wheelBlocker(m_wheel);
    const QSignalBlocker saturationBlocker(m_saturation);
    const QSignalBlocker valueBlocker(m_value);
    if (hue >= 0) m_wheel->setHue(hue);
    m_saturation->setValue(saturation);
    m_value->setValue(value);
    updateVisuals();
}

void ColorPanel::updateColorFromControls()
{
    m_currentColor = QColor::fromHsv(
        m_wheel->hue(),
        m_saturation->value(),
        m_value->value());
    m_currentColor.setAlpha(255);
    updateVisuals();
    emit colorChanged(m_currentColor);
}

void ColorPanel::updateVisuals()
{
    const int saturationPercent = qRound(m_saturation->value() * 100.0 / 255.0);
    const int valuePercent = qRound(m_value->value() * 100.0 / 255.0);
    m_saturationValue->setText(QString::number(saturationPercent) + "%");
    m_valueValue->setText(QString::number(valuePercent) + "%");
    m_hexValue->setText(m_currentColor.name(QColor::HexRgb).toUpper());

    m_preview->setStyleSheet(QString(
        "background-color: %1; border: 2px solid #f2f2f2; border-radius: 9px;")
        .arg(m_currentColor.name(QColor::HexRgb)));

    const int hue = m_wheel->hue();
    const int saturation = m_saturation->value();
    const int value = m_value->value();
    m_saturation->setStyleSheet(sliderStyle(
        QColor::fromHsv(hue, 0, value),
        QColor::fromHsv(hue, 255, value)));
    m_value->setStyleSheet(sliderStyle(
        QColor::fromHsv(hue, saturation, 0),
        QColor::fromHsv(hue, saturation, 255)));
}

} // namespace Skink::Ui::Color
