/*
Copyright 2015 Armin Zirkel

This file is part of lalamachine.

Lalamachine is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Lalamachine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with lalamachine.  If not, see <http://www.gnu.org/licenses/>.
*/
import QtQuick 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Dialog {
    width: 250
    height: 300

    title: "Settings"

    onAccepted: {
        config.playlistColumns = playlist_columns.getJson()
        config.saveConfig()
    }

    standardButtons: StandardButton.Ok | StandardButton.Cancel

    Item {
        id: columns_edit
        anchors.fill: parent

        KeyValueTableView {
            id: playlist_columns
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.right: up_button.left
            keyTitle: "Tag"
            valueTitle: "visible"
            list: config.playlistColumns
            onRightClick: rcm.popup()
        }

        Menu {
            id: rcm
            MenuItem {
                text: "enable"
                onTriggered: playlist_columns.setSelectionEnabled("true")
            }
            MenuItem {
                text: "disable"
                onTriggered: playlist_columns.setSelectionEnabled("false")
            }
        }

        ImageButton {
            id: up_button
            width: 50
            anchors.right: parent.right
            anchors.bottom: playlist_columns.verticalCenter
            source: "qrc:///images/images/go-up.png"
            onClicked: playlist_columns.moveUp()
        }
        ImageButton {
            id: down_button
            width: 50
            anchors.right: parent.right
            anchors.top: playlist_columns.verticalCenter
            source: "qrc:///images/images/go-down.png"
            onClicked: playlist_columns.moveDown()
        }
    }
}
