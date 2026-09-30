#!/usr/bin/env python3
import argparse,json,shutil,zipfile,tempfile,sys
from pathlib import Path,PurePosixPath
def emit(n,s): print('GATITO_STAGE|%d|%s'%(n,s),flush=True)
def main():
 p=argparse.ArgumentParser(); p.add_argument('recipe'); p.add_argument('--game-dir',required=True); a=p.parse_args()
 game=Path(a.game_dir).resolve(); r=json.loads(Path(a.recipe).read_text()); stage=Path(tempfile.mkdtemp(prefix='stage-',dir=game/'.gatito-extract'))
 try:
  files=[x for d in r.get('input',{}).get('search_dirs',['gamedata','.']) for x in (game/d).iterdir()] if False else []
  for d in r.get('input',{}).get('search_dirs',['gamedata','.']):
   q=game/d
   if q.is_dir(): files += [x for x in q.iterdir() if x.is_file()]
  if not files: raise RuntimeError('APK/OBB não encontrado')
  emit(5,'Entrada encontrada: '+', '.join(x.name for x in files[:3]))
  rules=r['extract']
  for i,rule in enumerate(rules):
   emit(10+i*70//len(rules),'Procurando: '+rule['id'])
   found=None
   for src in files:
    try: z=zipfile.ZipFile(src)
    except zipfile.BadZipFile: continue
    names=z.namelist()
    for pat in rule['source']['patterns']:
     found=next(((src,n) for n in names if PurePosixPath(n).match(pat)),None)
     if found: break
    if found: break
   if not found: raise RuntimeError('não encontrado: '+rule['id'])
   src,name=found; dst=stage/Path(rule['destination'])
   dst.parent.mkdir(parents=True,exist_ok=True)
   with zipfile.ZipFile(src) as z,z.open(name) as inp,dst.open('wb') as out: shutil.copyfileobj(inp,out)
   if not dst.is_file() or dst.stat().st_size==0: raise RuntimeError('extração inválida: '+rule['destination'])
   emit(10+(i+1)*70//len(rules),'Extraído e validado: '+rule['destination'])
  target=game/'game'; old=game/'.gatito-old'
  if old.exists(): shutil.rmtree(old)
  if target.exists(): target.rename(old)
  stage.rename(target)
  if old.exists(): shutil.rmtree(old)
  emit(100,'Todos os dados extraídos e validados')
  return 0
 except Exception as e:
  shutil.rmtree(stage,ignore_errors=True); print('GATITO EXTRACT ERROR:',e,file=sys.stderr); return 1
if __name__=='__main__': raise SystemExit(main())