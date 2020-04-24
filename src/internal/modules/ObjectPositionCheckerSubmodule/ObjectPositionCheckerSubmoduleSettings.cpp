#include "ObjectPositionCheckerSubmoduleSettings.h"

ObjectPositionCheckerSubmoduleSettings::ObjectPositionCheckerSubmoduleSettings()
{


    QLabel* label1 = new QLabel("lavel1");
    QLabel* label2 = new QLabel("lavel2", this);
    QLabel* label3 = new QLabel("lavel3", this);

   // QHBoxLayout* layout = new QHBoxLayout(this);
    //layout->addWidget(label1);


    setFixedSize(400, 400);


    QLineEdit* label11 = new QLineEdit(this);
    QLineEdit* label22 = new QLineEdit(this);
    QPushButton *button1 = new QPushButton("button1");


    QFormLayout* formLayout = new QFormLayout;

    formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    formLayout->setFormAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    formLayout->setLabelAlignment(Qt::AlignLeft);

    formLayout->addRow(label1, label11);
    formLayout->addRow(label2, label22);
    formLayout->addRow(label3, button1);

    formLayout->addItem(new QSpacerItem(0, 15));

    QFont font("ARI", 12);
    label1->setFont(font);
    label2->setFont(font);
    label3->setFont(font);

    QPushButton *button2 = new QPushButton("button2");
    QPushButton *button3 = new QPushButton("button3");
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(button2);
    buttonLayout->addWidget(button3);


    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(formLayout);
    layout->addStretch(1);
    layout->addLayout(buttonLayout);

   // setModal(false);

}
