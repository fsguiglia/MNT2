# Fusión 2020
## El mapa no es el territorio 2 (MNT2)

Pueden descargar un ejecutable para Windows (probado con Windows 10) desde [www.sguiglia.com.ar/soft/MNT2.zip](http://www.sguiglia.com.ar/soft/MNT2.zip). Este ejecutable se ofrece sólo a los fines de facilitar la evaluación en el marco del concurso, por lo que es posible que tenga algunos problemas que serán resueltos en etapas siguiente del proyecto. Entre ellos:  
* openFrameworks no soporta displays HiDPI en Windows, aunque trabajé sobre este problema puede ser que algunos íconos se vean pequeños en algunos monitores.
* Pyinstaller no se lleva bien con Tensorflow > 2.0, para acceder a los gestos generativos sigue siendo necesario instalar Python (probado con 3.9) con las siguientes librerías: tensorflow > 2.5, easygui, keras-mdn-layer, progressbar

Si no, pueden compilar la solución usando:
* Visual Studio 2017
* openFrameworks 0.11 con las siguientes librerías:
  * ofxDatGui
  * ofxMidi
  * ofxOsc
  * ofxSpatialHash
  
Para acceder a los gestos generativos y las operaciones de reducción dimensional es necesario Python (probado con 3.9) con las siguientes librerías: tensorflow > 2.5, easygui, keras-mdn-layer, progressbar, sklearn y librosa


MNT2 incluye un sintetizador para Max For Live, disponible en [github.com/fsguiglia/concatenate](https://github.com/fsguiglia/concatenate)
