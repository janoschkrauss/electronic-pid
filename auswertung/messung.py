# Komplette Messung mit einem Befehl:
#   python3 messung.py NAME
# 1. laedt komplett.ino auf den Arduino
# 2. schreibt alles in NAME.csv, bis der Arduino "ENDE" meldet
# 3. zeichnet den Plot NAME.png
import subprocess, sys, time
import serial
import pandas as pd
import matplotlib.pyplot as plt

PORT = "/dev/cu.usbmodem1101"
CLI = "/Applications/Arduino IDE.app/Contents/Resources/app/lib/backend/resources/arduino-cli"
name = sys.argv[1]

print("Lade komplett.ino auf den Arduino ...")
subprocess.run([CLI, "compile", "--upload", "-b", "arduino:avr:uno", "-p", PORT, "../komplett"], check=True)

ser = serial.Serial(PORT, 9600, timeout=1)
t0 = time.time()
with open(name + ".csv", "w") as f:
    f.write("t,T,Tset,duty\n")
    while True:
        line = ser.readline().decode(errors="ignore").strip()
        if line == "ENDE":
            break
        try:
            T, Tset, luefter = [float(p.split(":")[1]) for p in line.split(",")]
        except (ValueError, IndexError):   # angefangene/kaputte Zeile ueberspringen
            continue
        t = time.time() - t0
        f.write(f"{t:.1f},{T},{Tset},{luefter * 10:.0f}\n")
        f.flush()
        print(f"{t:6.0f} s   T = {T:.2f}   Tset = {Tset:.2f}   duty = {luefter * 10:.0f}")
print("Messung fertig:", name + ".csv")

d = pd.read_csv(name + ".csv")
fig, (a1, a2) = plt.subplots(2, 1, sharex=True, figsize=(8, 6), height_ratios=[3, 1])
a1.plot(d["t"], d["T"], label=r"$T$")
a1.plot(d["t"], d["Tset"], ls="--", c="k", label=r"$T_\mathrm{set}$")
a1.set_ylabel(r"$T$ [$^\circ$C]")
a1.legend()
a2.plot(d["t"], d["duty"] / 2.55)
a2.set_ylabel("Luefter [%]")
a2.set_xlabel(r"$t$ [s]")
fig.tight_layout()
fig.savefig(name + ".png", dpi=150)
print("Plot gespeichert:", name + ".png")
plt.show()
