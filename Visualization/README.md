# Detector and  event visualization 

| **Table of Contents**               |                                                                         |
| --:                                 |-------------------------------------------------------------------------|
| [Prerequisites](#Prerequisites)                     | Prerequisites needed for the macros                                     |
| [Configuration xml file](#Configuration xml file)   | How to create xml file defining colors and transparency of the detector |
| [Recoloring](#Recoloring) | How to recolor the detector model using configuration xml file           |
| [Detector visualization](#Detector visualization)           | How to visualize the recolored detector                                 |
| [Event visualization](#Event visualization)   | How to visualize particles in the recolored detector using a particle gun |
## Prerequisites
- TGeo geometry ROOT file - see https://github.com/eic/drich-dev for instructions on how to generate the geometry file (geometry.sh macro).
- Particle gun - see tutorial by Shyam Kumar https://indico.bnl.gov/event/18360/. (Visualize_DD4HEP/Eve_Gun/run_eve.sh) 


## Configuration xml file
If you want to create a new xml file including the information about the colors and transparency of the particular detectors and subdetectors, you can use the following macro:
`Create_xml.C` in the terminal:
```bash
root -l Create_xml.C"(\"detector_geometry.root\")"
```
This will create a new xml file called `detector_geometry.xml`. The example of the xml file is shown below:
```
<config>
   <detector name="PFRICH">
		<part name="GasVolume" color="20" transparency="0" />
		<part name="InnerWall" color="20" transparency="0" />
                ...
   </detector>
</config>
```    

By default, the color is set to 20 and transparency to 0. The colors have a typical ROOT format, see the table and the color wheel below.
![alt text](https://root.cern.ch/doc/master/pict1_TColor_001.png)

![alt text](https://root.cern.ch/doc/master/pict1_TColor_002.png)

The transparency is between 0 and 100, where 0 is fully opaque and 100 is fully transparent.
## Recoloring
If you want to recolor the detector using xml file `detector_geometry.xml`, you can use the following macro:
```bash
root -l Recoloring.C\(\"detector_geometry.root\",\"detector_colors.xml\",\"Recolored_detector.root\"\)
```
where `detector_geometry.root` is the geometry file and `Recolored_detector.root` is the new geometry file with the recolored detector.
- If color is defined improperly, the kWhite color is chosen.
- If transparency is set to 100, the particular part of the detector is not visualized.
- If xml file does not include the subdetector, the color is unchanged.
- If the xml file includes the subdetector not included in the geometry file, the macro will ignore it.

## Detector visualization
If you want to visualize the detector saved as `Recolored_detector.root`, you can use the following macro:
```bash
root -l Draw_model.C
```
If clipping is needed, you have two options:
- plane cut
- box cut

The plane cut can be set in the macro `Draw_model.C` by adding the following line:
```bash
//Ax+By+CZ+D = 0
double data[4] = {1.0, 0.0, 0.0, 0.0};
TGLViewer *viewer = gEve->GetDefaultGLViewer();
viewer->GetClipSet()->SetClipType(TGLClip::EType(1));
viewer->GetClipSet()->SetClipState(TGLClip::EType(1),data);
viewer->DoDraw();
```
However, it is easier to set plane or box cut in GLViewer. There the camera position can be set as well. The camera position and angle can be changed as well.
To save a picture, you can use the following command:
```bash
gEve->GetDefaultGLViewer()->SavePicture("output.png");
```
directly in the terminal. The available formats are gif, gif+, jpg, png, eps, and pdf.

## Event visualization
To visualize the event, you need to run the particle gun first. See the tutorial by Shyam Kumar https://indico.bnl.gov/event/18360/.
In the folder `Visualize_DD4HEP/Eve_Gun/` you can find the macro `run_eve.sh`. Or run directly in the terminal:
```bash
npsim --runType vis \
--macroFile myvis.mac \
--compactFile $DETECTOR_PATH/epic_tracking_only.xml \
--enableGun --gun.particle pi+ --gun.momentumMin 0.1*GeV
--gun.momentumMax 10.*GeV \
--gun.thetaMin 3*deg --gun.thetaMax 177*deg
--gun.distribution uniform --outputFile sim.edm4hep.root
```
where `myvis.mac` is the macro for visualization. After that, you will get the file `sim.edm4hep.root` that can be visualized using the macro `Draw_event.C`:
```bash
Draw_event.C\(true\)
```
In this case, it is better to visualize the needed detectors with suitable transparency. Clipping does not work properly.