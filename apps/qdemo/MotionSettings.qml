import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import qdemo.models 1.0

ColumnLayout {

    MotionModel {
        id: smodel
        a0: 0.4
        a1: 0.5
        a2: 0.05
        a3: 0.1
    }
	
	GridLayout {
		id: layout
		columns: 2
	    Layout.fillWidth: true

	    Label { text: "v(v)" }
	    TextField {
	    	id: a0
	    	text: smodel.a0
	    }
	    Binding { target: smodel; property: "a0"; value: a0.text }

	    Label { text: "v(w)" }
	    TextField {
	    	id: a1
	    	text: smodel.a1
	    }
	    Binding { target: smodel; property: "a1"; value: a1.text }

	    Label { text: "w(v)"	}
	    TextField {
	    	id: a2
	    	text: smodel.a2
	    }
	    Binding { target: smodel; property: "a2"; value: a2.text }

	    Label { text: "w(w)" }
	    TextField {
	    	id: a3
	    	text: smodel.a3
	    }
	    Binding { target: smodel; property: "a3"; value: a3.text }	
	}


	Canvas {
	    id: preview
	    height: 200
	    anchors.left: parent.left
	    anchors.right: parent.right
	    Layout.minimumWidth: 300
	    Layout.fillWidth: true

	   	Connections {
        	target: smodel
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
            for (var i = 0; i < smodel.rotationMotion.samples.length; i++) {
            	drawParticle(ctx, smodel.rotationMotion.samples[i], '#000000');
			}

			ctx.globalAlpha=1.0;
            for (var i = 0; i < smodel.rotationMotion.moves.length; i++) {
            	drawParticle(ctx, smodel.rotationMotion.moves[i], '#ff0000');
			}        	

			ctx.translate(50,0);
			
			ctx.globalAlpha=0.5;
            for (var i = 0; i < smodel.straightMotion.samples.length; i++) {
            	drawParticle(ctx, smodel.straightMotion.samples[i], '#000000');
			}

			ctx.globalAlpha=1.0;
            for (var i = 0; i < smodel.straightMotion.moves.length; i++) {
            	drawParticle(ctx, smodel.straightMotion.moves[i], '#ff0000');
			}  

            ctx.restore();
        }
	}

}