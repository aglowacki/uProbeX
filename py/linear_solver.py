
'''
d_dict = dictionary of parameters. Key = string, value = double
dict_min_coef
dict_options
dict_transform_coef
'''
from scipy.optimize import curve_fit
import numpy

#Place holders for our output tuple
X = 0
Y = 1
Z = 2
MX = 3
MY = 4

g_transDic = dict()
g_varIndexes = dict()
g_list_coord_points = []

g_dict_min_coef = {'SlopeX': 0.0,
                   'SlopeY': 0.0,
                   'InterceptX': 0.0,
                   'InterceptY': 0.0,
                   'SlopeXY': 0.0,
                   'InterceptXY': 0.0,
                   'SlopeYX': 0.0,
                   'InterceptYX': 0.0,
                   'm2xfm_x': 0.0,
                   'm2xfm_y': 0.0
                   }

g_options = {'NA': 0}

def getCoefDict():
    return g_dict_min_coef

def getOptionsDict():
    return g_options

#my custom transform function
def my_transform(d_params, inX, inY, inZ):
	#initialize our output variable
    out = [0.0, 0.0, 0.0]
    xPred = inX * d_params['SlopeX'] + d_params['InterceptX']
    yPred = inY * d_params['SlopeY'] + d_params['InterceptY']
    #calculate X
    out[X] = xPred + (yPred * d_params['SlopeYX'] + d_params['InterceptYX'] )
    #calculate Y
    out[Y] = yPred + (xPred * d_params['SlopeXY'] + d_params['InterceptXY'] )
    #calculate Z
    out[Z] = 0.0
    #return output
    return out

def fitFunc(x, slope, intercept):
    return slope * x + intercept

def my_solver( dict_min_coef, dict_options, dict_transform_coef, list_coord_points):
    #First move point data to separate lists
    x = []
    y = []
    mx = []
    my = []
    xDelta = []
    yDelta = []
    for t in list_coord_points:
        x+=[t[X]]
        y+=[t[Y]]
        mx+=[t[MX]]
        my+=[t[MY]]
    #print( 'x = ',x)
    #print( 'y = ',y)
    #print( 'mx = ',mx)
    #print( 'my = ',my)
    '''
    #Get the X slope and intercept
    '''
    p0 = [0.0, 0.0]
    coeff, var_matrix = curve_fit(fitFunc, x, mx, p0=p0)
    dict_min_coef['SlopeX'] = coeff[0]
    dict_min_coef['InterceptX'] = coeff[1]
    xPred = [fitFunc(xx, coeff[0], coeff[1]) for xx in x]
    for i in range(len(list_coord_points)):
        xDelta += [ list_coord_points[i][MX] - xPred[i] ]
    '''
    #Get the Y slope and intercept
    '''
    p0 = [0.0, 0.0]
    coeff, var_matrix = curve_fit(fitFunc, y, my, p0=p0)
    dict_min_coef['SlopeY'] = coeff[0]
    dict_min_coef['InterceptY'] = coeff[1]
    yPred = [fitFunc(xx, coeff[0], coeff[1]) for xx in y]
    for i in range(len(list_coord_points)):
        yDelta += [ list_coord_points[i][MY] - yPred[i] ]
    '''
    #Get the XY slope and intercept
    '''
    p0 = [0.0, 0.0]
    coeff, var_matrix = curve_fit(fitFunc, xPred, yDelta, p0=p0)
    dict_min_coef['SlopeXY'] = coeff[0]
    dict_min_coef['InterceptXY'] = coeff[1]
    '''
    #Get the YX slope and intercept
    '''
    p0 = [0.0, 0.0]
    coeff, var_matrix = curve_fit(fitFunc, yPred, xDelta, p0=p0)
    dict_min_coef['SlopeYX'] = coeff[0]
    dict_min_coef['InterceptYX'] = coeff[1]
    #print( 'xPred = ',xPred)
    #print( 'yPred = ',yPred)
    #print( 'xDelta = ',xDelta)
    #print( 'yDelta = ',yDelta)
    #plot results
    if 'plot' in dict_options:
        yfit = [fitFunc(xx, dict_min_coef['SlopeXY'], dict_min_coef['InterceptXY']) for xx in xPred]
        #print( 'yfit = ',yfit)
        import matplotlib.pyplot as plt
        plt.plot(xPred, yDelta, 'ro', label='Test data')
        plt.plot(xPred, yfit , label='Fitted data')
        plt.show()
    return dict_min_coef

if __name__ == '__main__':
    dicTrans = {'SlopeX':0.0,'InterceptX':0.0,'SlopeY':0.0,'InterceptY':0.0, 'SlopeYX': 0.0, 'InterceptYX': 0.0, 'SlopeXY': 0.0, 'InterceptXY': 0.0}
    dicMin = {'SlopeX':0.0,'InterceptX':0.0,'SlopeY':0.0,'InterceptY':0.0, 'SlopeYX': 0.0, 'InterceptYX': 0.0, 'SlopeXY': 0.0, 'InterceptXY': 0.0}
    dicOpt = {'plo':1.0}
    coords = ((71.486, 42.560, 0.0, 0.412, 3.102), (71.468, 43.090, 0.0, 0.4312, 2.5696), (71.320, 42.325, 0.0, 0.5862, 3.3343), (71.381, 42.390, 0.0, 0.523, 3.269), (70.809, 43.099, 0.0, 1.0906, 2.5590), (70.924, 43.129, 0.0, 0.978, 2.529), (71.942, 42.048, 0.0, -0.0373, 3.6134), (71.874, 42.601, 0.0, 0.029, 3.060), (71.415, 43.578, 0.0, 0.485, 2.083), (72.017, 43.216, 0.0, -0.118, 2.449))
    dicTrans = my_solver(dicMin, dicOpt, dicTrans, coords)
    print( dicTrans)
    print( my_transform(dicTrans, 71.486, 42.560, 0.0))

