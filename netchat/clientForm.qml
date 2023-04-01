import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Window {
    id: root
    width: 500
    height: 300
    visible: true
    color: "dimgray"

    ColumnLayout {

        RowLayout{
            Button {
                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: 100
                text: "подключиться"
                onClicked: {
                    client.newConnection()
                }
            }
            Button {
                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: 100
                text: "отключиться"
                onClicked: {
                    client.disconnect()
                }
            }
        }

        ListView {
            Layout.preferredWidth: 200
            Layout.preferredHeight: 200
            model: dialogModel
            delegate: Label {
                text: model.text
                color: model.color
            }

        }

        TextArea {
            id: messageArea
            Layout.preferredWidth: root.width
            placeholderText: "введите сообщение..."
            wrapMode: TextEdit.WordWrap
        }
        Button {
            Layout.alignment: Qt.AlignCenter
            Layout.preferredWidth: 100
            text: "отправить"
            onClicked: {
                client.postMessage(messageArea.text)
                messageArea.clear()
            }
        }
    }
}
