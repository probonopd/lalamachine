import QtQuick 2.0

Text {
    font.family: "Helvetica"
    font.pointSize: 12
    clip: true
    color: "white"
    text: styleData.value ? styleData.value : ""
    verticalAlignment: Qt.AlignVCenter
    horizontalAlignment: styleData.textAlignment
}
