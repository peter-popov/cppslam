import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import qdemo.models 1.0
import ShapesView 1.0

ApplicationWindow {
    id: applicationWindow1
    title: qsTr("Hello World")
    visible: true
    width: 800
    height: 680    

    SensorSettingsModel {
        id: smodel
        a0: 0.25
        a1: 0.25
        a2: 0.25
        a3: 0.25
        sigma: 400.0
        lambda: 0.1
        maxRange: 500.0
    }


    Pose {
        id: startPose
        position.x: 60
        position.y: -150        
    }

    VelocityControl {
        id: control
        v: 1
        w: 0
        t: 1.0
    }

    MclSettings {
        id: mclSettings     
        numberOfBeams: 6
    }

    Simulation {
        id: simulation
        mcl: mclSettings
        sensorModel: smodel
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
                    onClicked: simulation.move(control)
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
                       Layout.minimumWidth: 70
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
                    model: smodel
                }  
            }

            GroupBox {
                id: motionGroup
                title: qsTr("Motion settings")
            }

        }

        Item
        {
            Canvas {
                id: simulationCanvas
                Layout.minimumWidth: 50
                Layout.fillWidth: true
                anchors.fill: parent 

                Connections {
                    target: simulation
                    onInitialized: simulationCanvas.requestPaint()
                    onUpdated: simulationCanvas.requestPaint()
                }

                onPaint: {
                    var ctx = simulationCanvas.getContext('2d');
                    ctx.save();
                    ctx.clearRect(0, 0, simulationCanvas.width, simulationCanvas.height);                

                    ctx.scale(mapView.zoom, mapView.zoom);
                        
                    for (var i = 0; i < simulation.particles.length; i++) {
                        var x = simulation.particles[i].pose.position.x;
                        var y = -simulation.particles[i].pose.position.y;
                        var a = simulation.particles[i].pose.orientation;
                        ctx.beginPath();
                        ctx.lineWidth = 0.3;
                        ctx.ellipse(x - 1, y - 1, 2, 2);
                        ctx.moveTo(x, y);
                        var xd = x + 3 * Math.cos(a);
                        var yd = y - 3 * Math.sin(a);
                        ctx.lineTo(xd, yd);
                        ctx.stroke();
                    }


                    var pos_x = simulation.currentPose.position.x
                    var pos_y = -simulation.currentPose.position.y
                    var dashList = [12, 3, 3, 3];    
                    for (var i = 0; i < simulation.sensorBeams.length; i++) {
                        var x = simulation.sensorBeams[i].coord.x;
                        var y = -simulation.sensorBeams[i].coord.y;
                        ctx.beginPath();
                        ctx.moveTo(pos_x, pos_y);
                        ctx.lineTo(x, y);
                        ctx.lineWidth = 0.3;
                        ctx.strokeStyle = '#ff0000';
                        ctx.stroke();    
                    }

                    ctx.restore();
                }
            }

            ShapefileView {
                id: mapView
                anchors.fill: parent        
                source: "indoor_1.csv"
                transformOrigin : "TopLeft"             
            }
        }
    }
}