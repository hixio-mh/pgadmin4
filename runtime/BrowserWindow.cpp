//////////////////////////////////////////////////////////////////////////
//
// pgAdmin 4 - PostgreSQL Tools
//
// Copyright (C) 2013, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// BrowserWindow.cpp - Implementation of the main window class
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin4.h"

// QT headers
#include <QtGlobal>

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#include <QtWebKitWidgets>
#else
#include <QtWebKit>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>
#endif

// App headers
#include "BrowserWindow.h"


// Constructor
BrowserWindow::BrowserWindow()
{
    createActions();
    createMenus();

    // Create the WebKit control
    webView = new QWebView(this);
    setCentralWidget(webView);
    connect(webView, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));

    // Display the app
    m_initialload = true;
    m_loadattempt = 1;
    webView->setUrl(PGA_SERVER_URL);
}


// Create the actions for the window
void BrowserWindow::createActions()
{
    // Open an arbitrary URL
    openUrlAction = new QAction(tr("&Open URL..."), this);
    openUrlAction->setShortcut(tr("Ctrl+U"));
    openUrlAction->setStatusTip(tr("Open a URL"));
    connect(openUrlAction, SIGNAL(triggered()), this, SLOT(openUrl()));

    // Exit the app
    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setStatusTip(tr("Exit the application"));
    exitAction->setShortcuts(QKeySequence::Quit);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    // About box
    aboutAction = new QAction(tr("&About"), this);
    aboutAction->setStatusTip(tr("Show the application's About box"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
}


// Create the application menus
void BrowserWindow::createMenus()
{
    // File
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openUrlAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    menuBar()->addSeparator();

    // Help
    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
}


// Process loading finished signals from the web view.
void BrowserWindow::finishLoading(bool ok)
{
    if (m_initialload && !ok)
    {
        // The load attempt failed. Try again up to 4 times with an
        // incremental backoff.
        if (m_loadattempt < 5)
        {
            qDebug() << "Initial load failed. Retrying in" << m_loadattempt << "seconds.";
            webView->setHtml(QString(tr("<p>Failed to connect to the pgAdmin application server. Retrying in %1 seconds, ") +
                                     tr("or click <a href=\"%2\">here</a> to try again now.</p>")).arg(m_loadattempt).arg(PGA_SERVER_URL));

            pause(m_loadattempt);
            webView->setUrl(PGA_SERVER_URL);
            m_loadattempt++;
            return;
        }
        else
        {
            qDebug() << "Initial page load failed after multiple attempts. Aborting.";
            webView->setHtml(QString(tr("<p>Failed to connect to the pgAdmin application server. ") +
                                     tr("Click <a href=\"%1\">here</a> to try again.</p>")).arg(PGA_SERVER_URL));
        }
    }

    m_initialload = false;
}


// Pause for n seconds, without freezing the UI.
void BrowserWindow::pause(int seconds)
{
    QTime dieTime = QTime::currentTime().addSecs(seconds);

    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);    
}


// Display the about box
void BrowserWindow::about()
{
    QMessageBox::about(this, tr("About pgAdmin 4"), tr("pgAdmin 4 - PostgreSQL Tools"));
}


// Open an arbitrary URL
void BrowserWindow::openUrl()
{
    bool ok;
    QString url = QInputDialog::getText(this, tr("Enter a URL"), tr("URL:"), QLineEdit::Normal, "http://", &ok);

    if (ok && !url.isEmpty())
        webView->setUrl(url);
}

