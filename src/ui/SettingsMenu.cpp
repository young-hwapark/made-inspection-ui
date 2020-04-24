#include "SettingsMenu.h"
#include "../internal/GlobalVariables.h"

SettingsMenu::SettingsMenu(QWidget* parent) :
    QDialog(parent)
{
    setWindowTitle("Settings");

    customerLabel = new QLabel("Customer ID");
    licenseLabel = new QLabel("License");
    version = new QLabel("Version: v" + QCoreApplication::applicationVersion());

    customerID = new QLineEdit(GlobalVariables::customerID);
    license = new QLineEdit(GlobalVariables::license);

    QFont font("ARI", 12);

    customerLabel->setFont(font);
    licenseLabel->setFont(font);
    customerID->setFont(font);
    license->setFont(font);

    QFormLayout* formLayout = new QFormLayout;

    formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    formLayout->setFormAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    formLayout->setLabelAlignment(Qt::AlignLeft);

    updateButton = new QPushButton("Update");
    formLayout->addRow(customerLabel, customerID);
    formLayout->addRow(licenseLabel, license);
    formLayout->addRow(version, updateButton);
    formLayout->addItem(new QSpacerItem(0, 15));

    saveButton = new QPushButton("Save");
    cancelButton = new QPushButton("Cancel");
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);

    connect(saveButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(formLayout);
    layout->addStretch(1);
    layout->addLayout(buttonLayout);

    setModal(false);
}

QSize SettingsMenu::sizeHint() const
{
    return QSize(512, 600);
}

void SettingsMenu::accept()
{
    QSettings settings(QApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
    settings.setValue("customerID", customerID->text());
    settings.setValue("license", license->text());

    GlobalVariables::customerID = settings.value("customerID").toString();
    GlobalVariables::license = settings.value("license").toString();
    QDialog::accept();
}

void SettingsMenu::reject()
{
    QDialog::reject();
}
