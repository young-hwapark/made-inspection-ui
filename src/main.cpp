#include <QApplication>
#include <QLibrary>
#include <QDir>
#include <iostream>

int main(int argc, char *argv[])
{
    QString libPath;
    QApplication* a = new QApplication(argc, argv);

#ifdef Q_OS_WIN
    libPath = QDir(QApplication::applicationDirPath()).filePath("nvision1"); // Fix in future
#else
    libPath = QDir(QApplication::applicationDirPath()).filePath("libnvision");
#endif

    delete a;

    QLibrary library(libPath);

    typedef int (*createFunction)(int, char *[]);
    createFunction w = reinterpret_cast<createFunction>(library.resolve("create"));

    if (library.isLoaded())
        std::cout << "Library loaded." << std::endl;
    else
        std::cout << library.errorString().toStdString() << std::endl;

    if (w)
        return w(argc, argv);
    else
        std::cout << "Could not open MainWindow." << std::endl;

    return 0;
}
