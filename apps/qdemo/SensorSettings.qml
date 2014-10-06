import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1

GridLayout {
	id: layout
	columns: 2

    Label {
        text: "Max length"	         
    }
    TextField {
    	id: maxLength
    	text: "100.0"
    }
    Label {
        text: "\u03C3"	         
    }
    TextField {
    	id: sigma
    	text: "1.0"
    }

    Label {
        text: "\u03BB"	         
    }
    TextField {
    	id: lambda
    	text: "1.0"
    }	

   	Label {
        text: "Measurment"	         
    }
    TextField {
    	id: a0
    	text: "1.0"
    }
    Label {
        text: "Error"	         
    }
    TextField {
    	id: a1
    	text: "1.0"
    }
    Label {
        text: "No data"	         
    }
    TextField {
    	id: a2
    	text: "1.0"
    }
    Label {
        text: "Close object"	         
    }
    TextField {
    	id: a3
    	text: "1.0"
    }	
}