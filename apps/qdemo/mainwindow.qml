import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1


ApplicationWindow {
    id: applicationWindow1
    title: qsTr("Hello World")
    visible: true
    width: 800
    height: 680

    SplitView {
        id: splitView1
        anchors.fill: parent
        orientation: Qt.Horizontal


        ColumnLayout {
            id: columnLayout1
            width: 300

            RowLayout {
                Button {
                    text: "Start"
                }
                Button {
                    text: "Step"
                }
            }

            GroupBox {
                id: mclGroup
                title: qsTr("MCL settings")

                GridLayout
                {                    
                    columns: 2

                    Label { 
                        text: "Particles: " 
                    }                    
                    SpinBox {
                       id: particlesCount
                       maximumValue: 20000
                       minimumValue: 100
                       stepSize: 100
                    }

                    Label { 
                        text: "Resampling method" 
                    }
                    ComboBox {
                        id: resamplingMethod
                        model: [ "Residual", "Stratified", "Systematic" ]
                    }
                }   
            }

            GroupBox {
                id: sensorGroup
                title: qsTr("Sensor settings")
                SensorSettings{
                    id: sensor
                }  
            }

            GroupBox {
                id: motionGroup
                title: qsTr("Motion settings")
            }

        }

        Rectangle {
            id: viewPlaceHolder
            color: "#6379ee"
            border.width: 0
            Layout.minimumWidth: 50
            Layout.fillWidth: true
            Text {
                text: "Insert graphics view here"
                anchors.centerIn: parent
            }
        }
    }
}