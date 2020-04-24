#ifndef INSPECTIONSETTING_H
#define INSPECTIONSETTING_H

#include <QtWidgets>

class InspectionSetting : public QDialog
{
    Q_OBJECT

public:
    explicit InspectionSetting( QWidget *parent = nullptr);

private:
    QTabWidget *tabWidget;
    QDialogButtonBox *buttonBox;
};

#endif // INSPECTIONSETTING_H
