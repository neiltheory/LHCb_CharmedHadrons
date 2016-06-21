
def make_job(name, optsfile, data) :
    dv = DaVinci(optsfile = [optsfile])

    j = Job(name = name, application = dv, inputdata = data,
            backend = Dirac(), outputfiles = [LocalFile('*.root')],
            splitter = SplitByFiles(filesPerJob = 500)
    return j

optsfile = '/nfs/lhcb/malexander01/charm/baryon-lifetimes-2015/scripts/Data_Lc.py'
datafiles = '/nfs/lhcb/malexander01/charm/baryon-lifetimes-2015/data/run-II-data/LHCb_Collision15_Beam6500GeVVeloClosedMagDown_Real_Data_Turbo02_94000000_TURBO.MDST.py', \
    '/nfs/lhcb/malexander01/charm/baryon-lifetimes-2015/data/run-II-data/LHCb_Collision15_Beam6500GeVVeloClosedMagUp_Real_Data_Turbo02_94000000_TURBO.MDST.py'

def make_dataset(datafile) :
    gangadata = datafile.replace('.py', '.ganga')
    if not os.path.exists(gangadata) :
        data = DaVinci().readInputData(datafile)
        # tried using box to store the data, but it seems buggy. 
        export(data, gangadata)
    else :
        data = load(gangadata)[0]
    return data

def make_jobs() :
    js = []
    for datafile in datafiles :
        data = make_dataset(datafile)
        j = make_job(os.path.split(datafile)[1][:-3], optsfile, data)
        js.append(j)
    return js
