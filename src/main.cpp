#include <QApplication>
#include <QCoreApplication>

#include "app/MainWindow.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("Skink");
    QCoreApplication::setApplicationVersion("0.1.0");

    Skink::App::MainWindow window;
    window.show();

    return app.exec();
}
