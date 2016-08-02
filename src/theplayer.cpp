#include "theplayer.h"

ThePlayer::ThePlayer(QObject *parent) : QMediaPlayer(parent)
{
    connect(this,
            &QMediaPlayer::stateChanged,
            this,
            &ThePlayer::onStateChanged);
}

void ThePlayer::play()
{
    QMediaPlayer::play();
}

void ThePlayer::play(QString mrl)
{
    setMedia(QMediaContent(QUrl::fromLocalFile(mrl)));
    play();
}

bool ThePlayer::hasAudio()
{
    return !(mediaStatus() == QMediaPlayer::NoMedia
             || mediaStatus() == QMediaPlayer::UnknownMediaStatus
             || mediaStatus() == QMediaPlayer::InvalidMedia);
}

void ThePlayer::seek(qint64 pos)
{
    qDebug() << "pre set we are now at" << position();
    qDebug() << "ThePlayer::seek(" << pos << ")" << isSeekable();
    setPosition(pos);
    qDebug() << "we are now at" << position() << state() << mediaStatus()
             << isAudioAvailable();
}

QUrl ThePlayer::source()
{
    return currentMedia().canonicalUrl();
}

void ThePlayer::onStateChanged(QMediaPlayer::State state)
{
    setIsPlaying(false);

    switch (state) {
        case QMediaPlayer::StoppedState:
            emit stopped();
            break;
        case QMediaPlayer::PlayingState:
            setIsPlaying(true);
            emit playing();
            break;
        case QMediaPlayer::PausedState:
            emit paused();
            break;
        default:
            qWarning() << "This is not supposed to happen";
    }
}

void ThePlayer::onStopped()
{
    if (loops_) {
        play();
    }
}

bool ThePlayer::isPlaying() const
{
    return isPlaying_;
}

void ThePlayer::setIsPlaying(bool isPlaying)
{
    isPlaying_ = isPlaying;
    emit isPlayingChanged();
}

bool ThePlayer::loops() const
{
    return loops_;
}

void ThePlayer::setLoops(bool loops)
{
    loops_ = loops;
    emit loopsChanged();
}
