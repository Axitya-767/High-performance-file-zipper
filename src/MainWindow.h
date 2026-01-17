#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>   // Fixes 'Unknown type name QMainWindow'
#include <QWidget>       // Fixes 'Unknown type name QWidget'
#include <QLabel>        // Fixes 'Unknown type name QLabel'
#include <QProgressBar>  // Fixes 'Unknown type name QProgressBar'
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDropEvent>    // Fixes 'Unknown type name QDropEvent'
#include <QFileDialog>
#include "ZipperApp.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    QLabel *titleLabel;
    QLabel *dropZone;
    QProgressBar *progressBar;
    QPushButton *fileBtn;
    QPushButton *folderBtn;

    ZipperApp engine;

    void processInput(const QString &path);
    void setupStyle();
};

#endif
