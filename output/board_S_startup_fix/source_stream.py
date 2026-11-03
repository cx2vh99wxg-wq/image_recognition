from pathlib import Path
import base64,gzip,json,hashlib
p=Path('udp_rk_rk_yolo/S/Makefile')
data=p.read_bytes().replace(b'\r\n',b'\n')
rows=[('S/Makefile',base64.b64encode(data).decode(),hashlib.sha256(data).hexdigest())]
blob=gzip.compress(json.dumps(rows).encode())
print(json.dumps(dict(count=len(rows),hash=hashlib.sha256(blob).hexdigest(),data=base64.b64encode(blob).decode())))
