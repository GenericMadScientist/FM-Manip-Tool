#include <stdexcept>

#include <QTabWidget>

#include "easymodefinder.hpp"
#include "manipfinder.hpp"
#include "mainwindow.hpp"
#include "startsearcher.hpp"

MainWindow::MainWindow(const std::shared_ptr<FMData>& fmDataPtr,
                       QWidget* parent)
    : QMainWindow(parent)
{
    if (fmDataPtr == nullptr) {
        throw std::runtime_error("Null fmDataPtr");
    }

    EasyModeFinder* easyModes = new EasyModeFinder(fmDataPtr, this);
    ManipFinder* manipFinder = new ManipFinder(fmDataPtr, this);
    StartSearcher* startSearch = new StartSearcher(fmDataPtr, this);

    QTabWidget* tabPages = new QTabWidget(this);

    tabPages->addTab(easyModes, "Easy mode finder");
    tabPages->addTab(manipFinder, "Manip finder");
    tabPages->addTab(startSearch, "Starter deck search");
    setCentralWidget(tabPages);

    resize(300, 730);
    setWindowTitle("FM Manip Tool");
}
