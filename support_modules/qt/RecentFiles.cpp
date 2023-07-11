/*
    RecentFiles - Show recently used files on a QMenu.
*/


#include <QMenu>
#include <QFileInfo>
#include "RecentFiles.h"


void RecentFiles::install( QMenu* menu, QObject* receiver, const char* method )
{
    menu->setToolTipsVisible(true);     // Full paths are shown in tips.

    _separator = menu->addSeparator();
    int i;
    for( i = 0; i < MaxRecentFiles; ++i )
    {
        _action[i] = new QAction( receiver );
        _action[i]->setVisible( false );
        QObject::connect( _action[i], SIGNAL(triggered()), receiver, method );
        menu->addAction( _action[i] );
    }
}


void RecentFiles::setFiles( const QStringList& list )
{
    files = list;

    QStringList::iterator it;
    for( it = files.begin(); it != files.end(); ++it )
    {
        if( (*it).isEmpty() )
            files.erase( it );
    }

    addFile(NULL);
}


/**
  If str is NULL then the menu is rebuilt from the current list.
*/
void RecentFiles::addFile( const QString* str )
{
    if( str )
    {
        int pos = files.indexOf( *str );
        if( pos > -1 )
        {
            if( pos == 0 )
                return;
            files.removeAt( pos );
        }
        else
        {
            while( files.size() >= MaxRecentFiles )
                files.pop_back();
        }
        files.push_front( *str );
    }

    int i;
    int used = qMin( files.size(), (int) MaxRecentFiles );
    for( i = 0; i < used; ++i )
    {
        QString text = QString( "&%1 %2" )
                       .arg( i + 1 )
                       .arg( QFileInfo(files[i]).fileName() );

        _action[i]->setText( text );
        _action[i]->setToolTip( files[i] );
        _action[i]->setData( i );
        _action[i]->setVisible( true );
    }
    for( ; i < MaxRecentFiles; ++i )
        _action[i]->setVisible( false );

    _separator->setVisible( used > 0 );
}


QString RecentFiles::fileOpened( QObject* sender )
{
    QAction* action = qobject_cast<QAction*>(sender);
    if( action )
    {
        int id = action->data().toInt();
        if( id < files.size() )
            return files[ id ];
    }
    return QString();
}


//EOF
