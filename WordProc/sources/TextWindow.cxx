#include <QFile>
#include <QTextStream>
#include "TextWindow.hxx"
#include "MainWindow.hxx"

//--- internal stuff ---

static quint64 __counter = 0;

//--- public constructors ---

TextWindow::TextWindow(QWidget *parent)
: QWidget(parent), Ui::TextWindow(), _filename(""), _search_string(""), _id(__counter++),
  _undo_available(false), _redo_available(false), _copy_available(false), _text_changed(false),
  _filename_valid(false)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setupUi(this);
    setupActions();

    wid_text->setFontFamily(btn_font->currentFont().family());
    wid_text->setFontPointSize(btn_fontsize->value());
    btn_align_left->setChecked(true);
    formatText(Types::Format::Left);
    wid_text->setFocus();

    updateWindowTitle();
}

TextWindow::~TextWindow()
{
}

//--- public methods ---

void TextWindow::formatText(const Types::Format format)
{
    switch (format)
    {
        case Types::Format::Undo:
            wid_text->undo();
            return;

        case Types::Format::Redo:
            wid_text->redo();
            return;

        case Types::Format::Cut:
            wid_text->cut();
            return;

        case Types::Format::Copy:
            wid_text->copy();
            return;

        case Types::Format::Paste:
            wid_text->paste();
            return;

        case Types::Format::Clear:
            wid_text->textCursor().clearSelection();
            return;

        case Types::Format::SelectAll:
            wid_text->selectAll();
            return;

        case Types::Format::Bold:
            return;

        case Types::Format::Italic:
            wid_text->setFontItalic(!wid_text->fontItalic());
            return;

        case Types::Format::Underline:
            wid_text->setFontUnderline(!wid_text->fontUnderline());
            return;

        case Types::Format::Left:
            wid_text->setAlignment(Qt::AlignLeft);
            return;

        case Types::Format::Center:
            wid_text->setAlignment(Qt::AlignCenter);
            return;

        case Types::Format::Right:
            wid_text->setAlignment(Qt::AlignRight);
            return;

        case Types::Format::Fill:
            wid_text->setAlignment(Qt::AlignJustify);
            return;
    }
}

bool TextWindow::undoAvailable() const
{
    return _undo_available;
}

bool TextWindow::redoAvailable() const
{
    return _redo_available;
}

bool TextWindow::copyAvailable() const
{
    return _copy_available;
}

bool TextWindow::pasteAvailable() const
{
    return wid_text->canPaste();
}

bool TextWindow::textChanged() const
{
    return _text_changed;
}

bool TextWindow::filenameValid() const
{
    return _filename_valid;
}

QString TextWindow::filename() const
{
    return _filename;
}

QTextCursor TextWindow::textCursor() const
{
    return wid_text->textCursor();
}

bool TextWindow::loadFile(const QString &filename, const Types::File type)
{
    switch (type)
    {
        case Types::File::WPD:
            return loadWPD(filename);
        case Types::File::TXT:
            return loadTXT(filename);
        case Types::File::HTML:
            return loadHTML(filename);
        default:
            return false;;
    }
}

bool TextWindow::saveFile(const QString &filename, const Types::File type) const
{
    switch (type)
    {
        case Types::File::WPD:
            return saveWPD(filename);
        case Types::File::TXT:
            return saveTXT(filename);
        case Types::File::HTML:
            return saveHTML(filename);
        case Types::File::ODF:
            return saveODF(filename);
        case Types::File::PDF:
            return savePDF(filename);
        case Types::File::Undef:
        default:
            return false;;
    }
}

//--- protected methods ---

void TextWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
        retranslateUi(this);

    QWidget::changeEvent(event);
}

void TextWindow::setupActions()
{
    // toolbar buttons
    connect(btn_font, &QFontComboBox::currentFontChanged, [&](const QFont &font)
    {
        wid_text->setFontFamily(font.family());
        wid_text->setFocus();
    });
    connect(btn_fontsize, QOverload<qint32>::of(&QSpinBox::valueChanged), [&](const qint32 val)
    {
        wid_text->setFontPointSize(val);
        wid_text->setFocus();
    });

    connect(btn_font_italic, &QPushButton::clicked, [&]()
    {
        formatText(Types::Format::Italic);
        wid_text->setFocus();
    });
    connect(btn_font_underline, &QPushButton::clicked, [&]()
    {
        formatText(Types::Format::Underline);
        wid_text->setFocus();
    });

    connect(btn_align_left, &QPushButton::pressed, [&]()
    {
        btn_align_left->setChecked(true);
        btn_align_center->setChecked(false);
        btn_align_right->setChecked(false);
        btn_align_justify->setChecked(false);
        formatText(Types::Format::Left);
        wid_text->setFocus();
    });
    connect(btn_align_center, &QPushButton::pressed, [&]()
    {
        btn_align_left->setChecked(false);
        btn_align_center->setChecked(true);
        btn_align_right->setChecked(false);
        btn_align_justify->setChecked(false);
        formatText(Types::Format::Center);
        wid_text->setFocus();
    });
    connect(btn_align_right, &QPushButton::pressed, [&]()
    {
        btn_align_left->setChecked(false);
        btn_align_center->setChecked(false);
        btn_align_right->setChecked(true);
        btn_align_justify->setChecked(false);
        formatText(Types::Format::Right);
        wid_text->setFocus();
    });
    connect(btn_align_justify, &QPushButton::pressed, [&]()
    {
        btn_align_left->setChecked(false);
        btn_align_center->setChecked(false);
        btn_align_right->setChecked(false);
        btn_align_justify->setChecked(true);
        formatText(Types::Format::Fill);
        wid_text->setFocus();
    });

    // text area
    connect(wid_text, &QTextEdit::cursorPositionChanged, [&]()
    {
        btn_font->setCurrentFont(wid_text->currentFont());
        btn_fontsize->setValue(wid_text->fontPointSize());

        btn_font_italic->setChecked(wid_text->fontItalic());
        btn_font_underline->setChecked(wid_text->fontUnderline());
    });
    connect(wid_text, &QTextEdit::undoAvailable, [&](const bool avail){ _undo_available = avail; });
    connect(wid_text, &QTextEdit::redoAvailable, [&](const bool avail){ _redo_available = avail; });
    connect(wid_text, &QTextEdit::copyAvailable, [&](const bool avail){ _copy_available = avail; });
    connect(wid_text, &QTextEdit::textChanged, [&]()
    {
        _text_changed = true;
        updateWindowTitle();
    });
}

void TextWindow::updateWindowTitle()
{
    setWindowTitle("(" + QString::number(_id) + ") " + (_filename.size() ? _filename :
                   tr("I18N_UNKNOWN_FILE")) + (_text_changed ? "*" : ""));
}

void TextWindow::setIcons(const QString &prefix)
{
    // toolbar
    btn_font_bold->setIcon(QIcon(prefix + "Format/TextBold"));
    btn_font_italic->setIcon(QIcon(prefix + "Format/TextItalic"));
    btn_font_underline->setIcon(QIcon(prefix + "Format/TextUnderline"));
    btn_align_left->setIcon(QIcon(prefix + "Format/AlignLeft"));
    btn_align_center->setIcon(QIcon(prefix + "Format/AlignCenter"));
    btn_align_right->setIcon(QIcon(prefix + "Format/AlignRight"));
    btn_align_justify->setIcon(QIcon(prefix + "Format/AlignFill"));
}

bool TextWindow::loadWPD(const QString &filename)
{
    return false;
}

bool TextWindow::loadTXT(const QString &filename)
{
    if (QFile ifile(filename); ifile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&ifile);

        wid_text->setPlainText(in.readAll());
        ifile.close();

        _filename = filename;
        _text_changed = false;
        updateWindowTitle();

        return true;
    }

    return false;
}

bool TextWindow::loadHTML(const QString &filename)
{
    if (QFile ifile(filename); ifile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&ifile);

        wid_text->setHtml(in.readAll());
        ifile.close();

        _filename = filename;
        _text_changed = false;
        updateWindowTitle();

        return true;
    }

    return false;
}

bool TextWindow::saveWPD(const QString &filename) const
{
    return false;
}

bool TextWindow::saveTXT(const QString &filename) const
{
    if (QFile ofile(filename); ofile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&ofile);

        out << wid_text->toPlainText();
        ofile.close();

        return true;
    }

    return false;
}

bool TextWindow::saveHTML(const QString &filename) const
{
    if (QFile ofile(filename); ofile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&ofile);

        out << wid_text->toHtml();
        ofile.close();

        return true;
    }

    return false;
}

bool TextWindow::saveODF(const QString &filename) const
{
    return false;
}

bool TextWindow::savePDF(const QString &filename) const
{
    return false;
}
