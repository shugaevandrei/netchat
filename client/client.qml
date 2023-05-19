import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQml
import FiltersTypes 1.0
import PacketsTypes 1.0
//import QtQuick.Controls.Material 2.0

ApplicationWindow {
    id: root
    width: 400
    height: 380
    visible: true
    background: Rectangle{
        color: "#fcd03c"
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#fcd03c" }
            GradientStop { position: 1.0; color: "#5d9333" }
        }
    }
    title: qsTr("Corn")

    menuBar: MenuBar {
        Menu {
            title: qsTr("Меню")
            MenuItem {
                enabled: client.isAutorisation
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

    Loader {
        id: loader
        anchors.fill: parent
        sourceComponent: identFowrm
    }

    Component  {
        id: identFowrm
        ColumnLayout {
            Component.onCompleted: {
                client.toConnect(ip.text, port.text)
            }

            Item {
                Layout.fillHeight: true
            }
            RowLayout {
                Layout.alignment: Qt.AlignCenter

                Label {
                    id: infoLbl
                    text: client.isConnect ? "Введите логин и пароль" : "Нет подключения к серверу"
                    color: client.isConnect ? "black" : "red"

                    Connections {
                        target: client
                        function onIsAutorisationChanged(){
                            if (client.isAutorisation)
                                loader.sourceComponent = mainForm
                            else
                                infoLbl.text = "Введены неверные данные, попробуйте снова"
                        }
                        function onIsUnconnectingChanged(){
                            if (client.isUnconnecting) {
                                infoLbl.text = "Попытка подключения к серверу"
                                client.toConnect(ip.text, port.text)
                            }
                        }
                        function onIsConnectChanged(){
                            if (client.isConnect)
                                infoLbl.text = "Введите логин и пароль"
                        }
                        function onIsRegistrationChanged(){
                            if (client.isRegistration)
                                infoLbl.text = "Регистраиця успешна!"
                            else
                                infoLbl.text = "Такой пользователь уже есть"
                        }
                    }
                }
                BusyIndicator {
                    visible: client.isUnconnecting
                    running: client.isUnconnecting
                }
            }

            Rectangle {
                width: login.width
                height:  login.height
                Layout.alignment: Qt.AlignCenter
                color: "white"
                radius: 3

                TextInput {
                    id: login
                    width:  120
                    height: 20
                    maximumLength: 18
                    Layout.preferredWidth: 100
                    verticalAlignment: TextInput.AlignVCenter
                    horizontalAlignment: TextInput.AlignHCenter
                }
            }

            Rectangle {
                width: password.width
                height:  password.height
                Layout.alignment: Qt.AlignCenter
                color: "white"
                radius: 3

                TextInput {
                    id: password
                    width:  120
                    height: 20
                    maximumLength: 18
                    verticalAlignment: TextInput.AlignVCenter
                    horizontalAlignment: TextInput.AlignHCenter
                }
            }

            CheckBox {
                Layout.alignment: Qt.AlignCenter
                checked: true
                text: qsTr("Запомнить меня")
            }

            RowLayout {
                Layout.alignment: Qt.AlignCenter

                Button {
                    Layout.preferredWidth: 60
                    text: "Войти"
                    enabled: client.isConnect
                    onClicked: {
                            if (login.text && password.text) {
                                infoLbl.text = "Выполняется вход"
                                client.autorisation(login.text, password.text)
                            }
                            else
                                infoLbl.text = "Пустые поля!"
                    }
                }

                Button {
                    Layout.preferredWidth: 60
                    text: "Регистрация"
                    enabled: client.isConnect
                    onClicked: {
                            if (login.text && password.text) {
                                infoLbl.text = "Выполняется регистрация"
                                client.registration(login.text, password.text)
                            }
                            else
                                infoLbl.text = "Пустые поля!"
                    }
                }
            }

            Item {
                Layout.fillHeight: true
            }
        }
    }

    Component  {
        id: mainForm
        ColumnLayout {
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
                    clip: true

                    ListView {
                        id: contact
                        model: contactModel
                        currentIndex: -1
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
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    contact.currentIndex = index
                                    client.setCurReceiver(getDelegateInstanceAt(contact, index))
                                }
                            }
                        }

                        Rectangle {
                            z: -1
                            anchors.fill: parent
                            color: "#f0f0f0"
                        }
                    }
                }

                Column {
                    id: manageDialog
                    Layout.rightMargin: 6
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.minimumWidth: 100
                    Layout.preferredWidth: 270
                    Layout.minimumHeight: 50
                    Layout.preferredHeight: 200

                    Row {
                        id: searchForm
                        width: manageDialog.width
                        height: 0
                        spacing: 6

                        TextInput {
                            id: textinpuSearch
                            width: manageDialog.width - cancelBtn.width - parent.spacing
                            height: searchForm.height
                            verticalAlignment: TextInput.AlignVCenter
                            horizontalAlignment: TextInput.AlignHCenter
                            onTextChanged: {
                                client.applyFilter(FilterTypes.SearchMessage, text)
                            }

                            Rectangle {
                                anchors.fill: parent
                                z: -1
                                color: "#f0f0f0"
                            }
                        }

                        RoundButton {
                            id: cancelBtn
                            height: searchForm.height
                            width: height
                            text: "\u274C"
                            onClicked: {
                                searchForm.height = 0
                                searchForm.focus = false
                                textinpuSearch.clear()
                                client.applyFilter(FilterTypes.NoneFiler)
                            }
                        }
                    }

                    Item {
                        id: bottomPadding
                        width: manageDialog.width
                        height: searchForm.height ? 6 : 0
                    }

                    ScrollView {
                        id: viewForm
                        width: manageDialog.width
                        height: manageDialog.height - searchForm.height - bottomPadding.height
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
                                color: "#f0f0f0"
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.LeftButton | Qt.RightButton
                            onClicked: (mouse) => {
                                           if (mouse.button === Qt.RightButton)
                                           contextMenu.popup()
                                       }
                            onPressAndHold: {
                                if (mouse.source === Qt.MouseEventNotSynthesized)
                                    contextMenu.popup()
                            }

                            Menu {
                                id: contextMenu
                                MenuItem {
                                    text: "Поиск"
                                    onTriggered: {
                                        if (!searchForm.height) {
                                            searchForm.height = 20
                                            searchForm.focus = true
                                        }
                                    }
                                }
                            }
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
                                                    client.postMessage(messageArea.text, PacketTypes.ChatMessage, getDelegateInstanceAt(contact, contact.currentIndex))
                                                    messageArea.clear()
                                                    event.accepted = true;
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
                            client.postMessage(messageArea.text, PacketTypes.ChatMessage, getDelegateInstanceAt(contact, contact.currentIndex))
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
    }

    Popup {
        id: newContact
        anchors.centerIn: parent

        ColumnLayout {

            Connections {
                target: client
                function onIsAddContactChanged() {
                    if (client.isAddContact)
                        nameArea.text = "Новый контакт добавлен"
                    else
                        nameArea.text = "Такого контакта нет"
                }
            }

            Label {
                id: nameArea
                text: "Логин нового контакта"
            }

            Rectangle {
                width: ip.width
                height:  ip.height
                color: "transparent"
                border.color: "black"
                border.width: 1

                TextInput {
                    id: logNewCont
                    width:  120
                    height: 20
                    Layout.preferredWidth: 100
                    verticalAlignment: TextInput.AlignVCenter
                    horizontalAlignment: TextInput.AlignHCenter
                    onAccepted: {
                        client.addContact(logNewCont.text)
                    }
                }
            }

            Button {
                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: 100
                text: "добавить контакт"
                onClicked: {
                    client.addContact(logNewCont.text)
                    //newContact.close()
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
    function getDelegateInstanceAt(source, index) {
        return source.itemAtIndex(index).text
    }
}
