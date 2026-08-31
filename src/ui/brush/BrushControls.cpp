#include "ui/brush/BrushControls.hpp"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QObject>
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
    layout->addWidget(createControlBlock(this, QStringLiteral("TAMAÑO"), QStringLiteral("del pincel"), 1, 160, 14, 128, &sizeSlider));
    layout->addWidget(createControlBlock(this, QStringLiteral("OPACIDAD"), QStringLiteral("del pincel"), 5, 100, 100, 128, &opacitySlider));
    layout->addWidget(createControlBlock(this, QStringLiteral("PRESIÓN"), QStringLiteral("Sensibilidad del lápiz"), 0, 100, 85, 118, &pressureSlider));
    connect(sizeSlider, &QSlider::valueChanged, this, &BrushControls::brushSizeChanged);
    connect(opacitySlider, &QSlider::valueChanged, this, &BrushControls::opacityChanged);
    connect(pressureSlider, &QSlider::valueChanged, this, &BrushControls::pressureSensitivityChanged);
}

} // namespace Skink::Ui::Brush
