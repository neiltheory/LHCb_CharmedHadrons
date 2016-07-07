from getResVars_neil import *
from array import array
from ROOT import TFile, TTree
filename = "~/Documents/uni/LHCb_CharmSummerProj/learning_root/turbo_2015_data.root"

f = TFile.Open(filename, "read")
tree = f.Get("Lambda_cToKppiTuple/DecayTree;1")

parent = TLorentzVector(0,0,1,1)

# Mass bounds
lower_bound = 2216
upper_bound = 2356

# fout = open("test_output.txt", "w")
treefile = TFile('AngularTreefile.root', 'recreate')
newtree = TTree('newtree','Tree with angular dependancies')
#newtree.SetDirectory(0)

#Lambdac_sph1 = array('f', [0])
#Lambdac_sh1h2 = array('f', [0])
#Lambdac_sph2 = array('f', [0])
proton_LcRest_costheta = array('f', [0])
proton_LcRest_cosphi = array('f', [0])
Lambdac_LcRest_thetah1h2 = array('f', [0])


#newtree.Branch('Lambdac_sph1',Lambdac_sph1,'Lambdac_sph1/F')
#newtree.Branch('Lambdac_sh1h2',Lambdac_sh1h2,'Lambdac_sh1h2/F')
#newtree.Branch('Lambdac_sph2',Lambdac_sph2,'Lambdac_sph2/F')
newtree.Branch('proton_LcRest_costheta',proton_LcRest_costheta,'proton_LcRest_costheta/F')
newtree.Branch('proton_LcRest_cosphi',proton_LcRest_cosphi,'proton_LcRest_cosphi/F')
newtree.Branch('Lambdac_LcRest_thetah1h2',Lambdac_LcRest_thetah1h2,'Lambdac_LcRest_thetah1h2/F')

for leaf in tree:

	'''
	if leaf.Lambda_cplus_M < lower_bound or leaf.Lambda_cplus_M > upper_bound:
		continue
	'''

	lab_lamc = TLorentzVector(
		leaf.Lambda_cplus_PX,
		leaf.Lambda_cplus_PY,
		leaf.Lambda_cplus_PZ,
		leaf.Lambda_cplus_PE
	)

	lab_p = TLorentzVector(
		leaf.pplus_PX,
		leaf.pplus_PY,
		leaf.pplus_PZ,
		leaf.pplus_PE
	)

	lab_h1 = TLorentzVector(
		leaf.Kminus_PX,
		leaf.Kminus_PY,
		leaf.Kminus_PZ,
		leaf.Kminus_PE
	)

	lab_h2 = TLorentzVector(
		leaf.piplus_PX,
		leaf.piplus_PY,
		leaf.piplus_PZ,
		leaf.piplus_PE
	)

	tpl = getResVars_neil(parent,lab_lamc,lab_p,lab_h1,lab_h2)

#	Lambdac_sph1[0] = tpl[0]
#	Lambdac_sh1h2[0] = tpl[1]
#	Lambdac_sph2[0] = tpl[2]
	proton_LcRest_costheta[0] = tpl[0]
	proton_LcRest_cosphi[0] = tpl[1]
	Lambdac_LcRest_thetah1h2[0] = tpl[2]

	newtree.Fill()

treefile.Write()

'''	
	fout.write(
		str(tpl[0]) + "," +
		str(tpl[1]) + "," +
		str(tpl[2]) + "," +
		str(tpl[3]) + "," +
		str(tpl[4]) + "," +
		str(tpl[5]) + "\n"
	)

fout.close()
'''



