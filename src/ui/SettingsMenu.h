#ifndef SETTINGSMENU_H
#define SETTINGSMENU_H

#include <QtWidgets>

class SettingsMenu : public QDialog
{
public:
    SettingsMenu(QWidget* parent = nullptr);

    QSize sizeHint() const override;

public slots:
    void accept() override;
    void reject() override;

private:
    QLabel* customerLabel;
    QLabel* licenseLabel;
    QLabel* version;

    QLineEdit* customerID;
    QLineEdit* license;

    QPushButton* updateButton;
    QPushButton* saveButton;
    QPushButton* cancelButton;
};

#endif // SETTINGSMENU_H
