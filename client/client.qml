import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

ApplicationWindow {
    id: root
    width: 400
    height: 380
    visible: true
    title: qsTr("chat")

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
        anchors.fill: parent

        RowLayout {
            Layout.topMargin: 6
            spacing: 6

            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.leftMargin: 6
                Layout.minimumWidth: 50
                Layout.preferredWidth: 100
                Layout.minimumHeight: 50
                Layout.preferredHeight: 200
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                ScrollBar.vertical.interactive: true

                ListView {
                    id: contact
                    model: contactModel
                    highlight: Rectangle {
                        width: contact.width
                        color: "white"
                    }
                    delegate:
                        Label {
                        text: model.text
                        width: contact.width
                        font.pixelSize: 14
                        color: model.color
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                contact.currentIndex = index
                            }
                        }
                    }

                    Rectangle{
                        z: -1
                        anchors.fill: parent
                        color: "black"
                        opacity: 0.1
                    }
                }
            }

            ScrollView {
                Layout.rightMargin: 6
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 100
                Layout.preferredWidth: 270
                Layout.minimumHeight: 50
                Layout.preferredHeight: 200
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                ScrollBar.vertical.interactive: true
                clip: true

                ListView {
                    id: dialog
                    model: dialogModel
                    delegate: Label {
                        text: model.text
                        color: model.color
                    }

                    Rectangle{
                        z: -1
                        anchors.fill: parent
                        color: "black"
                        opacity: 0.1
                    }
                }
            }
        }

        RowLayout {
            Layout.bottomMargin:  6
            spacing: 6

            ScrollView {
                Layout.leftMargin: 6
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 100
                Layout.preferredWidth: 270
                Layout.minimumHeight: 50
                Layout.preferredHeight: 100

                TextArea {
                    id: messageArea
                    placeholderText: "введите сообщение..."
                    wrapMode: TextEdit.WordWrap
                    Component.onCompleted: {
                        messageArea.forceActiveFocus()
                    }
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

            }
            ColumnLayout {
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
                Button {
                    Layout.alignment: Qt.AlignCenter
                    Layout.preferredWidth: 100
                    enabled: messageArea.text.length
                    text: "отменить"
                    onClicked: {
                        messageArea.clear()
                    }
                }
            }
        }
    }

    Popup {
        id: newContact
        anchors.centerIn: parent

        ColumnLayout {

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
                    newContact.close()
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
        anchors.centerIn: parent
        Component.onCompleted: {
            getParams("params.json")
        }

        ColumnLayout {

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

            Rectangle {
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

    function getParams(path) {
        const xhr = new XMLHttpRequest()
        xhr.onreadystatechange = function() {
            if(xhr.readyState === XMLHttpRequest.DONE) {
                const response = JSON.parse(xhr.responseText.toString())
                ip.text = response.ip
                port.text = response.port
            }
        }
        xhr.open("GET", path)
        xhr.send()
    }
    function getDelegateInstanceAt(index) {
        return contact.itemAtIndex(index).text
    }
}
