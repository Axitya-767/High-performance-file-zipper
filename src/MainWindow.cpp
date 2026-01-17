#include "MainWindow.h"
#include <QFileInfo>
#include <QtConcurrent>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // 1. Window Config
    setWindowTitle("8-Core Ultra Zipper");
    setFixedSize(500, 450); // Slightly taller for buttons
    setAcceptDrops(true);

    // 2. Main Layout
    auto *centralWidget = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // 3. UI Elements
    // Title
    titleLabel = new QLabel("8-CORE COMPRESSOR", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFixedHeight(30);

    // Drop Zone (The big box)
    dropZone = new QLabel("🚀\nDrop File Here\n(Auto-Cycle)", this);
    dropZone->setAlignment(Qt::AlignCenter);
    dropZone->setMinimumHeight(180);

    // Progress Bar
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setTextVisible(true);
    progressBar->setFixedHeight(25);

    // Buttons Layout (Side by Side)
    auto *btnLayout = new QHBoxLayout();
    fileBtn = new QPushButton("📄 Pick File...", this);
    folderBtn = new QPushButton("📂 Pick Folder...", this);
    fileBtn->setCursor(Qt::PointingHandCursor);
    folderBtn->setCursor(Qt::PointingHandCursor);
    fileBtn->setFixedHeight(40);
    folderBtn->setFixedHeight(40);

    btnLayout->addWidget(fileBtn);
    btnLayout->addWidget(folderBtn);

    // Add everything to main layout
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(dropZone);
    mainLayout->addWidget(progressBar);
    mainLayout->addLayout(btnLayout); // Add the buttons at bottom

    setCentralWidget(centralWidget);
    setupStyle(); // Apply the cool CSS

    // 4. Connect Signals
    connect(&engine, &ZipperApp::progressUpdated, progressBar, &QProgressBar::setValue);
    connect(&engine, &ZipperApp::statusChanged, dropZone, &QLabel::setText);

    // Button Logic
    connect(fileBtn, &QPushButton::clicked, this, [this]() {
        QString path = QFileDialog::getOpenFileName(this, "Select File to Compress");
        if (!path.isEmpty()) processInput(path);
    });

    connect(folderBtn, &QPushButton::clicked, this, [this]() {
        QString path = QFileDialog::getExistingDirectory(this, "Select Folder to Compress");
        if (!path.isEmpty()) processInput(path);
    });
}

void MainWindow::setupStyle() {
    // Dark/Professional Theme
    this->setStyleSheet("QMainWindow { background-color: #2b2b2b; }");

    titleLabel->setStyleSheet("color: #888; font-weight: bold; letter-spacing: 2px; font-size: 12px;");

    dropZone->setStyleSheet(
        "QLabel { "
        "   border: 3px dashed #555; "
        "   border-radius: 15px; "
        "   background-color: #333; "
        "   color: #ccc; "
        "   font-size: 16px; "
        "   font-weight: bold; "
        "}"
        "QLabel:hover { background-color: #3a3a3a; border-color: #007AFF; color: #fff; }"
        );

    progressBar->setStyleSheet(
        "QProgressBar { border: none; background-color: #444; border-radius: 5px; text-align: center; color: white; }"
        "QProgressBar::chunk { background-color: #007AFF; border-radius: 5px; }"
        );

    QString btnStyle =
        "QPushButton { "
        "   background-color: #007AFF; "
        "   color: white; "
        "   border-radius: 8px; "
        "   font-weight: bold; "
        "   font-size: 14px; "
        "}"
        "QPushButton:hover { background-color: #0056b3; }"
        "QPushButton:pressed { background-color: #003d80; }";

    fileBtn->setStyleSheet(btnStyle);
    folderBtn->setStyleSheet(btnStyle);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
        dropZone->setStyleSheet("border: 3px solid #007AFF; border-radius: 15px; color: white; background: #222;");
    }
}

void MainWindow::dropEvent(QDropEvent *event) {
    dropZone->setStyleSheet("border: 3px dashed #555; border-radius: 15px; color: #ccc;");

    const QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) return;

    QString path = urls.first().toLocalFile();
    if (!path.isEmpty()) processInput(path);
}

void MainWindow::processInput(const QString &path) {
    dropZone->setText("Processing...");
    progressBar->setValue(0);

    (void)QtConcurrent::run([this, path]() {
        std::string stdPath = path.toStdString();
        QFileInfo fi(path);
        std::string outputDir = fi.absolutePath().toStdString();
        engine.processOneGoCycle(stdPath, outputDir);
    });
}
