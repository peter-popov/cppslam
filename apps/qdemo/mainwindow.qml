import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import qdemo.models 1.0

ApplicationWindow {
    id: applicationWindow1
    title: qsTr("Hello World")
    visible: true
    width: 800
    height: 680    

    

    Pose {
        id: startPose
        position.x: 100
        position.y: 200        
    }

    MclSettings {
        id: mclSettings     
    }

    Simulation {
        id: simulation
        mcl: mclSettings
    }

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
                    onClicked: simulation.init("indoor_1.csv", startPose)
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
                    Binding { target: simulation; property: "mcl.numberOfParticles"; value: particlesCount.value }

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

        Canvas {
            id: simulationCanvas
            Layout.minimumWidth: 50
            Layout.fillWidth: true
            
            Connections {
                target: simulation
                onInitialized: simulationCanvas.requestPaint()
            }

            onPaint: {
                var ctx = simulationCanvas.getContext('2d');
                ctx.save();
                ctx.clearRect(0, 0, simulationCanvas.width, simulationCanvas.height);                

                for (var i = 0; i < simulation.particles.length; i++) {
                    var x = simulation.particles[i].pose.position.x;
                    var y = -simulation.particles[i].pose.position.y;
                    ctx.beginPath()
                    ctx.ellipse(x - 3, y - 3, 6, 6);
                    ctx.stroke()
                    ctx.fill()
                }
                ctx.restore();
            }
        }
    }
}