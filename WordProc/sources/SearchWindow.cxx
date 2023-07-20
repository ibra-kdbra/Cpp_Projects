#include "SearchWindow.hxx"

//--- public constructors ---

SearchWindow::SearchWindow(QWidget *parent)
: QWidget(parent), Ui::SearchWindow()
{
    setupUi(this);
    setupActions();
    lay_main->setSizeConstraint(QLayout::SetFixedSize);
}

SearchWindow::~SearchWindow()
{
}

//--- protected methods ---

void SearchWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
        retranslateUi(this);

    QWidget::changeEvent(event);
}

void SearchWindow::setupActions()
{
    connect(btn_close, &QPushButton::clicked, this, &SearchWindow::close);
    connect(btn_prev, &QPushButton::clicked, [&]()
    {
        QTextDocument::FindFlags flags = QTextDocument::FindBackward;

        if (chk_case_sensitive->isChecked())
            flags |= QTextDocument::FindCaseSensitively;
        if (chk_whole_words->isChecked())
            flags |= QTextDocument::FindWholeWords;

        emit searchRequest(wid_lineedit->text(), flags, false);
    });
    connect(btn_next, &QPushButton::clicked, [&]()
    {
        QTextDocument::FindFlags flags;

        if (chk_case_sensitive->isChecked())
            flags |= QTextDocument::FindCaseSensitively;
        if (chk_whole_words->isChecked())
            flags |= QTextDocument::FindWholeWords;

        emit searchRequest(wid_lineedit->text(), flags, false);
    });
}

//--- public methods ---

void SearchWindow::setSearchString(const QString &str)
{
    wid_lineedit->setText(str);
}
