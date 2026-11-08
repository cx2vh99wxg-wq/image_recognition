import os
import unittest
from unittest.mock import patch
from prepare_display import plan,prepare

def fixture(width=3840,height=2160,primary=False,current='30.00',star4k=True):
    return f'''Screen 0: current 4560 x 2160
HDMI-1 connected{' primary' if primary else ''} {width}x{height}+0+0 (normal)
   3840x2160 30.00{'*+' if star4k else '+'} 60.00
   1920x1080 60.00{'*' if width==1920 and current=='60.00' else ''} 30.00{'*' if width==1920 and current=='30.00' else ''}
DSI-1 connected 720x1280+3840+0
   720x1280 69.05*+
'''
class DisplayTests(unittest.TestCase):
    def test_4k(self):
        self.assertEqual(plan(fixture()),[['xrandr','--output','HDMI-1','--primary','--mode','1920x1080','--rate','60']])
    def test_1080_60(self):self.assertEqual(plan(fixture(1920,1080,True,'60.00',False)),[])
    def test_1080_30(self):self.assertEqual(plan(fixture(1920,1080,True,'30.00',False))[0][-4:],['--mode','1920x1080','--rate','60'])
    def test_small(self):self.assertEqual(plan(fixture(1280,720,True)),[])
    def test_dsi_only(self):self.assertEqual(plan('DSI-1 connected 720x1280+0+0\n 720x1280 69.05*+\n'),[])
    def test_optout(self):
        with patch.dict(os.environ,{'VISION_KEEP_DISPLAY_MODE':'1','DISPLAY':':0'}),patch('subprocess.check_output') as query:
            prepare();query.assert_not_called()
    def test_missing_display(self):
        with patch.dict(os.environ,{},clear=True),patch('subprocess.check_output') as query:prepare();query.assert_not_called()
if __name__=='__main__':unittest.main()
