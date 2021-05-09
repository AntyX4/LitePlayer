#include "mediaplayer.h"

#include <QApplication>
#include <QIcon>
#include <QStyleFactory>
#include <QFileInfo>

QString FILENAME;
QString nameInfoQuick;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MediaPlayer w;
    const QIcon logo(":/MediaPlayer/138701-200.png");
        w.setWindowIcon(logo);
        QApplication::setStyle(QStyleFactory::create("Fusion"));
        if (QApplication::arguments().size() > 1) {
            QFileInfo getFileName(FILENAME = QApplication::arguments().at(1));
            nameInfoQuick = getFileName.fileName();
            w.setWindowTitle("LitePlayer -> " + nameInfoQuick);
            w.player->setMedia(QUrl::fromLocalFile(FILENAME));
            w.onStart();
            if(w.isFullScreen())
                w.playVideo();
            else
            {
                w.resize(1300,750);
                w.playVideo();
            }
            w.playVideo();
        }
    w.show();
    w.createThumbnailToolBar();
    return a.exec();
}
