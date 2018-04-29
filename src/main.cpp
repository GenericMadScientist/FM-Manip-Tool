#include <QApplication>

#include "mainwindow.hpp"

int main(int argc, char** argv)
{
    std::shared_ptr<FMData> dbDump = readFromDbs("FmDatabase.db",
                                                 "AnimationDatabase.db");
    QApplication app(argc, argv);

    MainWindow window(dbDump);
    window.show();

    return app.exec();
}
