import pathlib, subprocess
pool=pathlib.Path('/mnt/d/BaiduNetdiskDownload/紫光同创杯赛题三参考素材')
stage=pathlib.Path('/mnt/d/Study_Object/image_recognition/image_recognition/output/poster_frames_20261119')
mountain=next(pool.glob('20261119-0716*.mp4'))
for name, start in [('mountain_left',2),('mountain_right',47)]:
 subprocess.run(['ffmpeg','-v','error','-nostdin','-y','-threads','2','-ss',str(start),'-i',mountain,'-t','4.4','-vf','fps=12,scale=640:360','-an','-c:v','libx264','-threads','2','-preset','fast','-crf','26',stage/(name+'.mp4')],check=True)
 print(name,(stage/(name+'.mp4')).stat().st_size,flush=True)
