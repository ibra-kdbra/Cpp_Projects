#pragma once

#include <QObject>
#include <QString>

namespace Types
{
    enum class File {
        Undef,
        WPD,
        TXT,
        HTML,
        ODF,
        PDF
    };

    enum class Format {
        Undo,
        Redo,
        Cut,
        Copy,
        Paste,
        Clear,
        SelectAll,
        Bold,
        Italic,
        Underline,
        Left,
        Center,
        Right,
        Fill
    };

    enum class Language {
        Dutch,
        English,
        French,
        Italian,
        German,
        Spanish
    };

    enum class Theme {
        Fusion,
        Macos,
        QtCurve,
        Windows
    };
}

Q_DECLARE_METATYPE(Types::File)
Q_DECLARE_METATYPE(Types::Format)
Q_DECLARE_METATYPE(Types::Language)
Q_DECLARE_METATYPE(Types::Theme)

const QString fileToString(const Types::File file);
Types::File stringToFile(const QString &str);

const QString languageToString(const Types::Language lang);
Types::Language stringToLanguage(const QString &str);

const QString themeToString(const Types::Theme theme);
Types::Theme stringToTheme(const QString &str);
