#include "ui/bottom/BottomDock.hpp"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

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
    layout->addWidget(recordDot);
    layout->addWidget(recording);
    layout->addWidget(recordTime);
    layout->addStretch();

    auto* duration = new QComboBox(this);
    duration->setObjectName("processSelect");
    duration->addItem("30 seg", 30);
    duration->addItem("1 min", 60);
    duration->addItem("3 min", 180);
    duration->addItem("5 min", 300);
    duration->setCurrentIndex(2);

    auto* preview = makeDockButton(this, "VISTA PREVIA");
    auto* stop = makeDockButton(this, "DETENER");
    auto* save = makeDockButton(this, "GUARDAR PSD");
    layout->addWidget(duration);
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
    layout->addWidget(zoomReadout);
    layout->addWidget(center);

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
