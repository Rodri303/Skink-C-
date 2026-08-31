#include "ui/bottom/BottomDock.hpp"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace Skink::Ui::Bottom {

namespace {

QPushButton* makeDockButton(QWidget* parent, const QString& text)
{
    auto* button = new QPushButton(text, parent);
    button->setObjectName("bottomButton");
    button->setFixedHeight(38);
    return button;
}

} // namespace

BottomDock::BottomDock(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("bottomBar");
    setFixedHeight(72);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(18, 10, 18, 12);
    layout->setSpacing(12);

    auto* recordDot = new QLabel(this);
    recordDot->setObjectName("recordDot");
    recordDot->setFixedSize(14, 14);
    auto* recording = new QLabel("GRABANDO", this);
    recording->setObjectName("bottomStrong");
    auto* recordTime = new QLabel("00:00:00", this);
    recordTime->setObjectName("bottomText");
    auto* recordChevron = new QLabel(QStringLiteral("⌄"), this);
    recordChevron->setObjectName("bottomText");
    layout->addWidget(recordDot);
    layout->addWidget(recording);
    layout->addWidget(recordTime);
    layout->addWidget(recordChevron);
    layout->addStretch();

    auto* durationWrap = new QWidget(this);
    auto* durationLayout = new QVBoxLayout(durationWrap);
    durationLayout->setContentsMargins(0, 0, 0, 0);
    durationLayout->setSpacing(2);

    auto* processCaption = new QLabel("PROCESO", durationWrap);
    processCaption->setObjectName("processCaption");
    auto* duration = new QComboBox(durationWrap);
    duration->setObjectName("processSelect");
    duration->addItem("30 seg", 30);
    duration->addItem("1 min", 60);
    duration->addItem("3 min", 180);
    duration->addItem("5 min", 300);
    duration->setCurrentIndex(2);
    durationLayout->addWidget(processCaption);
    durationLayout->addWidget(duration);

    auto* preview = makeDockButton(this, QStringLiteral("▶  VISTA PREVIA"));
    auto* stop = makeDockButton(this, QStringLiteral("■  DETENER"));
    auto* save = makeDockButton(this, "GUARDAR PSD");
    layout->addWidget(durationWrap);
    layout->addWidget(preview);
    layout->addWidget(stop);
    layout->addWidget(save);
    layout->addStretch();

    auto* zoomReadout = new QWidget(this);
    zoomReadout->setObjectName("zoomReadout");
    auto* zoomLayout = new QHBoxLayout(zoomReadout);
    zoomLayout->setContentsMargins(10, 0, 10, 0);
    zoomLayout->setSpacing(10);
    auto* zoomOut = new QPushButton(QStringLiteral("−"), zoomReadout);
    zoomOut->setObjectName("zoomButton");
    m_zoomPercent = new QLabel("100%", zoomReadout);
    m_zoomPercent->setObjectName("bottomText");
    auto* zoomIn = new QPushButton("+", zoomReadout);
    zoomIn->setObjectName("zoomButton");
    zoomLayout->addWidget(zoomOut);
    zoomLayout->addWidget(m_zoomPercent);
    zoomLayout->addWidget(zoomIn);

    auto* center = makeDockButton(this, "C   CENTRAR");
    auto* hideDock = new QPushButton(QStringLiteral("⌄"), this);
    hideDock->setObjectName("squareBottomButton");
    hideDock->setFixedSize(42, 38);
    layout->addWidget(zoomReadout);
    layout->addWidget(center);
    layout->addWidget(hideDock);

    connect(zoomOut, &QPushButton::clicked, this, &BottomDock::zoomOutRequested);
    connect(zoomIn, &QPushButton::clicked, this, &BottomDock::zoomInRequested);
    connect(center, &QPushButton::clicked, this, &BottomDock::resetViewRequested);
    connect(preview, &QPushButton::clicked, this, &BottomDock::previewRequested);
    connect(stop, &QPushButton::clicked, this, &BottomDock::stopRequested);
    connect(save, &QPushButton::clicked, this, &BottomDock::savePsdRequested);
    connect(duration, &QComboBox::currentIndexChanged, this, [this, duration] {
        emit processDurationChanged(duration->currentData().toInt());
    });
}

void BottomDock::setZoomPercent(int percent)
{
    m_zoomPercent->setText(QString::number(percent) + "%");
}

} // namespace Skink::Ui::Bottom
