from pathlib import Path
import subprocess
pool=Path('/mnt/d/BaiduNetdiskDownload/紫光同创杯赛题三参考素材'); stage=Path('/mnt/d/Study_Object/image_recognition/image_recognition/output/poster_frames_20261119')
for video,stamp in [('112454',.60),('112454',2.24),('113539',18.38),('113539',70.37),('113539',71.13),('113539',75.11),('113421',8.57),('112507',40.46),('112507',41.67),('113343',10.14),('113343',10.9)]:
 subprocess.run(['ffmpeg','-v','error','-nostdin','-y','-threads','2','-ss',str(stamp),'-i',pool/('video_20260929_'+video+'.mp4'),'-vf','scale=640:360','-frames:v','1',stage/('ped_'+video+'_'+str(stamp)+'.jpg')],check=True)
