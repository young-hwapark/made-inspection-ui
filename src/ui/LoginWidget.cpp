#include "LoginWidget.h"
#include "../internal/GlobalVariables.h"
#include "../internal/utils/RestClient.h"

LoginWidget::LoginWidget(QWidget* parent) :
    QDialog(parent)
{
    setWindowTitle("Login");
    setFixedSize(512, 144);

    emailLabel = new QLabel("Email", this);
    passwordLabel = new QLabel("Password", this);

    email = new QLineEdit("steven@neuronaware.com", this);
    password = new QLineEdit("qwer1234", this);
    password->setEchoMode(QLineEdit::Password);

    QFont font("ARI", 14);

    emailLabel->setFont(font);
    passwordLabel->setFont(font);
    email->setFont(font);
    password->setFont(font);

    QFormLayout* formLayout = new QFormLayout;

    formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    formLayout->setFormAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    formLayout->setLabelAlignment(Qt::AlignLeft);

    formLayout->addRow(emailLabel, email);
    formLayout->addRow(passwordLabel, password);

    loginButton = new QPushButton("Login", this);
    cancelButton = new QPushButton("Cancel", this);
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(cancelButton);
    connect(loginButton, SIGNAL(clicked()), this, SLOT(login()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(formLayout);
    layout->addStretch(1);
    layout->addLayout(buttonLayout);

    setModal(false);
}

void LoginWidget::login()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (email->text() == "") {
        QMessageBox msg(QMessageBox::Critical, "Validation Failed", "Please input your email");
        msg.exec();
        email->setFocus();
        QApplication::restoreOverrideCursor();
        return;
    }

    if (password->text() == "") {
        QMessageBox msg(QMessageBox::Critical, "Validation Failed", "Please input your password");
        msg.exec();
        password->setFocus();
        QApplication::restoreOverrideCursor();
        return;
    }

    // Temporary
    GlobalVariables::license = "5Y2V5-Y126H-W1XU0-56E51-77810-EF58";
    GlobalVariables::customerID = "Dongjin";

    QString license = GlobalVariables::license;
    QString customerID = GlobalVariables::customerID;


    QVariantMap map;
    map.insert("email", email->text());
    map.insert("pw", password->text());
    map.insert("licenseID", license);
    QByteArray payload = QJsonDocument::fromVariant(map).toJson();

    RestClient* rest = new RestClient;
    std::pair<int, QJsonDocument>result = rest->reqPost("/auth/signin", payload, "", customerID);
    int httpStatus = result.first;
    QJsonDocument json = result.second;
    delete rest;

    QJsonObject obj = json.object();
    QVariantMap resultMap = obj.toVariantMap();
    QString tokenReceived = obj["token"].toString();
    QString name = obj["name"].toString();

    if (httpStatus == 200 && tokenReceived != "") {
        GlobalVariables::token = tokenReceived;
        GlobalVariables::name = name;
        emit loginSuccess();
        QApplication::restoreOverrideCursor();
        QDialog::accept();
    } else {
        QMessageBox msg(QMessageBox::Critical, "Invalid Login", "Your login information is incorrect.");
        msg.exec();
        QApplication::restoreOverrideCursor();
        return;
    }
}

void LoginWidget::cancel()
{
    QDialog::reject();
}
