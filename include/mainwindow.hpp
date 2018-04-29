#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <memory>

#include <QMainWindow>

#include "fmdata.hpp"

class MainWindow : public QMainWindow {
public:
    MainWindow(const std::shared_ptr<FMData>&, QWidget* parent = nullptr);
};

#endif
