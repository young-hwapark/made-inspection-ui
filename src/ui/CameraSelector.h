#ifndef CAMERASELECTOR_H
#define CAMERASELECTOR_H

#include <QtWidgets>

class CameraSelector : public QDialog
{
    Q_OBJECT

public:
    explicit CameraSelector();

signals:
    void openTrainingWindow(const QString);

public slots:
    void accept() override;
    void reject() override;
    void handleCameraComboChanged(const QString);

private:
    // Members
    QString cameraIp;
    QMap<QString, QList<QString>> cameraList;
    QList<QString> currentModel;

    // Layouts, groups, combos
    QVBoxLayout* layout;
    QGroupBox* groupBox;
    QComboBox* cameraCombo;

    // Buttons
    QPushButton* acceptButton;
    QPushButton* cancelButton;

    // Forms
    QLineEdit* ipLineEdit;
    QLineEdit* modelLineEdit;
    QLineEdit* serialLineEdit;
    QLineEdit* versionLineEdit;
    QLineEdit* userNameLineEdit;
    QLineEdit* friendlyNameLineEdit;
    QLineEdit* vendorLineEdit;
    QLineEdit* deviceClassLineEdit;

    void createForms();

private slots:
    void updateCameraList();
};

#endif // CAMERASELECTOR_H
