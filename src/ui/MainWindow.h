#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>

class TemperatureMonitor;
class TestObjectPositionCheckerSubmodule;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    QString customerID;
    QString license;

    TemperatureMonitor* tempMonitor;
    QLabel* tempLabel;

    QMdiArea* mdiArea;
    QMdiSubWindow* loginWindow;

    // Actions
    QAction* loginAct;
    QAction* settingsAct;
    QAction* exitAct;
    QAction* basicAct;
    QAction* integratedAct;
    QAction* trainingAct;

    TestObjectPositionCheckerSubmodule* test;

    void setPaths();
    void createStatusBar();
    void createActions();
    void createMenus();

private slots:
    void login();
    void openLogin();
    void openSettings();
    void closeApplication();
    void openCameraSelector();
    void openTrainingWindow(const QString);
    void handleTemperatureChanged(unsigned int, unsigned int);
};

#endif // MAINWINDOW_H
