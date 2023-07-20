#pragma once

#include <QTextDocument>
#include <QWidget>
#include "ui_SearchWindow.h"

class SearchWindow : public QWidget, protected Ui::SearchWindow {
    Q_OBJECT
public:
    //--- public constructors ---
    SearchWindow(QWidget *parent = nullptr);
    SearchWindow(const SearchWindow &rhs) = delete;
    SearchWindow(SearchWindow &&rhs) = delete;
    virtual ~SearchWindow();

    //--- public operators ---
    SearchWindow &operator=(const SearchWindow &rhs) = delete;
    SearchWindow &operator=(SearchWindow &&rhs) = delete;

    //--- public methods ---
    void setSearchString(const QString &str);
    void setupActions();

protected:
    //--- protected methods ---
    virtual void changeEvent(QEvent *event) override final;

signals:
    //--- Qt signals ---
    void searchRequest(const QString &str, const QTextDocument::FindFlags flags,
                       const bool use_regexp);
};
