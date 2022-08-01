# uProbeX GUI for XRF-Maps

## Loading a Workspace

### Dataset links 
- [XRF-Demo](https://anl.box.com/s/zb8kzyaz0rcco927lvjbisprpc58q4xt) : 574 MB compressed download, 20 GB uncompressed
- [XRF-Demo-mini](https://anl.box.com/s/ls2k20s1qbebm0089tolnjp71c1mzz22) : 7.3 MB compressed download, 185 MB uncompressed
- [XRF-Demo2](https://anl.box.com/s/u5gp0n1vgjpg4ufnmhbh3rz6z2njpgrd) : 2.5 GB compressed download, 30 GB uncompressed
- [XRF-Demo2-mini](https://anl.box.com/s/194dd7ldoj9lmbhpue2p33w8hf3ppo3d) : 158 MB compressed download , 1.25 GB uncompressed

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

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-14-model.png)

You can make changes to the fit parameters and fit elements and then click Model Spectra again to update the model. There is a shortcut you can use so you don't have to keep clicking Model Spectra. If you **check** the **Auto update model** box, this will automatically update the model (same as clicking the model button) whenever you change a fit parameter or element. 

Lets select the **Fit Elements** tab to be able to change their properties. 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-12-elements-2.png)

Note that all the elements you add to this table will appear on the top of the Spectra view ( except it will not show pile-up labels or it would get too crouded).

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-12-elements-1.png)

Scroll to element **Ar** and change the value to **6.0** . Note that this value is in **log10**. 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-15.png)

If you would like to add new elements to fit, you can on the bottom right side of this table. You have the options to add element by their K,L,or M shell. Let's add and element. Select the new element (in this example V) and select **Add Element**

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-16.png)

Note that when you select the element, the graph will add the Ka1, Ka2, Kb1 ... lines to show you where the element lines are. It is color coded by Ka, Kb, L1, L2, L3. 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-17.png)

If you would like to remove an element from fitting, you can select the element in the table on and click Delete Element. 

Doing this by hand can be time consumming so we are going to use an iterative fitting algorithm to do this for us. There are 2 main ones and and experimental one. 

**Levenberg-Marquardt** Will try to fit you integrated spectra **with out** max and min contraints

**MPFit** Will try to fit you integrated spectra **with** max and min contraints. 

Let click the **Fit Spectra** button. 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-17-fit.png)

This will bring up the fitting dialog.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-18.png)

On the top you will have your integrated spectra with the previous fit (it will be 0's if this is the first time fitting). 

The left table are you fitting parameters that we are going to fit. You can make changes to these still before fitting. 

The bottom options are parameters for the least squares fitting algorithm.

Click **Run** on the bottom to start fitting.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-19.png)

The top graph will add a **New Fit Spectra** .

The right table will fill in with the output fit parameters from the alorythm. You can compare with the left table to see the differences.

If you like the results you can click **Accept** and the new fit paramters will be used. If you are displeased with the results, you can change some of the fitting optoins and rerun. If you click Cancel, it will keep your old fitting paramters.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-20.png)

If you accepted the results you will be brought back to the previous screen with values filled in. This fit parameters are only for the selected detector (detector 0 in this case). 

You can see more details about the fitting if you select the **setting gear icon**

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-20-settings.png)

This will bring up the display settings.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-21.png)

Select the **Show Details Fit Spectra** check box and select accept.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-22.png)

Now the graph will display Ka, La, ect as new spectra. 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-23.png)

These new fit parameters are in computer memory. To use them in the whole dataset per pixel fitting, we need to **Export Fit Parameters**. 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-23-export.png)

If you are interested in 2d Scaler meta data in this dataset. You can select the **Scalers** Tab and select the drop down to select the specific scaler to look at. 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-24.png)

There are also single values scaler values in the **Extra PV's** tab.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-25.png)

Now that we have optimized out fit parameters for this dataset detector 0. Let's perform a per pixel analysis. **Right click** on the dataset name and select **Per Pixel Process**

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-26.png)

This will bring up your per pixel fitting dialog.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-27.png)

The top row is the detectors you want to fit. **Remove** all except for **0** since we only want to fit the first detector. If you have optimized the fit parameters for each detector then you can process all of them here.

The processing options group has 3 main fitting routines we currently support. 

**ROI** is a general estimate. It will treat the Ka of the element as a center and will sum the bins between -.1 kEv and +.1 kEv . This does not take into account the Kb or the other L lines. 

**Non-Negative Least Squares** is a modified Singual Value Decomposition fitting that does not allow negative values. Seems to be just as precise as Matrix fitting but is a lot faster.

**Per Pixel Matrix Analysis** is an iterative method that is precise but can take many iteration so it slow.

For this example only check **Non-Negative Least Squares** . 

The Export Options group has options for other information you wish to save. 

**Generate Avg H5** will take all the detectors you have listed at the top and create an average H5 file from them.

**Add v9 soft links** will create hdf5 links so the processed h5 dataset's can be opened in IDL MAPS. 

**Add Exchange format** is used when for when you scanned an XRF Tomography dataset so that it can be imported in TomoPy for reconstruction. 

**Save CSV** will save a CSV file of the integrated spectra for each detector.

**Perform Quantification** will load maps_standardinfo.txt file and create a mass calibration curve to be able to quantify the analyzed data from counts per second to micro grams per centimeter squared.

Click **Run** and wait for the fitting to finish. 
When finished click **Close** to close this dialog. 

To display the new analyzed results. Select the **Analyzed Data** on the files tab. **Right click** on the name **2xfm_0009.h50** . Note the 0 on the end of the dataset represents detector 0. If you process other detectors they will have similar naming. The averaged file will simply be .h5 . 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-28.png)

This will load the dataset and display it.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-29.png)

Before looking at the results, lets take a quick look at the **Integrated Spectra** tab to see how well our fit is.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-30.png)

Note the orange line **Fitted_Int_NNLS**. This is the sum of the each fitted spectra. If this aligns well over your integrated spectra then you have a good fit. If you notice some area that is not fit well, you can change fit parameters or add/remove elements , export fit parameters, and rerun the per pixel processing. 

Lets go back to **Analyzed Counts** tab to view out elemental maps. Select the **Grid Icon** 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-29-grid.png)

To bring up the multi-element view options.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-31.png)

This allows you to create a Rows x Cols grid view to display multiple elements at once. Select something resonable for your computer screen display. This demo will use 3 x 3.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-32.png)

We want to look at the NNLS data so if your analyzed dataset has multiple analysis types, you can switch between them with this drop down box.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-33.png)

The current display is in counts per second. If you ran analysis with **Perform Quantification** you will be able to normalize your data by the upstream ION chamber: **US_IC** or the downstream ION chamber: **DS_IC**

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-34-1.png)


![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-35-1.png)

The bottom of each element view has a drop down box that allows you to select all the elements we fit and scalers that were in the raw dataset. 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-36.png)

While looking through you may notice that **COMPTON_AMPLITUTE** looks very dark. This is because the last two rows seem to have very large values. You can change the contrast per view by **un-checking** the **Global Contrast** checkbox. 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-37-cont.png)

This will make new **C**  button visible per view. Click it to bring up the **Contrast dialog**.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-38-c.png)

Set the **Max** text box to **2000** and press the **Enter** button on your keyboard.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-39.png)

Select **Accept** to keep the settings.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-40.png)

Note that the other views did not change but only this one. To the right of the **Global Contrast** you can change the contrast for all the views if it is selected. 

Lets go back to our visible light microscope dataset by selecting the top drop down box to **8-20x-2.tif** . 

**Right click** on **Micro Probe Region1** and select **Zoom to Region** to zoom the view into the selected region.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-41.png)

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-42.png)

We want to link this image with the **2xfm_0009.mda.h5** datasets. To do this, **Right click** on the **Micro Probe Region1** dataset again, this time select **Link to dataset**.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-43.png)

and select the dataset **2xfm_0009**

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-44.png)

We now need to **Unload** and re**Load** the dataset for this to take effect.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-45.png)

Once loaded, you can select the **Micro Probe Region1** in the element selector drop down box.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-46.png)

This will now display the VLM link along with your elements.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-47.png)

In the future we will have a co-localization view that will allow you to overlay multiple views into one. 

If you would like to export these as images or ascii values. You can select the **Export** button.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-47-export.png)

This will bring up the export images dialog 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-48.png)

By default the images will be exported to the workspace_dir/img.dat/export/dataset_name

Select **Export** to start the export process. When it is finished it will bring up a folder of where it was exported. 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/uProbeX-49.png)

