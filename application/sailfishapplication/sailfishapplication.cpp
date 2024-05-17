
#include <QGuiApplication>
#include <QDir>

#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickView>

#ifdef HAS_BOOSTER
#include <MDeclarativeCache>
#endif

#include "sailfishapplication.h"

QGuiApplication *Sailfish::createApplication(int &argc, char **argv)
{
#ifdef HAS_BOOSTER
    return MDeclarativeCache::qApplication(argc, argv);
#else
    return new QGuiApplication(argc, argv);
#endif
}

QQuickView *Sailfish::createView(const QString &file)
{
    QQuickView *view;
#ifdef HAS_BOOSTER
    view = MDeclarativeCache::qQuickView();
#else
    view = new QQuickView;
#endif

    bool isDesktop = qApp->arguments().contains("-desktop");

    QString path;
    if (isDesktop) {
        path = qApp->applicationDirPath() + QDir::separator();
    } else {
        path = QString(DEPLOYMENT_PATH);
    }
    view->setSource(QUrl::fromLocalFile(path + file));

    return view;
}

void Sailfish::showView(QQuickView* view) {
    view->setResizeMode(QQuickView::SizeRootObjectToView);

    bool isDesktop = qApp->arguments().contains("-desktop");

    if (isDesktop) {
        view->show();
    } else {
        view->showFullScreen();
    }
}

