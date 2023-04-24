# uProbeX GUI for XRF-Maps

## Working with ROI's

Open uProbeX and load a dataset you would like to perform ROI processing on. 
On the right side of the screen select the **ROI** tab and click the **ROI Dialog** button

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/roi_01.png)

This will bring up the ROI dialog screen.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/roi_02.png)

On the top left you will have a list of elements and scalers you can select from. You are allowed to select a single map or sum multiple maps.
In this example we will first select **K** map.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/roi_03.png)

This will show you the elemental map using the same colormap from the previous window. Let's take a look at element **Ti** by itself. I will uncheck **K**. 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/roi_04.png)

Now lets select both **K** and **Ti** 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/roi_05.png)

Notice that we lost **Ti**. This is because we are just summing up the two maps and there are a lot more counts for **K** than **Ti**. There is an option to **Normalize before summing** which will rescale the selected maps before summing them.  

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/roi_06.png)

After normalizing you can see that we can see **Ti** again at the bottom right. We are currently on the **KMeans** tab. This will run the KMeans nearest neighbor algorithm to segment the image. 
- [OpenCV KMeans](https://docs.opencv.org/3.4/d1/d5c/tutorial_py_kmeans_opencv.html)
Update the parameters to your preferences and select **Run** . It will run the algorithm and generate ROI's

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/roi_07.png)

This generated 2 ROI's and color coded them Red and Green. In this example I am only interested in the Green ROI_2 so I am going to right click on the first ROI and select delete.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/roi_08.png)

This will leave us with 1 ROI. 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/roi_09.png)

You can also rename the ROI's by double clicking the name.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/roi_10.png)

You have a ability to manually paint your own ROI's also. Let's select the **Manual** tab to the right of KMeans. 
Under **Action** we will select **Brush Erase** to remove parts of this ROI.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/roi_11.png)

Now move the mouse cursor over the parts you wasnt to subtract from the current ROI. **Left Click and drag** the mouse to remove.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/roi_12.png)

You can also manually add new ROI's by selecting **New ROI** button. This adds **ROI_3** with the color blue. Remeber to change the **Action** to **Brush Add** and **Left click and drag** to draw a new ROI. 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/roi_13.png)

This is a bit hard to see with the current Color Map so lets **double click** the blue color square and select a new color.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/roi_14.png)

When we are satisfied with the ROI's we have made, we can select **Accept** on the bottom left of the dialog. We can always come back to this dialog windows to create, edit, or delete ROI's by clicking **ROI Dialog** button. 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/roi_15.png)

Note that it may take a little while after selecting Accept since the application will open the raw spectra and integrate the ROI's to be displayed under the **Integraded Spectra** tab. Let's go take a look by clicking Integrade Spectra tab.

Note that in order to see ROI's on the integrated spectra tab, you need to have the **ROI** tab selected. If the **Annotations** tab is selected, then the ROI's will not be displayed on the Counts or Integrated Spectra tabs.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/roi_16.png)

We can see the integrated ROI's with a label at the top of the plot and the same color. We can individually fit Integrated ROI's by selecting **Fit ROI Spectra** to bring up the fitting dialog. 
If you have more that one ROI, a dialog will pop up asking which one you would like to fit. 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/roi_17.png)

In this case we will fit ROI_K_Ti

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/roi_18.png)

This will bring up the fitting dialog. The ROI will be called Integrated Spectra. Lets select **Run** to fit this ROI.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/roi_19.png)

When finished you should have a **New Fit Spectra** that should overlap the ROI Integrated Spectra. To save these results to files. Select **Accept**. Once the files are finished exporting, a file window will open pointing to the location they were saved. 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/roi_20.png)

This can be combersome if you have many ROI's in many datasets. To perform a **Batch ROI processing** you can go to the files on the left of the screen and select **Batch ROI**.

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/roi_21.png)

This will scan for all dataset's with ROI's and bring up a dialog with a list of them. 

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/roi_22.png)

Select **Run** to process all ROI's in the displayed dataset files. The output will be saved in the **output** folder.

