#include "mediaplayer.h"
#include "ui_mediaplayer.h"

MediaPlayer::MediaPlayer(QWidget *parent): QMainWindow(parent), ui(new Ui::MediaPlayer)
{
    ui->setupUi(this);

    mv = new QMovie(":/MediaPlayer/rotate2.webp");

        this->ToolBar = addToolBar(tr("Operacje"));
        QPixmap openVideoIcon(":/MediaPlayer/open-folder.png");
        QAction openVideothis(openVideoIcon, tr("&Otworz"), this);
        ToolBar->addAction(openVideoIcon, "", this, SLOT(openFileVideo()));

        QPixmap playIcon(":/MediaPlayer/play.png");
        QAction playthis(playIcon, tr("&Odwtorz"), this);
        ToolBar->addAction(playIcon, "", this, SLOT(playVideo()));

        QPixmap pauseIcon(":/MediaPlayer/pause.png");
        QAction pausethis(pauseIcon, tr("&Zatrzymaj"), this);
        ToolBar->addAction(pauseIcon, "", this, SLOT(pauseVideo()));

        QPixmap stopIcon(":/MediaPlayer/stop.png");
        QAction stopThis(stopIcon, tr("&Stop"), this);
        ToolBar->addAction(stopIcon, "", this, SLOT(stopVideo()));

        QPixmap fullscreenIcon(":/MediaPlayer/fullscreen.png");
        QAction fullscreenThis(fullscreenIcon, tr("&FullScreen"), this);
        ToolBar->addAction(fullscreenIcon, "", this, SLOT(fullScreenWindow()));

        ToolBar->setMovable(false);
        ToolBar->setContextMenuPolicy(Qt::PreventContextMenu);

        player = new QMediaPlayer(this);
        vw = new QVideoWidget(this);
        player->setVideoOutput(vw);
        vw->hide();

        setStyleSheet("QVideoWidget {background: 'black';}");

        sliderBar = new MySlider(this);
        volumeSlider = new MySlider(this);
        timeDuration = new QLabel(this);
        volumeSpeak = new ClickableLabel(this);

        vinylMusic = new QLabel(this);
        vinylMusic->setStyleSheet("QLabel {background: 'black';}");
        vinylMusic->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        vinylMusic->setPixmap(QPixmap(":/MediaPlayer/vinyl.png"));
        setCentralWidget(vinylMusic);

        rateBox = new QComboBox(this);
        rateBox->setStyleSheet("padding: '2'");
        rateBox->addItem("0.25x", QVariant(0.25));
        rateBox->addItem("0.5x", QVariant(0.5));
        rateBox->addItem("1.0x", QVariant(1.0));
        rateBox->addItem("1.5x", QVariant(1.5));
        rateBox->addItem("2.0x", QVariant(2.0));
        rateBox->setCurrentIndex(2);

        connect(rateBox, QOverload<int>::of(&QComboBox::activated), this, &MediaPlayer::updateRate);

        QPixmap pix(":/MediaPlayer/sound_speaker.png");
        volumeSpeak->setPixmap(pix);
        timeDuration->setText("00:00:00 / 00:00:00");
        sliderBar->setOrientation(Qt::Horizontal);
        sliderBar->setTracking(true);
        volumeSlider->setOrientation(Qt::Horizontal);
        volumeSlider->setRange(0, 100);
        volumeSlider->setFixedWidth(100);
        volumeSlider->setValue(100);

        statusBar()->addWidget(timeDuration);
        statusBar()->addWidget(sliderBar);

        statusBar()->addPermanentWidget(rateBox);
        statusBar()->addPermanentWidget(volumeSlider);
        statusBar()->addPermanentWidget(volumeSpeak);

        createTaskbar();

        connect(player, &QMediaPlayer::durationChanged, sliderBar, &MySlider::setMaximum);
        connect(player, &QMediaPlayer::positionChanged, sliderBar, &MySlider::setValue);
        connect(player, &QMediaPlayer::positionChanged, this, &MediaPlayer::positionChanged);
        connect(sliderBar, &MySlider::onClick, player, &QMediaPlayer::setPosition);
        connect(sliderBar, &MySlider::sliderMoved, player, &QMediaPlayer::setPosition);
        connect(this, &MediaPlayer::changeRate, player, &QMediaPlayer::setPlaybackRate);
        connect(volumeSlider, SIGNAL(valueChanged(int)), player, SLOT(setVolume(int)));
        connect(volumeSpeak, SIGNAL(clicked()), this, SLOT(muteThis()));
        setAcceptDrops(true);
}

void MediaPlayer::createTaskbar()
{
    buttonFile = new QWinTaskbarButton(this);
    buttonFile->setWindow(windowHandle());

    progressFile = buttonFile->progress();
    progressFile->show();
    connect(sliderBar, &QSlider::valueChanged, progressFile, &QWinTaskbarProgress::setValue);
    connect(sliderBar, &QSlider::rangeChanged, progressFile, &QWinTaskbarProgress::setRange);

    connect(player, &QMediaPlayer::stateChanged, this, &MediaPlayer::updateTaskbar);
}

void MediaPlayer::openFileVideo()
{
    QFileInfo getFileName(filename = QFileDialog::getOpenFileName(this, "Otworz plik...", "", "Pliki multimedialne (*.mkv; *.mp4; *.webm; *.avi; *.vob; *.mp2; *.mp3; *.aac;)"));
    nameInfo = getFileName.fileName();
    connect(player, SIGNAL(positionChanged(qint64)), SLOT(positionChanged(qint64)));
    connect(player, &QMediaPlayer::mediaStatusChanged, this, [=]() { checkFile(); });

    if (filename != NULL)
    {
        changeWindowTitle();
        player->setMedia(QUrl::fromLocalFile(filename));

        if(isFullScreen())
            playVideo();
        else
        {
            this->resize(1300,750);
            playVideo();
        }
    }
}

void MediaPlayer::playVideo()
{
    player->play();
    mv->setPaused(false);
}

void MediaPlayer::pauseVideo()
{
    player->pause();
    mv->setPaused(true);
}

void MediaPlayer::stopVideo()
{
    player->stop();
    mv->setPaused(true);
}

void MediaPlayer::fullScreenWindow()
{
    if (!isFullScreen())
        showFullScreen();
    else
    {
        showNormal();
        ToolBar->setVisible(true);
        statusBar()->setVisible(true);
    }
}

void MediaPlayer::positionChanged(qint64 progress)
{
    updateDurationInfo(progress / 1000);
}

void MediaPlayer::updateDurationInfo(qint64 currentInfo)
{
    qint64 duration = player->duration();
    if (currentInfo || duration)
    {
        QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60, currentInfo % 60, (currentInfo * 1000) % 1000);
        int seconds = (duration / 1000) % 60;
        int minutes = (duration / 60000) % 60;
        int hours = (duration / 3600000) % 24;
        QTime totalTime(hours, minutes, seconds);
        QString format = "mm:ss";
        if (duration > 3600)
            format = "hh:mm:ss";
        time = currentTime.toString(format) + " / " + totalTime.toString(format);

        if (isFullScreen())
        {
            if (currentTime == totalTime)
            {
                stopVideo();
                filename == NULL;
                showNormal();
                ToolBar->setVisible(true);
                statusBar()->setVisible(true);
            }
        }
    }
    timeDuration->setText(time);
}

void MediaPlayer::keyPressEvent(QKeyEvent* event)
{
    if (isFullScreen()) {
        if (this->isPlaying == true)
        {
            if (event->key() == Qt::Key_Space)
            {
                player->pause();
                this->isPlaying = false;
            }
        }
        else
        {
            player->play();
            this->isPlaying = true;
        }
    }


    if (!isFullScreen()) {
        if (this->isPlaying == true)
        {
            if (event->key() == Qt::Key_Space)
            {
                player->pause();
                this->isPlaying = false;
            }
        }
        else
        {
            player->play();
            this->isPlaying = true;
        }
    }

    if (event->key() == Qt::Key_Escape)
    {
        showNormal();
        ToolBar->setVisible(true);
        statusBar()->setVisible(true);
    }
}

void MediaPlayer::mouseMoveEvent(QMouseEvent* event)
{
    if (!isFullScreen())
        vw->setCursor(Qt::ArrowCursor);
    else
    {
        vw->setMouseTracking(true);
        vinylMusic->setMouseTracking(true);
        setMouseTracking(true);

        int programWight = this->width();
        int programHeight = this->height();

        int programHeightUp = 40;
        int mouseDock = 90;

        QPoint currentPosition = mapFromGlobal(QCursor::pos());

        if (currentPosition.y() < programHeight - 30)
            statusBar()->setVisible(false);
        else
            statusBar()->setVisible(true);

        if(currentPosition.y() > programHeightUp)
            ToolBar->setVisible(false);
        else
            ToolBar->setVisible(true);

        if (currentPosition.x() > programWight - mouseDock)
        {
            vw->setCursor(Qt::BlankCursor);
            vinylMusic->setCursor(Qt::BlankCursor);
        }
        else
        {
            vw->setCursor(Qt::ArrowCursor);
            vinylMusic->setCursor(Qt::ArrowCursor);
        }

    }
}

void MediaPlayer::mousePressEvent(QMouseEvent* event)
{
    if (event->buttons() == Qt::RightButton)
    {
        pauseVideo();
    }
    else
    {
        playVideo();
    }
}

void MediaPlayer::updateTaskbar()
{
    switch (player->state())
    {
        case QMediaPlayer::PlayingState:
            buttonFile->setOverlayIcon(QIcon(":/MediaPlayer/playVideo.png"));
            progressFile->show();
            progressFile->resume();
            break;
        case QMediaPlayer::PausedState:
            buttonFile->setOverlayIcon(QIcon(":/MediaPlayer/pauseVideo.png"));
            progressFile->show();
            progressFile->pause();
            break;
        case QMediaPlayer::StoppedState:
            buttonFile->setOverlayIcon(QIcon(":/MediaPlayer/stopVideo.png"));
            progressFile->hide();
            break;
    }
}

void MediaPlayer::changeWindowTitle()
{
    setWindowTitle("LitePlayer -> " + nameInfo);
}

void MediaPlayer::defaultWindowTitle()
{
    setWindowTitle("LitePlayer");
}

void MediaPlayer::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MediaPlayer::dropEvent(QDropEvent* event)
{
    foreach(const QUrl & url, event->mimeData()->urls())
    {
        QFileInfo getFileName(dropFileName = url.toLocalFile());
        dropNameInfo = getFileName.fileName();
        player->setMedia(QUrl::fromLocalFile(dropFileName));
        connect(player, SIGNAL(MediaPlayer::positionChanged(qint64)), SLOT(MediaPlayer::positionChanged(qint64)));
        connect(player, &QMediaPlayer::mediaStatusChanged, this, [=]() { checkFile(); });
        if (dropFileName == NULL)
            defaultWindowTitle();
        else
        {
            changeWindowTitleDragAndDrop();
            player->setMedia(QUrl::fromLocalFile(dropFileName));

            if(isFullScreen())
                playVideo();
            else
            {
                this->resize(1300,750);
                playVideo();
            }
        }
    }
}

void MediaPlayer::changeWindowTitleDragAndDrop()
{
    setWindowTitle("LitePlayer -> " + dropNameInfo);
}

void MediaPlayer::createThumbnailToolBar()
{
    thumbnailToolBar = new QWinThumbnailToolBar(this);
    thumbnailToolBar->setWindow(this->windowHandle());

    playToolButton = new QWinThumbnailToolButton(thumbnailToolBar);
    playToolButton->setEnabled(true);
    playToolButton->setToolTip(tr("Odtworz"));
    playToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(playToolButton, &QWinThumbnailToolButton::clicked, this, &MediaPlayer::togglePlayback);

    backwardToolButton = new QWinThumbnailToolButton(thumbnailToolBar);
    backwardToolButton->setEnabled(true);
    backwardToolButton->setToolTip(tr("Stop"));
    backwardToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    connect(backwardToolButton, &QWinThumbnailToolButton::clicked, this, &MediaPlayer::stopPlayback);

    thumbnailToolBar->addButton(playToolButton);
    thumbnailToolBar->addButton(backwardToolButton);

    connect(player, &QMediaPlayer::positionChanged, this, &MediaPlayer::updateThumbnailToolBar);
    connect(player, &QMediaPlayer::durationChanged, this, &MediaPlayer::updateThumbnailToolBar);
    connect(player, &QMediaPlayer::stateChanged, this, &MediaPlayer::updateThumbnailToolBar);
}

void MediaPlayer::updateThumbnailToolBar()
{
    if (player->state() == QMediaPlayer::PlayingState)
    {
        playToolButton->setToolTip(tr("Pauza"));
        playToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    }
    else
    {
        playToolButton->setToolTip(tr("Odtworz"));
        playToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }
}

void MediaPlayer::togglePlayback()
{
    if (player->mediaStatus() == QMediaPlayer::NoMedia)
        openFileVideo();
    else if (player->state() == QMediaPlayer::PlayingState)
    {
        pauseVideo();
    }
    else
    {
        playVideo();
    }

}

void MediaPlayer::stopPlayback()
{
    stopVideo();
}

void MediaPlayer::updateRate()
{
    emit changeRate(playbackRate());
}

qreal MediaPlayer::playbackRate() const
{
    return rateBox->itemData(rateBox->currentIndex()).toDouble();
}

void MediaPlayer::setPlaybackRate(float rate)
{
    for (int i = 0; i < rateBox->count(); ++i)
    {
        if (qFuzzyCompare(rate, float(rateBox->itemData(i).toDouble())))
        {
            rateBox->setCurrentIndex(i);
            return;
        }
    }
}

void MediaPlayer::checkFile()
{
    if (player->isVideoAvailable())
    {
        vinylMusic->close();
        vinylMusic->setParent(0);
        setCentralWidget(vw);
        vw->show();
    }
    else
    {
        vw->close();
        vw->setParent(0);
        setCentralWidget(vinylMusic);
        mv->start();
        vinylMusic->setMovie(mv);
        vinylMusic->show();
    }


    switch (player->state())
    {
    case QMediaPlayer::PlayingState:
        if (player->isVideoAvailable())
            mv->setPaused(true);
        else
            mv->setPaused(false);
        break;
    case QMediaPlayer::PausedState:
        if (player->isVideoAvailable())
            mv->setPaused(true);
        else
            mv->setPaused(true);
        break;
    case QMediaPlayer::StoppedState:
        if (player->isVideoAvailable())
            mv->setPaused(true);
        else
            mv->setPaused(true);
        break;
    }
}

void MediaPlayer::muteThis()
{
    if (clicked) {
        clicked = false;
        player->setMuted(false);
        QPixmap unmuted(":/MediaPlayer/sound_speaker.png");
        volumeSpeak->setPixmap(QPixmap(unmuted));

    }
    else
    {
        clicked = true;
        player->setMuted(true);
        QPixmap muted(":/MediaPlayer/sound_speaker_muted.png");
        volumeSpeak->setPixmap(QPixmap(muted));
    }
}

void MediaPlayer::onStart()
{
    connect(player, &QMediaPlayer::mediaStatusChanged, this, [=]() { checkFile(); });
}

MediaPlayer::~MediaPlayer()
{
    delete ui;
}
