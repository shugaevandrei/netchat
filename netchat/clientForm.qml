import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Window {
    id: root
    width: 500
    height: 500
    visible: true

    ColumnLayout {


        RowLayout{
            Button {
                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: 100
                text: "подключиться"
                onClicked: {
                    client.newConnection(ip.text, port.text)
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
            Button {
                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: 100
                text: "добавить контакт"
                onClicked: {
                    client.addContact(ipNewCont.text)
                }
            }
        }

        RowLayout {

            Label {
                text: "Введите IP-адрес"
            }
            Rectangle{
                width: ip.width
                height:  ip.height
                color: "transparent"
                border.color: "black"
                border.width: 1
                TextInput {
                    id: ip
                    width:  120
                    height: 20
                    Layout.preferredWidth: 100
                    verticalAlignment: TextInput.AlignVCenter
                    horizontalAlignment: TextInput.AlignHCenter
                    text: "127.0.0.1"
                    validator: RegularExpressionValidator {
                        regularExpression:  /^((?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\.){0,3}(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])$/
                    }
                }
            }

            Label {
                text: "IP-адрес нового контакта"
            }
            Rectangle{
                width: ip.width
                height:  ip.height
                color: "transparent"
                border.color: "black"
                border.width: 1
                TextInput {
                    id: ipNewCont
                    width:  120
                    height: 20
                    Layout.preferredWidth: 100
                    verticalAlignment: TextInput.AlignVCenter
                    horizontalAlignment: TextInput.AlignHCenter
                    validator: RegularExpressionValidator {
                        regularExpression:  /^((?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\.){0,3}(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])$/
                    }
                    onAccepted: {
                        client.addContact(ipNewCont.text)
                    }
                }
            }
        }

        RowLayout {

            Label {
                text: "Введите номер порта"
            }

            Rectangle{
                width: port.width
                height:  port.height
                color: "transparent"
                border.color: "black"
                border.width: 1
                TextInput {
                    id: port
                    width:  52
                    height: 20
                    text: "6000"
                    inputMask: "9999"
                    verticalAlignment: TextInput.AlignVCenter
                    horizontalAlignment: TextInput.AlignHCenter
                }
            }
        }
        RowLayout {
            Rectangle {
                width: dialog.width
                height: dialog.height
                Layout.leftMargin: 10
                color: "transparent"
                border.color: "black"
                border.width: 1

                ListView {
                    id: dialog
                    width: root.width * 0.6
                    height: 200
                    model: dialogModel
                    delegate: Label {
                        text: model.text
                        color: model.color
                    }
                }
            }

            Rectangle {
                id: frameContact
                width: root.width * 0.3
                height: 200
                Layout.leftMargin: 10
                color: "transparent"
                border.color: "black"
                border.width: 1

                ListView {
                    id: contact
                    width: root.width * 0.6
                    height: 200
                    model: contactModel
                    highlight: Rectangle {
                        width: frameContact.width
                        color: "green"
                    }
                    delegate:
                        Label {
                        text: model.text
                        color: model.color
                        Rectangle {
                            height: 1
                            width: frameContact.width
                            anchors.bottom: parent.bottom
                            color: "black"
                        }
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                contact.currentIndex = index
                            }
                        }
                    }
                }
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
                client.postMessage(messageArea.text, getDelegateInstanceAt(contact.currentIndex))
                messageArea.clear()
            }
        }
    }

    Keys.onPressed: (event)=> {
                    switch(event.key){
                            case Qt.Key_Enter:
                            case Qt.Key_Return: {
                                client.postMessage(messageArea.text, getDelegateInstanceAt(contact.currentIndex))
                                messageArea.clear()
                            }
                            break
                    }
    }

    function getDelegateInstanceAt(index) {
        return contact.itemAtIndex(index).text
    }
}
