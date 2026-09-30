#!/usr/bin/env python3
"""Gatito Extractor UI for Need for Speed Shift."""
import os,sys,time
from pathlib import Path
BAR=36
def screen(pct,stage,detail=""):
    sys.stdout.write("\033[2J\033[H")
    print("+"+"-"*60+"+")
    print("|"+"GATITO EXTRACTOR".center(60)+"|")
    print("|"+"Need for Speed Shift".center(60)+"|")
    print("|"+"".center(60)+"|")
    n=max(0,min(BAR,int(BAR*pct/100)))
    print("|"+("["+"█"*n+"░"*(BAR-n)+"] %3d%%"%pct).center(60)+"|")
    print("|"+stage.center(60)+"|")
    print("|"+detail[:60].center(60)+"|")
    print("+"+"-"*60+"+")
    sys.stdout.flush()
def ok(path,min_size=1):
    p=Path(path)
    return p.is_file() and p.stat().st_size>=min_size
def main():
    root=Path(os.environ.get("GAMEDIR",".")).resolve()
    game=root/"game"
    screen(5,"Preparando","Verificando diretório do port")
    time.sleep(.35)
    if not game.is_dir():
        screen(100,"Falha","game/ não encontrado")
        return 72
    screen(20,"Validando S3E","Procurando NFSShift.s3e.unpacked")
    time.sleep(.35)
    if not ok(game/"NFSShift.s3e.unpacked",1024*1024):
        screen(100,"Extração incompleta","Falta o S3E desempacotado")
        print("\nO Gatito Extractor encontrou os dados ainda não preparados.")
        print("A etapa de unpack do Marmalade S3E ainda precisa de um backend validado.")
        return 73
    screen(45,"Validando common.dz","Verificando arquivo")
    time.sleep(.35)
    if not ok(game/"common.dz",1024):
        screen(100,"Falha","common.dz ausente/inválido")
        return 74
    screen(65,"Validando gfx.dz","Verificando arquivo")
    time.sleep(.35)
    if not ok(game/"gfx.dz",1024):
        screen(100,"Falha","gfx.dz ausente/inválido")
        return 75
    screen(88,"Validação final","Payload completo")
    time.sleep(.5)
    screen(100,"Pronto","Dados do jogo validados")
    time.sleep(.5)
    return 0
if __name__=="__main__":
    raise SystemExit(main())
