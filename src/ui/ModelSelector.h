#ifndef MODELSELECTOR_H
#define MODELSELECTOR_H


#include <QtWidgets>

struct ModelInformation;

class ModelSelector : public QDialog
{
    Q_OBJECT

public:
    explicit ModelSelector();

signals:
    void openModel(ModelInformation*);

public slots:
    void accept() override;
    void reject() override;

private:
    // Members
    bool modelSelected;
    QMap<QString, ModelInformation*> modelMap;
    ModelInformation* currentModel;
    QString currentModelID;

    // Layouts, groups, combos
    QVBoxLayout* leftLayout;
    QVBoxLayout* rightLayout;
    QGroupBox* leftGroupBox;
    QGroupBox* rightGroupBox;
    QComboBox* modelCombo;

    // Buttons
    QWidget* rightButtonBox;
    QHBoxLayout* rightButtonBoxLayout;
    QPushButton* acceptButton;
    QPushButton* cancelButton;

    // Forms
    QWidget* rightForm;
    QFormLayout* rightFormLayout;
    QLineEdit* modelNameLineEdit;
    QTextEdit* descTextEdit;
    QLineEdit* statusLineEdit;
    QLineEdit* lastUpdatedLineEdit;

    void createRight();
    bool checkFiles();

public slots:
    void handleModelComboChanged(QString);
};

#endif // MODELSELECTOR_H
