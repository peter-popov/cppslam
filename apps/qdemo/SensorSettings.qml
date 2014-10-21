import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import qdemo.models 1.0

ColumnLayout {

	property SensorSettingsModel model

	GridLayout {
		id: layout
		columns: 2

	    Label { text: "Max range" }
	    TextField {
	    	id: maxRange
	    	text: smodel.maxRange
	    }
	    Binding { target: smodel; property: "maxRange"; value: maxRange.text }


	    Label { text: "\u03C3" }
	    TextField {
	    	id: sigma
	    	text: smodel.sigma
	    }
		Binding { target: smodel; property: "sigma"; value: sigma.text }
	    

	    Label { text: "\u03BB" }
	   	Slider {
	   		id: lambda
 		   	value: smodel.lambda
 		   	minimumValue: 0.005
 		   	maximumValue: 0.2
			stepSize: 0.001 
		}
	    Binding { target: smodel; property: "lambda"; value: lambda.value }
		

	   	Label { text: "Measurment" }
	   	Slider {
	   		id: a0
 		   	value: smodel.a0
 		   	maximumValue: 1.0
			stepSize: 0.01 
		}

	    Binding { target: smodel; property: "a0"; value: a0.value }

	    Label { text: "Error" }
	   	Slider {
	   		id: a1
	   		value: smodel.a1
 		   	maximumValue: 1.0
			stepSize: 0.01 
		}
	    Binding { target: smodel; property: "a1"; value: a1.value }

	    Label { text: "No data"	}
	   	Slider {
	   		id: a2
 		   	value: smodel.a2
 		   	maximumValue: 1.0
			stepSize: 0.01 
		}
	    Binding { target: smodel; property: "a2"; value: a2.value }

	    Label { text: "Close object" }
	   	Slider {
	   		id: a3
 		   	value: smodel.a3
 		   	maximumValue: 1.0
			stepSize: 0.01 
		}
	    Binding { target: smodel; property: "a3"; value: a3.value }	
	}


	Canvas {
	    id: preview
	    height: 150
	    anchors.left: parent.left
	    anchors.right: parent.right
	     

	   	Connections {
        	target: smodel
        	onModelChanged: preview.requestPaint()
     	}

	    onPaint: {
            var ctx = preview.getContext('2d');
            ctx.save();
            ctx.clearRect(0, 0, preview.width, preview.height);

			ctx.beginPath();			
            ctx.moveTo(0,preview.height);
            for (var i = 0; i < smodel.samples.length; i++) {
            	var x = i * preview.width / smodel.samples.length
            	var y = preview.height - smodel.samples[i] * preview.height
            	//!console.log("x = " + x + ", y = " + y)
            	ctx.lineTo(x,y);
            }
            ctx.lineTo(preview.width,preview.height)

            ctx.closePath();
            ctx.fill();
            ctx.stroke();
            ctx.restore();
        }
	}

}