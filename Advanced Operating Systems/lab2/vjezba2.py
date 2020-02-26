import re
import sys
from Crypto import Random
from Crypto.Util import Padding
from Crypto.Util import Counter
from Crypto.Util.number import bytes_to_long
from Crypto.Cipher import AES
from Crypto.Cipher import DES3
from Crypto.PublicKey import RSA
from Crypto.Hash import SHA3_224
from Crypto.Hash import SHA3_256
from Crypto.Hash import SHA3_384
from Crypto.Hash import SHA3_512
from Crypto.Cipher import PKCS1_OAEP
from Crypto.Signature import PKCS1_PSS

def upis(podatak, poruka):
    print(poruka)
    for i in range(len(podatak)):
        print('\t{} - {}'.format(i+1, podatak[i]))
    pom = sys.stdin.readline().strip()
    return podatak[int(pom)-1]

def simetricni_kljuc_u_datoteku(ime_izlazne_dat, nacin, kljuc):
    f = open(ime_izlazne_dat, 'w')
    f.write('''---BEGIN NOS CRYPTO DATA---\n\nDescription:\n\tSecret key\n\nMethod:\n\t{nacin}\n\nSecret key:\n\t{k}\n\n---END NOS CRYPTO DATA---'''.format(nacin=nacin, k=kljuc))

def rsa_kljuc_u_dat(ime_izlazne_dat, rsa_kljuc, velicina_kljuca, javni):
    opis = "Private key"
    if javni:
        opis = "Public key"

    modulus = rsa_kljuc.n
    pom = ""
    exp = None
    if javni:
        pom = "Public "
        exp = rsa_kljuc.e
    else:
        pom = "Private "
        exp = rsa_kljuc.d

    f = open(ime_izlazne_dat, 'w')
    f.write('''---BEGIN NOS CRYPTO DATA---\n\nDescription:\n\t{opis}\n\nMethod:\n\tRSA\n\nKey length:\n\t{vel}\n\nModulus:\n\t{modulus}\n\n{pom}exponent:\n\t{exp}\n\n---END NOS CRYPTO DATA---'''.format(opis=opis, vel=velicina_kljuca, modulus=modulus, pom=pom, exp=exp))

def omotnica_u_datoteku(ime_datoteke, ime_izlazne_dat, metoda_s, metoda_a, duljina_s, duljina_a, podaci, kljuc):
    f = open(ime_izlazne_dat, 'w')
    f.write('''---BEGIN NOS CRYPTO DATA---\n\nDescription:\n\tEnvelope\n\nFile name:\n\t{ime_datoteke}\n\nMethod:\n\t{metoda_s}\n\t{metoda_a}\n\nKey length:\n\t{duljina_s}\n\t{duljina_a}\n\nEnvelope data:\n\t{podaci}\n\nEnvelope crypt key:\n\t{kljuc}\n\n---END NOS CRYPTO DATA---'''
            .format(ime_datoteke=ime_datoteke, metoda_s=metoda_s, metoda_a=metoda_a, duljina_s=duljina_s,
            duljina_a=duljina_a, podaci=podaci, kljuc=kljuc))

def potpis_u_datoteku(ime_datoteke, ime_izlazne_dat, sazetak, metoda_a, duljina_saz, duljina_a, potpis):
    f = open(ime_izlazne_dat, 'w')
    f.write('''---BEGIN NOS CRYPTO DATA---\n\nDescription:\n\tSignature\n\nFile name:\n\t{ime_datoteke}\n\nMethod:\n\t{sazetak}\n\t{metoda_a}\n\nKey length:\n\t{duljina_saz}\n\t{duljina_a}\n\nSignature:\n\t{potpis}\n\n---END NOS CRYPTO DATA---'''
            .format(ime_datoteke=ime_datoteke, sazetak=sazetak, metoda_a=metoda_a,
            duljina_saz=duljina_saz, duljina_a=duljina_a, potpis=potpis))


class AES_kriptiranje:
    def __init__(self):
        self.ime = "AES"
        self.velicine_kljuca = {'128' : 16, '192' : 24, '256' : 32}
        self.nacini = { 'CBC' : AES.MODE_CBC, 'CFB' : AES.MODE_CFB, 'OFB' : AES.MODE_OFB, 'CTR' : AES.MODE_CTR }
    
    def inicijaliziraj(self, velicina_kljuca, nacin):
        self.velicina_kljuca = self.velicine_kljuca[velicina_kljuca]
        self.nacin = self.nacini[nacin]
        self.velicina_bloka = AES.block_size
        self.kljuc = Random.new().read(self.velicina_kljuca)
        self.IV = Random.new().read(self.velicina_bloka) # inicijalizacijski vektor
        self.brojac = Counter.new(self.velicina_kljuca, initial_value = bytes_to_long(self.IV))
        if self.nacin == AES.MODE_CTR:
            self.x = AES.new(self.kljuc, self.nacin, counter = self.brojac)
        else:
            self.x = AES.new(self.kljuc, self.nacin, self.IV)
        simetricni_kljuc_u_datoteku('aes.key', self.ime, self.kljuc) # spremi kljuc u datoteku aes.key

    def kopiraj(self):
        novi = AES_kriptiranje()
        novi.velicina_kljuca = self.velicina_kljuca
        novi.velicina_bloka = AES.block_size
        novi.nacin = self.nacin
        novi.kljuc = self.kljuc
        novi.IV = self.IV
        novi.brojac = self.brojac
        if novi.nacin == AES.MODE_CTR:
            novi.x = AES.new(novi.kljuc, novi.nacin, counter = novi.brojac)
        else:
            novi.x = AES.new(novi.kljuc, novi.nacin, novi.IV)
        return novi	

    def enkripcija(self, ulaz):
        ulaz = Padding.pad(ulaz, self.velicina_bloka) # popuni blok
        izlaz = ''
        for i in range(len(ulaz) // self.velicina_bloka): # posebno svaki blok
            izlaz += self.x.encrypt(ulaz[i * self.velicina_bloka:(i + 1) * self.velicina_bloka])
        return izlaz

    def dekripcija(self, ulaz):
        izlaz = ''
        for i in range(len(ulaz) // self.velicina_bloka):
            izlaz += self.x.decrypt(ulaz[i * self.velicina_bloka:(i + 1) * self.velicina_bloka])
        return Padding.unpad(izlaz, self.velicina_bloka)
    
class DES3_kriptiranje:
    def __init__(self):
        self.ime = "DES3"
        self.velicine_kljuca = {'16' : 16, '24' : 24}
        self.nacini = { 'CBC' : DES3.MODE_CBC, 'CFB' : DES3.MODE_CFB, 'OFB' : DES3.MODE_OFB, 'CTR' : DES3.MODE_CTR }

    def inicijaliziraj(self, velicina_kljuca, nacin):
        self.velicina_kljuca = self.velicine_kljuca[velicina_kljuca]
        self.nacin = self.nacini[nacin]
        self.velicina_bloka = DES3.block_size
        self.kljuc = Random.new().read(self.velicina_kljuca)
        self.IV = Random.new().read(self.velicina_bloka) # inicijalizacijski vektor
        self.brojac = Counter.new(self.velicina_kljuca, initial_value = bytes_to_long(self.IV))
        if self.nacin == DES3.MODE_CTR:
            self.x = DES3.new(self.kljuc, self.nacin, counter = self.brojac)
        else:
            self.x = DES3.new(self.kljuc, self.nacin, self.IV)
        simetricni_kljuc_u_datoteku('des3.key', self.ime, self.kljuc) # spremi kljuc u datoteku des3.key

    def kopiraj(self):
        novi = DES3_kriptiranje()
        novi.velicina_kljuca = self.velicina_kljuca
        novi.velicina_bloka = DES3.block_size
        novi.nacin = self.nacin
        novi.kljuc = self.kljuc
        novi.IV = self.IV
        novi.brojac = self.brojac
        if novi.nacin == DES3.MODE_CTR:
            novi.x = DES3.new(novi.kljuc, novi.nacin, counter = novi.brojac)
        else:
            novi.x = DES3.new(novi.kljuc, novi.nacin, novi.IV)
        return novi

    def enkripcija(self, ulaz):
        ulaz = Padding.pad(ulaz, self.velicina_bloka) # popuni blok
        izlaz = ''
        for i in range(len(ulaz) // self.velicina_bloka): # posebno svaki blok
            izlaz += self.x.encrypt(ulaz[i * self.velicina_bloka:(i + 1) * self.velicina_bloka])
        return izlaz

    def dekripcija(self, ulaz):
        izlaz = ''
        for i in range(len(ulaz) // self.velicina_bloka):
            izlaz += self.x.decrypt(ulaz[i * self.velicina_bloka:(i + 1) * self.velicina_bloka])
        return Padding.unpad(izlaz, self.velicina_bloka)
    
class RSA_kriptiranje:
    def __init__(self):
        self.ime = "RSA"
        self.velicine_kljuca = { '2048' : 2048, '4096' : 4096 }

    def inicijaliziraj(self, velicina_kljuca, naziv):
        self.velicina_kljuca = self.velicine_kljuca[velicina_kljuca] # duljina broja n
        self.privatni_kljuc = RSA.generate(self.velicina_kljuca)
        self.javni_kljuc = self.privatni_kljuc.publickey()
        rsa_kljuc_u_dat(naziv + '_privatni_rsa.key', self.privatni_kljuc, self.velicina_kljuca, False)
        rsa_kljuc_u_dat(naziv + '_javni_rsa.key', self.javni_kljuc, self.velicina_kljuca, True)
        
        self.kljuc_za_potpis = PKCS1_PSS.new(self.privatni_kljuc)
        self.kljuc_za_verifikaciju = PKCS1_PSS.new(self.javni_kljuc)
        self.privatni_kljuc = PKCS1_OAEP.new(self.privatni_kljuc)
        self.javni_kljuc = PKCS1_OAEP.new(self.javni_kljuc)

    def enkripcija(self, ulaz, javni_privatni):
        if javni_privatni == "javni":
            return self.javni_kljuc.encrypt(ulaz)
        else:
            return self.privatni_kljuc.encrypt(ulaz)
    
    def potpisi(self, sazetak):
        return self.kljuc_za_potpis.sign(sazetak)

    def dekripcija(self, ulaz, javni_privatni):
        if javni_privatni == "javni":
            return self.javni_kljuc.decrypt(ulaz)
        else:
            return self.privatni_kljuc.decrypt(ulaz)

    def provjeri_potpis(self, potpis, sazetak):
        return self.kljuc_za_verifikaciju.verify(sazetak, potpis)

class izracun_SHA:
    def __init__(self):
        self.vrste = { 'SHA3_224': (SHA3_224, 224), 'SHA3_256': (SHA3_256, 256),
                       'SHA3_384': (SHA3_384, 384), 'SHA3_512': (SHA3_512, 512) }
    
    def inicijaliziraj(self, vrsta):
        self.ime = vrsta
        self.vrsta, self.duljina_sazetka = self.vrste[vrsta]
        self.s = self.vrsta.new()
        self.velicina_sazetka = self.s.digest_size

    def kopiraj(self):
        x = izracun_SHA()
        x.inicijaliziraj(self.ime)
        return x

    def azuriraj(self, ulaz):
        self.s.update(ulaz)

    def sazetak(self, ulaz):
        return self.s.update(ulaz).digest()
    
def ucitaj_kljuc(poruka):
    x = AES_kriptiranje()
    if upis(["AES", "DES3"], poruka) == "DES3":
        x = DES3_kriptiranje()
    velicina_kljuca = upis(list(x.velicine_kljuca.keys()), "Odaberi velicinu kljuca")
    nacin = upis(list(x.nacini.keys()), "Odaberi nacin")
    x.inicijaliziraj(velicina_kljuca, nacin)
    return x

def ucitaj_rsa_kljuc(poruka, naziv):
    print(poruka)
    x = RSA_kriptiranje()
    velicina_kljuca = upis(list(x.velicine_kljuca.keys()), "Odaberi velicinu kljuca")
    x.inicijaliziraj(velicina_kljuca, naziv)
    return x

def ucitaj_hash(poruka):
    x = izracun_SHA()
    vrsta = upis(list(x.vrste.keys()), poruka)
    x.inicijaliziraj(vrsta)
    return x

def main():
    ulazna_datoteka = "ulaz.txt"
    ulazni_podaci = str(open(ulazna_datoteka, 'r').read())
    tajni_kljuc = ucitaj_kljuc("Odaberi algoritam kriptiranja tajnog kljuca")
    anin_kljuc = ucitaj_rsa_kljuc("Odabir Aninog RSA kljuca", "anin")
    brankov_kljuc = ucitaj_rsa_kljuc("Odabir Brankovog RSA kljuca", "brankov")
    hash = ucitaj_hash("Odaberi funkciju sazimanja")

    print(">>> ENKRIPCIJA <<<")

    print("Radim omotnicu...")
    # kriptiraj tekst tajnim kljucem
    kriptirani_podaci = tajni_kljuc.enkripcija(ulazni_podaci) # C1
    # kriptiraj tajni kljuc brankovim javnim kljucem
    kriptirani_tajni_kljuc = brankov_kljuc.enkripcija(tajni_kljuc.kljuc, "javni") # C2
    # omotnica = C1, C2
    omotnica_u_datoteku(ulazna_datoteka, 'omotnica.txt', tajni_kljuc.ime, brankov_kljuc.ime, tajni_kljuc.velicina_kljuca,
                            brankov_kljuc.velicina_kljuca, kriptirani_podaci, kriptirani_tajni_kljuc)
    
    print("Radim potpis...")
    # napravi sazetak omotnice
    sazetak_omotnice = hash.sazetak(kriptirani_podaci) # S
    # kriptiraj aninim privatnim kljucem sazetak omotnice
    potpis = anin_kljuc.potpisi(hash.s) # C3
    potpis_u_datoteku(ulazna_datoteka, 'potpis.txt', hash.ime, anin_kljuc.ime, hash.duljina_sazetka,
                        anin_kljuc.velicina_kljuca, potpis)


    print(">>> DEKRIPCIJA <<<")

    # Branko svojim privatnim kljucem dekriptira C2 i dobiva tajni kljuc
    tajni_kljuc2 = tajni_kljuc.kopiraj()
    tajni_kljuc2.kljuc = brankov_kljuc.dekripcija(kriptirani_tajni_kljuc, "privatni")

    # dobivenim tajnim kljucem dekriptira C1 i dobiva razgovjetni tekst
    dekriptirani_podaci = tajni_kljuc2.dekripcija(kriptirani_podaci)

    # dekriptira C3 Aninim javnim kljucem i saznaje sazetak (provjera potpisa)
    hash2 = hash.kopiraj()
    hash2.azuriraj(kriptirani_podaci)
    provjera = anin_kljuc.provjeri_potpis(potpis, hash2.s)

    print("Sazetak?", provjera)
    print("Podaci?", dekriptirani_podaci == ulazni_podaci)

    dekriptirana_datoteka = "dekriptirani_ulaz.txt"
    f = open(dekriptirana_datoteka, 'w')
    f.write(dekriptirani_podaci)


if __name__ == '__main__':
    main()