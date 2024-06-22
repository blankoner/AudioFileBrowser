#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->playButton, SIGNAL(clicked(bool)), this, SLOT(playSound()));
    connect(ui->stopButton, SIGNAL(clicked(bool)), this, SLOT(stopSound()));

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
    if( sound )
    {
        Mix_FreeChunk( sound );
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

void MainWindow::loadSound()
{
    QString filePath = m_fileModel->fileInfo(ui->fileView->currentIndex()).absoluteFilePath();
    sound = Mix_LoadWAV(filePath.toStdString().c_str());
    if( !sound )
    {
        ui->errorView->setText(QString::fromStdString(Mix_GetError()));
    }
}

void MainWindow::playSound()
{
    loadSound();
    int channel = Mix_PlayChannel(0, sound, 0);
    if(channel == -1)
    {
        ui->errorView->setText(QString::fromStdString(Mix_GetError()));
    }
}

void MainWindow::stopSound()
{
    Mix_HaltChannel(-1);
}
