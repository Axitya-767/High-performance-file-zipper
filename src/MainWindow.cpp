#include "MainWindow.h"
#include <QFileInfo>
#include <QtConcurrent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDropEvent>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // 1. Window Config
    setWindowTitle("8-Core Ultra Zipper");
    setFixedSize(500, 500); // Made slightly taller for the new dropdown
    setAcceptDrops(true);

    // 2. Main Layout
    auto *centralWidget = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // 3. UI Elements
    titleLabel = new QLabel("8-CORE COMPRESSOR", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFixedHeight(30);

    // NEW: Mode Selector Dropdown
    modeSelector = new QComboBox(this);
    modeSelector->addItem("📦 Compress File (Save Space)");
    modeSelector->addItem("📂 Decompress File (Restore)");
    modeSelector->addItem("🛡️ Verify Integrity (Debug Mode)");
    modeSelector->setCursor(Qt::PointingHandCursor);
    modeSelector->setFixedHeight(35);

    dropZone = new QLabel("\nDrop File Here\n", this);
    dropZone->setAlignment(Qt::AlignCenter);
    dropZone->setMinimumHeight(150);

    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setTextVisible(true);
    progressBar->setFixedHeight(25);

    auto *btnLayout = new QHBoxLayout();
    fileBtn = new QPushButton("📄 Pick File...", this);
    folderBtn = new QPushButton("📂 Pick Folder...", this);
    fileBtn->setCursor(Qt::PointingHandCursor);
    folderBtn->setCursor(Qt::PointingHandCursor);
    fileBtn->setFixedHeight(40);
    folderBtn->setFixedHeight(40);

    btnLayout->addWidget(fileBtn);
    btnLayout->addWidget(folderBtn);

    // 4. Add to Layout
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(modeSelector); // Add Dropdown
    mainLayout->addWidget(dropZone);
    mainLayout->addWidget(progressBar);
    mainLayout->addLayout(btnLayout);

    setCentralWidget(centralWidget);
    setupStyle();

    // 5. Connect Signals
    connect(&engine, &ZipperApp::progressUpdated, progressBar, &QProgressBar::setValue);
    connect(&engine, &ZipperApp::statusChanged, dropZone, &QLabel::setText);

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
        dropZone->setStyleSheet("QLabel { border: 3px solid #007AFF; background-color: #202020; border-radius: 15px; color: #fff; font-size: 16px; font-weight: bold; }");
    }
}

void MainWindow::dropEvent(QDropEvent *event) {
    setupStyle(); // Restore style
    const QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) return;
    QString path = urls.first().toLocalFile();
    if (!path.isEmpty()) processInput(path);
}

void MainWindow::processInput(const QString &path) {
    dropZone->setText("Processing...");
    progressBar->setValue(0);

    // GET SELECTED MODE
    int index = modeSelector->currentIndex();
    ZipperApp::AppMode mode = ZipperApp::MODE_COMPRESS;
    if (index == 1) mode = ZipperApp::MODE_DECOMPRESS;
    if (index == 2) mode = ZipperApp::MODE_VERIFY;

    (void)QtConcurrent::run([this, path, mode]() {
        std::string stdPath = path.toStdString();
        QFileInfo fi(path);
        std::string outputDir = fi.absolutePath().toStdString();

        // Pass the mode to the engine
        engine.processFile(stdPath, outputDir, mode);
    });
}

void MainWindow::setupStyle() {
    // 1. Global Dark Theme
    setStyleSheet("QMainWindow { background-color: #1e1e1e; }");

    // 2. Title Styling
    titleLabel->setStyleSheet("color: #ffffff; font-size: 18px; font-weight: bold; letter-spacing: 1px; margin-bottom: 10px;");

    // 3. Dropdown Styling (The "Arrow" Fix)
    modeSelector->setStyleSheet(
        "QComboBox {"
        "   background-color: #333;"
        "   color: white;"
        "   border: 1px solid #555;"
        "   border-radius: 6px;"
        "   padding: 6px 15px;"
        "   font-size: 14px;"
        "}"
        "QComboBox::drop-down {"
        "   subcontrol-origin: padding;"
        "   subcontrol-position: top right;"
        "   width: 30px;"
        "   border-left-width: 1px;"
        "   border-left-color: #555;"
        "   border-left-style: solid;"
        "   border-top-right-radius: 6px;"
        "   border-bottom-right-radius: 6px;"
        "   background: #444;" // Slightly lighter button for the arrow area
        "}"
        "QComboBox::down-arrow {"
        "   width: 0;"
        "   height: 0;"
        "   border-left: 6px solid transparent;"
        "   border-right: 6px solid transparent;"
        "   border-top: 8px solid white;" // This draws the inverted triangle (v)
        "   margin-right: 2px;"
        "}"
        );

    // 4. Drop Zone Styling
    dropZone->setStyleSheet(
        "QLabel { "
        "   border: 2px dashed #666; "
        "   border-radius: 12px; "
        "   background-color: #262626; "
        "   color: #aaa; "
        "   font-size: 16px; "
        "   font-weight: bold; "
        "}"
        );

    // 5. Progress Bar Styling
    progressBar->setStyleSheet(
        "QProgressBar { "
        "   border: 0px; "
        "   border-radius: 4px; "
        "   text-align: center; "
        "   color: white; "
        "   background-color: #2d2d2d; "
        "}"
        "QProgressBar::chunk { "
        "   background-color: #007AFF; "
        "   border-radius: 4px; "
        "}"
        );

    // 6. Button Styling
    QString btnStyle =
        "QPushButton { "
        "   background-color: #3a3a3a; "
        "   color: white; "
        "   border: 1px solid #555; "
        "   border-radius: 6px; "
        "   font-size: 14px; "
        "   font-weight: 500;"
        "}"
        "QPushButton:hover { "
        "   background-color: #4a4a4a; "
        "   border: 1px solid #007AFF; "
        "}";

    fileBtn->setStyleSheet(btnStyle);
    folderBtn->setStyleSheet(btnStyle);
}

