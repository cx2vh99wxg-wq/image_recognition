"""Passive packet and deduplication-reference tests; no hardware or processes."""
from pathlib import Path
import socket
import struct
import sys
import tempfile
import unittest
import zlib
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tools/calibration'))
from terminal_center import decode_packet,reference_session
from install_terminal_center import insert_hook,BEGIN,END,ANCHOR

def packet(session=7,flags=1,age=3):
    wire=bytearray(b'STF1\0\1\0\x30'+bytes(40))
    struct.pack_into('!3I',wire,8,session,100,flags)
    struct.pack_into('!I',wire,28,age);struct.pack_into('!Q',wire,32,1234)
    struct.pack_into('!I',wire,44,zlib.crc32(wire[:44]))
    ip=bytearray(20);ip[0]=0x45;ip[9]=17
    ip[12:16]=socket.inet_aton('192.168.100.10');ip[16:20]=socket.inet_aton('192.168.100.20')
    return bytes(12)+b'\x08\x00'+ip+struct.pack('!4H',40000,54321,56,0)+wire

class TerminalCenterTests(unittest.TestCase):
    def test_hook_precedes_foreground_start_and_installs_idempotently(self):
        original='case $- in *i*) ;; *) return;; esac\n'+ANCHOR+'\nmake run\n'
        block=BEGIN+'\nprintf hook\n'+END+'\n'
        result=insert_hook(original,block)
        self.assertLess(result.index(BEGIN),result.index('make run'))
        self.assertEqual(insert_hook(result,block),result)
        self.assertIn(original,result.replace(block+'\n',''))
    def test_ambiguous_startup_is_not_modified(self):
        for text in ['make run\n',BEGIN+'\nmake run\n',ANCHOR+'\n'+ANCHOR]:
            with self.assertRaises(ValueError):insert_hook(text,BEGIN+'\n'+END)
    def test_real_packets(self):
        self.assertEqual(decode_packet(packet()),7)
        self.assertEqual(decode_packet(packet(session=8,flags=3)),8)
    def test_invalid_source_crc_simulated_and_age(self):
        for raw in [packet(session=0),packet(flags=5),packet(age=100),packet()[:-1],packet()[:-1]+b'X']:
            self.assertIsNone(decode_packet(raw))
        raw=bytearray(packet());raw[29]=99
        self.assertIsNone(decode_packet(raw))
    def test_reference_requires_explicit_valid_declaration(self):
        build=ROOT/'build/tests';build.mkdir(parents=True,exist_ok=True)
        with tempfile.TemporaryDirectory(prefix='terminal_center_',dir=build) as folder:
            Path(folder).resolve().relative_to(ROOT.resolve())
            p=Path(folder)/'reference'
            for text in ['STEER_PHYSICAL_REFERENCE_V1 7 0.2 1','STEER_PHYSICAL_REFERENCE_V1 7 10.5 1']:
                p.write_text(text);self.assertEqual(reference_session(p),7)
            for text in ['STEER_PHYSICAL_REFERENCE_V1 7 0.2 0','STEER_PHYSICAL_REFERENCE_V1 0 0.2 1','STEER_PHYSICAL_REFERENCE_V1 7 nan 1']:
                p.write_text(text);self.assertIsNone(reference_session(p))

if __name__=='__main__':unittest.main()
