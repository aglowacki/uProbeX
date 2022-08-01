# uProbeX GUI for XRF-Maps

## Loading a Workspace

### Dataset links 
- XRF-Demo : ~ 600 MB compressed download, 20 GB uncompressed
- XRF-Demo-mini : 7 MB compressed download, 185 MB uncompressed
- XRF-Demo2 :  MB compressed download, 30 GB uncompressed
- XRF-Demo2-mini : MB compressed download , 1.25 GB uncompressed

### XRF-Maps Workspace
The workspace consists of multiple directories which represent information ranging from raw xrf dataset, visible light microscope data, override parameters, and analyzed datasets.
The layout of a workspace folder are the following:
- Workspace root folder
  - flyXRF (folder)
    - *.nc : netcdf files containing raw data for fly scans. Spectra data and meta data including input counts, output counts, elapsed live time, and elapsed real time. Note, you need mda file with similar name to process these
  - mda (folder)
    - *.mda : Self contained raw xrf data for step scans. Contain scaler information for fly scans and you need to load proper netcdf files for raw xrf spectra.
  - img.dat (folder)
    - contains .h5 , h50 - .h5N analyzed datasets. .h0 = detector 1, .hN = detector N+1, .h5 is avg of all detectors.
  - ouput (folder)
    - Contains images and csv files of latest analysis. 
  - rois (folder)
    - .roi files that are sub region of intereest of a larger dataset.
  - maps_fit_parameters_override.txt (file)
    - Avgeraged parameter override file. 
    - One for each detector is created with detector number -1 on the end of the file name.
  - maps_standardinfo.txt (file)
    - File that contains link information for performing quantification calculations. 

Open uProbeX 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-01.png)

To open a workspace select **File** -> **Open Maps Workspace **

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-02.png)

This will bring up a new window of the selected workspace. 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-03.png)

On the let side of the windows are 3 tabs. **Analyzed Data, Raw Datasets, Light Microscope**
If your workspace contains data in the **img.dat** then you will see a list of h5 files in the **Analyzed datasets** tab. 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-03-1.png)

We will sue **XRF-Demo2** or **XRF-Demo2-mini** for this first part of the tutorial.
Click the right arrow on the file tabs to get to 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-05.png)

Right click on **8-20x-2.tif** and select **Load**

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-06.png)

This will load the Light Microscope dataset that is located in the Workspace_dir/VLM directory. This data can consist of just a single tiff file or it can be an SWS workspace which can contain positioning information for the light microscope dataset.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-07.png)

There are also **Scan Regions** that you can create before and experiment. 

In this demo, the first scan region in the dataset **Micro Probe Region1**, is associated with raw dataset **2xfm_009.mda**. If you are performing experiments at the APS, it is recommended to name your regions to the raw dataset names as a reference for the future. 

Lets open the raw dataset **2xfm_0009** and take a look at the raw data. Select the **Raw Data** tab and **right click** on **2xfm_0009.mda** and select **Load**

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-08.png)

If the dataset loaded without error, the ball icon to the left of the name will turn **green**. If there was an error loading then it will be red. You can go back to the initial uProbeX windows (before opening a workspace) and look at the bottom log widget to get information why the dataset had trouble loading. 

The date set is loaded the but screen didn't change. Go to the top of the screen and select the dataset 2xfm_0009.mda from the drop down box.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-09.png)

You can use that drop down box to switch between loaded datasets. Note that the screen shows a different main display. You can now view the **Integrated Spectra** , the **Scalers**, and **Extra PV's** that are in this dataset. 

The Integrated spectra is displayed in **log10** by default.

The data we are interested in is between **1 kEv - 11 kEv** . We can zoom in on the area by **Left click** around 1 kEv and **Drag** the mouse the 11 kEv and **release** the mouse button.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-11.png)

Note that this dataset has **4 detectors** since this was collected with a 4 element detector. Some beamlines have upgrade to 7 element detectors. 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-13.png)

The bottom table has you **Fit Parameters** and **Fit Elements** . The fit parameters are used to change the shape of you spetra model. The fit elements are the elements you want to fit for. 

Lets **click** the **Model Spectra** button on the bottom

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-12-model.png)

This will add a new line to the spectra view called Model Spectra in orange. 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-12-model.png)

You can make changes to the fit parameters and fit elements and then click Model Spectra again to update the model. There is a shortcut you can use so you don't have to keep clicking Model Spectra. If you **check** the **Auto update model** box, this will automatically update the model (same as clicking the model button) whenever you change a fit parameter or element. 

Lets select the **Fit Elements** tab to be able to change their properties. 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-12-elements-2.png)

Note that all the elements you add to this table will appear on the top of the Spectra view ( except it will not show pile-up labels or it would get too crouded).

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-12-elements-1.png)

Scroll to element **Ar** and change the value to **6.0** . Note that this value is in **log10**. 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-15.png)

