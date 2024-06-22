#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("AudioBrowser");

    for(int i = 0; i < 4; i++)
    {
        m_tracksSet[i] = false;
        m_channels[i] = 0;
    }
    m_sounds.reserve(4);

    connect(ui->playButton, SIGNAL(clicked(bool)), this, SLOT(playSound()));
    connect(ui->stopButton, SIGNAL(clicked(bool)), this, SLOT(stopSound()));
    connect(ui->track1Button, &QPushButton::clicked, this, [this](){ mountTrack(0); });
    connect(ui->track2Button, &QPushButton::clicked, this, [this](){ mountTrack(1); });
    connect(ui->track3Button, &QPushButton::clicked, this, [this](){ mountTrack(2); });
    connect(ui->track4Button, &QPushButton::clicked, this, [this](){ mountTrack(3); });

    m_dirModel = new QFileSystemModel(this);
    m_fileModel = new QFileSystemModel(this);
    setupFileSystemModel();

    m_audioFormat.frequency = 44100;
    m_audioFormat.format = AUDIO_S16SYS;
    m_audioFormat.channels = 2;
    m_audioFormat.buffer = 2048;
    setupSDL2();
}

MainWindow::~MainWindow()
{
    if( m_previewSound )
    {
        Mix_FreeChunk( m_previewSound );
    }
    for(int i = 0; i < 4; i++)
    {
        Mix_FreeChunk( m_sounds[i] );
    }
    Mix_CloseAudio();
    SDL_Quit();
    delete m_dirModel;
    delete m_fileModel;
    delete ui;
}

void MainWindow::setupFileSystemModel()
{
    /* Drive path */
    QString rootPath("/");
    m_dirModel->setRootPath(rootPath);

    /* Only folders filter */
    m_dirModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);

    /* Default path to be seen in the dirView */
    QString indexPath("/home/marceltracz/Projekty/audio-programming/AudioFileBrowser/AudioFileBrowser/sounds");
    ui->dirView->setModel(m_dirModel);
    ui->dirView->setRootIndex(m_dirModel->index(indexPath));

    /* Customize dirView headers/informations */
    ui->dirView->setColumnWidth(0, 150);
    ui->dirView->hideColumn(1);
    ui->dirView->hideColumn(2);

    /* Default path to be seen in the fileView */
    m_fileModel->setRootPath(rootPath);
    m_fileModel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    ui->fileView->setModel(m_fileModel);
}

void MainWindow::on_dirView_clicked(const QModelIndex &index)
{
    QString clickedPath = m_dirModel->fileInfo(index).absoluteFilePath();
    ui->fileView->setRootIndex(m_fileModel->setRootPath(clickedPath));
}

bool MainWindow::initSDL2()
{
    if(SDL_Init( SDL_INIT_AUDIO ) < 0)
    {
        return false;
    }
    return true;
}

bool MainWindow::createAudioDevice()
{
    if(Mix_OpenAudio(m_audioFormat.frequency, m_audioFormat.format, m_audioFormat.channels, m_audioFormat.buffer) < 0)
    {
        return false;
    }
    Mix_AllocateChannels(5); //channel0: preview, channel1,2,3,4 to play
    return true;
}

void MainWindow::setupSDL2()
{
    if(!initSDL2())
    {
        ui->errorView->setText(QString::fromStdString(SDL_GetError()));
    }
    else
    {
        if(!createAudioDevice())
        {
            ui->errorView->setText(QString::fromStdString(Mix_GetError()));
        }
    }
}

QString MainWindow::getFilePath() const
{
    return (m_fileModel->fileInfo(ui->fileView->currentIndex()).absoluteFilePath());
}

void MainWindow::loadSound()
{
    QString filePath = getFilePath();
    m_previewSound = Mix_LoadWAV(filePath.toStdString().c_str());
    if( !m_previewSound )
    {
        ui->errorView->setText(QString::fromStdString(Mix_GetError()));
    }
}

void MainWindow::playSound()
{
    loadSound();
    int channel = Mix_PlayChannel(0, m_previewSound, 0);
    if(channel == -1)
    {
        ui->errorView->setText(QString::fromStdString(Mix_GetError()));
    }
}

void MainWindow::stopSound()
{
    Mix_HaltChannel(-1);
}

void MainWindow::mountTrack(const int& trackNum)
{
    QString filePath = getFilePath();
    if(filePath != "")
    {
        m_tracksSet[trackNum] = true;
        m_sounds[trackNum] = Mix_LoadWAV(filePath.toStdString().c_str());
        if(!m_sounds[trackNum])
        {
            ui->errorView->setText(QString::fromStdString(Mix_GetError()));
        }
    }
}

void MainWindow::keyPressEvent( QKeyEvent* event )
{
    if( event->key() == Qt::Key_1)
    {
        if(m_tracksSet[0])
        {
            m_channels[0] = Mix_PlayChannel(1, m_sounds[0], 0);
        }
        event->accept();
    }

    if( event->key() == Qt::Key_2)
    {
        if(m_tracksSet[1])
        {
            m_channels[1] = Mix_PlayChannel(2, m_sounds[1], 0);
        }
        event->accept();
    }

    if( event->key() == Qt::Key_3)
    {
        if(m_tracksSet[2])
        {
            m_channels[2] = Mix_PlayChannel(3, m_sounds[2], 0);
        }
        event->accept();
    }

    if( event->key() == Qt::Key_4)
    {
        if(m_tracksSet[3])
        {
            m_channels[3] = Mix_PlayChannel(4, m_sounds[3], 0);
        }
        event->accept();
    }
}
