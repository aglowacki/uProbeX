
'''
d_dict = dictionary of parameters. Key = string, value = double
dict_min_coef
dict_options
dict_transform_coef
'''
import math
from scipy.optimize import minimize

#Place holders for our output tuple
X = 0
Y = 1
Z = 2
MX = 3
MY = 4

g_transDic = dict()
g_varIndexes = dict()
g_list_coord_points = []

#my custom transform function
def my_transform(d_params, inX, inY, inZ):
	#initialize our output variable
	out = [0.0, 0.0, 0.0]
	#calculate X
	out[X] = -( ( inX -  d_params['offset_c'] ) * math.sin( ( d_params['angle_alpha'] + d_params['omega_prime'] ) / 180.0 * math.pi ) / math.sin( d_params['omega_prime'] / 180.0 * math.pi ) - ( inY - d_params['offset_d'] ) * math.sin( ( d_params['omega'] - d_params['omega_prime'] - d_params['angle_alpha'] ) / 180.0 * math.pi ) / math.sin( d_params['omega_prime'] / 180.0 * math.pi ) ) * d_params['scaling_XFM_X'] + d_params['offset_a'] + ( inZ - d_params['z_offset'] ) * d_params['z_lin_x'] - d_params['m2xfm_x']
	#calculate Y
	out[Y] = -( -( inX -  d_params['offset_c'] ) * math.sin( d_params['angle_alpha']  / 180.0 * math.pi ) / math.sin( d_params['omega_prime'] / 180.0 * math.pi ) + ( inY - d_params['offset_d'] ) * math.sin( ( d_params['omega_prime'] + d_params['angle_alpha'] ) / 180.0 * math.pi ) / math.sin( d_params['omega_prime'] / 180.0 * math.pi ) ) * d_params['scaling_XFM_Y'] + d_params['offset_b'] + ( inZ - d_params['z_offset'] ) * d_params['z_lin_y'] - d_params['m2xfm_y']
	#calculate Z
	out[Z] = 0.0
	#return output
	return out

def NelderMeadTotalError(minList):
   global g_transDic
   global g_varIndexes
   global g_list_coord_points
   dxSum = 0.0
   dySum = 0.0
   lSize = len(g_list_coord_points)
   for key, value in g_varIndexes.iteritems():
      g_transDic[key] = minList[value]
   for p in g_list_coord_points:
      tRet = my_transform(g_transDic, p[X], p[Y], p[Z])
      dx = (tRet[X] - p[MX]) * 1000.0
      dxSum += dx * dx
      dy = (tRet[Y] - p[MY]) * 1000.0
      dySum += dy * dy
   dxSum /= lSize
   dySum /= lSize
   return dxSum + dySum

def my_solver( dict_min_coef, dict_options, dict_transform_coef, list_coord_points):
   global g_transDic
   global g_varIndexes
   global g_list_coord_points
   #f = file('nmSolver.log', 'w')
   #f.write( 'minDict = '+str(dict_min_coef)+'\r\n')
   #initialize global variables to be used in solver functions
   # init transform coef
   for key, value in dict_transform_coef.iteritems():
      g_transDic[key] = dict_transform_coef[key]
   # init min value list
   minList = []
   i = 0
   for key,value in dict_min_coef.iteritems():
      minList += [value]
      g_varIndexes[key] = i
      i+=1
   # init coord points
   g_list_coord_points = list_coord_points
   #debug writes
   #f.write( 'minList = '+ str( minList )+'\r\n')
   #f.write( 'g_list_coord_points = '+ str(g_list_coord_points) +'\r\n')
   #f.write( 'len g list = ' + str( len(g_list_coord_points) ) +'\r\n')
   #Run the solver

   #res = minimize(NelderMeadTotalError, minList, method='Nelder-Mead')
   res = minimize(NelderMeadTotalError, minList, method='SLSQP')

   #f.write( 'res.success = '+str(res.success)+'\r\n')
   #update results and return
   for key, value in g_varIndexes.iteritems():
      dict_min_coef[key] = res.x[value]
   #f.write( 'minDict = '+str(dict_min_coef)+'\r\n')
   #f.close()
   return dict_min_coef

if __name__ == '__main__':
   dicTrans = {'angle_alpha':1.19,'offset_a':10.27,'offset_b':2.13,'offset_c':0.0,'offset_d':0.0,'omega':90.0,'omega_prime':90.04,'scaling_XFM_X':1.03,'scaling_XFM_Y':1.0,'z_offset':0.0,'z_lin_x':-0.18,'z_lin_y':0.02,'m2xfm_x':-64.1418,'m2xfm_y':-41.8941}
   dicTrans2 = {'angle_alpha':1.0,'offset_a':10.27,'offset_b':2.13,'offset_c':0.0,'offset_d':0.0,'omega':95.0,'omega_prime':94.04,'scaling_XFM_X':1.03,'scaling_XFM_Y':1.0,'z_offset':0.0,'z_lin_x':-0.18,'z_lin_y':0.02,'m2xfm_x':-64.1418,'m2xfm_y':-41.8941}
   dicMin = {'m2xfm_x':-64.1418,'m2xfm_y':-41.8941}
   dicOpt = {'op':1.0}
   coords = ((60.0, 40.0, 0.0, -11.7, 5.2), (-77.0, -42.0, 0.0, 154.6, 84.4))
   #print my_transform(dicTrans, -77.0, -42.0, 0.0)
   #print my_solver(dicMin, dicOpt, dicTrans, coords)
   new_coef = my_solver(dicTrans2, dicOpt, dicTrans, coords)
   print 'o_coef ', dicTrans
   print 'n_coef ', new_coef
   for k,v in dicTrans.iteritems():
      print k, v - new_coef[k]

