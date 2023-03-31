import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import client 1.0

Window {
    id: root
    width: 500
    height: 300
    visible: true

    ColumnLayout {

        Button {
            Layout.alignment: Qt.AlignCenter
            Layout.preferredWidth: 100
            text: "подключиться"
            onClicked: {
                client.newConnection()
            }
        }

        ListView {
            Layout.preferredWidth: 200
            Layout.preferredHeight: 200
            delegate: Label{text: texts}
            //model: client.getModel()

        }
        TextArea {
            Layout.preferredWidth: root.width
            placeholderText: "введите сообщение..."
            wrapMode: TextEdit.WordWrap
        }
    }

    Client {
        id: client
    }
}
