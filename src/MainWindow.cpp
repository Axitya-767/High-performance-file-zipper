#include "MainWindow.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QtConcurrent>
#include <QApplication>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi();
    setupStyle();
}

void MainWindow::setupUi() {
    // Window Config
    setWindowTitle("Huffman Archiver Pro");
    setFixedSize(450, 520);
    setAcceptDrops(true);

    // Central Widget
    auto *centralWidget = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    // Title
    titleLabel = new QLabel("HUFFMAN ARCHIVER", this);
    titleLabel->setAlignment(Qt::AlignCenter);

    // Mode Selector (Clean Text)
    modeSelector = new QComboBox(this);
    modeSelector->addItem("Compression Mode (Default)");
    modeSelector->addItem("Decompression Mode");
    modeSelector->setCursor(Qt::PointingHandCursor);
    modeSelector->setFixedHeight(40);

    // Drop Zone
    dropZone = new QLabel("Drag and drop files here", this);
    dropZone->setAlignment(Qt::AlignCenter);
    dropZone->setMinimumHeight(160);

    // Progress Bar
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setTextVisible(false); // Clean look
    progressBar->setFixedHeight(6);     // Slim, modern bar

    // Buttons
    auto *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(15);

    fileBtn = new QPushButton("Select File", this);
    folderBtn = new QPushButton("Select Folder", this);

    fileBtn->setFixedHeight(45);
    folderBtn->setFixedHeight(45);
    fileBtn->setCursor(Qt::PointingHandCursor);
    folderBtn->setCursor(Qt::PointingHandCursor);

    btnLayout->addWidget(fileBtn);
    btnLayout->addWidget(folderBtn);

    // Status Footer
    statusLabel = new QLabel("Ready", this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("color: #666; font-size: 12px;");

    // Add to Layout
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(modeSelector);
    mainLayout->addWidget(dropZone);
    mainLayout->addLayout(btnLayout);
    mainLayout->addWidget(progressBar);
    mainLayout->addWidget(statusLabel);

    setCentralWidget(centralWidget);

    // Connections
    connect(fileBtn, &QPushButton::clicked, this, [this]() {
        QString path = QFileDialog::getOpenFileName(this, "Select File");
        if (!path.isEmpty()) processInput(path);
    });

    connect(folderBtn, &QPushButton::clicked, this, [this]() {
        QString path = QFileDialog::getExistingDirectory(this, "Select Folder");
        if (!path.isEmpty()) processInput(path);
    });
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
        dropZone->setStyleSheet("QLabel { border: 2px solid #007AFF; background-color: #1a1a1a; border-radius: 8px; color: #fff; }");
        dropZone->setText("Release to Process");
    }
}

void MainWindow::dropEvent(QDropEvent *event) {
    setupStyle(); // Reset style
    const QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) return;

    QString path = urls.first().toLocalFile();
    if (!path.isEmpty()) processInput(path);
}

void MainWindow::processInput(const QString &inputPath) {
    setBusyState(true);

    int modeIndex = modeSelector->currentIndex();
    bool isCompressing = (modeIndex == 0);

    // Run heavy lifting in background
    QFuture<void> future = QtConcurrent::run([this, inputPath, isCompressing]() {
        std::string stdInPath = inputPath.toStdString();

        // 1. Get clean output name (Now returns .hz file path)
        std::string stdOutPath = getSmartOutputPath(stdInPath, isCompressing);

        // 2. Call Engine
        // IMPORTANT: Your compressor must write to this FILE path, not create a folder.
        ZipperApp::AppMode mode = isCompressing ? ZipperApp::MODE_COMPRESS : ZipperApp::MODE_DECOMPRESS;
        engine.processFile(stdInPath, stdOutPath, mode);

        // 3. Create SFX (Only if compressing)
        if (isCompressing) {
            std::string stubPath = QCoreApplication::applicationDirPath().toStdString() + "/stub_executable";
            createSFX(stdOutPath, stubPath);
        }
    });

    // Watcher for completion
    auto *watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, this, [this, watcher]() {
        setBusyState(false);

        // --- SUCCESS POPUP WITH GREEN TICK ---
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Success");
        msgBox.setText("Operation finished successfully.");

        // Pull the standard "Apply" icon (Green Tick)
        QIcon checkIcon = QApplication::style()->standardIcon(QStyle::SP_DialogApplyButton);
        msgBox.setIconPixmap(checkIcon.pixmap(64, 64));

        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        // -------------------------------------

        watcher->deleteLater();
    });
    watcher->setFuture(future);
}
void MainWindow::setBusyState(bool isBusy) {
    fileBtn->setEnabled(!isBusy);
    folderBtn->setEnabled(!isBusy);
    modeSelector->setEnabled(!isBusy);

    if (isBusy) {
        // "Indeterminate" mode makes the bar bounce back and forth
        progressBar->setRange(0, 0);
        dropZone->setText("Processing...");
        statusLabel->setText("Please wait...");
    } else {
        progressBar->setRange(0, 100);
        progressBar->setValue(100);
        dropZone->setText("Drag and drop files here");
        statusLabel->setText("Ready");
        setupStyle(); // Restore clean looks
    }
}

void MainWindow::setupStyle() {
    // Professional Dark Theme (CSS-like)
    setStyleSheet("QMainWindow { background-color: #121212; }");

    titleLabel->setStyleSheet("color: #e0e0e0; font-size: 16px; font-weight: 600; letter-spacing: 1px; margin-bottom: 5px;");

    // Modern Flat Dropdown
    modeSelector->setStyleSheet(
        "QComboBox { background-color: #1e1e1e; color: #ddd; border: 1px solid #333; border-radius: 6px; padding: 5px 10px; font-size: 13px; }"
        "QComboBox::drop-down { border: none; width: 30px; }"
        "QComboBox::down-arrow { image: none; border: none; border-top: 5px solid #888; border-left: 5px solid transparent; border-right: 5px solid transparent; margin-right: 8px; }"
        );

    // Dashed Drop Zone
    dropZone->setStyleSheet(
        "QLabel { border: 2px dashed #444; border-radius: 8px; background-color: #181818; color: #888; font-size: 14px; }"
        );

    // Slim Progress Bar
    progressBar->setStyleSheet(
        "QProgressBar { border: none; background-color: #222; border-radius: 3px; }"
        "QProgressBar::chunk { background-color: #007AFF; border-radius: 3px; }"
        );

    // Flat Buttons
    QString btnStyle =
        "QPushButton { background-color: #252525; color: #ddd; border: 1px solid #333; border-radius: 6px; font-size: 13px; font-weight: 500; }"
        "QPushButton:hover { background-color: #333; border-color: #555; }"
        "QPushButton:pressed { background-color: #007AFF; border-color: #007AFF; color: white; }";

    fileBtn->setStyleSheet(btnStyle);
    folderBtn->setStyleSheet(btnStyle);
}
