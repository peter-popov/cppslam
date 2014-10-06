import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0

ApplicationWindow {
    title: "Hello World"
    visible: true
    width: 640
    height: 480

    TabView {
        Tab {
            title: "Tab 1"            
            ColumnLayout
            {                    
                spacing: 5
                Label {text: "Label 1"}
                Label {text: "Label 2"}
                Label {text: "Label 3"}
                Label {text: "Label 4"}
                Label {text: "Label 5"}
                Label {text: "Label 6"}
            }           
        }
    }
}