#include "passwordgenerator.h"
#include "switchbutton.h"
#include "icons.h"

#include <QLineEdit>
#include <QToolButton>
#include <QSlider>
#include <QSpinBox>
#include <QSettings>
#include <QHBoxLayout>
#include <QLabel>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QApplication>
#include <QClipboard>
#include <QTimer>

PasswordGenerator::PasswordGenerator(QWidget *const parent)
    : Window{ parent }
{
    createWidgets();
    configureWidgets();
    createUi();
    updateAllIcons(false);
    connectSignals();
}

void PasswordGenerator::createWidgets()
{
    passwordLineEdit = new QLineEdit;
    newPasswordButton = new QToolButton;
    showPasswordButton = new QToolButton;
    copyToClipBoardButton = new QToolButton;

    passwordLengthSlider = new QSlider(Qt::Horizontal);
    passwordLengthSpinBox = new QSpinBox;

    const qsizetype currentIndexTheme{ getCurrentIndexTheme() };
    numericSwitch = new SwitchButton(QObject::tr("Numeric characters"), currentIndexTheme);
    symbolsSwitch = new SwitchButton(QObject::tr("Symbols characters"), currentIndexTheme);
    lowercaseSwitch = new SwitchButton(QObject::tr("Lowercase letters"), currentIndexTheme);
    uppercaseSwitch = new SwitchButton(QObject::tr("Uppercase letters"), currentIndexTheme);

    qDebug() << QObject::tr("Numeric characters");
    qDebug() << numericSwitch->text();

    addCharsLineEdit = new QLineEdit;
    removeCharLineEdit = new QLineEdit;
}

void PasswordGenerator::configureWidgets()
{
    passwordLineEdit->setReadOnly(true);
    passwordLineEdit->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    passwordLineEdit->setPlaceholderText(QObject::tr("Your password will be generated here"));

    showPasswordButton->setAutoRaise(true);
    showPasswordButton->setCheckable(true);
    showPasswordButton->setChecked(userSettings->value("Settings/HidePassword").toBool());
    onShowPasswordButtonClicked();

    newPasswordButton->setAutoRaise(true);
    newPasswordButton->setToolTip(QObject::tr("Generate a new password"));

    copyToClipBoardButton->setAutoRaise(true);
    copyToClipBoardButton->setToolTip(QObject::tr("Copy the password to the clipboard"));

    passwordLengthSlider->setRange(1, 512);
    passwordLengthSlider->setValue(userSettings->value("Settings/Length").toInt());
    passwordLengthSpinBox->setRange(1, 512);
    passwordLengthSpinBox->setValue(userSettings->value("Settings/Length").toInt());

    numericSwitch->setLayoutDirection(Qt::RightToLeft);
    numericSwitch->setChecked(userSettings->value("Settings/Numeric").toBool());
    numericSwitch->setToolTip(QObject::tr("Contain the numbers: 0-9"));

    symbolsSwitch->setLayoutDirection(Qt::RightToLeft);
    symbolsSwitch->setChecked(userSettings->value("Settings/Symbols").toBool());
    symbolsSwitch->setToolTip(QObject::tr("Contain special symbols:\n!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"));

    lowercaseSwitch->setLayoutDirection(Qt::RightToLeft);
    lowercaseSwitch->setChecked(userSettings->value("Settings/Lowercase").toBool());
    lowercaseSwitch->setToolTip(QObject::tr("Contain lowercase letters: a-z"));

    uppercaseSwitch->setLayoutDirection(Qt::RightToLeft);
    uppercaseSwitch->setChecked(userSettings->value("Settings/Uppercase").toBool());
    uppercaseSwitch->setToolTip(QObject::tr("Contain uppercase letters: A-Z"));

    addCharsLineEdit->setPlaceholderText(QObject::tr("Characters to add"));
    addCharsLineEdit->setText(userSettings->value("Settings/Add").toString());

    removeCharLineEdit->setPlaceholderText(QObject::tr("Characters to remove"));
    removeCharLineEdit->setText(userSettings->value("Settings/Remove").toString());
}

void PasswordGenerator::createUi()
{
    std::array<QHBoxLayout *const, 6> horizontalLayout
        {
            new QHBoxLayout,
            new QHBoxLayout,
            new QHBoxLayout,
            new QHBoxLayout,
            new QHBoxLayout,
            new QHBoxLayout
        };

    horizontalLayout[0]->addWidget(passwordLineEdit);
    horizontalLayout[0]->addWidget(newPasswordButton);
    horizontalLayout[0]->addWidget(showPasswordButton);
    horizontalLayout[0]->addWidget(copyToClipBoardButton);

    horizontalLayout[1]->addWidget(passwordLengthSlider);
    horizontalLayout[1]->addWidget(passwordLengthSpinBox);

    horizontalLayout[2]->addWidget(numericSwitch);
    horizontalLayout[2]->addWidget(symbolsSwitch);

    horizontalLayout[3]->addWidget(lowercaseSwitch);
    horizontalLayout[3]->addWidget(uppercaseSwitch);

    horizontalLayout[4]->addWidget(new QLabel(QObject::tr("Add")), 0, Qt::AlignCenter);
    horizontalLayout[4]->addWidget(new QLabel(QObject::tr("Remove")), 0, Qt::AlignCenter);

    horizontalLayout[5]->addWidget(addCharsLineEdit);
    horizontalLayout[5]->addWidget(removeCharLineEdit);

    QGridLayout *const mainGridLayout{ new QGridLayout };
    mainGridLayout->addWidget(getTitleBar());
    mainGridLayout->addLayout(horizontalLayout[0], 1, 0);
    mainGridLayout->addLayout(horizontalLayout[1], 2, 0);
    mainGridLayout->addLayout(horizontalLayout[2], 3, 0, Qt::AlignCenter);
    mainGridLayout->addLayout(horizontalLayout[3], 4, 0, Qt::AlignCenter);
    mainGridLayout->addLayout(horizontalLayout[4], 5, 0);
    mainGridLayout->addLayout(horizontalLayout[5], 6, 0);

    QWidget *const mainWidget{ new QWidget };
    mainWidget->setLayout(mainGridLayout);
    setCentralWidget(mainWidget);
}

void PasswordGenerator::updateAllIcons(bool allIcons)
{
    const qsizetype currentIndexTheme{ getCurrentIndexTheme() };
    if (currentIndexTheme) {
        newPasswordButton->setIcon(REFRESH_ICON_LIGHT);
        showPasswordButton->setIcon(showPasswordButton->isChecked()? VISIBILITYOFF_ICON_LIGHT: VISIBILITYON_ICON_LIGHT);
        copyToClipBoardButton->setIcon(COPY_ICON_LIGHT);
    } else {
        newPasswordButton->setIcon(REFRESH_ICON_DARK);
        showPasswordButton->setIcon(showPasswordButton->isChecked()? VISIBILITYOFF_ICON_DARK: VISIBILITYON_ICON_DARK);
        copyToClipBoardButton->setIcon(COPY_ICON_DARK);
    }

    if(allIcons) {
        if (currentIndexTheme) {
            numericSwitch->setThemeDark();
            symbolsSwitch->setThemeDark();
            lowercaseSwitch->setThemeDark();
            uppercaseSwitch->setThemeDark();
        } else {
            numericSwitch->setThemeLight();
            symbolsSwitch->setThemeLight();
            lowercaseSwitch->setThemeLight();
            uppercaseSwitch->setThemeLight();
        }

        Window::updateIcons();
    }
}

void PasswordGenerator::connectSignals()
{
    connect(this, &Window::themeChanged, this, &PasswordGenerator::updateAllIcons);
    connect(this, &Window::newPasswordSignal, this, &PasswordGenerator::generatePassword);
    connect(this, &Window::cutPasswordSignal, this, &PasswordGenerator::copyPasswordToClipboard);

    connect(newPasswordButton, &QToolButton::clicked, this, &PasswordGenerator::generatePassword);
    connect(showPasswordButton, &QToolButton::clicked, this, &PasswordGenerator::onShowPasswordButtonClicked);
    connect(copyToClipBoardButton, &QToolButton::clicked, this, &PasswordGenerator::copyPasswordToClipboard);

    connect(passwordLengthSlider, &QSlider::valueChanged,
            this, [this] (int value) { passwordLengthSpinBox->setValue(value); });
    connect(passwordLengthSpinBox, &QSpinBox::valueChanged,
            this, [this] (int value) { passwordLengthSlider->setValue(value); });

    connect(this, &Window::newAndCopyPasswordSignal,
            this, [this] { generatePassword(); copyPasswordToClipboard(false); });

    connect(this, &Window::saveSettingsSignal, this, [this] {
        userSettings->setValue("Settings/HidePassword", showPasswordButton->isChecked());
        userSettings->setValue("Settings/Length", passwordLengthSpinBox->value());
        userSettings->setValue("Settings/Numeric", numericSwitch->isChecked());
        userSettings->setValue("Settings/Symbols", symbolsSwitch->isChecked());
        userSettings->setValue("Settings/Lowercase", lowercaseSwitch->isChecked());
        userSettings->setValue("Settings/Uppercase", uppercaseSwitch->isChecked());
        userSettings->setValue("Settings/Add", addCharsLineEdit->text());
        userSettings->setValue("Settings/Remove", removeCharLineEdit->text());
    });
}

void PasswordGenerator::generatePassword()
{
    const std::array<bool, 4> selectedCharacters
        {
            numericSwitch->isChecked(),
            symbolsSwitch->isChecked(),
            lowercaseSwitch->isChecked(),
            uppercaseSwitch->isChecked()
        };

    bool hasAnyCharactersSelected
        {
            std::any_of(selectedCharacters.begin(), selectedCharacters.end(),
                        [] (bool valueSwitch) { return valueSwitch; })
        }, hasAdditionalCharactersToInclude{ !addCharsLineEdit->text().isEmpty() };

    if (hasAnyCharactersSelected || hasAdditionalCharactersToInclude) {
        const static std::array<QString, 4> possibleCharacters
            {
                "0123456789",
                "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~",
                "abcdefghijklmnopqrstuvwxyz",
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            };

        QString charactersToUse;
        for(const QString &characters: possibleCharacters)
            charactersToUse.append(characters);

        for(size_t index{0}; index < selectedCharacters.size(); ++index)
            if (!selectedCharacters[index])
                charactersToUse.remove(possibleCharacters[index]);

        for(QChar &c: addCharsLineEdit->text())
            if (!charactersToUse.contains(c))
                charactersToUse.append(c);

        for(QChar &c: removeCharLineEdit->text())
            if (charactersToUse.contains(c))
                charactersToUse.remove(c);

        QString password{ passwordLengthSpinBox->value(), ' ' };

        bool hasCharactersToUse{ !charactersToUse.isEmpty() };
        if (hasCharactersToUse) {
            for(QChar &letter: password)
                letter = charactersToUse[QRandomGenerator::
                                         global()->bounded(0, charactersToUse.size())];
            passwordLineEdit->setText(password);
            return;
        }
    }
    QMessageBox::information(this, QObject::tr("Information"),
                             QObject::tr("No characters have been "
                                         "selected for the password. "
                                         "\nPlease select at least one character."));
}

void PasswordGenerator::onShowPasswordButtonClicked()
{
    if (showPasswordButton->isChecked()) {
        showPasswordButton->setIcon(getCurrentIndexTheme()? VISIBILITYOFF_ICON_LIGHT: VISIBILITYOFF_ICON_DARK);
        passwordLineEdit->setEchoMode(QLineEdit::Password);
        showPasswordButton->setToolTip(QObject::tr("Hide Password"));
    } else {
        showPasswordButton->setIcon(getCurrentIndexTheme()? VISIBILITYON_ICON_LIGHT: VISIBILITYON_ICON_DARK);
        passwordLineEdit->setEchoMode(QLineEdit::Normal);
        showPasswordButton->setToolTip(QObject::tr("Show Password"));
    }
}

void PasswordGenerator::copyPasswordToClipboard(bool isCutAction)
{
    QString password{ passwordLineEdit->text() };
    if (!password.isEmpty()) {
        if (isCutAction)
            passwordLineEdit->clear();
        QApplication::clipboard()->setText(password);
        if (getCurrentIndexTheme()) {
            copyToClipBoardButton->setIcon(DONE_ICON_LIGHT);
            QTimer::singleShot(1000, this,  [this] {
                copyToClipBoardButton->setIcon(COPY_ICON_LIGHT);
            });
        } else {
            copyToClipBoardButton->setIcon(DONE_ICON_DARK);
            QTimer::singleShot(1000, this,  [this] {
                copyToClipBoardButton->setIcon(COPY_ICON_DARK);
            });
        }
    }
}
