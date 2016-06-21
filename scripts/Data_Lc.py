from Configurables import CondDB
from DecayTreeTuple.Configuration import *
from Configurables import TupleToolKinematic
from Configurables import TupleToolTrackInfo
from Configurables import TupleToolANNPID
from Configurables import TupleToolEventInfo
from Configurables import DaVinci
from Configurables import TupleToolPid
from Configurables import DstConf
from Configurables import TupleToolPrimaries
from Configurables import TupleToolTISTOS
from Configurables import TupleToolRecoStats
from Configurables import TupleToolPropertime
from Configurables import TupleToolGeometry

CondDB('CondDB',
       LatestGlobalTagByDataType = '2015')

DstConf('DstConf',
        Turbo = True)

TupleToolEventInfo('Lambda_cToKppiTuple.TupleToolEventInfo')

TupleToolTrackInfo('Lambda_cToKppiTuple.TupleToolTrackInfo')

TupleToolPrimaries('Lambda_cToKppiTuple.TupleToolPrimaries')

TupleToolKinematic('Lambda_cToKppiTuple.TupleToolKinematic')

TupleToolRecoStats('Lambda_cToKppiTuple.TupleToolRecoStats')

TupleToolANNPID('Lambda_cToKppiTuple.TupleToolANNPID')

TupleToolPid('Lambda_cToKppiTuple.TupleToolPid')

TupleToolGeometry('Lambda_cToKppiTuple.TupleToolGeometry')

TupleToolPropertime('Lambda_cToKppiTuple.TupleToolPropertime')

DecayTreeTuple('Lambda_cToKppiTuple',
               Inputs = ['Hlt2CharmHadLcpToPpKmPip_LTUNBTurbo/Particles'],
               WriteP2PVRelations = False,
               ToolList = [],
               Decay = '[Lambda_c+ -> ^K- ^p+ ^pi+]CC')
DecayTreeTuple('Lambda_cToKppiTuple').addTupleTool(TupleToolPrimaries('Lambda_cToKppiTuple.TupleToolPrimaries'))
DecayTreeTuple('Lambda_cToKppiTuple').addTupleTool(TupleToolPid('Lambda_cToKppiTuple.TupleToolPid'))
DecayTreeTuple('Lambda_cToKppiTuple').addTupleTool(TupleToolGeometry('Lambda_cToKppiTuple.TupleToolGeometry'))
DecayTreeTuple('Lambda_cToKppiTuple').addTupleTool(TupleToolANNPID('Lambda_cToKppiTuple.TupleToolANNPID'))
DecayTreeTuple('Lambda_cToKppiTuple').addTupleTool(TupleToolRecoStats('Lambda_cToKppiTuple.TupleToolRecoStats'))
DecayTreeTuple('Lambda_cToKppiTuple').addTupleTool(TupleToolTrackInfo('Lambda_cToKppiTuple.TupleToolTrackInfo'))
DecayTreeTuple('Lambda_cToKppiTuple').addTupleTool(TupleToolPropertime('Lambda_cToKppiTuple.TupleToolPropertime'))
DecayTreeTuple('Lambda_cToKppiTuple').addTupleTool(TupleToolKinematic('Lambda_cToKppiTuple.TupleToolKinematic'))
DecayTreeTuple('Lambda_cToKppiTuple').addTupleTool(TupleToolEventInfo('Lambda_cToKppiTuple.TupleToolEventInfo'))

DecayTreeTuple('Lambda_cToKppiTuple').addBranches({'lab0': '[Lambda_c+ -> K- p+ pi+]CC'})

TupleToolTISTOS('Lambda_cToKppiTuple.lab0_TupleToolTISTOS',
                Verbose = True,
                VerboseHlt1 = True,
                VerboseHlt2 = True,
                TriggerList = ['Hlt2CharmHadLcpToPpKmPip_LTUNBTurboDecision'],
                VerboseL0 = True)

DecayTreeTuple('Lambda_cToKppiTuple').lab0.ToolList = []

DecayTreeTuple('Lambda_cToKppiTuple').lab0.addTupleTool(TupleToolTISTOS('Lambda_cToKppiTuple.lab0_TupleToolTISTOS'))

DaVinci('DaVinci',
        DataType = '2015',
        TupleFile = 'DVTuples.root',
        UserAlgorithms = [DecayTreeTuple('Lambda_cToKppiTuple')],
        RootInTES = '/Event/Turbo',
        HistogramFile = 'DVHistos.root',
        Lumi = True,
        InputType = 'MDST')

dv = DaVinci('DaVinci')

