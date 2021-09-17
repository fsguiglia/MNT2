# Fusión 2020
## El mapa no es el territorio 2 (MNT2)

Pueden descargar un ejecutable para Windows (probado con Windows 10) desde [www.sguiglia.com.ar/soft/MNT2.zip](http://www.sguiglia.com.ar/soft/MNT2.zip). Este ejecutable se ofrece sólo a los fines de facilitar la evaluación en el marco del concurso, por lo que es posible que tenga algunos problemas que serán resueltos en etapas siguientes del proyecto. Entre ellos:  
* Requiere [Microsoft Visual C++ Redistributable for VS 2017](https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads).
* openFrameworks no soporta displays HiDPI en Windows, aunque trabajé sobre este problema puede ser que algunos íconos se vean pequeños en algunos monitores.
* Pyinstaller no se lleva bien con Tensorflow > 2.0, para acceder a los gestos generativos por ahora es necesario necesario instalar Python (probado con 3.9) con las siguientes librerías: tensorflow > 2.5, easygui, keras-mdn-layer, progressbar

Si no, pueden compilar la solución usando:
* Visual Studio 2017
* openFrameworks 0.11 con los siguientes addons:
  * ofxDatGui
  * ofxMidi
  * ofxOsc
  * ofxSpatialHash
* Python 3.9 con los siguientes paquetes:
  * tensorflow > 2.5
  * easygui
  * progressbar
  * keras-mdn-layer
  * progressbar
  * sklearn
  * librosa
  

Aún me encuentro trabajando en un manual, pero puede descargarse una guía de inicio rápido desde [www.sguiglia.com.ar/soft/MNT2_guia.pdf](http://www.sguiglia.com.ar/soft/MNT2_guia.pdf).

MNT2 incluye un sintetizador para Max For Live, disponible en [github.com/fsguiglia/concatenate](https://github.com/fsguiglia/concatenate).
