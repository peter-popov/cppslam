import sys
from xml.dom import minidom



def point_to_wkt(p):
	x, y = p
	return str(x) + " " + str(y)

def polygon_to_wkt(poly):
	result = 'POLYGON(('
	l = [point_to_wkt(p) for p in poly]
	#l.reverse()	
	result += ', '.join(l)
	result += '))'
	#result += point_to_wkt(poly[-1]) + '))'
	return result	

doc = minidom.parse(sys.argv[1])

for rect in doc.getElementsByTagName('rect'):
	x = float(rect.getAttribute('x'))
	y = float(rect.getAttribute('y'))
	w = float(rect.getAttribute('width'))
	h = float(rect.getAttribute('height'))
	print "\"" + polygon_to_wkt([(x, y), (x + w, y), (x + w, y + h), (x, y + h)]) + "\""




doc.unlink()