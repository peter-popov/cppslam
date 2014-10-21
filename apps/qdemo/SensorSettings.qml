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
	    	text: model.maxRange
	    }
	    Binding { target: model; property: "maxRange"; value: maxRange.text }


	    Label { text: "\u03C3" }
	    TextField {
	    	id: sigma
	    	text: model.sigma
	    }
		Binding { target: model; property: "sigma"; value: sigma.text }
	    

	    Label { text: "\u03BB" }
	   	Slider {
	   		id: lambda
 		   	value: model.lambda
 		   	minimumValue: 0.005
 		   	maximumValue: 0.2
			stepSize: 0.001 
		}
	    Binding { target: model; property: "lambda"; value: lambda.value }
		

	   	Label { text: "Measurment" }
	   	Slider {
	   		id: a0
 		   	value: model.a0
 		   	maximumValue: 1.0
			stepSize: 0.01 
		}

	    Binding { target: model; property: "a0"; value: a0.value }

	    Label { text: "Error" }
	   	Slider {
	   		id: a1
	   		value: model.a1
 		   	maximumValue: 1.0
			stepSize: 0.01 
		}
	    Binding { target: model; property: "a1"; value: a1.value }

	    Label { text: "No data"	}
	   	Slider {
	   		id: a2
 		   	value: model.a2
 		   	maximumValue: 1.0
			stepSize: 0.01 
		}
	    Binding { target: model; property: "a2"; value: a2.value }

	    Label { text: "Close object" }
	   	Slider {
	   		id: a3
 		   	value: model.a3
 		   	maximumValue: 1.0
			stepSize: 0.01 
		}
	    Binding { target: model; property: "a3"; value: a3.value }	
	}


	Canvas {
	    id: preview
	    height: 150
	    anchors.left: parent.left
	    anchors.right: parent.right
	     

	   	Connections {
        	target: model
        	onModelChanged: preview.requestPaint()
     	}

	    onPaint: {
            var ctx = preview.getContext('2d');
            ctx.save();
            ctx.clearRect(0, 0, preview.width, preview.height);

			ctx.beginPath();			
            ctx.moveTo(0,preview.height);
            for (var i = 0; i < model.samples.length; i++) {
            	var x = i * preview.width / model.samples.length
            	var y = preview.height - model.samples[i] * preview.height
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