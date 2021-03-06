#-- GAUDI jobOptions generated on Fri Aug  1 14:32:17 2014
#-- Contains event types : 
#--   90000000 - 344 files - 16032826 events - 1652.75 GBytes


#--  Extra information about the data processing phases:

from Gaudi.Configuration import * 
from GaudiConf import IOHelper
IOHelper('ROOT').inputFiles(['LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000267_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000268_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000269_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000270_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000271_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000272_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000273_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000274_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000275_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000276_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000277_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000278_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000279_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000280_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000281_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000282_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000283_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000285_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000286_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000287_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000288_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000290_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000291_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000292_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000293_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000295_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000296_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000298_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000299_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000301_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000302_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000303_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000304_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000305_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000307_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000308_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000309_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000310_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000311_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000312_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000313_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000314_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000315_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000316_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000317_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000318_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000319_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000320_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000322_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000323_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000324_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000325_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000326_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000327_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000328_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000329_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000330_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000331_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000332_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000333_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000334_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000336_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000337_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000338_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000339_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000341_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000342_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000343_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000344_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000346_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000347_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00000348_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003623_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003624_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003625_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003626_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003627_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003628_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003629_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003630_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003631_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003632_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003633_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003634_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003635_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003636_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003637_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003638_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003639_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003640_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003641_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003642_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003643_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003644_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003645_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003646_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003647_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003648_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003649_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003650_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003651_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003652_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003653_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003654_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003655_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003656_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003657_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003658_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003659_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003660_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003661_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003662_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003663_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003664_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003665_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003666_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003667_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003668_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003669_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003670_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003671_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003672_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003673_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003674_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003675_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003676_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003677_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003678_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003679_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003680_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003681_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003682_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003683_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003684_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003685_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003686_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003687_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003688_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003689_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003690_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003691_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003692_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003693_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003694_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003695_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003696_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003697_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003698_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003699_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003700_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003701_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003702_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003704_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003706_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003709_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003710_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003711_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003713_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003714_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003715_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003716_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003717_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003718_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003719_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003720_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003721_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003722_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003723_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003724_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003727_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003728_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003729_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003730_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003731_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003732_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003733_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003734_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003735_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003736_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003737_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003738_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003739_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003740_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003741_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003742_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003743_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003744_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003745_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003746_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003747_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003748_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003749_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003750_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003751_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003752_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003753_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003754_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003755_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003756_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003757_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003758_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003759_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003760_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003761_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003762_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003763_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003764_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003765_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003766_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003767_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003769_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003770_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003773_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003774_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003775_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003776_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003777_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003778_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003779_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003780_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003781_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003782_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003783_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003784_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003786_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003787_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003788_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003789_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003790_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003791_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003792_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003793_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003794_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003795_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003796_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003797_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003799_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003800_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003801_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003802_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003803_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003805_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003806_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003808_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003809_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003810_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003811_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003812_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003813_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003814_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003815_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003816_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003817_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003818_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003819_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003820_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003822_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003823_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003824_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003825_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003826_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003827_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003828_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003829_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003830_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003832_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003833_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003834_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003835_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003836_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003838_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003839_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003840_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003841_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003842_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003843_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003844_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003845_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003846_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003847_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003848_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003849_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003850_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003851_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003852_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003853_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003854_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003855_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003856_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003857_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003858_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003859_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003864_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003865_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003867_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003868_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003869_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003870_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003871_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003872_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003873_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003874_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003875_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003876_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003877_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003878_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003879_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003880_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003881_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003882_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00003883_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016637_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016657_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016658_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016666_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016794_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016832_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016833_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016849_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016884_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016885_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016886_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016887_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00016947_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00017003_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00017004_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00017005_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00017006_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00017185_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0001/00020736_00017192_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0002/00020736_00022829_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00037946_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00037947_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038108_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038128_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038195_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038419_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038450_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038567_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038568_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00038958_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0003/00020736_00039707_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0004/00020736_00040225_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0004/00020736_00042710_1.full.dst'
], clear=True)

FileCatalog().Catalogs = [ 'xmlcatalog_file:$STRIPPINGSELECTIONSROOT/tests/data/Reco14_Run130393.xml' ]
