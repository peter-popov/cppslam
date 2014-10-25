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
        sigma: 10000.0
        lambda: 0.1
        maxRange: 2000.0
    }

    MotionModel {
        id: mmodel
        a0: 0.4
        a1: 0.5
        a2: 0.01
        a3: 0.5
    }

    Pose {
        id: startPose
        position.x: 300
        position.y: -250        
    }

    VelocityControl {
        id: control
        v: 10
        w: 0
        t: 1.0
    }

    MclSettings {
        id: mclSettings     
        numberOfBeams: 12
    }

    Simulation {
        id: simulation
        mcl: mclSettings
        sensorModel: smodel
        motionModel: mmodel
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
                    onClicked: simulation.init("wkt_from_svg.txt", startPose)
                    //!onClicked: simulation.init("indoor_1.csv", startPose)
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
                MotionSettings{
                    Layout.fillWidth: true
                    id: motion
                    model: mmodel
                } 
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
                    ctx.translate(mapView.translate.x, mapView.translate.y);
                    console.log("Zoom: " + mapView.zoom);                    
                    console.log("Translate: " + mapView.translate);

                    var particle_size = 2 / mapView.zoom;
                        
                    for (var i = 0; i < simulation.particles.length; i++) {
                        var x = simulation.particles[i].pose.position.x;
                        var y = simulation.particles[i].pose.position.y;
                        var a = simulation.particles[i].pose.orientation;                        
                        ctx.beginPath();
                        ctx.lineWidth = 0.3*particle_size ;
                        ctx.ellipse(x - particle_size, y - particle_size, 2*particle_size, 2*particle_size);
                        ctx.moveTo(x, y);
                        var xd = x + 3 * particle_size * Math.cos(a);
                        var yd = y - 3 * particle_size * Math.sin(a);
                        ctx.lineTo(xd, yd);
                        ctx.stroke();
                    }


                    var pos_x = simulation.currentPose.position.x
                    var pos_y = simulation.currentPose.position.y
                    var dashList = [12, 3, 3, 3];    
                    for (var i = 0; i < simulation.sensorBeams.length; i++) {
                        var x = simulation.sensorBeams[i].coord.x;
                        var y = simulation.sensorBeams[i].coord.y;
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
                source: "wkt_from_svg.txt"
                //!source: "indoor_1.csv"
                transformOrigin : "TopLeft"             
            }
        }
    }
}