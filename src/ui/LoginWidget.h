#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QtWidgets>

class LoginWidget : public QDialog
{
    Q_OBJECT

public:
    LoginWidget(QWidget* parent = nullptr);

public slots:
    void login();
    void cancel();

signals:
    void loginSuccess();

private:
    QString customerID;
    QLabel* emailLabel;
    QLabel* passwordLabel;

    QLineEdit* email;
    QLineEdit* password;

    QPushButton* loginButton;
    QPushButton* cancelButton;
};

#endif // LOGINWIDGET_H
