#include "ui/brush/BrushControls.hpp"

#include "core/brush/BrushState.hpp"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QObject>
#include <QSignalBlocker>
#include <QSlider>
#include <QVBoxLayout>

namespace Skink::Ui::Brush {
namespace {

QWidget* createControlBlock(QWidget* parent, const QString& title, const QString& subtitle, int minimum, int maximum, int initialValue, int sliderHeight, QSlider** sliderOut)
{
    auto* block = new QWidget(parent);
    block->setObjectName("controlBlock");
    auto* row = new QHBoxLayout(block);
    row->setContentsMargins(0, 0, 0, 0);
    row->setSpacing(15);

    auto* sliderFrame = new QFrame(block);
    sliderFrame->setObjectName("sliderColumn");
    sliderFrame->setFixedSize(42, sliderHeight);
    auto* sliderLayout = new QVBoxLayout(sliderFrame);
    sliderLayout->setContentsMargins(12, 9, 12, 9);
    auto* slider = new QSlider(Qt::Vertical, sliderFrame);
    slider->setRange(minimum, maximum);
    slider->setValue(initialValue);
    sliderLayout->addWidget(slider);

    auto* copy = new QWidget(block);
    copy->setObjectName("controlCopy");
    auto* copyLayout = new QVBoxLayout(copy);
    copyLayout->setContentsMargins(0, 8, 0, 0);
    copyLayout->setSpacing(4);
    auto* titleLabel = new QLabel(title, copy);
    titleLabel->setObjectName("controlTitle");
    auto* subtitleLabel = new QLabel(subtitle, copy);
    subtitleLabel->setObjectName("controlSubtitle");
    subtitleLabel->setWordWrap(true);
    auto* output = new QLabel(copy);
    output->setObjectName("controlOutput");
    const auto updateOutput = [output, title](int value) { output->setText(QString::number(value) + (title == QStringLiteral("TAMAÑO") ? " px" : "%")); };
    updateOutput(initialValue);
    QObject::connect(slider, &QSlider::valueChanged, output, updateOutput);
    copyLayout->addWidget(titleLabel);
    copyLayout->addWidget(subtitleLabel);
    copyLayout->addWidget(output);
    copyLayout->addStretch();
    row->addWidget(sliderFrame);
    row->addWidget(copy);
    *sliderOut = slider;
    return block;
}

} // namespace

BrushControls::BrushControls(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("leftControls");
    setFixedSize(205, 410);
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(18);
    QSlider* sizeSlider = nullptr;
    QSlider* opacitySlider = nullptr;
    QSlider* pressureSlider = nullptr;
    layout->addWidget(createControlBlock(
        this,
        QStringLiteral("TAMAÑO"),
        QStringLiteral("del pincel"),
        Core::Brush::kMinimumBrushSize,
        Core::Brush::kMaximumBrushSize,
        Core::Brush::kDefaultBrushSize,
        128,
        &sizeSlider));
    layout->addWidget(createControlBlock(
        this,
        QStringLiteral("OPACIDAD"),
        QStringLiteral("del pincel"),
        Core::Brush::kMinimumBrushOpacity,
        Core::Brush::kMaximumBrushOpacity,
        Core::Brush::kDefaultBrushOpacity,
        128,
        &opacitySlider));
    layout->addWidget(createControlBlock(
        this,
        QStringLiteral("PRESIÓN"),
        QStringLiteral("Sensibilidad del lápiz"),
        Core::Brush::kMinimumPressureSensitivity,
        Core::Brush::kMaximumPressureSensitivity,
        Core::Brush::kDefaultPressureSensitivity,
        118,
        &pressureSlider));
    m_sizeSlider = sizeSlider;
    m_opacitySlider = opacitySlider;
    m_pressureSlider = pressureSlider;
    connect(m_sizeSlider, &QSlider::valueChanged, this, &BrushControls::brushSizeChanged);
    connect(m_opacitySlider, &QSlider::valueChanged, this, &BrushControls::opacityChanged);
    connect(m_pressureSlider, &QSlider::valueChanged, this, &BrushControls::pressureSensitivityChanged);
}

void BrushControls::setBrushSize(int value)
{
    const QSignalBlocker blocker(m_sizeSlider);
    m_sizeSlider->setValue(value);
}

void BrushControls::setOpacity(int value)
{
    const QSignalBlocker blocker(m_opacitySlider);
    m_opacitySlider->setValue(value);
}

void BrushControls::setPressureSensitivity(int value)
{
    const QSignalBlocker blocker(m_pressureSlider);
    m_pressureSlider->setValue(value);
}

} // namespace Skink::Ui::Brush
