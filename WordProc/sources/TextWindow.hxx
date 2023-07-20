#pragma once

#include <QWidget>
#include "ui_TextWindow.h"
#include "Types.hxx"

class MainWindow;

class TextWindow : public QWidget, public Ui::TextWindow {
    Q_OBJECT
    friend class MainWindow;
public:
    //--- public constructors ---
    TextWindow(QWidget *parent = nullptr);
    TextWindow(const TextWindow &rhs) = delete;
    TextWindow(TextWindow &&rhs) = delete;
    virtual ~TextWindow();

    //--- public operators ---
    TextWindow &operator=(const TextWindow &rhs) = delete;
    TextWindow &operator=(TextWindow &&rhs) = delete;

    //--- public methods ---
    void formatText(const Types::Format format);
    bool undoAvailable() const;
    bool redoAvailable() const;
    bool copyAvailable() const;
    bool pasteAvailable() const;
    bool textChanged() const;
    bool filenameValid() const;
    QString filename() const;
    QTextCursor textCursor() const;
    bool loadFile(const QString &filename, const Types::File type = Types::File::Undef);
    bool saveFile(const QString &filename, const Types::File type = Types::File::Undef) const;

protected:
    //--- protected methods ---
    virtual void changeEvent(QEvent *event) override final;
    void setupActions();
    void updateWindowTitle();
    void setIcons(const QString &prefix);
    bool loadWPD(const QString &filename);
    bool loadTXT(const QString &filename);
    bool loadHTML(const QString &filename);
    bool saveWPD(const QString &filename) const;
    bool saveTXT(const QString &filename) const;
    bool saveHTML(const QString &filename) const;
    bool saveODF(const QString &filename) const;
    bool savePDF(const QString &filename) const;

private:
    //--- private properties ---
    QString _filename;
    QString _search_string;
    quint64 _id;
    bool _undo_available;
    bool _redo_available;
    bool _copy_available;
    bool _text_changed;
    bool _filename_valid;
};
