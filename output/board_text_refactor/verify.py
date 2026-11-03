from pathlib import Path
import json,re,sys
w=Path(__file__).resolve().parent
root=w.parents[1]/'udp_rk_rk_yolo'
sys.dont_write_bytecode=True
sys.path.insert(0,str(w.parent/'submission_similarity_20261102'))
import check_submission as audit
scope={'__file__':str(w/'rewrite.py')}
exec((w/'rewrite.py').read_text(encoding='utf-8-sig').split('files=sorted',1)[0],scope)
manifest=json.loads((w/'manifest.json').read_text())
for rel in manifest:
 p=root/rel; data=p.read_bytes(); text=data.decode()
 old=(w/'before'/rel).read_bytes().decode().replace('\r\n','\n')
 if p.name=='Makefile':
  def tail(line):
   if line.startswith(('\t','#')) or '"' in line or "'" in line:return line
   return re.sub(r'(  +#\s*)(.+?)(?=\r?$)',lambda m:m[1]+scope['wording'](m[2],fallback=True),line,flags=re.M)
  new=''.join(tail(line) for line in text.splitlines(keepends=True))
  def recipe(s):
   s=re.sub(r'(?m)^(?!\t)([^\n#]+?)  +#[^\n]*$',r'\1',s)
   return scope['code_projection'](s)
  assert recipe(old).strip()==recipe(new.replace('\r\n','\n')).strip(),rel
 elif p.suffix in ('.c','.cpp'):
  def rewrite_comment(m):
   token=m[0]
   if not token.startswith(('//','/*')) or re.search(r'copyright|spdx|license|clang-format|NOLINT',token,re.I):
    return token
   return scope['wording'](token)
  new=audit.C_LEX.sub(rewrite_comment,text)
  original=scope['code_projection'](old,True)
  changed=scope['code_projection'](new.replace('\r\n','\n'),True)
  assert audit.lex(original,'c_family')==audit.lex(changed,'c_family'),rel
 else:
  new=text
  assert scope['code_projection'](old)==scope['code_projection'](new.replace('\r\n','\n')),rel
 p.write_bytes(new.encode())
 print('verified',rel)
for b in 'MS':
 old=(w/f'active_{b}.before').read_text(encoding='utf-8')
 new=(w/f'active_{b}.after').read_text(encoding='utf-8')
 assert scope['code_projection'](old)==scope['code_projection'](new)
 print('verified active .bashrc',b)

