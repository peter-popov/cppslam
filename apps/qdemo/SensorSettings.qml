import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import qdemo.models 1.0

ColumnLayout {
    SensorSettingsModel {
        id: smodel
        a0: 0.25
        a1: 0.25
        a2: 0.25
        a3: 0.25
        sigma: 10.0
        lambda: 0.1
        maxRange: 101
    }

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
	    TextField {
	    	id: lambda
	    	text: smodel.lambda
	    }
	    Binding { target: smodel; property: "lambda"; value: lambda.text }
		

	   	Label { text: "Measurment" }
	    TextField {
	    	id: a0
	    	text: smodel.a0
	    }
	    Binding { target: smodel; property: "a0"; value: a0.text }

	    Label { text: "Error" }
	    TextField {
	    	id: a1
	    	text: smodel.a1
	    }
	    Binding { target: smodel; property: "a1"; value: a1.text }

	    Label { text: "No data"	}
	    TextField {
	    	id: a2
	    	text: smodel.a2
	    }
	    Binding { target: smodel; property: "a2"; value: a2.text }

	    Label { text: "Close object" }
	    TextField {
	    	id: a3
	    	text: smodel.a3
	    }
	    Binding { target: smodel; property: "a3"; value: a3.text }	
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