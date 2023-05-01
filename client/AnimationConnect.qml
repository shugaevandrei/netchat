import QtQuick
import QtQuick.Window
import QtQuick.Layouts

Image  {
    id:root
    property alias startRunning: ra.running
    Layout.preferredWidth: 50
    Layout.preferredHeight: 50
    source: "qrc:/backCorn.png"

    RotationAnimation on rotation {
        id: ra
        to: 360
        loops: Animation.Infinite
        duration: 2000
        running: false
    }
}
