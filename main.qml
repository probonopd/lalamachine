import QtQuick 2.3
import QtQuick.Controls 1.2
import QtMultimedia 5.0
import QtQuick.Dialogs 1.2

import "qrc:/functions.js" as Functions

ApplicationWindow {
    visible: true
    width: 750
    height: 550
    title: qsTr("lalamachine")

    Component.onCompleted: {
        playlist.readPlaylist("misc")
    }

    onClosing: {
        playlist.writePlaylist("misc")
    }

    Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        gradient: Gradient {
            GradientStop {
                position: 0.49
                color: "#000000"
            }
            GradientStop {
                position: 1.00
                color: "#717171"
            }
        }
    }

    MediaPlayer {
        id: playMusic
        volume: volume_control.value

        onStopped: {
            if (Functions.millisToSec(position) === Functions.millisToSec(
                        duration)) {
                playlist.playNext()
            }
        }
    }

    Rectangle {
        id: playlist_container
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: now_playing_container.top
        color: "transparent"

        PlaylistButtons {
            id: playlist_buttons
            anchors.left: parent.left
            anchors.bottom: parent.bottom

            onSaveList: playlist.savePlaylistVisible = true
            onMoveTop: playlist.moveTop()
            onMoveUp: playlist.moveUp()
            onMoveDown: playlist.moveDown()
            onMoveBottom: playlist.moveBottom()
            onClearList: playlist.clearList()
        }

        Playlist {
            id: playlist
            anchors.left: playlist_buttons.right
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            nowPlayingSource: playMusic.source

            onStop: {
                playMusic.stop()
            }

            onPlay: {
                playMusic.source = path
                playMusic.play()
            }
        }
    }

    NowPlayingDisplay {
        id: now_playing_container
        anchors.bottom: btn_row.top
        anchors.right: parent.right
        width: playlist.width

        title: Functions.getSafeValue(playMusic.metaData.title)
        albumArtist: Functions.getSafeValue(playMusic.metaData.albumArtist)
        duration: playMusic.duration
        position: playMusic.position
        hasAudio: playMusic.hasAudio

        onSeek: {
            playMusic.seek(pos)
        }
    }

    PlayerControlButtons {
        id: btn_row
        anchors.left: parent.left
        anchors.bottom: parent.bottom

        onPlayPrevious: playlist.playPrevious()

        onPlay: {
            if (playMusic.hasAudio) {
                playMusic.play()
            }
        }

        onPause: playMusic.pause()
        onOpen: {
            console.log("open clicked")
            fileDialog.visible = true
        }
        onPlayNext: playlist.playNext()
    }

    VolumeControl {
        id: volume_control
        anchors.bottom: parent.bottom
        anchors.left: btn_row.right
        anchors.right: parent.right
    }

    FileDialog {
        id: fileDialog
        visible: false
        title: "Please choose a file"
        selectMultiple: true
        nameFilters: ["Audio files (*.mp3 *.m4a *.ogg *.wav)"]

        onAccepted: {
            console.log("You chose: " + fileDialog.fileUrls)
            playlist.addList(fileDialog.fileUrls)
            visible = false
        }
        onRejected: {
            console.log("Canceled")
            visible = false
        }
    }
}
