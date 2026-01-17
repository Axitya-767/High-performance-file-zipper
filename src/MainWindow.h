#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDropEvent>
#include <QFileDialog>
#include <QComboBox> // NEW INCLUDE
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
    QComboBox *modeSelector; // NEW DROPDOWN
    QLabel *dropZone;
    QProgressBar *progressBar;
    QPushButton *fileBtn;
    QPushButton *folderBtn;

    ZipperApp engine;

    void processInput(const QString &path);
    void setupStyle();
};

#endif
