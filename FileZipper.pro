QT       += core gui widgets concurrent
CONFIG   += c++17

TARGET = FileZipper
TEMPLATE = app

# Tell the compiler where to look for your headers
INCLUDEPATH += src

# -------------------------------------------------
# YOUR SOURCE FILES (Make sure these exist in 'src')
# -------------------------------------------------
SOURCES += \
    main.cpp \
    src/Utils.cpp \
    src/ZipperApp.cpp \
    src/MainWindow.cpp \
    src/FrequencyCounter.cpp \
    src/HuffmanTree.cpp \
    src/BitWriter.cpp \
    src/Decompressor.cpp

# -------------------------------------------------
# YOUR HEADER FILES
# -------------------------------------------------
HEADERS += \
    src/Utils.h \
    src/ZipperApp.h \
    src/MainWindow.h \
    src/FrequencyCounter.h \
    src/HuffmanNode.h \
    src/HuffmanTree.h \
    src/BitWriter.h \
    src/Decompressor.h


# --- AUTOMATION SCRIPT ---
# This copies the tool we just created into the Mac App Bundle
STUB_SOURCE = $$PWD/tools/stub_executable

macx {
    DEST_DIR = $$OUT_PWD/$${TARGET}.app/Contents/MacOS
} else {
    DEST_DIR = $$OUT_PWD
}

copy_stub.target = $$DEST_DIR/stub_executable
copy_stub.depends = $$STUB_SOURCE
copy_stub.commands = $(COPY) $$shell_path($$STUB_SOURCE) $$shell_path($$DEST_DIR)
QMAKE_EXTRA_TARGETS += copy_stub
PRE_TARGETDEPS += $$DEST_DIR/stub_executable
