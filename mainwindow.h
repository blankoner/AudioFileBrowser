#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QKeyEvent>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string.h>
#include <vector>

#include "audio.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_dirView_clicked(const QModelIndex &index);
    void playSound();
    void stopSound();
    void mountTrack(const int& trackNum);

private:
    Ui::MainWindow *ui;
    QFileSystemModel* m_dirModel;
    QFileSystemModel* m_fileModel;
    Audio m_audioFormat;
    Mix_Chunk* m_previewSound;
    std::vector<Mix_Chunk*> m_sounds;
    bool m_tracksSet[4];
    int m_channels[4];

    void setupFileSystemModel();
    void setupSDL2();
    bool initSDL2();
    bool createAudioDevice();
    QString getFilePath() const;
    void loadSound();
    void keyPressEvent(QKeyEvent* event);
};
#endif // MAINWINDOW_H
