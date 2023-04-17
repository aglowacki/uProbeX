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

![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/roi_07.png)


![This is an image](https://github.com/aglowacki/uProbeX/blob/master/docs/images/roi_08.png)


