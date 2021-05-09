#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QMovie>
#include <QFileDialog>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QToolBar>
#include <QDragEnterEvent>
#include <QStyle>
#include <QStatusBar>
#include <QDropEvent>
#include <QMimeData>
#include <QAction>
#include <QMessageBox>
#include <QTime>
#include <QWinTaskbarProgress>
#include <QWinTaskbarButton>
#include <QWinThumbnailToolBar>
#include <QWinThumbnailToolButton>
#include <QDesktopServices>
#include <QComboBox>
#include "myslider.h"
#include "clickablelabel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MediaPlayer; }
QT_END_NAMESPACE

class MediaPlayer : public QMainWindow
{
    Q_OBJECT

public:
    MediaPlayer(QWidget *parent = nullptr);
    ~MediaPlayer();
    QMediaPlayer* player;
    QVideoWidget* vw;
    void createThumbnailToolBar();
    void changeWindowTitle();
    void updateDurationInfo(qint64 currentInfo);
    void positionChanged(qint64 progress);
    qreal playbackRate() const;

private:
    Ui::MediaPlayer *ui;

    MySlider* sliderBar;
    MySlider* volumeSlider;
    QLabel* timeDuration;
    ClickableLabel* volumeSpeak;
    QLabel* vinylMusic;
    QToolBar* ToolBar;
    QWinTaskbarProgress* progressFile;
    QWinTaskbarButton* buttonFile;
    QWinThumbnailToolBar* thumbnailToolBar;
    QWinThumbnailToolButton* playToolButton;
    QWinThumbnailToolButton* backwardToolButton;
    QComboBox* rateBox;
    QMovie* mv;

    QString filename;
    QString time;
    QString nameInfo;
    QString dropFileName;
    QString dropNameInfo;

    bool isPlaying = false;
    bool clicked = false;

protected:
    void mouseDoubleClickEvent(QMouseEvent* event)
    {
        if (!isFullScreen())
        {
            ToolBar->setVisible(false);
            statusBar()->setVisible(false);
            showFullScreen();
        }
        else
        {
            ToolBar->setVisible(true);
            statusBar()->setVisible(true);
            showNormal();
        }
        event->accept();
    }

    void keyPressEvent(QKeyEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mousePressEvent(QMouseEvent*);
    void dragEnterEvent(QDragEnterEvent*);
    void dropEvent(QDropEvent*);

public slots:
    void playVideo();
    void setPlaybackRate(float rate);
    void checkFile();
    void onStart();

private slots:
    void openFileVideo();
    void fullScreenWindow();
    void pauseVideo();
    void stopVideo();
    void updateTaskbar();
    void defaultWindowTitle();
    void changeWindowTitleDragAndDrop();
    void updateThumbnailToolBar();
    void togglePlayback();
    void stopPlayback();
    void createTaskbar();
    void updateRate();
    void muteThis();

signals:
    void changeRate(qreal rate);

};
#endif // MEDIAPLAYER_H
