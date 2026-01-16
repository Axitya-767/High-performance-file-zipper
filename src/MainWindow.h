#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>      // <--- NEW: For the buttons
#include <QVBoxLayout>
#include <QHBoxLayout>      // <--- NEW: To put buttons side-by-side
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileDialog>      // <--- NEW: To open the file browser
#include "ZipperApp.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    QLabel *titleLabel;       // "8-Core Ultra Zipper" text
    QLabel *dropZone;         // The big dashed box
    QProgressBar *progressBar;
    QPushButton *fileBtn;     // "Pick File" button
    QPushButton *folderBtn;   // "Pick Folder" button

    ZipperApp engine;

    void processInput(const QString &path);
    void setupStyle();        // Helper to keep code clean
};

#endif
