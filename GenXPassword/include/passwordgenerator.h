#ifndef PASSWORDGENERATOR_H
#define PASSWORDGENERATOR_H

#include "window.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QSlider;
class QSpinBox;
class SwitchButton;
QT_END_NAMESPACE

class PasswordGenerator final: public Window
{
public:
    explicit PasswordGenerator(QWidget *const parent = nullptr);
    virtual ~PasswordGenerator() = default;

private:
    void createWidgets();
    void configureWidgets();
    void createUi();
    void updateAllIcons(bool allIcons = true);
    void connectSignals();

    void generatePassword();
    void onShowPasswordButtonClicked();
    void copyPasswordToClipboard(bool isCutAction);

    QLineEdit *passwordLineEdit,
        *addCharsLineEdit, *removeCharLineEdit;
    QToolButton *newPasswordButton, *showPasswordButton, *copyToClipBoardButton;
    QSlider *passwordLengthSlider;
    QSpinBox *passwordLengthSpinBox;
    SwitchButton *numericSwitch, *symbolsSwitch,
        *lowercaseSwitch, *uppercaseSwitch;
};
#endif
