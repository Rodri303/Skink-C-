#include "ui/topbar/TopBar.hpp"

#include <QColorDialog>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

namespace Skink::Ui::TopBar {

namespace {
QPushButton* makeButton(QWidget* parent, const QString& text, const char* objectName)
{
    auto* button = new QPushButton(text, parent);
    button->setObjectName(objectName);
    button->setCursor(Qt::PointingHandCursor);
    return button;
}
}

TopBar::TopBar(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("topBar");
    setFixedHeight(54);

    auto* grid = new QGridLayout(this);
    grid->setContentsMargins(22, 0, 22, 0);
    grid->setHorizontalSpacing(8);
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(2, 1);

    auto* left = new QWidget(this);
    auto* leftLayout = new QHBoxLayout(left);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(8);
    auto* wordmark = new QLabel("SW!NK", left);
    wordmark->setObjectName("wordmark");
    auto* file = makeButton(left, "Archivo", "topTextButton");
    auto* settings = makeButton(left, "Ajustes", "topTextButton");
    auto* divider = new QFrame(left);
    divider->setObjectName("topDivider");
    divider->setFixedSize(1, 26);
    auto* undo = makeButton(left, QStringLiteral("↶"), "topIconButton");
    auto* redo = makeButton(left, QStringLiteral("↷"), "topIconButton");
    leftLayout->addWidget(wordmark);
    leftLayout->addSpacing(12);
    leftLayout->addWidget(file);
    leftLayout->addWidget(settings);
    leftLayout->addSpacing(1);
    leftLayout->addWidget(divider);
    leftLayout->addWidget(undo);
    leftLayout->addWidget(redo);
    leftLayout->addStretch();

    auto* title = new QWidget(this);
    auto* titleLayout = new QHBoxLayout(title);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(10);
    auto* appTitle = new QLabel("SW!NK Alpha 1.1.1", title);
    appTitle->setObjectName("appTitle");
    auto* penStatus = new QLabel("Lápiz: Qt nativo", title);
    penStatus->setObjectName("penStatus");
    titleLayout->addWidget(appTitle);
    titleLayout->addWidget(penStatus);

    auto* right = new QWidget(this);
    auto* rightLayout = new QHBoxLayout(right);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(6);
    rightLayout->addStretch();
    auto* brush = makeButton(right, QStringLiteral("╱"), "paintToolActive");
    auto* eraser = makeButton(right, QStringLiteral("▱"), "paintTool");
    auto* layers = makeButton(right, QStringLiteral("◇"), "paintTool");
    auto* color = makeButton(right, QStringLiteral("●"), "colorTool");
    for (auto* button : {brush, eraser, layers, color}) button->setFixedSize(50, 46);
    rightLayout->addWidget(brush);
    rightLayout->addWidget(eraser);
    rightLayout->addWidget(layers);
    rightLayout->addWidget(color);

    grid->addWidget(left, 0, 0);
    grid->addWidget(title, 0, 1, Qt::AlignCenter);
    grid->addWidget(right, 0, 2);

    connect(undo, &QPushButton::clicked, this, &TopBar::undoRequested);
    connect(redo, &QPushButton::clicked, this, &TopBar::redoRequested);
    connect(color, &QPushButton::clicked, this, [this] {
        const QColor selected = QColorDialog::getColor(QColor("#151515"), this, "Color del pincel");
        if (selected.isValid()) emit colorSelected(selected);
    });
}

} // namespace Skink::Ui::TopBar
