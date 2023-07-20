#include <QRegExp>
#include "Types.hxx"

static const QString WPDFile(".*\\.([Ww][Pp][Dd])");
static const QString TXTFile(".*\\.([Tt][Ee]?[Xx][Tt]|[Aa][Ss][Cc][Ii]{0,2}|[Mm][Dd]|[Dd][Oo][Cc])");
static const QString HTMLFile(".*\\.([Hh][Tt]?[Mm][Ll]?)");
static const QString ODFFile(".*\\.([Oo][Dd][Ff])");
static const QString PDFFile(".*\\.([Pp][Dd][Ff])");

const QString fileToString(const Types::File file)
{
    switch (file)
    {
        case Types::File::WPD:
            return WPDFile;
        case Types::File::TXT:
            return TXTFile;
        case Types::File::HTML:
            return HTMLFile;
        case Types::File::ODF:
            return ODFFile;
        case Types::File::PDF:
            return PDFFile;
        case Types::File::Undef:
        default:
            return "";
    }
}

Types::File stringToFile(const QString &str)
{
    if (QRegExp regexp(WPDFile); regexp.exactMatch(str))
        return Types::File::WPD;

    if (QRegExp regexp(TXTFile); regexp.exactMatch(str))
        return Types::File::TXT;

    if (QRegExp regexp(HTMLFile); regexp.exactMatch(str))
        return Types::File::HTML;

    if (QRegExp regexp(ODFFile); regexp.exactMatch(str))
        return Types::File::ODF;

    if (QRegExp regexp(PDFFile); regexp.exactMatch(str))
        return Types::File::PDF;

    return Types::File::Undef;
}

const QString languageToString(const Types::Language lang)
{
    switch (lang)
    {
        case Types::Language::Dutch:
            return "Dutch";

        case Types::Language::French:
            return "French";

        case Types::Language::Italian:
            return "Italian";

        case Types::Language::German:
            return "German";

        case Types::Language::Spanish:
            return "Spanish";

        case Types::Language::English:
        default:
            return "English";
    }
}

Types::Language stringToLanguage(const QString &str)
{
    if (str == "Dutch")
        return Types::Language::Dutch;

    if (str == "French")
        return Types::Language::French;

    if (str == "Italian")
        return Types::Language::Italian;

    if (str == "German")
        return Types::Language::German;

    if (str == "Spanish")
        return Types::Language::Spanish;

    return Types::Language::English;
}

const QString themeToString(const Types::Theme theme)
{
    switch (theme)
    {
        case Types::Theme::Macos:
            return "Macos";

        case Types::Theme::QtCurve:
            return "QtCurve";

        case Types::Theme::Windows:
            return "Windows";

        case Types::Theme::Fusion:
        default:
            return "Fusion";
    }
}

Types::Theme stringToTheme(const QString &str)
{
    if (str == "Macos")
        return Types::Theme::Macos;

    if (str == "QtCurve")
        return Types::Theme::QtCurve;

    if (str == "Windows")
        return Types::Theme::Windows;

    return Types::Theme::Fusion;
}
