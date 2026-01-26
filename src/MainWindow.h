#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QDragEnterEvent>
#include <QMimeData>
#include "ZipperApp.h"
#include "Utils.h" // Import our Helper tools

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    // UI Components
    QLabel *titleLabel;
    QComboBox *modeSelector;
    QLabel *dropZone;
    QProgressBar *progressBar;
    QPushButton *fileBtn;
    QPushButton *folderBtn;
    QLabel *statusLabel; // New footer status

    // Logic Engine
    ZipperApp engine;

    // Functions
    void processInput(const QString &path);
    void setupUi();
    void setupStyle();
    void setBusyState(bool isBusy); // Handles UI locking during processing
};

#endif // MAINWINDOW_H
