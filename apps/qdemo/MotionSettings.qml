import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import qdemo.models 1.0

ColumnLayout {

	property MotionModel model

	
	GridLayout {
		id: layout
		columns: 2
	    Layout.fillWidth: true

	    Label { text: "\u03C3(v)" }
	   	Slider {
	   		id: a0
 		   	value: model.a0
 		   	maximumValue: 10.0
			stepSize: 0.1 
		}
	    Binding { target: model; property: "a0"; value: a0.value }

	    Label { text: "v<->w" }
	   	Slider {
	   		id: a1
 		   	value: model.a1
 		   	maximumValue: 1.0
			stepSize: 0.05 
		}
	    Binding { target: model; property: "a1"; value: a1.value }


	    Label { text: "\u03C3(w)"	}
	   	Slider {
	   		id: a2
 		   	value: model.a2
 		   	maximumValue: 0.1
			stepSize: 0.005
		}
	    Binding { target: model; property: "a2"; value: a2.value }

	    Label { text: "v<->w" }
	   	Slider {
	   		id: a3
 		   	value: model.a3
 		   	maximumValue: 1.0
			stepSize: 0.05 
		}
	    Binding { target: model; property: "a3"; value: a3.value }	
	}


	Canvas {
	    id: preview
	    height: 200
	    anchors.left: parent.left
	    anchors.right: parent.right
	    Layout.minimumWidth: 300
	    Layout.fillWidth: true

	   	Connections {
        	target: model
        	onModelChanged: preview.requestPaint()
     	}

	    onPaint: {

	    	function drawParticle(c, pose, style)
			{
			    var x = pose.position.x;
	            var y = pose.position.y;
	            var a = pose.orientation;
	            c.beginPath();
	            c.lineWidth = 0.3;
	            c.ellipse(x - 1, y - 1, 2, 2);
	            c.moveTo(x, y);
	            var xd = x + 3 * Math.cos(a);
	            var yd = y + 3 * Math.sin(a);
	            c.lineTo(xd, yd);
	            ctx.strokeStyle = style;
	            c.stroke();
			}


            var ctx = preview.getContext('2d');
            ctx.save();
            ctx.clearRect(0, 0, preview.width, preview.height);

            ctx.scale(4, 4);

			ctx.globalAlpha=0.5;
            for (var i = 0; i < model.rotationMotion.samples.length; i++) {
            	drawParticle(ctx, model.rotationMotion.samples[i], '#000000');
			}

			ctx.globalAlpha=1.0;
            for (var i = 0; i < model.rotationMotion.moves.length; i++) {
            	drawParticle(ctx, model.rotationMotion.moves[i], '#ff0000');
			}        	

			ctx.translate(50,0);
			
			ctx.globalAlpha=0.5;
            for (var i = 0; i < model.straightMotion.samples.length; i++) {
            	drawParticle(ctx, model.straightMotion.samples[i], '#000000');
			}

			ctx.globalAlpha=1.0;
            for (var i = 0; i < model.straightMotion.moves.length; i++) {
            	drawParticle(ctx, model.straightMotion.moves[i], '#ff0000');
			}  

            ctx.restore();
        }
	}

}