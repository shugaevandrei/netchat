import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

ApplicationWindow {
    id: root
    visible: true
    title: qsTr("chat")
    width: 340
    height: 480

    menuBar: MenuBar {
        Menu {
            title: qsTr("Меню")
            MenuItem {
                text: !client.isConnect ? qsTr("Подключиться") :qsTr("Отключиться")
                onTriggered: !client.isConnect ? client.newConnection(ip.text, port.text) : client.disconnect()
            }
            MenuItem {
                text: qsTr("Добавить контакт")
                onTriggered: {
                    newContact.focus = true
                    newContact.open()
                }
            }
            MenuItem {
                text: qsTr("Настройки")
                onTriggered: {
                    params.focus = true
                    params.open()
                }
            }
            MenuItem {
                text: qsTr("Выход")
                onTriggered: Qt.quit()
            }
        }
    }

    ColumnLayout {

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
                    anchors.fill: parent
                    height: 200
                    model: contactModel
                    highlight: Rectangle {
                        width: root.width * 0.6
                        color: "#800000FF"
                    }
                    delegate:
                        Label {
                        width: ListView.view.width
                        text: model.text
                        font.pixelSize: 14
                        color: model.color
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
            focus: true
            placeholderText: "введите сообщение..."
            wrapMode: TextEdit.WordWrap

            Keys.onPressed: (event)=> {
                            switch(event.key){
                                    case Qt.Key_Enter:
                                    case Qt.Key_Return: {
                                        client.postMessage(messageArea.text, getDelegateInstanceAt(contact.currentIndex))
                                        messageArea.clear()
                                        event.accepted = true;
                                        console.log("PRESS")
                                    }
                                    break
                            }
            }
        }
        Button {
            Layout.alignment: Qt.AlignCenter
            Layout.preferredWidth: 100
            enabled: messageArea.text.length && (contact.currentIndex >= 0)
            text: "отправить"
            onClicked: {
                client.postMessage(messageArea.text, getDelegateInstanceAt(contact.currentIndex))
                messageArea.clear()
            }
        }
    }


    Popup {
        id: newContact
        RowLayout {
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
            Button {
                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: 100
                text: "добавить контакт"
                onClicked: {
                    client.addContact(ipNewCont.text)
                }
            }
        }
        onOpenedChanged:  {
            sharedPallete.visible = true
        }
        onClosed: {
            sharedPallete.visible = false
        }
    }
    Popup {
        id: params
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
        onOpenedChanged:  {
            sharedPallete.visible = true
        }
        onClosed: {
            sharedPallete.visible = false
        }
    }
    Rectangle {
        id: sharedPallete
        visible: false
        anchors.fill: parent
        color: "black"
        opacity: 0.3
    }

    function getDelegateInstanceAt(index) {
        return contact.itemAtIndex(index).text
    }

}
